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

#ifndef _INCL_BIJFUNCTION_CPP
#define _INCL_BIJFUNCTION_CPP

namespace BF{
  double GLOBAL_EPSILON=0.000000001;
  struct PermutationSequences{
  public:
    long lenEach=8;
    std::vector<std::vector<long> > indicatorCombinations;
    std::vector<std::vector< std::vector<long> > > pairsPermutations;
  };
  struct PrimeSequences{
  public:
    std::vector<long> primes;
    std::vector<long> mults;
    std::vector<long> inverses;
    std::vector<long> shifts;
    long paddingNumB=1000000000000000;
  };
  PermutationSequences GLOBAL_PERMUTATION_SEQUENCES;
  long GLOBAL_NUM_PERMUTATION_SEQUENCES=10;
  PrimeSequences GLOBAL_PRIME_SEQUENCES;
  long GLOBAL_NUM_PRIME_SEQUENCES=300;
  const char CONST_DECIMAL_SEPARATOR='.';
  const std::string CONST_DECIMAL_SEPARATOR_ST=".";
  const char CONST_THOUSANDS_SEPARATOR=',';
  const std::string CONST_THOUSANDS_SEPARATOR_ST=",";
  int isLetter(const char& c){
    if((c>='a')&&(c<='z')){return 1;}
    if((c>='A')&&(c<='Z')){return 1;}
    return 0;
  }
  int isNumeric(const std::string & s,const long & tolerateSignsInFront = 1){
    long sz=s.length();
    long i=0;
    long numDecPoints=0;
    int res=1;
    if(tolerateSignsInFront){
      while( (i<sz) && ((s[i]=='-')||(s[i]=='+'))){
        ++i;
      }
    }
    while((res==1)&&(i<sz)){
      if(s[i]==CONST_DECIMAL_SEPARATOR){
        ++numDecPoints;
      }
      else{
        if((s[i]!=CONST_THOUSANDS_SEPARATOR)&&((s[i]<'0')||(s[i]>'9'))){
          res=0;
        }
      }
      if(numDecPoints>1){res=0;}
      ++i;
    }
    return res;
  }
  std::string makeItZeroIfNotNumeric(const std::string &s, const long & tolerateSignsInFront = 1){
    if(isNumeric(s,tolerateSignsInFront)){
      return s;
    }
    return "0";
  }
  std::string padded(const long & _x,const long & _d, const std::string &paddingSt=" "){
    // Examples:
    // BF::padded(3,100,"0")=003
    // BF::padded(33,100,"0")=033
    // BF::padded(0,100,"0")=000
    // BF::padded(33,100000,"0")=000033
      long x=_x;
      if(x<0){x*=-1;}
      long d=_d;
      std::string fR="";
      while(d>x){
          fR+=paddingSt;
          d/=10;
      }
      if(x>0){fR+=std::to_string(x);}
      return fR;
  }
  std::string removeASCII10AND13(const std::string &_input,const std::string & _replaceWith=""){
      std::string output;
      long len=_input.length();
      long i=0;
      while(i<len){
          if(( static_cast<long>(_input[i])!=13)&&( static_cast<long>(_input[i])!=10)){
              output+=_input[i];
          }
          else{
            output+=_replaceWith;
          }
          ++i;
      }
      return output;
  }
  std::string cleanSpaces(const std::string & _input, const int & removeAllSpaces=0, const int & remove10And13 = 1){
      // removes double space characters
      // removes leading spaces in string
      std::string input=_input;
      if(remove10And13){input=removeASCII10AND13(_input);}
      std::string output="";
      int spaceBefore=1;
      int spaceNow;
      long len=input.length();
      for(long i=0;i<len;++i){
          spaceNow=0;
          if((input[i]==' ')||(input[i]=='\t')||(input[i]=='\n')||(static_cast<long>(input[i])==13)){
              spaceNow=1;
          }
          if(spaceNow!=1){
              output+=input[i];
              spaceBefore=0;
          }
          else{
              if(spaceBefore!=1){
                  if(removeAllSpaces==0){output+=input[i];}
              }
              spaceBefore=1;
          }
      }
      return output;
  }
  std::string cleanAllSpaces(const std::string &_input){
    return cleanSpaces(_input,1);
  }
  std::string permuteString(const std::string & _in, const std::vector<long> & _p){
    // assumes that _p is the permutation of {0, 1, ..., _in.size()-1}.
    std::string out=_in;
    long psz=_in.size();
    if(_p.size() != psz){return out;}
    for(long i=0;i<psz;++i){
      if(_p[i]<psz){
        out[i]=_in[_p[i]];
      }
    }
    return out;
  }
  long stringToInteger(const std::string &_s){
    std::string t=cleanSpaces(_s,1);
    long sign=1;
    long pos=0;
    long len=t.length();
    if(len<1){
      return 0;
    }
    long fR=0;
      while((pos<len)&&((t[pos]=='+')||(t[pos]=='-'))){
          if(t[pos]=='-'){
              sign *=-1;
          }
          ++pos;
      }
    long zero=static_cast<long>('0');
    while(pos<len){
      if(t[pos]!=CONST_THOUSANDS_SEPARATOR){
        fR*=10;
        fR+=static_cast<long>(t[pos])-zero;
      }
      ++pos;
    }
    fR*=sign;
    return fR;
  }
  std::string removeNonNumericStart(const std::string & _s){
    long sz=_s.length(), i=sz;
    while(i>0 && _s[i-1]>='0' && _s[i-1]<='9'){ --i; }   // i = start of trailing digit run
    if(i==sz){ return ""; }                              // no trailing digit
    long j=i;
    while(j>0 && (_s[j-1]==' '||_s[j-1]=='\t'||_s[j-1]=='\n')){ --j; }  // skip spaces before digits
    if(j>0 && (_s[j-1]=='-'||_s[j-1]=='+')){ return _s[j-1] + _s.substr(i); }
    return _s.substr(i);
  }
  long stringToIntegerRemoveStart(const std::string& _s){
    return stringToInteger(removeNonNumericStart(_s));
  }
  long stringToIntegerRemoveStartAndEnd(const std::string& _s, long maxInt=1000000){
    long res=0;
    long i=0; long sz=_s.length();
    long zero=static_cast<long>('0');
    if(maxInt<1){maxInt=1000000;}
    while((i<sz)&&((_s[i]<'0')||(_s[i]>'9'))){++i;}
    while((i<sz)&&(_s[i]>='0')&&(_s[i]<='9')){
      res*=10;
      res+=static_cast<long>(_s[i])-zero;
      res%=maxInt;
      ++i;
    }
    return res;
  }
  double stringToDouble(const std::string &_s){
    std::string t=cleanSpaces(_s,1);
    double sign=1.0;
    long pos=0;
    long len=t.length();
    if(len<1){
      return 0.0;
    }
    double fR=0.0;
      while((pos<len)&&((t[pos]=='+')||(t[pos]=='-'))){
          if(t[pos]=='-'){
              sign *=-1.0;
          }
          ++pos;
      }
    double zero=static_cast<double>(static_cast<long>('0'));
    while((pos<len)&&(t[pos]!=CONST_DECIMAL_SEPARATOR)){
      if(t[pos]!=CONST_THOUSANDS_SEPARATOR){
        fR*=10.0;
        fR+=static_cast<long>(t[pos])-zero;
      }
      ++pos;
    }
    if(pos<len){
      // We reached the decimal separator
      double decimalMultiplier=0.1;
      ++pos;
      while(pos<len){
        fR+= decimalMultiplier*(static_cast<double>( (t[pos]))-zero);
        decimalMultiplier*=0.1;
        ++pos;
      }
    }
    fR*=sign;
    return fR;
  }
  std::string eraseTrailingZeros(const std::string &st){
    long len=st.length();
    long pos=len-1;int decimalSeparatorFound=0;
    while((pos>-1)&&(st[pos]=='0')){
      --pos;
    }
    if((pos>-1)&&(st[pos]==CONST_DECIMAL_SEPARATOR)){
      --pos;decimalSeparatorFound=1;
    }
    if(pos==-1){
      return "0";
    }
    std::string output="";
    ++pos;
    for(long i=0;i<pos;++i){
      output+=st[i];
      if(st[i]==CONST_DECIMAL_SEPARATOR){decimalSeparatorFound=1;}
    }
    if(decimalSeparatorFound==0){
      return st;
    }
    return output;
  }
  std::string forceDecimals(const double& _d, const long& nD){
    double d=_d;std::string stSign="";
    if(d<0){d=(-1.0)*d;stSign="-";}
    double modified=d;
    for(long i=0;i<nD;++i){
      modified *= 10.0;
    }
    modified *= 10.0;
    long intVal=static_cast<long>(modified);
    long roundUp=0;
    if(intVal%10>4){roundUp=1;}
    intVal/=10;
    intVal+=roundUp;
    std::string result; int dotPrinted=0;
    long i=0;
    if(intVal==0){
      if(nD<1){
        return "0";
      }
      result="0"+CONST_DECIMAL_SEPARATOR_ST;
      for(long i=0;i<nD;++i){
        result+="0";
      }
      return result;
    }
    if(nD<1){
      return stSign+std::to_string(intVal);
    }
    while(intVal>0){
      result = std::to_string(intVal%10)+result;
      ++i;
      if(i==nD){
        result = CONST_DECIMAL_SEPARATOR_ST+result;
        dotPrinted=1;
      }
      intVal/=10;
    }
    while(i<nD){
      result = "0"+result;
      ++i;
    }
    if(dotPrinted==0){
      result= CONST_DECIMAL_SEPARATOR_ST+result;
    }
    if(result[0]==CONST_DECIMAL_SEPARATOR){
      result = "0"+result;
    }
    return stSign+result;
  }
  std::string forceDecimals(const std::string& st, const long& nD){
    return forceDecimals(stringToDouble(st),nD);
  }
  std::string double_to_string_withPrecision(const double & d, const long &p){
    std::ostringstream tmp;
    tmp.precision(p);
    tmp<<std::fixed;
    tmp<<d;
    return tmp.str();
  }
  std::string doubleToString(const double &d,const long &precision=10){
    return eraseTrailingZeros(double_to_string_withPrecision(d,precision));
  }
  long cantorDiag(const long &i, const long &j){
    long z=i+j;
    return z*(z+1)/2+i;
  }
  long oddRootFloor(const long & q){
    long zL=std::sqrt(q);
    if(zL%2==0){
      --zL;
    }
    return zL;
  }
  std::pair<long,long> cantorDiagInv(const long & p){
    std::pair<long, long> fR;
    long q= 8*p+1;
    long z=(oddRootFloor(q)-1)/2;
    fR.first= p-(z*(z+1)/2);
    fR.second=z-fR.first;
    return fR;
  }
  struct AlphaBetaD{
public:
  long alpha=-1;
  long beta=-1;
  long d=-1;
};
AlphaBetaD euclideanAlgorithm(long a, long b){
    // returns res.alpha, res.beta, and res.d such that 
    // res.d=gcd(a,b)
    // res.alpha*a+res.beta*b=gcd(a,b)
    // res.d will be -1 if any of a and be is negative
    // or if both are 0
    AlphaBetaD res;
    if((a<0)||(b<0) ){
        return res;
    } 
    if(a<b){
        if(a==0){
            res.alpha=0;res.beta=1;res.d=b;
            return res;
        }
        res=euclideanAlgorithm(b,a);
        long t=res.alpha;res.alpha=res.beta;res.beta=t;
        return res;
    }
    if(a==b){
        if(b==0){return res;}
        res.d=a;res.alpha=1;res.beta=0; return res;
    }
    if(b==0){
        res.alpha=1;res.beta=0;res.d=a; return res;
    }
    std::stack<long> sQ;
    long r=a%b;long q=a/b;
    while(r!=0){
        sQ.push(q);
        a=b;b=r;r=a%b;q=a/b;
    }
    res.d=b; 
    res.beta=1;res.alpha=0;
    long alphaNew,betaNew;
    while(!sQ.empty()){
        alphaNew=res.beta;
        betaNew=res.alpha-sQ.top()*res.beta;
        res.alpha=alphaNew;res.beta=betaNew;
        sQ.pop();
    }
    return res;
}
long inverse(long a, long modul){
    if((modul<2)||(a<1)){return -1;}
    if(a==1){return 1;}
    AlphaBetaD eA=euclideanAlgorithm(a,modul);
    if(eA.d!=1){return -1;}
    while(eA.alpha<0){eA.alpha+=modul;}
    return eA.alpha;
}
  class BijectiveFunction{
  private:
    long bigPrimeModul;
    long bigPrimeMultiplier;
    long bigShift;
    long invMult;
  public:
    BijectiveFunction(const long& =48773177, const long & =48771253, const long& = 38748171, const long & =39368370);
    void setParameters(const long& =48773177, const long & =48771253, const long& = 38748171, const long & =39368370);
    long eval(const long &) const;
    long inv(const long &) const;
  };
  void BijectiveFunction::setParameters(const long & _bPMo, const long & _bPMu, const long & _bS, const long & _in){
    bigPrimeModul=_bPMo;
    bigPrimeMultiplier=_bPMu;
    bigShift=_bS;
    if(_in<1){
      invMult=inverse(bigPrimeMultiplier,bigPrimeModul);
    }
    else{
      invMult=_in;
    }
  }
  BijectiveFunction::BijectiveFunction(const long & _bPMo, const long & _bPMu, const long & _bS, const long & _in){
    setParameters(_bPMo,_bPMu,_bS,_in);
  }
  long BijectiveFunction::eval(const long & x) const{
    return (x* bigPrimeMultiplier+bigShift)%bigPrimeModul;
  }
  long BijectiveFunction::inv(const long & a) const{
    long a1=a-bigShift;
    if(a1<0){
      a1 += (-a1)*bigPrimeModul;
      a1 %=bigPrimeModul;
    }
    return (a1*invMult)%bigPrimeModul;
  }
  std::pair<long,long> getTwoNumbers(const std::vector<long> & indicators01, const std::vector<std::vector<long> > & p01, const std::string &pass){
    std::pair<long,long> fR;
    std::vector<std::string> twoNums;
    twoNums.resize(2);
    long len0,len1,len;
    len0=p01[0].size();
    len1=p01[1].size();
    len=indicators01.size();
    for(long i=0;i<len0;++i){
      twoNums[0]+="z";
    }
    for(long i=0;i<len1;++i){
      twoNums[1]+="z";
    }
    std::vector<long> counters;
    counters.resize(2);counters[0]=0;counters[1]=0;
    long zoo;
    for(long i=0;i<len;++i){
      zoo=indicators01[i];
      (twoNums[zoo])[(p01[zoo])[counters[zoo]]]=pass[i];
      ++counters[zoo];
    }
    fR.first=stringToInteger(twoNums[0]);
    fR.second=stringToInteger(twoNums[1]);
    return fR;
  }
  std::vector<long> plainSeqFromString(const std::string & input, const std::vector<long> & indCombs,
                                       const std::vector<std::vector<long> > & permuts01,
                                       const BijectiveFunction &f0, const BijectiveFunction &f1,
                                       const long & shift,
                                       const long & indSimple){
    std::vector<long> fR;
    long sz=input.length();
    long lenEach=permuts01[0].size();
    if(sz%(2*lenEach)!=0){
      fR.resize(1);
      fR[0]=-1;
      return fR;
    }
    long lenEach2=2*lenEach;
    long numPairs=sz/(lenEach2);
    fR.resize(numPairs);
    std::string tempSt;
    long tempStStart;
    std::pair<long,long> pairNums;
    long x0,x1;
    for(long i=0;i<numPairs;++i){
      tempStStart= i*(lenEach2);
      tempSt="";
      for(long j=0;j<lenEach2;++j){
        tempSt+="z";
      }
      for(long j=0;j<lenEach2;++j){
        tempSt[j]=input[tempStStart+j];
      }
      if(indSimple==1){
        fR[i]=stringToInteger(tempSt);
      }
      else{
        pairNums=getTwoNumbers(indCombs, permuts01, tempSt);
        x0=pairNums.first;
        x1=pairNums.second;
        x0=f0.inv(pairNums.first);
        x1=f1.inv(pairNums.second);
        fR[i]=cantorDiag(x0,x1)-shift;
      }
    }
    return fR;
  }
  std::string combineTwoNumbers(const std::vector<long> & indicators01, const std::vector<std::vector<long> > & p01, const long & x, const long & y){
    std::string fR="";
    long len0=(p01[0]).size();
    long len1=(p01[1]).size();
    long pad0=1,pad1=1;
    for(long i=0;i<len0;++i){pad0*=10;}
    if(pad0>10){pad0/=10;}
    for(long i=0;i<len1;++i){pad1*=10;}
    if(pad1>10){pad1/=10;}
    std::vector<std::string> pXY;
    pXY.resize(2);
    pXY[0]=padded(x,pad0,"0");
    pXY[1]=padded(y,pad1,"0");
    long len=indicators01.size();
    std::vector<long> counters;
    counters.resize(2);counters[0]=0;counters[1]=0;
    long zoo;
    for(long i=0;i<len;++i){
      zoo=indicators01[i];
      fR+=  (pXY[zoo])[(p01[zoo])[counters[zoo]]] ;
      ++counters[zoo];
    }
    return fR;
  }
  std::string getCodeWord(const std::vector<long> & v, const std::vector<long> & indCombs,
    const std::vector<std::vector<long> > & permuts01,
    const BijectiveFunction &f0, const BijectiveFunction &f1,
    const long &shift, const long & indSimple){
    if((v.size()==1)&&(v[0]==-1)){
      return "notFound";
    }
    std::string fR;
    long numPairs=v.size();
    std::string tempSt;
    long x0,x1;
    std::pair<long,long> pNums;
    for(long i=0;i<numPairs;++i){
      if(indSimple==1){
        tempSt=padded(v[i],GLOBAL_PRIME_SEQUENCES.paddingNumB,"0");
      }
      else{
        pNums=cantorDiagInv(v[i]+shift);
        x0=f0.eval(pNums.first);
        x1=f1.eval(pNums.second);
        tempSt=combineTwoNumbers(indCombs, permuts01, x0, x1);
      }
      fR+=tempSt;
    }
    return fR;
  }
  template<typename TTT>
  std::vector<std::vector<TTT> > transposeMatrix(const std::vector<std::vector<TTT> > &v){
    std::vector<std::vector<TTT> > vt,emptyVect;
    long vsz=v.size();
    if(vsz<1){
      return emptyVect;
    }
    long vtsz=v[0].size();
    if(vtsz<1){
      return emptyVect;
    }
    vt.resize(vtsz);
    for(long j=0;j<vtsz;++j){
      vt[j].resize(vsz);
    }
    for(long i=0;i<vsz;++i){
      if(v[i].size()!=vtsz){
        return emptyVect;
      }
      for(long j=0;j<vtsz;++j){
        vt[j][i]=v[i][j];
      }
    }
    return vt;
  }
template<typename T1, typename T2>
std::map<T2, T1> inverse(const std::map<T1, T2>& in) {
    std::map<T2, T1> out;
    for (const auto& [key, val] : in)
        out[val] = key;
    return out;
}
}
#endif
