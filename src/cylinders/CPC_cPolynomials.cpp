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


#ifndef _INCL_CPC_CPP
#define _INCL_CPC_CPP
namespace CPC{
    long GL_skipMultVerification=0;
    long GL_skipOrthVerification=0;
    long GL_skipTxtOutput=0;
    long GL_stringSizeLimit=1000;
    std::string GL_openTag="{";
    std::string GL_closeTag="}";
    template<typename TT>
    std::vector<TT> getColumn(const std::vector<std::vector<TT> > & A, long j){
        std::vector<TT> res;
        long numRows=A.size();
        if(numRows<1){return res;}
        long numCols=A[0].size();
        if(numCols<1){return res;}
        if((j<0)||(j>=numCols)){return res;}
        res.resize(numRows);
        for(long i=0;i<numRows;++i){
            res[i]=A[i][j];
        }
        return res;
    }
    template<typename TT>
    std::vector<std::vector<TT> > transpose(const std::vector<std::vector<TT> >& A){
        std::vector<std::vector<TT> > AT;
        long numRows=A.size();
        if(numRows<1){return AT;}
        long numCols=A[0].size();
        if(numCols<1){return AT;}
        AT.resize(numCols);
        for(long j=0;j<numCols;++j){
            AT[j]=getColumn(A,j);
        }
        return AT;
    }
template<typename BigNum>
std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > createR(){
    std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > res;
    res.resize(3);
    std::vector<PA::Polynomial<Frac<BigNum> > > row;
    row.resize(3);
    Frac<BigNum> one(1,1);
    Frac<BigNum> two(2,1);
    PA::Monomial<Frac<BigNum> > q0(one,"q_0",1);
    PA::Monomial<Frac<BigNum> > q1(one,"q_1",1);
    PA::Monomial<Frac<BigNum> > q2(one,"q_2",1);
    PA::Monomial<Frac<BigNum> > q3(one,"q_3",1);
    PA::Monomial<Frac<BigNum> > q0Sq=q0; q0Sq*=q0;
    PA::Monomial<Frac<BigNum> > q1Sq=q1; q1Sq*=q1;
    PA::Monomial<Frac<BigNum> > q2Sq=q2; q2Sq*=q2;
    PA::Monomial<Frac<BigNum> > q3Sq=q3; q3Sq*=q3;
    PA::Monomial<Frac<BigNum> > q0q1=q0; q0q1*=q1;
    PA::Monomial<Frac<BigNum> > q0q2=q0; q0q2*=q2;
    PA::Monomial<Frac<BigNum> > q0q3=q0; q0q3*=q3;
    PA::Monomial<Frac<BigNum> > q1q2=q1; q1q2*=q2;
    PA::Monomial<Frac<BigNum> > q1q3=q1; q1q3*=q3;
    PA::Monomial<Frac<BigNum> > q2q3=q2; q2q3*=q3;
    PA::Polynomial<Frac<BigNum> > tmp;
    tmp.terms.clear();tmp+=q0Sq;tmp+=q1Sq;tmp-=q2Sq;tmp-=q3Sq;row[0]=tmp;
    tmp.terms.clear();tmp+=q1q2;tmp-=q0q3;tmp*=two;row[1]=tmp;
    tmp.terms.clear();tmp+=q1q3;tmp+=q0q2;tmp*=two;row[2]=tmp;
    res[0]=row;
    tmp.terms.clear();tmp+=q1q2;tmp+=q0q3;tmp*=two;row[0]=tmp;
    tmp.terms.clear();tmp+=q0Sq;tmp-=q1Sq;tmp+=q2Sq;tmp-=q3Sq;row[1]=tmp;
    tmp.terms.clear();tmp+=q2q3;tmp-=q0q1;tmp*=two;row[2]=tmp;
    res[1]=row;
    tmp.terms.clear();tmp+=q1q3;tmp-=q0q2;tmp*=two;row[0]=tmp;
    tmp.terms.clear();tmp+=q2q3;tmp+=q0q1;tmp*=two;row[1]=tmp;
    tmp.terms.clear();tmp+=q0Sq;tmp-=q1Sq;tmp-=q2Sq;tmp+=q3Sq;row[2]=tmp;
    res[2]=row;
    return res;
}
template<typename TT>
TT dotProduct(const std::vector<TT>& a,const std::vector<TT>& b){
    TT defaultT;
    if((a.size()<1)||(a.size()!=b.size())){return defaultT;}
    TT result=a[0];
    result*=b[0];
    for(long i=1;i<a.size();++i){
        TT intermediate=a[i];
        intermediate*=b[i];
        result+=intermediate;
    }
    return result;
}
template<typename TT>
std::vector<TT> crossProduct(const std::vector<TT>& a, const std::vector<TT>& b){
    std::vector<TT> res;
    if( (a.size()!=3)||(b.size()!=3) ){return res;}
    res.resize(3);
    {
        res[0]=a[1]; res[0]*=b[2];
        TT oth=a[2]; oth*=b[1];
        res[0]-=oth;
    }
    {
        res[1]=a[2]; res[1]*=b[0];
        TT oth=a[0]; oth*=b[2];
        res[1]-=oth;
    }
    {
        res[2]=a[0]; res[2]*=b[1];
        TT oth=a[1]; oth*=b[0];
        res[2]-=oth;
    }
    return res;
}
}
template<typename TT>
std::vector<TT > operator*(const std::vector<std::vector<TT> >& M, const std::vector<TT>& col){
    std::vector<TT> result;
    long numRows=M.size();
    if(numRows<1){return result;}
    long numCols=M[0].size();
    if(numCols<1){return result;}
    if(numCols!=col.size()){return result;}
    result.resize(numRows);
    for(long i=0;i<numRows;++i){
        result[i]=CPC::dotProduct(M[i],col);
    }
    return result;
}
template<typename TT>
std::vector<TT> operator-(const std::vector<TT>& a, const std::vector<TT>& b){
    std::vector<TT> res;
    if((a.size()<1)||(a.size()!=b.size())){return res;}
    res=a;
    for(long i=0;i<b.size();++i){res[i]-=b[i];}
    return res;
}
template<typename TT>
std::vector<std::vector<TT > >operator*(const std::vector<std::vector<TT> >& A,
                                        const std::vector<std::vector<TT> >& B){
    std::vector<std::vector<TT > > ABT;
    long numRowsA=A.size();
    if(numRowsA<1){return ABT;}
    long numColsA=A[0].size();
    if(numColsA<1){return ABT;}
    long numRowsB=B.size();
    if(numColsA!=numRowsB){return ABT;}
    long numColsB=B[0].size();
    ABT.resize(numColsB);
    for(long i=0;i<numColsB;++i){
        ABT[i]=A*CPC::getColumn(B,i);
    }
    return CPC::transpose(ABT);
}

namespace CPC{
    template<typename BigNum>
    PA::Polynomial<Frac<BigNum> > createQSquare(){
        PA::Polynomial<Frac<BigNum> > res;
        Frac<BigNum> one(1,1);
        for(long i=0;i<4;++i){
            PA::Monomial<Frac<BigNum> > tmp(one,"q_"+std::to_string(i),1);
            tmp*=tmp;
            res+=tmp;
        }
        return res;
    }
    template<typename BigNum>
    struct Components{
    public:
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > paper_R;
        PA::Polynomial<Frac<BigNum> > qSq;
        PA::Polynomial<Frac<BigNum> > H_h;
        PA::Polynomial<Frac<BigNum> > Q_part1;
        PA::Polynomial<Frac<BigNum> > Q_part2;
        PA::Polynomial<Frac<BigNum> > paper_Uh;
        PA::Polynomial<Frac<BigNum> > paper_Qh;
        Components();
        void updateUhQh(BigNum , BigNum );
    };
    template<typename BigNum>
    Components<BigNum>::Components(){
        qSq=createQSquare<BigNum>();
        paper_R=CPC::createR<BigNum>();
        Frac<BigNum> one(1,1);
        Frac<BigNum> four(4,1);
        
        {
            H_h=paper_R[2][2];
            H_h-=qSq;
            H_h*=H_h;
            PA::Polynomial<Frac<BigNum> > tmp=paper_R[1][2];
            tmp*=paper_R[1][2];
            H_h+=tmp;
        }
        {   PA::Polynomial<Frac<BigNum> > q0Sq;
            
            PA::Monomial<Frac<BigNum> > tmp0(one,"q_0",2);
            q0Sq+=tmp0;
            PA::Polynomial<Frac<BigNum> > q1Sq;
            PA::Monomial<Frac<BigNum> > tmp1(one,"q_1",2);
            q1Sq+=tmp1;
            PA::Polynomial<Frac<BigNum> > q2Sq;
            PA::Monomial<Frac<BigNum> > tmp2(one,"q_2",2);
            q2Sq+=tmp2;
            PA::Polynomial<Frac<BigNum> > q3Sq;
            PA::Monomial<Frac<BigNum> > tmp3(one,"q_3",2);
            q3Sq+=tmp3;
            Q_part1=q1Sq;
            Q_part1*=q2Sq;
            Q_part1*=four;
            Q_part2=q0Sq;
            Q_part2+=q1Sq;
            PA::Polynomial<Frac<BigNum> > Q_part22=q2Sq;
            Q_part22+=q3Sq;
            Q_part2*=Q_part22;
        }
    }
template<typename BigNum>
void Components<BigNum>::updateUhQh(BigNum A, BigNum B){
    PA::Polynomial<Frac<BigNum> > q4=qSq;
    q4*=q4;
    paper_Uh=H_h;
    paper_Uh*=B;
    PA::Polynomial<Frac<BigNum> > tmp=q4;
    tmp*=A;
    paper_Uh-=tmp;
    paper_Qh=Q_part1;
    paper_Qh*=B;
    tmp=Q_part2;
    tmp*=A;
    paper_Qh-=tmp;
}
template<typename BigNum>
    PA::Polynomial<Frac<BigNum> > createP(const std::vector<std::vector<std::vector<Frac<BigNum> > > >& array_C,
                                   const std::vector<std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > >& array_TPolynomials,
                                   const std::vector<std::vector<Frac<BigNum> > >& array_invWDiagonals,
                                 long L){
        PA::Polynomial<Frac<BigNum> > res;
        PA::Polynomial<Frac<BigNum> > qSq=createQSquare<BigNum>();
        PA::Polynomial<Frac<BigNum> > multiplier;
        Frac<BigNum> one(1,1);
        PA::Monomial<Frac<BigNum> > oneM(one);
        multiplier+=oneM;
        Frac<BigNum> zeroFrac(0,1);
        for(long ell=L;ell>0;--ell){
            PA::Polynomial<Frac<BigNum>> FracSummation;
            for(long i=0;i<array_C[ell-1].size();++i){
                for(long j=0;j<array_C[ell-1].size();++j){
                    PA::Polynomial<Frac<BigNum> > Th_lqij=(array_TPolynomials[ell-1])[i][j];
                    Frac<BigNum> C_lij=(array_C[ell-1])[i][j];
                    if( (Th_lqij.terms.size()>0) && ( !( C_lij==zeroFrac ) ) ){
                        Th_lqij *= C_lij*((array_invWDiagonals[ell-1])[j]);
                        FracSummation+=Th_lqij;
                    }
                }
            }
            FracSummation*=multiplier;
            res+=FracSummation;
            multiplier*=qSq;
        }
        return res;
    }
template<typename BigNum>
    std::vector<Frac<BigNum> > divideVectors(const std::vector<BigNum>& num, const std::vector<BigNum>& den){
        std::vector<Frac<BigNum> > res;
        if(num.size()<1){return res;}
        if(num.size()!=den.size()){return res;}
        res.resize(num.size());
        for(long i=0;i<num.size();++i){
            if(den[i]!=0){
                res[i].setNum(num[i]);
                res[i].setDen(den[i]);
            }
        }
        return res;
    }
template<typename BigNum>
    std::vector<PA::Polynomial<Frac<BigNum> > > hamiltonProductBasic(){
        std::vector<PA::Polynomial<Frac<BigNum> > > res;
        res.resize(4);
        std::vector<PA::Polynomial<Frac<BigNum> > > p,q;
        p.resize(4);q.resize(4);
        Frac<BigNum> fr1(1,1);
        for(long i=0;i<4;++i){
            PA::Monomial<Frac<BigNum> > mP(fr1,"p_"+std::to_string(i),1);
            PA::Monomial<Frac<BigNum> > mQ(fr1,"q_"+std::to_string(i),1);
            p[i]+=mP;q[i]+=mQ;
        }
        PA::Polynomial<Frac<BigNum> > tmp;
        //                       (p*q)_0 = p0q0 - p1q1 - p2q2 - p3q3
        tmp.terms.clear(); tmp+=p[0];tmp*=q[0];
        res[0]+=tmp;
        tmp.terms.clear(); tmp+=p[1];tmp*=q[1];
        res[0]-=tmp;
        tmp.terms.clear(); tmp+=p[2];tmp*=q[2];
        res[0]-=tmp;
        tmp.terms.clear(); tmp+=p[3];tmp*=q[3];
        res[0]-=tmp;
        //                       (p*q)_1 = p0q1 + p1q0 + p2q3 - p3q2
        tmp.terms.clear(); tmp+=p[0];tmp*=q[1];
        res[1]+=tmp;
        tmp.terms.clear(); tmp+=p[1];tmp*=q[0];
        res[1]+=tmp;
        tmp.terms.clear(); tmp+=p[2];tmp*=q[3];
        res[1]+=tmp;
        tmp.terms.clear(); tmp+=p[3];tmp*=q[2];
        res[1]-=tmp;
        //                       (p*q)_2 = p0q2 - p1q3+ p2q0 + p3q1
        tmp.terms.clear(); tmp+=p[0];tmp*=q[2];
        res[2]+=tmp;
        tmp.terms.clear(); tmp+=p[1];tmp*=q[3];
        res[2]-=tmp;
        tmp.terms.clear(); tmp+=p[2];tmp*=q[0];
        res[2]+=tmp;
        tmp.terms.clear(); tmp+=p[3];tmp*=q[1];
        res[2]+=tmp;
        //                       (p*q)_3 = p0q3 + p1q2 - p2q1 + p3q0
        tmp.terms.clear(); tmp+=p[0];tmp*=q[3];
        res[3]+=tmp;
        tmp.terms.clear(); tmp+=p[1];tmp*=q[2];
        res[3]+=tmp;
        tmp.terms.clear(); tmp+=p[2];tmp*=q[1];
        res[3]-=tmp;
        tmp.terms.clear(); tmp+=p[3];tmp*=q[0];
        res[3]+=tmp;
        return res;
    }
template<typename BigNum>
void substitutionsP(long jobNumber,long* nThreads,
                    long* dimSquare,
                    std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > >* TLH,
                    std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > >* TP,
                    std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > const * TL,
                    std::map<std::string,PA::Polynomial<Frac<BigNum> > > const * hamiltonSubstitution,
                    std::map<std::string,PA::Polynomial<Frac<BigNum> > > const * qpSubstitution){
    //there is a total of dimSquare jobs
    long i;long j;
    long dim=(*TLH).size();
    while(jobNumber<*dimSquare){
        i=jobNumber%dim;
        j=(jobNumber/dim)%dim;
        (*TLH)[i][j]=PA::substitute((*TL)[i][j],*hamiltonSubstitution);
        (*TP)[i][j]=PA::substitute((*TL)[i][j],*qpSubstitution);
        jobNumber+=*nThreads;
    }
}
template<typename BigNum>
    int checkMultiplicativity(const std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > >& TL){
        TMF::Timer innerT;
        innerT.start();
        if(GL_skipMultVerification){return 1;}
        std::map<std::string,PA::Polynomial<Frac<BigNum> > > hamiltonSubstitution,qpSubstitution;
        std::vector<PA::Polynomial<Frac<BigNum> > > hamiltonProduct=hamiltonProductBasic<BigNum>();
        Frac<BigNum> one(1,1);
        for(long i=0;i<4;++i){
            hamiltonSubstitution["q_"+std::to_string(i)]=hamiltonProduct[i];
            PA::Polynomial<Frac<BigNum> > pI;
            PA::Monomial<Frac<BigNum> > mI(one,"p_"+std::to_string(i),1);
            pI+=mI;
            qpSubstitution["q_"+std::to_string(i)]=pI;
        }
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > TLH=TL, TP=TL;//just to get the formats right
        innerT.end();
        std::cout<<"Phase 1: map creation and hamilton product finished in "<<innerT.getTimeSec()<<" seconds.\n";
        innerT.start();
        if(TLH.size()<6){
            for(long i=0;i<TLH.size();++i){
                for(long j=0;j<TLH[i].size();++j){
                    TLH[i][j]=PA::substitute(TL[i][j],hamiltonSubstitution);
                    TP[i][j]=PA::substitute(TL[i][j],qpSubstitution);
                }
            }
        }
        else{
            long nThreads=std::thread::hardware_concurrency();
            if(nThreads==0){nThreads=1;}
            std::thread* allTh=new std::thread[nThreads];
            long dimSquare=TLH.size()*TLH.size();
            for(long i=0;i<nThreads;++i){
                allTh[i]=std::thread(substitutionsP<BigNum>,
                                     i,&nThreads,&dimSquare,
                                     &TLH, &TP, &TL,
                                     &hamiltonSubstitution,
                                     &qpSubstitution
                                     );
            }
            for(long i=0;i<nThreads;++i){
                allTh[i].join();
            }
            delete[] allTh;
        }
        innerT.end();
        std::cout<<"Phase 2: Hamilton substitution and qp substitution finished in "<<innerT.getTimeSec()<<" seconds. \n";
        innerT.start();
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > TProd=TP*TL;
        innerT.end();
        std::cout<<"Phase 3: Product of matrices finished in "<<innerT.getTimeSec()<<" seconds. \n";
        innerT.start();
        for(long i=0;i<TLH.size();++i){
            for(long j=0;j<TLH[i].size();++j){
                if(PA::equal(TLH[i][j],TProd[i][j])==0){
                    return 0;
                }
            }
        }
        innerT.end();
        std::cout<<"Phase 4: Equality check finished in "<<innerT.getTimeSec()<<" seconds. \n";
        return 1;
    }
template<typename BigNum>
    int checkOrthogonality(const std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > >& TL,
                const std::vector<BigNum>& array_dNum,
                const std::vector<BigNum>& array_dDen){
        if(GL_skipOrthVerification){return 1;}
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > matrixD;
        std::vector<PA::Polynomial<Frac<BigNum> > > placeHolderRow;
        long dim=TL.size();
        placeHolderRow.resize(dim);
        matrixD.resize(dim);
        for(long i=0;i<dim;++i){matrixD[i]=placeHolderRow;}
        BigNum lcmDen=1;
        for(long i=0;i<dim;++i){
            lcmDen=FA::leastCommonMultiple(lcmDen,array_dDen[i]);
        }
        for(long i=0;i<dim;++i){
            PA::Monomial<Frac<BigNum> > mI(static_cast<Frac<BigNum> >(array_dNum[i]*lcmDen/array_dDen[i]));
            matrixD[i][i]+=mI;
        }
        long ell=(dim-1)/2;
        PA::Polynomial<Frac<BigNum> > qSq=createQSquare<BigNum>();
        Frac<BigNum> one(1,1);
        PA::Polynomial<Frac<BigNum> > qFourL(one);
        for(long i=0;i<ell;++i){
            qFourL*=qSq;
        }
        qFourL*=qFourL;
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > LHS;
        LHS=transpose(TL) * matrixD;
        LHS= LHS * TL;
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > RHS=matrixD;
        for(long i=0;i<dim;++i){
            RHS[i][i]*=qFourL;
        }
        for(long i=0;i<LHS.size();++i){
            for(long j=0;j<LHS[i].size();++j){
                if(PA::equal(LHS[i][j],RHS[i][j])==0){return 0;}
            }
        }
        return 1;
    }
template<typename BigNum>
    int checkMultiplicativityAndOrthogonality(const std::vector<std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > >& array_TPolynomials,
                               const std::vector<std::vector<BigNum> >& array_dNums,
                               const std::vector<std::vector<BigNum> >& array_dDens){
        int success=1;
        long l=0;
        while(l<array_TPolynomials.size()){
            std::cout<<"Multiplicativity and orthogonality checks for ell="<<l+1<<".";
            std::cout<<"\n";
            success*=checkMultiplicativity(array_TPolynomials[l])*checkOrthogonality(array_TPolynomials[l],array_dNums[l],array_dDens[l]);
            if(success==1){
                std::cout<<"Passed. \n";
            }
            if(success==0){
                std::cout<<"Failed. \n";
                return 0;
            }
            ++l;
        }
        return success;
    }
template<typename BigNum>
    std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > createTPolynomial(const std::vector<std::vector<long> >& tRaw,
                                                                         BigNum tDenominator, long ell){
        std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > res;
        res.resize(2*ell+1);
        Frac<BigNum> one(1,1);
        std::vector<PA::Polynomial<Frac<BigNum> > > placeHolder;
        placeHolder.resize(2*ell+1);
        for(long i=0;i<res.size();++i){res[i]=placeHolder;}
        for(long k=0;k<tRaw.size();++k){
            PA::Monomial<Frac<BigNum> > mn;
            mn.c.setNum(tRaw[k][6]);
            mn.c.setDen(tDenominator);
            for(long j=0;j<4;j++){
                if(tRaw[k][j+2]>0){
                    PA::Monomial<Frac<BigNum> > tmp(one,"q_"+std::to_string(j),tRaw[k][j+2]);
                    mn*=tmp;
                }
            }
            res[tRaw[k][0]][tRaw[k][1]]+=mn;
        }
        return res;
    }
    std::vector<std::vector<long> > matrixLongFromString(const std::string& _in){
        std::vector<std::string> rows=SF::stringToVector(_in,GL_openTag,GL_closeTag);
        std::vector<std::vector<long> > res;
        res.resize(rows.size());
        for(long i=0;i<rows.size();++i){
            res[i]=SF::stringToVectorLong(rows[i],GL_openTag,GL_closeTag);
        }
        return res;
    }
    int checkHomogeneity(const std::vector<std::vector<long> >& T, long twoEll){
        int res=1;
        long i=T.size();
        while((i>0)&&(res==1)){
            --i;
            if(T[i][2]+T[i][3]+T[i][4]+T[i][5]!=twoEll){res=0;}
        }
        return res;
    }
template<typename BigNum>
    int areWsPositive(const std::vector<BigNum>& dens, const std::vector<BigNum>& nums){
        int res=1;
        long i=dens.size();
        if(nums.size()!=i){return 0;}
        while((i>0)&&(res==1)){
            --i;
            if((dens[i]<1)||(nums[i]<1)){res=0;}
        }
        return res;
    }
template<typename BigNum>
std::vector<BigNum> getBNVector(const std::vector<long>& vL){
    std::vector<BigNum> res;
    if(vL.size()<1){return res;}
    res.resize(vL.size());
    for(long i=0;i<vL.size();++i){
        res[i]=vL[i];
    }
    return res;
}
template<typename BigNum>
    int getFromMap(const std::map<std::string,std::string>& mainMap,
                      std::vector<std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > >& array_TPolynomials,
                      std::vector<std::vector<Frac<BigNum> > >& array_invWDiagonals,
                      long LMax
                     ){
        array_TPolynomials.resize(LMax);
        array_invWDiagonals.resize(LMax);
        int indicatorHomogeneity=1;
        int indicatorWPositive=1;
        std::vector<std::vector<BigNum> > array_dNums,array_dDens;
        array_dNums.resize(LMax);array_dDens.resize(LMax);
        std::map<std::string,std::string>::const_iterator it;
        it=mainMap.find("skipMultVerification");
        if(it!=mainMap.end()){
            if(it->second=="yes"){GL_skipMultVerification=1;}
        }
        it=mainMap.find("skipOrthVerification");
        if(it!=mainMap.end()){
            if(it->second=="yes"){GL_skipOrthVerification=1;}
        }
        it=mainMap.find("skipTxtOutput");
        if(it!=mainMap.end()){
            if(it->second=="yes"){GL_skipTxtOutput=1;}
        }
        it=mainMap.find("stringSizeLimit");
        if(it!=mainMap.end()){
            GL_stringSizeLimit=BF::stringToInteger(it->second);
        }
        for(long l=0;l<LMax;++l){
            long ell=l+1;
            std::string zeros="000";
            if(ell>9){zeros="00";}
            it=mainMap.find("Tden"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            BigNum tDenominator=SF::stringToVectorLong(SF::stringToVector(it->second,GL_openTag,GL_closeTag)[0],GL_openTag,GL_closeTag)[0];
            it=mainMap.find("WNum"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            array_dNums[l] = getBNVector<BigNum>(SF::stringToVectorLong(SF::stringToVector(it->second,GL_openTag,GL_closeTag)[0],GL_openTag,GL_closeTag));
            it=mainMap.find("WDen"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            array_dDens[l]=getBNVector<BigNum>(SF::stringToVectorLong(SF::stringToVector(it->second,GL_openTag,GL_closeTag)[0],GL_openTag,GL_closeTag));
            it=mainMap.find("Tpoly"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            std::vector<std::vector<long> > tRaw=matrixLongFromString(it->second);
            indicatorHomogeneity*=checkHomogeneity(tRaw,2*ell);
            indicatorWPositive*=areWsPositive(array_dDens[l],array_dNums[l]);
            array_invWDiagonals[l]=divideVectors(array_dDens[l],array_dNums[l]);
            array_TPolynomials[l]=createTPolynomial(tRaw,tDenominator,ell);
        }
        return indicatorHomogeneity*indicatorWPositive*checkMultiplicativityAndOrthogonality(array_TPolynomials,array_dNums,array_dDens);
    }
template<typename BigNum>
    std::vector<std::vector<Frac<BigNum> > > divideByScalar(const std::vector<std::vector<BigNum> >& M, BigNum den){
        std::vector<std::vector<Frac<BigNum> > > res;
        std::vector<Frac<BigNum> > placeHolder;
        if(M.size()<1){return res;}
        res.resize(M.size());
        placeHolder.resize(M.size());
        for(long i=0;i<M.size();++i){res[i]=placeHolder;}
        for(long i=0;i<M.size();++i){
            for(long j=0;j<M.size();++j){
                res[i][j].setNum(M[i][j]);
                res[i][j].setDen(den);
            }
        }
        return res;
    }
template<typename BigNum>
    int checkPsdWitness(const std::vector<std::vector<BigNum> >& C,
                            const std::vector<std::vector<BigNum> >& W,
                            long sWitt){
        //number sWitt must be positive for this test to be meaningful
        if(sWitt<=0){return 0;}
        std::vector<std::vector<BigNum> > E=C;
        std::vector<std::vector<BigNum> > WTW=transpose(W)*W;
        for(long i=0;i<E.size();++i){
            for(long j=0;j<E.size();++j){
                E[i][j]=sWitt*C[i][j]-WTW[i][j];
            }
        }
        BigNum aw;
        for(long i=0;i<E.size();++i){
            for(long j=i+1;j<E.size();++j){
                if(E[i][j]!=E[j][i]){return 0;}
            }
            aw=E[i][i];
            aw+=E[i][i];
            for(long j=0;j<E.size();++j){
                aw-=FA::absBig(E[i][j]);
            }
            if(aw<0){return 0;}
        }
        return 1;
    }
long getNumber(const std::map<std::string,std::string>& mainMap, const std::string& st){
    std::map<std::string,std::string>::const_iterator it=mainMap.find(st);
    if(it==mainMap.end()){return 0;}
    std::vector<std::string> v=SF::stringToVector(it->second,GL_openTag,GL_closeTag);
    if(v.size()!=1){return 0;}
    v=SF::stringToVector(v[0],GL_openTag,GL_closeTag);
    if(v.size()!=1){return 0;}
    return BF::stringToInteger(v[0]);
}
template<typename BigNum>
std::vector<std::vector<BigNum> > matrixBN(const std::vector<std::vector<long> >& in){
    std::vector<std::vector<BigNum> > res;
    if(in.size()<1){return res;}
    res.resize(in.size());
    for(long i=0;i<in.size();++i){
        res[i]=getBNVector<BigNum>(in[i]);
    }
    return res;
}
template<typename BigNum>
    int getFromMap(const std::map<std::string,std::string>& mainMap,
                      std::vector<std::vector<std::vector<Frac<BigNum> > > >& array_C,
                      const std::vector<std::vector<Frac<BigNum> > >& array_invWDiagonals,
                      long LMax,
                      const Frac<BigNum>& TStar){
        array_C.resize(LMax);
        std::map<std::string,std::string>::const_iterator it;
        long cd2=20;                                   // default: legacy certificates
        it=mainMap.find("cDenLog2");
        if(it!=mainMap.end()){cd2=getNumber(mainMap,"cDenLog2");}
        if(cd2<0 || cd2>60){return 0;}   // malformed certificate
        long L_twoToCorrectExponent=1L<<cd2;
        BigNum twoToCorrectExponent=L_twoToCorrectExponent;
        for(long l=0;l<LMax;++l){
            long ell=l+1;
            std::string zeros="000";
            if(ell>9){zeros="00";}
            it=mainMap.find("Swit"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            long sWitt=SF::stringToVectorLong(SF::stringToVector(it->second,GL_openTag,GL_closeTag)[0],GL_openTag,GL_closeTag)[0];
            it=mainMap.find("Cscaled"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            std::vector<std::vector<BigNum> > cBN=matrixBN<BigNum>(matrixLongFromString(it->second));
            it=mainMap.find("Wwit"+zeros+std::to_string(ell));
            if(it==mainMap.end()){return 0;}
            std::vector<std::vector<BigNum> > wLong=matrixBN<BigNum>(matrixLongFromString(it->second));
            if(!checkPsdWitness(cBN,wLong,sWitt)){
                return 0;
            }
            array_C[l]=divideByScalar(cBN,twoToCorrectExponent);
        }
        Frac<BigNum> wTracesTotal(0,1);
        for(long l=0;l<LMax;++l){
            for(long i=0;i<array_C[l].size();++i){
                wTracesTotal += (array_C[l])[i][i] * array_invWDiagonals[l][i];
            }
        }
        return (wTracesTotal==TStar);
    }
    template<typename BigNum>
    PA::Polynomial<Frac<BigNum> > createG(const std::vector<std::vector<std::vector<Frac<BigNum> > > >& array_C,
                                     const std::vector<std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > >& array_TPolynomials,
                                     const std::vector<std::vector<Frac<BigNum> > >& array_invWDiagonals,
                                 long L,
                                 const Frac<BigNum> & M){
        PA::Polynomial<Frac<BigNum> > resG=createP(array_C,array_TPolynomials,array_invWDiagonals,L);
        Frac<BigNum> one(1,1);
        PA::Polynomial<Frac<BigNum> > tmp(one);
        PA::Polynomial<Frac<BigNum> > qSq=createQSquare<BigNum>();
        for(long i=0;i<L;++i){
            tmp*=qSq;
        }
        tmp*=M.getNum();
        resG*=-M.getDen();
        resG+=tmp;
        return resG;
    }

template<typename BigNum>
    struct TestSetup{
    public:
        std::vector<std::vector<std::vector<PA::Polynomial<Frac<BigNum> > > > > array_TPolynomials;
        std::vector<std::vector<Frac<BigNum> > > array_invWDiagonals;
        std::vector<std::vector<std::vector<Frac<BigNum> > > > array_C;
        PA::Polynomial<Frac<BigNum> > pol_G;
        PA::Polynomial<Frac<BigNum> > pol_Uh;
        PA::Polynomial<Frac<BigNum> > pol_Qh;
        long LMax;
        long targetBound;
        Frac<BigNum> tauSq;
        Frac<BigNum> M;
        Frac<BigNum> TStar;
        int success=0;
    };
template<typename BigNum>
    TestSetup<BigNum> getFromMap(const std::map<std::string,std::string>& mainMap){
        // ============================================================================
        // SOS VERIFICATION PLAN (replaces box subdivision; everything else unchanged)
        // Record names below must match Appendix app:data of the paper exactly.
        // ============================================================================
        //
        // NEW CERTIFICATE RECORDS
        //   sosExponentK              the integer k >= 0 of the identity; requires L+k >= 4
        //   sosDenLog2                ONE shared even integer kappa_s scaling all four Grams
        //                             (no per-block denominators; W carries no denominator at all:
        //                             the witness check is purely integer)
        //   sosGram000a  (a=0..3)     integer SYMMETRIC matrix = 2^{kappa_s} * Sigma_a
        //   sosSwit000a  (a=0..3)     integer scalar S_a  (verifier must check S_a >= 1)
        //   sosWwit000a  (a=0..3)     integer MATRIX W_a, same shape as sosGram000a
        //   UhPoly, QhPoly            integer coefficient lists of the generators, Tpoly row
        //                             format (paper check V:gens; see DECISION below)
        //   Old files without these records: refuse with a clear "pre-SOS certificate" message
        //   (upgrade path is re-emission by cylgen, never file patching).
        //
        // VERIFIER = old checks V1..V5 verbatim (well-formedness, tables/H:mult/H:orth,
        // C-witnesses, trace = T*, counting inequality), MINUS all box code (presplit, corner
        // bounds, adaptive stack, threads, saved-state/resume -- delete, git remembers), PLUS:
        //
        // (S0) SHAPE. Each sosGram000a is symmetric, with the prescribed size:
        //        Sigma_0: n_{L+k},  Sigma_1, Sigma_2: n_{L+k-2},  Sigma_3: n_{L+k-4},
        //      where n_d = binom(d+3,3) = #monomials of degree d in q_0..q_3, computed from the
        //      deposited L and k. (L+k >= 4 exists only so Sigma_3's basis exists.)
        //      Reject asymmetry -- do not silently symmetrize: step (S2) presumes it.
        //
        // (S1) PSD, by the existing Prop prop:psd machinery, verbatim as for the C_ell:
        //        E_a = S_a * (2^{kappa_s} Sigma_a) - W_a^T W_a
        //      is symmetric, diagonally dominant, nonnegative diagonal, AND S_a >= 1
        //      (the implication fails for S_a <= 0; one-line check, real hole).
        //      ==> Sigma_a is positive semidefinite ==> sigma_a(q) >= 0 for ALL q.
        //
        // (S2) EXPANSION (symbolic -- no point q is ever evaluated anywhere):
        //        2^{kappa_s} * sigma_a = sum over upper triangle i <= j of
        //        (2 - delta_ij) * (sosGram000a)_{ij} * monomial_i * monomial_j.
        //      Monomial order of the basis vector m_d is a FIXED shared convention with cylgen
        //      (lexicographic on exponent vectors). A mismatch only fails a valid certificate
        //      (completeness), never passes an invalid one (soundness) -- still, document it.
        //
        // (S3) THE IDENTITY, entirely over integers (clear 2^{kappa_s}; U_h, Q_h are already
        //      integer polynomials for integer A, B; never divide the deposited Grams):
        //        2^{kappa_s} * (|q|^2)^k * G
        //          == [2^{kappa_s} sigma_0] + [2^{kappa_s} sigma_1]*U_h
        //           + [2^{kappa_s} sigma_2]*Q_h + [2^{kappa_s} sigma_3]*U_h*Q_h,
        //      compared coefficient by coefficient: n_{2L+2k} integer equalities
        //      (455 at L=4, k=2; Gram sizes 84/35/35/10).
        //      G, U_h, Q_h come from the battle-tested CPC::getFromMap path, unchanged,
        //      including the trace fingerprint. Word-width selection and overflow guards now
        //      depend on k as well as L.
        //
        // DECISION (resolved): U_h, Q_h are CONSTRUCTED by getFromMap as today (maximal reuse
        // of the vetted path), AND deposited as UhPoly/QhPoly; new check V:gens compares the
        // constructed polynomials against the deposited lists coefficient by coefficient
        // (~10 lines). This keeps the paper's lean contract honest without touching the old code.
        //
        // SOUNDNESS INVARIANT (preserves the HSC_helpString contract verbatim): every failure
        // mode -- bad witness, S_a < 1, wrong shape, identity mismatch, missing record, any
        // arithmetic flag -- refuses to certify; no certificate content can make the program
        // print the PASS line for a false statement. Search/provenance of the Sigma_a is
        // irrelevant to the verifier.
        //
        // THEN THE THEOREM IS PROVED: on K = {U_h >= 0, Q_h >= 0}, every RHS term of (S3) is
        // >= 0 by (S1), so (|q|^2)^k G >= 0, and q != 0 lets us divide. (Theorem thm:sos.)
        //
        // REUSE MAP: parser (SF), Frac/FA (checked ints, flags), PA polynomial multiply,
        // CPC::getFromMap, prop:psd witness check, V1-V5, CLI/verdict, pinwheel mode: UNCHANGED.
        // NEW CODE (~150-250 lines, no new arithmetic/threading): monomial basis enumeration in
        // the fixed order, Gram->polynomial expansion, RHS assembly, integer comparison, record
        // reading + (S0) + S_a>=1, width logic extended by k.
        //
        // TESTS: (T1) one certificate verified BOTH ways (old box verifier vs this) -- shared
        // V1-V5 and G-assembly isolate any divergence to the new lines; (T2) negative battery:
        // perturb one Gram entry / one witness entry / one dimension / drop one record -- each
        // must refuse without printing PASS; (T3) develop against a ground-truth certificate
        // (Claude to generate: tau=1, M retuned with interior slack, rounded Sigmas + witnesses)
        // so the verifier is never debugged against untested cylgen output.
        // ============================================================================
        
        TestSetup<BigNum> res;res.success=0;
        res.LMax=getNumber(mainMap,"Lmax");
        if(res.LMax==0){return res;}
        res.targetBound=getNumber(mainMap,"targetBound");
        if(res.targetBound==0){return res;}
        res.tauSq.setNum(getNumber(mainMap,"tauSqNum"));
        if(res.tauSq.getNum()==0){return res;}
        res.tauSq.setDen(getNumber(mainMap,"tauSqDen"));
        if(FA::GL_DIVBY0){return res;}
        res.M.setNum(getNumber(mainMap,"MNum"));
        if(res.M.getNum()==0){return res;}
        res.M.setDen(getNumber(mainMap,"MDen"));
        if(FA::GL_DIVBY0){return res;}
        res.TStar.setNum(getNumber(mainMap,"TstarNum"));
        if(res.TStar.getNum()==0){return res;}
        res.TStar.setDen(getNumber(mainMap,"TstarDen"));
        if(FA::GL_DIVBY0){return res;}
        Components<BigNum> mainComps;
        mainComps.updateUhQh(res.tauSq.getNum(),res.tauSq.getDen());
        res.success=getFromMap(mainMap,res.array_TPolynomials,res.array_invWDiagonals,res.LMax);
        if(res.success==0){
            std::cout<<"T failed the required conditions\n";
            return res;
        }
        std::cout<<"T satisfies the required conditions\n";
        res.success=getFromMap(mainMap, res.array_C, res.array_invWDiagonals,res.LMax,res.TStar);
        if(res.success==0){
            std::cout<<"C failed the required conditions\n";
            return res;
        }
        std::cout<<"C satisfies the required conditions\n";
        // ---- claim-level checks: connect the verified data to the stated theorem ----
        // TStar has just been verified equal to the exact trace sum recomputed from C.
        // The counting theorem yields N < TStar/(-M) + 1, so proving "at most targetBound"
        // requires the strict inequality TStar < (-M)*targetBound, equivalently
        // targetBound >= 1 + floor(TStar/(-M)). All comparisons exact.
        res.success=0;
        if(res.targetBound<1){
            std::cout<<"Claim check failed: targetBound must be a positive integer\n";
            return res;
        }
        if(!(res.M.getNum()<0)){
            std::cout<<"Claim check failed: M must be negative\n";
            return res;
        }
        {
            Frac<BigNum> zero(0,1); Frac<BigNum> four(4,1);
            if(!(zero<res.tauSq)||!(res.tauSq<four)){
                std::cout<<"Claim check failed: tau^2 must lie in (0,4) so that omega=tau/(2-tau) is meaningful\n";
                return res;
            }
        }
        {
            Frac<BigNum> minusM=res.M;
            minusM.setNum(-minusM.getNum());
            Frac<BigNum> tbF(res.targetBound,1);
            Frac<BigNum> rhs=minusM; rhs*=tbF;
            if(!(res.TStar<rhs)){
                std::cout<<"Claim check failed: TStar >= (-M)*targetBound; the certificate does not support this targetBound\n";
                return res;
            }
        }
        if(FA::GL_OVERFLOW||FA::GL_DIVBY0){
            std::cout<<"Claim check failed: arithmetic flag raised during claim checks\n";
            return res;
        }
        std::cout<<"Claim inequality TStar < (-M)*targetBound verified\n";
        res.success=1;
        res.pol_G=createG(res.array_C,res.array_TPolynomials,res.array_invWDiagonals,res.LMax,res.M);
        res.pol_Uh=mainComps.paper_Uh;
        res.pol_Qh=mainComps.paper_Qh;
        return res;
    }
    template<typename TTT>
    struct Box{
        std::vector<TTT> ls;
        std::vector<TTT> hs;
        Box();
        int inequalityCode;
        int operator<(const Box<TTT>& ) const;
    };
    template<typename TTT>
    Box<TTT>::Box(){
        inequalityCode=0;
        ls.resize(3);hs.resize(3);
        ls[0]=-1;hs[0]=1;ls[1]=-1;hs[1]=1;ls[2]=-1;hs[2]=1;
    }
    template<typename TTT>
    int Box<TTT>::operator<(const Box<TTT>& oth)const{
        long i=0;
        TTT sz=hs[0]-ls[0];
        TTT oth_sz=oth.hs[0]-oth.ls[0];
        if(sz<oth_sz){return 1;}
        if(oth_sz<sz){return 0;}
        TTT distFromZero=ls[0]+ls[1]+ls[2];
        TTT othDistFromZero=oth.ls[0]+oth.ls[1]+oth.ls[2];
        if(distFromZero<othDistFromZero){return 1;}
        if(othDistFromZero<distFromZero){return 0;}
        while(i<3){
            if(ls[i]<oth.ls[i]){return 1;}
            if(oth.ls[i]<ls[i]){return 0;}
            if(hs[i]<oth.hs[i]){return 1;}
            if(oth.hs[i]<hs[i]){return 0;}
            ++i;
        }
        return 0;
    }
    template<typename TTT>
    void removeTopThenSplitAndAddToStack(fst::stack<Box<TTT> >& _st){
        if(_st.size()==0){return;}
        Box<TTT> b=_st.top();
        _st.pop();
        TTT half(1,2);
        Box<TTT> tmp;
        tmp.ls[0]=b.ls[0];
        tmp.hs[0]=(b.hs[0]+b.ls[0])* half;
        tmp.ls[1]=b.ls[1];
        tmp.hs[1]=(b.hs[1]+b.ls[1])* half;
        tmp.ls[2]=b.ls[2];
        tmp.hs[2]=(b.hs[2]+b.ls[2])* half;
        _st.push(tmp);
        tmp.ls[0]=(b.hs[0]+b.ls[0])*half;
        tmp.hs[0]=b.hs[0];
       /* no need to do these instructions because they stayed from the old code:
        tmp.ls[1]=b.ls[1];
        tmp.hs[1]=(b.hs[1]+b.ls[1])* half;
        tmp.ls[2]=b.ls[2];
        tmp.hs[2]=(b.hs[2]+b.ls[2])* half;*/
        _st.push(tmp);
        tmp.ls[1]=(b.ls[1]+b.hs[1])*half;
        tmp.hs[1]=b.hs[1];
        // happy with previous [0] and [2]
        _st.push(tmp);
        tmp.ls[2]=(b.ls[2]+b.hs[2])*half;
        tmp.hs[2]=b.hs[2];
        // happy with previous [0] and [1]
        _st.push(tmp);
        tmp.ls[1]=b.ls[1];
        tmp.hs[1]=(b.hs[1]+b.ls[1])* half;
        // happy with previous [0] and [2]
        _st.push(tmp);
        tmp.ls[0]=b.ls[0];
        tmp.hs[0]=(b.hs[0]+b.ls[0])* half;
        // happy with previous [1] and [2]
        _st.push(tmp);
        tmp.ls[1]=(b.ls[1]+b.hs[1])*half;
        tmp.hs[1]=b.hs[1];
        //happy with previous [0] and [2]
        _st.push(tmp);
        tmp.ls[2]=b.ls[2];
        tmp.hs[2]=(b.ls[2]+b.hs[2])*half;
        //happy with previous [0] and [1]
        _st.push(tmp);
    }
}

#endif
