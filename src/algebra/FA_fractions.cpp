//*************************************************************************************************
//*************************************************************************************************
//* The MIT License (MIT)                                                                         *
//* Copyright (C) 2026 Ivan Matic                                                                 *
//* fundamental toolbox, revision 2026-09                                                         *
//*                                                                                               *
//* Permission is hereby granted, free of charge, to any person obtaining a copy of this          *
//* software and associated documentation files (the "Software"), to deal in the Software         *
//* without restriction, including without limitation the rights to use, copy, modify, merge,     *
//* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons    *
//* to whom the Software is furnished to do so, subject to the following conditions:              *
//*                                                                                               *
//* The above copyright notice and this permission notice shall be included in all copies or      *
//* substantial portions of the Software.                                                         *
//*                                                                                               *
//* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,           *
//* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR      *
//* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE     *
//* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR          *
//* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
//* DEALINGS IN THE SOFTWARE.                                                                     *
//*************************************************************************************************

#ifndef _INCL_FA_CPP
#define _INCL_FA_CPP
namespace mp = boost::multiprecision;
using Int256 = mp::number<mp::cpp_int_backend<256, 256,
                    mp::signed_magnitude, mp::checked, void>>;
using Int512 = mp::number<mp::cpp_int_backend<512, 512,
                    mp::signed_magnitude, mp::checked, void>>;
namespace FA{
std::atomic<int> GL_OVERFLOW=0;
std::atomic<int> GL_DIVBY0=0;
template<typename BigNum> requires (mp::is_number<BigNum>::value)
BigNum multiplyBigNumbers(BigNum a, BigNum b){
    try{ return a*b; }
    catch(const std::overflow_error&){ GL_OVERFLOW=1; return 0; }
}
__int128 multiplyBigNumbers(__int128 a, __int128 b){
    __int128 c;
    if(__builtin_mul_overflow(a, b, &c)){ GL_OVERFLOW = 1; }
    return c;
}
template<typename BigNum> requires (mp::is_number<BigNum>::value)
BigNum addBigNumbers(BigNum a, BigNum b){
    try{ return a+b; }
    catch(const std::overflow_error&){ GL_OVERFLOW=1; return 0; }
}
__int128 addBigNumbers(__int128 a, __int128 b) {
    __int128 c;
    if (__builtin_add_overflow(a, b, &c)) { GL_OVERFLOW = 1; }
    return c;
}
template<typename BigNum> requires (mp::is_number<BigNum>::value)
BigNum subBigNumbers(BigNum a, BigNum b){
    try{ return a-b; }
    catch(const std::overflow_error&){ GL_OVERFLOW=1; return 0; }
}
__int128 subBigNumbers(__int128 a, __int128 b) {
    __int128 c;
    if (__builtin_sub_overflow(a, b, &c)) { GL_OVERFLOW = 1; }
    return c;
}
template<typename BigNum>
BigNum absBig(BigNum a){
    if(a<0){
        BigNum zero=0;
        return subBigNumbers(zero,a);
    }
    return a;
}
template<typename BigNum>
BigNum gcdBig(BigNum a, BigNum b){
    if( (a==0)&&(b==0)){GL_DIVBY0=1; return 1;}
    a=absBig(a);
    b=absBig(b);
    BigNum t;
    while(b != 0){
        t = a % b;
        a = b;
        b = t;
    }
    return a;
}
template<typename BigNum>
BigNum leastCommonMultiple(BigNum a, BigNum b){
    a=absBig(a);
    b=absBig(b);
    if(a == 0 || b == 0){ return 0; }
    return multiplyBigNumbers(a / gcdBig(a, b), b);
}
template<typename BigNum>
int discreteLog2(BigNum a){
    //returns -1 if a is not a positive power of two, otherwise the exponent
    if(a<1){return -1;}
    if(a==1){return 0;}
    int counter=0;
    while(a>1){
        if(a%2){return -1;}
        ++counter;
        a/=2;
    }
    return counter;
}


// Materializes 2^e. USED ONLY on the non-dyadic paths, never in dyadic mode.
template<typename BigNum>
BigNum powerOfTwo(int e){
    static_assert(std::numeric_limits<BigNum>::is_specialized,
                      "powerOfTwo requires a numeric type with numeric_limits");
    if(e<0){return 1;}
    if(e > std::numeric_limits<BigNum>::digits - 1){GL_OVERFLOW=1;return 1;}
    BigNum res=1;
    for(int i=0;i<e;++i){res=addBigNumbers(res,res);}
    return res;
}
template<typename BigNum>
std::string writeAsPowerOfTwoIfPossible(BigNum a){
    int dl2=discreteLog2(a);
    if(dl2<0){return "";}
    return "2^{"+std::to_string(dl2)+"}";
}

template<typename BigNum>
std::string toString(BigNum a){
    if(a==0){return "0";}
    std::string res;
    std::string sgn;
    if(a<0){sgn+="-";a = absBig(a);}
    while(a>0){
        res=std::to_string(static_cast<int>(a%10))+res;
        a/=10;
    }
    return sgn+res;
}
}
template<typename BigNum>
class Frac{
    // REPRESENTATION (two modes, selected by log2Den):
    //
    //   log2Den >= 0  ("dyadic mode"):  value = num / 2^log2Den .
    //                 The field `den` is NOT PART OF THE VALUE and is never read.
    //                 It is held at 1 purely so that the object is deterministic.
    //                 2^log2Den is NEVER materialized, so log2Den may be arbitrarily
    //                 large (thousands) with no overflow.
    //                 Canonical form: num odd, or log2Den==0.  Zero is (0,1,0).
    //
    //   log2Den == -1 ("general mode"): value = num / den, with den >= 3 not a power
    //                 of two, gcd(|num|,den)==1, den > 0, sign carried by num.
    //
    // Any new code path that writes num/den/log2Den MUST restore this by calling
    // reduce().  No path may read `den` without first checking log2Den == -1.
private:
    BigNum num;
    BigNum den;
    int log2Den;
    void normalizeDyadic();
    void normalizeGeneral();
    int cmp(const Frac&) const;   // -1, 0, or +1
public:
    Frac(const BigNum& =0, const BigNum& =1);
    BigNum getNum() const;
    int isDyadic() const;
    int getLog2Den() const;
    BigNum getDenGeneral() const;   // valid only when !isDyadic()
    BigNum getDen() const;
    void setNum(const BigNum &);
    void setDen(const BigNum &);
    int operator>(const Frac& ) const;
    int operator<(const Frac& ) const;
    int operator==(const Frac& ) const;
    void reduce();
    Frac getReciprocal() const;
    Frac& operator+=(const Frac& );
    Frac& operator-=(const Frac& );
    Frac& operator*=(const Frac& );
    Frac& operator/=(const Frac& );
    Frac& operator=(const BigNum &);
    Frac& operator=(const Frac &);
    std::string toString() const;
};

// ---------------------------------------------------------------- normalization

template<typename BigNum>
void Frac<BigNum>::normalizeDyadic(){
    // assumes log2Den >= 0; strips common factors of two WITHOUT touching den
    BigNum zero=0;
    if(num==zero){den=1;log2Den=0;return;}
    if(log2Den<0){log2Den=0;}
    BigNum a=FA::absBig(num);
    int neg=(num<zero);
    while( (log2Den>0) && (a%2==zero) ){
        a/=2;--log2Den;
    }
    num = neg ? FA::subBigNumbers(zero,a) : a;
    den = 1;
}
template<typename BigNum>
void Frac<BigNum>::normalizeGeneral(){
    // assumes the value is num/den with den != 0; picks the right mode
    BigNum zero=0;
    if(den==zero){den=1;FA::GL_DIVBY0=1;}
    if(den<zero){ num=FA::subBigNumbers(zero,num); den=FA::subBigNumbers(zero,den); }
    if(num==zero){den=1;log2Den=0;return;}
    BigNum g=FA::gcdBig(num,den);
    if(g>1){ num/=g; den/=g; }
    int e=FA::discreteLog2(den);
    if(e>=0){
        log2Den=e;
        normalizeDyadic();     // drops into dyadic mode; den released
        return;
    }
    log2Den=-1;                // genuinely non-dyadic; den is meaningful
}
template<typename BigNum>
void Frac<BigNum>::reduce(){
    if(log2Den>-1){normalizeDyadic();}
    else{normalizeGeneral();}
}

// ---------------------------------------------------------------- construction

template<typename BigNum>
Frac<BigNum>::Frac(const BigNum & n, const BigNum &d){
    num=n;den=d;log2Den=-1;
    if(den==0){den=1;FA::GL_DIVBY0=1;}
    normalizeGeneral();
}
template<typename BigNum>
BigNum Frac<BigNum>::getNum() const{return num;}
template<typename BigNum>
int Frac<BigNum>::isDyadic() const{return (log2Den>-1);}
template<typename BigNum>
int Frac<BigNum>::getLog2Den() const{return log2Den;}
template<typename BigNum>
BigNum Frac<BigNum>::getDenGeneral() const{return den;}
template<typename BigNum>
BigNum Frac<BigNum>::getDen() const{
    if(log2Den==-1){return den;}
    return FA::powerOfTwo<BigNum>(log2Den);
}
template<typename BigNum>
void Frac<BigNum>::setNum(const BigNum &n){
    num=n;
    reduce();
}
template<typename BigNum>
void Frac<BigNum>::setDen(const BigNum &d){
    den=d;log2Den=-1;
    if(den==0){den=1;FA::GL_DIVBY0=1;}
    normalizeGeneral();
}
template<typename BigNum>
Frac<BigNum>& Frac<BigNum>::operator=(const BigNum & x){
    num=x;den=1;log2Den=0;
    return *this;
}
template<typename BigNum>
Frac<BigNum>& Frac<BigNum>::operator=(const Frac<BigNum> & b){
    num=b.num;den=b.den;log2Den=b.log2Den;
    return *this;
}

// ---------------------------------------------------------------- comparison
// All comparisons are overflow-free for dyadics of arbitrary exponent: instead of
// scaling one side by 2^delta, we double it step by step and stop the moment it
// exceeds the other side, at which point the answer is already determined.
template<typename BigNum>
int compareDyadicMagnitudes(BigNum a1, int m1, BigNum a2, int m2){
    // a1,a2 > 0; compares a1/2^m1 with a2/2^m2; returns -1,0,1
    while( (m1>m2) && (a2<=a1) ){ a2=FA::addBigNumbers(a2,a2); --m1; }
    while( (m2>m1) && (a1<=a2) ){ a1=FA::addBigNumbers(a1,a1); --m2; }
    if(m1>m2){return -1;}   // a2 outgrew a1 and still has scaling left
    if(m2>m1){return 1;}
    if(a1<a2){return -1;}
    if(a2<a1){return 1;}
    return 0;
}
template<typename BigNum>
int compareMixedMagnitudes(BigNum a1, int e, BigNum a2, BigNum d){
    // a1,a2>0, d>0; compares a1/2^e with a2/d  <=>  a1*d with a2*2^e
    BigNum lhs=FA::multiplyBigNumbers(a1,d);
    while( (e>0) && (a2<=lhs) ){ a2=FA::addBigNumbers(a2,a2); --e; }
    if(e>0){return -1;}
    if(lhs<a2){return -1;}
    if(a2<lhs){return 1;}
    return 0;
}
template<typename BigNum>
int Frac<BigNum>::cmp(const Frac<BigNum>& b) const{
    BigNum zero;
    if( (num==zero) && (b.num==zero) ){return 0;}
    if(num==zero){return (b.num>zero)?-1:1;}
    if(b.num==zero){return (num>zero)?1:-1;}
    int s1=(num>zero)?1:-1;
    int s2=(b.num>zero)?1:-1;
    if(s1!=s2){return (s1>s2)?1:-1;}
    BigNum a1=FA::absBig(num);
    BigNum a2=FA::absBig(b.num);
    int c;
    if( (log2Den>-1) && (b.log2Den>-1) ){
        c=compareDyadicMagnitudes(a1,log2Den,a2,b.log2Den);
    }
    else if( (log2Den>-1) && (b.log2Den==-1) ){
        c=compareMixedMagnitudes(a1,log2Den,a2,b.den);
    }
    else if( (log2Den==-1) && (b.log2Den>-1) ){
        c=-compareMixedMagnitudes(a2,b.log2Den,a1,den);
    }
    else{
        BigNum l=FA::multiplyBigNumbers(a1,b.den);
        BigNum r=FA::multiplyBigNumbers(a2,den);
        c = (l<r) ? -1 : ((r<l)?1:0);
    }
    return (s1>0)?c:(-c);
}
template<typename BigNum>
int Frac<BigNum>::operator<(const Frac<BigNum> & b) const{ return (cmp(b)<0)?1:0; }
template<typename BigNum>
int Frac<BigNum>::operator>(const Frac<BigNum> & b) const{ return (cmp(b)>0)?1:0; }
template<typename BigNum>
int Frac<BigNum>::operator==(const Frac<BigNum> & b) const{
    // canonical forms are unique within each mode, and a canonical dyadic is
    // never equal to a canonical general value, so mode mismatch means unequal
    if(log2Den!=b.log2Den){return 0;}
    if(!(num==b.num)){return 0;}
    if(log2Den==-1){return (den==b.den)?1:0;}
    return 1;                     // dyadic mode: den is not part of the value
}

// ---------------------------------------------------------------- addition

template<typename BigNum>
Frac<BigNum> addDyadicAndGeneral(BigNum dyNum, int dyExp, BigNum gNum, BigNum gDen){
    // dyNum/2^dyExp + gNum/gDen, with gDen>0 not a power of two.
    // The result is generally non-dyadic, so its denominator 2^dyExp*gDen must be
    // formed; FA::powerOfTwo flags an overflow if dyExp is too large to allow it.
    BigNum p=FA::powerOfTwo<BigNum>(dyExp);
    BigNum numNew=FA::addBigNumbers(FA::multiplyBigNumbers(dyNum,gDen),
                                      FA::multiplyBigNumbers(gNum,p));
    BigNum denNew=FA::multiplyBigNumbers(p,gDen);
    return Frac<BigNum>(numNew,denNew);
}
template<typename BigNum>
Frac<BigNum>& Frac<BigNum>::operator+=(const Frac<BigNum>& b) {
    if(b.num==0){return *this;}
    if(num==0){ *this=b; return *this; }
    if( (log2Den>-1) && (b.log2Den>-1) ){
        // pure dyadic path: align exponents by doubling numerators only
        BigNum n1=num, n2=b.num;
        int m1=log2Den, m2=b.log2Den;
        int m=(m1>m2)?m1:m2;
        while(m1<m){++m1;n1=FA::addBigNumbers(n1,n1);}
        while(m2<m){++m2;n2=FA::addBigNumbers(n2,n2);}
        num=FA::addBigNumbers(n1,n2);
        log2Den=m;
        normalizeDyadic();       // no 2^m anywhere
        return *this;
    }
    if( (log2Den>-1) && (b.log2Den==-1) ){
        *this=addDyadicAndGeneral(num,log2Den,b.num,b.den);
        return *this;
    }
    if( (log2Den==-1) && (b.log2Den>-1) ){
        *this=addDyadicAndGeneral(b.num,b.log2Den,num,den);
        return *this;
    }
    BigNum lcm=FA::leastCommonMultiple(den,b.den);
    BigNum numNew=FA::addBigNumbers(FA::multiplyBigNumbers(num,lcm/den),
                                    FA::multiplyBigNumbers(b.num,lcm/b.den));
    num=numNew;
    den=lcm;
    normalizeGeneral();
    return *this;
}
template<typename BigNum>
Frac<BigNum>& Frac<BigNum>::operator-=(const Frac<BigNum>& b) {
    Frac<BigNum> mb=b;
    BigNum zero=0;
    mb.num=FA::subBigNumbers(zero,mb.num);
    return operator+=(mb);
}

// ---------------------------------------------------------------- multiplication

template<typename BigNum>
Frac<BigNum>& Frac<BigNum>::operator*=(const Frac<BigNum>& b) {
    BigNum zero=0;
    if(b.num==zero){num=zero;den=1;log2Den=0;return *this;}
    if(num==zero){den=1;log2Den=0;return *this;}
    if( (log2Den>-1) && (b.log2Den>-1) ){
        // pure dyadic path: exponents add, nothing is materialized
        int e=log2Den+b.log2Den;
        num=FA::multiplyBigNumbers(num,b.num);
        log2Den=e;
        normalizeDyadic();
        return *this;
    }
    if( (log2Den>-1) || (b.log2Den>-1) ){
        // one dyadic, one general: cancel twos against the general numerator first,
        // then the result's denominator 2^e * d has to be formed (never happens for
        // the deposited certificate, in which every quantity is dyadic)
        BigNum dyNum, gNum, gDen; int e;
        if(log2Den>-1){ dyNum=num; e=log2Den; gNum=b.num; gDen=b.den; }
        else          { dyNum=b.num; e=b.log2Den; gNum=num; gDen=den; }
        while( (e>0) && (gNum%2==zero) && (gNum!=zero) ){ gNum/=2; --e; }
        BigNum g=FA::gcdBig(dyNum,gDen);
        if(g>1){ dyNum/=g; gDen/=g; }
        BigNum p=FA::powerOfTwo<BigNum>(e);
        num=FA::multiplyBigNumbers(dyNum,gNum);
        den=FA::multiplyBigNumbers(p,gDen);
        log2Den=-1;
        normalizeGeneral();
        return *this;
    }
    BigNum gcdDenBNum=FA::gcdBig(den,b.num);
    BigNum gcdNumBDen=FA::gcdBig(num,b.den);
    BigNum numNew=FA::multiplyBigNumbers(num/gcdNumBDen,b.num/gcdDenBNum);
    BigNum denNew=FA::multiplyBigNumbers(den/gcdDenBNum,b.den/gcdNumBDen);
    num=numNew;
    den=denNew;
    log2Den=-1;
    normalizeGeneral();
    return *this;
}

// ----------------------------------------------------------------- division
template<typename BigNum>
Frac<BigNum> Frac<BigNum>::getReciprocal() const{
    Frac<BigNum> res;
    res.setNum(getDen());
    res.setDen(getNum()); // this could set the flag GL_DIVBY0
    return res;
}
template<typename BigNum>
Frac<BigNum>& Frac<BigNum>::operator/=(const Frac<BigNum>& b){
    return operator*=(b.getReciprocal());
}
template<typename BigNum>
Frac<BigNum> operator+(Frac<BigNum> a, const Frac<BigNum>& b){ a+=b; return a; }
template<typename BigNum>
Frac<BigNum> operator-(Frac<BigNum> a, const Frac<BigNum>& b){ a-=b; return a; }
template<typename BigNum>
Frac<BigNum> operator*(Frac<BigNum> a, const Frac<BigNum>& b){ a*=b; return a; }
template<typename BigNum>
Frac<BigNum> operator/(Frac<BigNum> a, const Frac<BigNum>& b){ a/=b; return a; }

// ---------------------------------------------------------------- output

template<typename BigNum>
std::string Frac<BigNum>::toString() const{
    BigNum zero=0;
    if(num==zero){return "0";}
    std::string sgn="";
    BigNum absNum=FA::absBig(num);
    if(num<zero){sgn="-";}
    std::string numStr=FA::toString(absNum);
    if(log2Den==0){return sgn+numStr;}
    if(log2Den>0){
        return sgn+"\\frac{"+numStr+"}{2^{"+std::to_string(log2Den)+"}}";
    }
    if(den==1){return sgn+numStr;}
    std::string denStr=FA::writeAsPowerOfTwoIfPossible(den);
    if(denStr==""){denStr=FA::toString(den);}
    return sgn+"\\frac{"+numStr+"}{"+denStr+"}";
}
namespace FA{
    // For f>0: the multiple of 2^-K just above f (roundUp=1) or just below it (roundUp=0).
    // Rounding a BOUND in its own safe direction costs at most 2^-K of slack and replaces
    // a possibly huge denominator by 2^K, keeping all later integers small.
    template<typename BigNum>
    Frac<BigNum> dyadicBound(const Frac<BigNum>& f, long K, int roundUp){
        BigNum p=FA::powerOfTwo<BigNum>(K);
        BigNum n=f.getNum();
        BigNum d=f.getDen();
        BigNum scaled=FA::multiplyBigNumbers(n,p);
        BigNum q=scaled/d;
        if(roundUp==1){
            if(FA::multiplyBigNumbers(q,d)!=scaled){q+=1;}
        }
        return Frac<BigNum>(q,p);
    }
}
#endif
