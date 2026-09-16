//*************************************************************************************************
//*************************************************************************************************
//* The MIT License (MIT)                                                                         *
//* Copyright (C) 2026                                                                            *
//* Ivan Matic, Rados Radoicic, and Andreas Spomer                                                *
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

#ifndef _INCL_SOSC_CPP
#define _INCL_SOSC_CPP
namespace SOSC{

    // Sanity bounds on the deposited small integers. They keep a malformed certificate from
    // asking the program for absurd amounts of work; every real certificate is far below them.
    const long GL_maxExponentK=64;
    const long GL_maxKappaS=440; 
    const long GL_maxGramSize=5000;
    // Absolute ceiling on decimal digits per entry; the effective bound is the smaller of this
    // and numeric_limits<BigNum>::digits10 (76 for Int256, 153 for Int512), so that an entry
    // too wide for the selected word is rejected by the parser with a message instead of
    // overflowing inside it.
    const long GL_maxDigits=200;
    template<typename BigNum>
    long maxDigitsFor(){
        long d=GL_maxDigits;
        if(std::numeric_limits<BigNum>::is_bounded){
            long w=std::numeric_limits<BigNum>::digits10;
            if((w>0)&&(w<d)){d=w;}
        }
        return d;
    }

    const long GL_numBlocks=4;

    std::string blockName(const std::string& stem, long a){
        return stem+"000"+std::to_string(a);
    }
    int hasRecord(const std::map<std::string,std::string>& mainMap, const std::string& key){
        return (mainMap.find(key)!=mainMap.end())?1:0;
    }
    std::vector<std::string> allSOSRecordNames(){
        std::vector<std::string> res;
        res.push_back("sosExponentK");
        res.push_back("sosDenLog2");
        for(long a=0;a<GL_numBlocks;++a){res.push_back(blockName("sosGram",a));}
        for(long a=0;a<GL_numBlocks;++a){res.push_back(blockName("sosSwit",a));}
        for(long a=0;a<GL_numBlocks;++a){res.push_back(blockName("sosWwit",a));}
        res.push_back("UhPoly");
        res.push_back("QhPoly");
        return res;
    }
    int isSOSCertificate(const std::map<std::string,std::string>& mainMap){
        std::vector<std::string> names=allSOSRecordNames();
        for(long i=0;i<names.size();++i){
            if(hasRecord(mainMap,names[i])){return 1;}
        }
        return 0;
    }
    template<typename BigNum>
    BigNum bigFromString(const std::string& _in, int* a_ok){
        BigNum res=0;
        BigNum ten=10;
        long pos=0;
        long len=_in.length();
        int neg=0;
        long digits=0;
        long maxDigits=maxDigitsFor<BigNum>();
        while((pos<len)&&((_in[pos]==' ')||(_in[pos]=='\n')||(_in[pos]=='\t')||(_in[pos]=='\r'))){++pos;}
        if((pos<len)&&((_in[pos]=='-')||(_in[pos]=='+'))){
            if(_in[pos]=='-'){neg=1;}
            ++pos;
        }
        while((pos<len)&&(_in[pos]>='0')&&(_in[pos]<='9')){
            ++digits;            
            if(digits>maxDigits){*a_ok=0;return 0;}
            BigNum digit=_in[pos]-'0';
            res=FA::addBigNumbers(FA::multiplyBigNumbers(res,ten),digit);
            ++pos;
        }
        if(digits==0){*a_ok=0;return 0;}
        while((pos<len)&&((_in[pos]==' ')||(_in[pos]=='\n')||(_in[pos]=='\t')||(_in[pos]=='\r'))){++pos;}
        if(pos!=len){*a_ok=0;return 0;}
        if(neg){BigNum zero=0;res=FA::subBigNumbers(zero,res);}
        return res;
    }
    template<typename BigNum>
    std::vector<std::vector<BigNum> > matrixBigFromString(const std::string& _in, int* a_ok){
        std::vector<std::string> rows=SF::stringToVector(_in,CPC::GL_openTag,CPC::GL_closeTag);
        std::vector<std::vector<BigNum> > res;
        res.resize(rows.size());
        for(long i=0;i<rows.size();++i){
            std::vector<std::string> entries=SF::stringToVector(rows[i],CPC::GL_openTag,CPC::GL_closeTag);
            res[i].resize(entries.size());
            for(long j=0;j<entries.size();++j){
                res[i][j]=bigFromString<BigNum>(entries[j],a_ok);
                if(*a_ok==0){return res;}
            }
        }
        return res;
    }
 
    template<typename BigNum>
    long smallNumberFromRecord(const std::map<std::string,std::string>& mainMap,
                               const std::string& key, int* a_ok){
        std::map<std::string,std::string>::const_iterator it=mainMap.find(key);
        if(it==mainMap.end()){*a_ok=0;return 0;}
        std::vector<std::vector<BigNum> > m=matrixBigFromString<BigNum>(it->second,a_ok);
        if(*a_ok==0){return 0;}
        if(m.size()!=1){*a_ok=0;return 0;}
        if(m[0].size()!=1){*a_ok=0;return 0;}
        BigNum v=m[0][0];
        BigNum bound=1000000000;
        if(FA::absBig(v)>bound){*a_ok=0;return 0;}
        return static_cast<long>(v);
    }

    // ---------------------------------------------------------------- basis and expansion

    long numberOfMonomials(long d){
        // n_d = binom(d+3,3), the number of monomials of degree d in q_0..q_3
        if(d<0){return 0;}
        return ((d+1)*(d+2)*(d+3))/6;
    }
    // The fixed shared convention with the generator: exponent vectors of degree d, ordered
    // lexicographically DESCENDING with e0 outermost:
    //   (d,0,0,0), (d-1,1,0,0), (d-1,0,1,0), (d-1,0,0,1), (d-2,2,0,0), ...
    // A mismatch here can only fail a valid certificate, never pass an invalid one.
    template<typename BigNum>
    std::vector<PA::Monomial<Frac<BigNum> > > monomialBasis(long d){
        std::vector<PA::Monomial<Frac<BigNum> > > res;
        Frac<BigNum> one(1,1);
        for(long e0=d;e0>=0;--e0){
            for(long e1=d-e0;e1>=0;--e1){
                for(long e2=d-e0-e1;e2>=0;--e2){
                    long e[4];
                    e[0]=e0;e[1]=e1;e[2]=e2;e[3]=d-e0-e1-e2;
                    PA::Monomial<Frac<BigNum> > m(one);
                    for(long v=0;v<4;++v){
                        if(e[v]>0){
                            PA::Monomial<Frac<BigNum> > factor(one,"q_"+std::to_string(v),e[v]);
                            m*=factor;
                        }
                    }
                    res.push_back(m);
                }
            }
        }
        return res;
    }
    // (S2) 2^{kappa_s} sigma_a = sum_{i<=j} (2 - delta_ij) X_ij m_i m_j.
    template<typename BigNum>
    PA::Polynomial<Frac<BigNum> > gramToPolynomial(const std::vector<std::vector<BigNum> >& X,
                                                   const std::vector<PA::Monomial<Frac<BigNum> > >& basis){
        PA::Polynomial<Frac<BigNum> > res;
        BigNum zero=0;
        long n=basis.size();
        for(long i=0;i<n;++i){
            for(long j=i;j<n;++j){
                if(X[i][j]==zero){continue;}
                BigNum c=X[i][j];
                if(i!=j){c=FA::addBigNumbers(c,c);}
                PA::Monomial<Frac<BigNum> > m=basis[i];
                m*=basis[j];
                m.c=Frac<BigNum>(c,1);
                res+=m;
            }
        }
        return res;
    }
    // UhPoly / QhPoly rows: { {0}{0}{e0}{e1}{e2}{e3}{coeff} } with integer coefficients.
    template<typename BigNum>
    PA::Polynomial<Frac<BigNum> > polynomialFromRows(const std::string& _in, int* a_ok){
        PA::Polynomial<Frac<BigNum> > res;
        std::vector<std::vector<BigNum> > rows=matrixBigFromString<BigNum>(_in,a_ok);
        if(*a_ok==0){return res;}
        Frac<BigNum> one(1,1);
        for(long r=0;r<rows.size();++r){
            if(rows[r].size()!=7){*a_ok=0;return res;}
            if((rows[r][0]!=0)||(rows[r][1]!=0)){*a_ok=0;return res;}
            PA::Monomial<Frac<BigNum> > m(Frac<BigNum>(rows[r][6],1));
            for(long v=0;v<4;++v){
                BigNum e=rows[r][2+v];
                if((e<0)||(e>126)){*a_ok=0;return res;}
                long eL=static_cast<long>(e);
                if(eL>0){
                    PA::Monomial<Frac<BigNum> > factor(one,"q_"+std::to_string(v),eL);
                    m*=factor;
                }
            }
            res+=m;
        }
        return res;
    }
    template<typename BigNum>
    int isIntegerFrac(const Frac<BigNum>& x){
        // canonical dyadic with exponent 0 is exactly the integer case; general mode has den>=3
        if(!x.isDyadic()){return 0;}
        return (x.getLog2Den()==0)?1:0;
    }
    std::string exponentsOfMonomial(uint64_t exps8){
        std::string res="q^(";
        for(long i=0;i<4;++i){
            if(i>0){res+=",";}
            res+=std::to_string(exps8%256);
            exps8/=256;
        }
        return res+")";
    }

    // ---------------------------------------------------------------- the SOS stage
    //
    // Returns 1 only if (S0), (S1), (S2)+(S3) and V:gens all hold for the deposited data.
    template<typename BigNum>
    int verifyCertificate(const std::map<std::string,std::string>& mainMap,
                          const CPC::TestSetup<BigNum>& ts){
        std::vector<std::string> names=allSOSRecordNames();
        for(long i=0;i<names.size();++i){
            if(!hasRecord(mainMap,names[i])){
                std::cout<<"SOS check failed: record "<<names[i]<<" is missing. This file is not a\n";
                std::cout<<"complete SOS certificate; it must be re-emitted by the generator, not patched.\n";
                return 0;
            }
        }
        long L=ts.LMax;
        int okScalar=1;
        long k=smallNumberFromRecord<BigNum>(mainMap,"sosExponentK",&okScalar);
        long kappaS=smallNumberFromRecord<BigNum>(mainMap,"sosDenLog2",&okScalar);
        if(okScalar==0){
            std::cout<<"SOS check failed: sosExponentK or sosDenLog2 is not a single integer\n";
            return 0;
        }
        if((k<0)||(k>GL_maxExponentK)){
            std::cout<<"SOS check failed: sosExponentK out of range\n";
            return 0;
        }
        if(L+k<4){
            std::cout<<"SOS check failed: L+k must be at least 4 so that the basis of Sigma_3 exists\n";
            return 0;
        }
        if((kappaS<0)||(kappaS>GL_maxKappaS)||(kappaS%2)){
            std::cout<<"SOS check failed: sosDenLog2 must be a nonnegative even integer, at most "<<GL_maxKappaS<<"\n";
            return 0;
        }
        long degreeOfBlock[GL_numBlocks];
        degreeOfBlock[0]=L+k;
        degreeOfBlock[1]=L+k-2;
        degreeOfBlock[2]=L+k-2;
        degreeOfBlock[3]=L+k-4;

        // ---- (S0) shape and symmetry, sizes computed from the deposited L and k ----
        std::vector<std::vector<BigNum> > X[GL_numBlocks];
        std::vector<std::vector<BigNum> > W[GL_numBlocks];
        long S[GL_numBlocks];
        for(long a=0;a<GL_numBlocks;++a){
            long n=numberOfMonomials(degreeOfBlock[a]);
            if((n<1)||(n>GL_maxGramSize)){
                std::cout<<"SOS check failed: implausible Gram size for block "<<a<<"\n";
                return 0;
            }
            int ok=1;
            X[a]=matrixBigFromString<BigNum>(mainMap.find(blockName("sosGram",a))->second,&ok);
            if(ok==0){
                std::cout<<"SOS check failed: "<<blockName("sosGram",a)<<" contains an entry that is not an integer\n";
                return 0;
            }
            W[a]=matrixBigFromString<BigNum>(mainMap.find(blockName("sosWwit",a))->second,&ok);
            if(ok==0){
                std::cout<<"SOS check failed: "<<blockName("sosWwit",a)<<" contains an entry that is not an integer\n";
                return 0;
            }
            if(X[a].size()!=n){
                std::cout<<"(S0) failed: "<<blockName("sosGram",a)<<" has "<<X[a].size();
                std::cout<<" rows, expected "<<n<<"\n";
                return 0;
            }
            if(W[a].size()!=n){
                std::cout<<"(S0) failed: "<<blockName("sosWwit",a)<<" has "<<W[a].size();
                std::cout<<" rows, expected "<<n<<"\n";
                return 0;
            }
            for(long i=0;i<n;++i){
                if((X[a][i].size()!=n)||(W[a][i].size()!=n)){
                    std::cout<<"(S0) failed: block "<<a<<" has a row of the wrong length\n";
                    return 0;
                }
            }
            for(long i=0;i<n;++i){
                for(long j=i+1;j<n;++j){
                    if(!(X[a][i][j]==X[a][j][i])){
                        std::cout<<"(S0) failed: "<<blockName("sosGram",a)<<" is not symmetric at (";
                        std::cout<<i<<","<<j<<")\n";
                        return 0;
                    }
                }
            }
            S[a]=smallNumberFromRecord<BigNum>(mainMap,blockName("sosSwit",a),&ok);
            if(ok==0){
                std::cout<<"SOS check failed: "<<blockName("sosSwit",a)<<" is not a single integer\n";
                return 0;
            }
            if(S[a]<1){
                std::cout<<"(S1) failed: "<<blockName("sosSwit",a)<<" must be at least 1\n";
                return 0;
            }
        }
        std::cout<<"(S0) shapes and symmetry verified: Gram sizes ";
        for(long a=0;a<GL_numBlocks;++a){std::cout<<X[a].size()<<((a<GL_numBlocks-1)?"/":"");}
        std::cout<<" for k="<<k<<", kappa_s="<<kappaS<<"\n";

        // ---- (S1) PSD, by CPC::checkPsdWitness verbatim ----
        // That function (and the matrix product it calls) uses raw operators on the checked
        // multiprecision type, which THROW std::overflow_error instead of raising FA::GL_OVERFLOW.
        // Catch here, raise the flag, and fail with a message; the vetted function stays untouched
        // and the non-throwing path costs nothing.
        for(long a=0;a<GL_numBlocks;++a){
            int psdOK=0;
            try{ psdOK=CPC::checkPsdWitness(X[a],W[a],S[a]); }
            catch(const std::overflow_error&){ FA::GL_OVERFLOW=1; psdOK=0; }
            if(FA::GL_OVERFLOW){
                std::cout<<"(S1) failed: arithmetic overflow while checking the witness for block "<<a;
                std::cout<<" (entries too large for the selected word width)\n";
                return 0;
            }
            if(!psdOK){
                std::cout<<"(S1) failed: the witness for block "<<a<<" does not certify that Sigma_";
                std::cout<<a<<" is positive semidefinite\n";
                return 0;
            }
        }
        std::cout<<"(S1) all four Gram matrices certified positive semidefinite\n";

        // ---- V:gens: deposited generators equal the constructed ones ----
        {
            int ok=1;
            PA::Polynomial<Frac<BigNum> > depositedUh=polynomialFromRows<BigNum>(mainMap.find("UhPoly")->second,&ok);
            PA::Polynomial<Frac<BigNum> > depositedQh=polynomialFromRows<BigNum>(mainMap.find("QhPoly")->second,&ok);
            if(ok==0){
                std::cout<<"V:gens failed: UhPoly/QhPoly rows are malformed\n";
                return 0;
            }
            if(PA::equal(depositedUh,ts.pol_Uh)==0){
                std::cout<<"V:gens failed: deposited UhPoly differs from the U_h constructed from tauSq\n";
                return 0;
            }
            if(PA::equal(depositedQh,ts.pol_Qh)==0){
                std::cout<<"V:gens failed: deposited QhPoly differs from the Q_h constructed from tauSq\n";
                return 0;
            }
        }
        std::cout<<"V:gens verified: deposited generators equal the constructed U_h, Q_h\n";

        // ---- left-hand side: 2^{kappa_s} (|q|^2)^k G, with G = pol_G / den(M) ----
        // pol_G is den(M)*G with den(M) the REDUCED denominator of M; the raw MDen record of the
        // file is not what createG used and must not be used here.
        PA::Polynomial<Frac<BigNum> > lhs=ts.pol_G;
        PA::Polynomial<Frac<BigNum> > qSq=CPC::createQSquare<BigNum>();
        for(long i=0;i<k;++i){lhs*=qSq;}
        {
            Frac<BigNum> scale(FA::powerOfTwo<BigNum>(kappaS),ts.M.getDen());
            lhs*=scale;
        }
        for(long i=0;i<lhs.terms.size();++i){
            if(!isIntegerFrac(lhs.terms[i].c)){
                std::cout<<"SOS check failed: 2^kappa_s (|q|^2)^k G is not an integer polynomial";
                std::cout<<" (sosDenLog2 too small)\n";
                return 0;
            }
        }

        // ---- right-hand side: sum_a [2^{kappa_s} sigma_a] * gen_a ----
        PA::Polynomial<Frac<BigNum> > gen[GL_numBlocks];
        {
            Frac<BigNum> one(1,1);
            PA::Monomial<Frac<BigNum> > oneM(one);
            gen[0]+=oneM;
            gen[1]=ts.pol_Uh;
            gen[2]=ts.pol_Qh;
            gen[3]=ts.pol_Uh;
            gen[3]*=ts.pol_Qh;
        }
        PA::Polynomial<Frac<BigNum> > rhs;
        for(long a=0;a<GL_numBlocks;++a){
            std::vector<PA::Monomial<Frac<BigNum> > > basis=monomialBasis<BigNum>(degreeOfBlock[a]);
            if(basis.size()!=X[a].size()){
                std::cout<<"(S2) failed: internal basis size does not match block "<<a<<"\n";
                return 0;
            }
            PA::Polynomial<Frac<BigNum> > part=gramToPolynomial(X[a],basis);
            if(a>0){part*=gen[a];}
            rhs+=part;
        }

        // ---- (S3) the identity, coefficient by coefficient ----
        long numEquations=numberOfMonomials(2*L+2*k);
        PA::Polynomial<Frac<BigNum> > diff=lhs;
        diff-=rhs;
        if(diff.terms.size()!=0){
            std::cout<<"(S3) failed: the identity is violated at "<<diff.terms.size()<<" of ";
            std::cout<<numEquations<<" coefficients, first at "<<exponentsOfMonomial(diff.terms[0].exps8)<<"\n";
            return 0;
        }
        if(FA::GL_OVERFLOW||FA::GL_DIVBY0||PA::GL_errorWithMonomials){
            std::cout<<"SOS check failed: an arithmetic flag was raised during the SOS stage\n";
            return 0;
        }
        std::cout<<"(S2)-(S3) identity 2^kappa_s (|q|^2)^k G = Sigma_0 + Sigma_1 U_h + Sigma_2 Q_h";
        std::cout<<" + Sigma_3 U_h Q_h verified exactly ("<<numEquations<<" coefficient equations)\n";
        std::cout<<"Hypothesis (H6) is proved: G >= 0 on K_h.\n";
        return 1;
    }
    template<typename BigNum>
    std::string printSOSSummary(const std::map<std::string,std::string>& mainMap,
                                const CPC::TestSetup<BigNum>& ts){
        int okScalar=1;
        long k=smallNumberFromRecord<BigNum>(mainMap,"sosExponentK",&okScalar);
        long kappaS=smallNumberFromRecord<BigNum>(mainMap,"sosDenLog2",&okScalar);
        std::string res;
        res+="\\section{Sum of squares certificate}\n";
        res+="The hypothesis $G\\ge 0$ on $K_h$ is established by the identity\n";
        res+="\\[2^{"+std::to_string(kappaS)+"}\\left(|q|^2\\right)^{"+std::to_string(k)+"}G=";
        res+="\\Sigma_0+\\Sigma_1U_h+\\Sigma_2Q_h+\\Sigma_3U_hQ_h,\\]\n";
        res+="where each $\\Sigma_a$ is a positive semidefinite quadratic form in the monomials of\n";
        res+="degree $L+k-\\deg(\\mathrm{gen}_a)/2$, deposited as an integer Gram matrix together with\n";
        res+="an integer witness of positive semidefiniteness. The identity was verified over the\n";
        res+="integers, coefficient by coefficient (";
        res+=std::to_string(numberOfMonomials(2*ts.LMax+2*k))+" equations).\n";
        return res;
    }
}

#endif
