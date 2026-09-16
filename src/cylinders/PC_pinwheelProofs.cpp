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


#ifndef __INCL_PC_CPP
#define __INCL_PC_CPP
namespace PC{
long GL_PI_NUM_SMALL_LOW_PRECISION=3141592;
long GL_PI_DEN_LOW_PRECISION      =1000000;
template<typename BigNum> int check2PhiBiggerThan21Over20(const Frac<BigNum>& PhiSq){
        // The function assumes that PhiSq is positive
        // 2Phi>21/20 <=> PhiSq>(21^2/20^2)/4
        Frac<BigNum> oth(21*21,4*20*20);
        return oth<PhiSq;
    }
    template<typename BigNum> int checkPsiInvBiggerThan21Over20(const Frac<BigNum>& PsiSq){
        // The function assumes that PsiSq is positive
        // Psi^{-1}>21/20 <=> Psi<20/21 <=> Psi^2<20^2<21^2
        Frac<BigNum> oth(20*20,21*21);
        return PsiSq<oth;
    }
    template<typename BigNum> int checkTwoConditionsForUBar(const BigNum& N,
                                                            const BigNum& s,
                                                            const Frac<BigNum>& c0,
                                                            const Frac<BigNum>& a,
                                                            const Frac<BigNum>& one){
                                
        // Condition 1:
        // \bar{u}=\sqrt{c0}\cdot(\theta+\theta^3)
        // \theta=\pi/(N-s)
        //
        // Indequality is equivalent to
        // \bar{u}^2<1+a^2 <=> c0*(\theta+\theta^3)^2<1+a^2
        //   Suffices to prove
        // c0*( piUpperBound/(N-s) + (piUpperBound/(N-s))^3)^2<1+a^2
        // we will introduce subsitution RSq=( piUpperBound/(N-s) + (piUpperBound/(N-s))^3)^2
        // Then need to check
        // **************************
        // ***     CHECK 1        ***
        // *** c0 * RSq < 1 + a^2 ***
        // **************************

        // Condition 2:
        // N\bar{u}\leq \pi (1+a^2-\bar{u}^2)
        //
        // Inequality is equivalent to
        // N^2\bar{u}^2\leq \pi^2(1+a^2-\bar{u}^2)^2 <=>
        // N^2 * c0 * ( (1/(N-s)) + pi^2*(1/(N-s))^3)^2
        //            <=
        // ( 1+a^2-((pi/(N-s)) + (pi/(N-s))^3 )^2 )^2
        // Sufficient to prove
        // **************************************************************
        // ***                      CHECK 2                           ***
        // *** N^2 * c0 * ( (1/(N-s)) + piUpperBound^2*(1/(N-s))^3)^2 ***
        // ***                           <=                           ***
        // ***                     ( 1+a^2-RSq )^2                    ***
        // **************************************************************


        Frac<BigNum> oneOverNMS(1,N-s);
        Frac<BigNum> piUpperBound(GL_PI_NUM_SMALL_LOW_PRECISION+1,GL_PI_DEN_LOW_PRECISION);
        Frac<BigNum> piOverNMS=piUpperBound*oneOverNMS;
        Frac<BigNum> RSq=piOverNMS*piOverNMS*piOverNMS+piOverNMS;
        RSq*=RSq;
        // **************************
        // ***     CHECK 1        ***
        // *** c0 * RSq < 1 + a^2 ***
        // **************************
        Frac<BigNum> check1LeftSide=c0*RSq;
        Frac<BigNum> check1RightSide=one+a*a;
        int success1=(check1LeftSide<check1RightSide);
        if(success1==0){std::cout<<"Failed check 1 for (N,s)=("<<FA::toString(N)<<","<<FA::toString(s)<<")\n";}
        // **************************************************************
        // ***                      CHECK 2                           ***
        // *** N^2 * c0 * ( (1/(N-s)) + piUpperBound^2*(1/(N-s))^3)^2 ***
        // ***                           <=                           ***
        // ***                     ( 1+a^2-RSq )^2                    ***
        // **************************************************************
        Frac<BigNum> NSq(N*N,1);
        Frac<BigNum> check2LeftSide=NSq*c0;
        Frac<BigNum> secondMultiplierLeft=oneOverNMS+oneOverNMS*oneOverNMS*oneOverNMS*piUpperBound*piUpperBound;
        secondMultiplierLeft*=secondMultiplierLeft;
        
        check2LeftSide*=secondMultiplierLeft;
        Frac<BigNum> check2RightSide=one+a*a;
        check2RightSide-=RSq;
        check2RightSide*=check2RightSide;
        int success2=(check2LeftSide<check2RightSide);
        if(success2==0){std::cout<<"Failed check 2 for (N,s)=("<<N<<","<<s<<")\n";}
        return success1*success2;
    } 
    template<typename BigNum>
    void increment(BigNum& i,BigNum& iFact,Frac<BigNum>& factReciprocal,
                   Frac<BigNum>& upperBoundXToi, Frac<BigNum>& lowerBoundXToi,
                   const Frac<BigNum>& upperBoundX,const Frac<BigNum>& lowerBoundX){
        ++i;
        iFact*=i;
        factReciprocal.setDen(iFact);
        upperBoundXToi*=upperBoundX;
        lowerBoundXToi*=lowerBoundX;
    }
    template<typename BigNum>
    Frac<BigNum> lowerBoundForTan2PiOverN(BigNum N, int approximationOrder){
        N*=GL_PI_DEN_LOW_PRECISION;
        Frac<BigNum> lowerBoundX(2*GL_PI_NUM_SMALL_LOW_PRECISION,N);
        Frac<BigNum> upperBoundX(2*GL_PI_NUM_SMALL_LOW_PRECISION+2,N);
        Frac<BigNum> lowerBoundSin,upperBoundCos;
        BigNum i=0;
        BigNum iFact=1;
        Frac<BigNum> lowerBoundXToi(1,1);
        Frac<BigNum> upperBoundXToi(1,1);
        Frac<BigNum> factReciprocal(1,1);
        for(long k=0;k<approximationOrder;++k){
            upperBoundCos+=factReciprocal*upperBoundXToi;
            increment(i,iFact,factReciprocal,upperBoundXToi,lowerBoundXToi,upperBoundX,lowerBoundX);
            lowerBoundSin+=factReciprocal*lowerBoundXToi;
            increment(i,iFact,factReciprocal,upperBoundXToi,lowerBoundXToi,upperBoundX,lowerBoundX);
            upperBoundCos-=factReciprocal*lowerBoundXToi;
            increment(i,iFact,factReciprocal,upperBoundXToi,lowerBoundXToi,upperBoundX,lowerBoundX);
            lowerBoundSin-=factReciprocal*upperBoundXToi;
            increment(i,iFact,factReciprocal,upperBoundXToi,lowerBoundXToi,upperBoundX,lowerBoundX);
        }
        upperBoundCos+=factReciprocal*upperBoundXToi;
        // Round DOWN to the 2^-40 grid: preserves the lower-bound direction and keeps
        // every later product within Int512 despite the 7-digit pi above.
        return FA::dyadicBound(lowerBoundSin/upperBoundCos,40,0);
    }
    template<typename BigNum>
    int star1Check(BigNum N, BigNum s,
                   const Frac<BigNum>& c0,
                   const Frac<BigNum>& a,
                   const Frac<BigNum>& one,
                   int approximationOrder){
        // \arctan(a+\sqrt{c0}\tan\theta)-\arctan(a-\sqrt{c0}\tan\theta)\leq \frac{2\pi}{N}
        // \theta=\frac{\pi}{N-s}
        // Due to \arctan(p)-\arctan(q)=\arctan\frac{p-q}{1+pq}, the inequality is equivalent to
        // \frac{2\sqrt{c0}\tan\theta}{1+a^2-c0\tan^2{\theta} } <=\tan \frac{2\pi}{N}
        // Since z\mapsto z/(1+a^2-z^2) is increasing on [0,\sqrt{1+a^2}]
        // and \tan\theta\leq \tan \rho \leq \rho+\rho^3
        // (where \rho is a rational upper bound on \theta)
        // It suffices to find a rational \rho that is an upper bound on \theta
        // and perform two checks:
        // Check 1: 1+a^2-c0*(\rho+\rho^3)^2>=0 <=> c0*(rho+rho^3)^2<=1+a^2
        // suffices to prove for Check 1:
        // c0*(rho+rho^3)^2<=1+a^2
        // Check 2: \frac{2\sqrt{c0}(\rho+\rho^3)}{1+a^2-c0(\rho+\rho^3)^2} <=\tan \frac{2\pi}{N}
        // Define tLB=lowerBoundForTan2PiOverN( N, approximationOrder);
        // Suffices to prove for check 2:
        // \frac{2\sqrt{c0}(\rho+\rho^3)}{1+a^2-c0(\rho+\rho^3)^2} < tLB
        // <=> \sqrt{c0}< \frac{ tLB*(1+a^2-c0(\rho+\rho^3)^2)}{2(\rho+\rho^3)}
        // <=> c0<(\frac{ tLB*(1+a^2-c0(\rho+\rho^3)^2)}{2(\rho+\rho^3)})^2
        int check1;
        Frac<BigNum> rho(GL_PI_NUM_SMALL_LOW_PRECISION+1,(N-s)*GL_PI_DEN_LOW_PRECISION);
        // Upper bound on tan(theta), theta<=rho<1:  tan rho <= sinUpper(rho)/cosLower(rho),
        // where by the alternating-series bound (0<rho<1, term magnitudes strictly decreasing)
        //     sin rho <= rho - rho^3/6 + rho^5/120     (truncation ends on a + term),
        //     cos rho >= 1 - rho^2/2                   (truncation ends on a - term; >0 here).
        // The former bound tan rho <= rho+rho^3 is also valid but overshoots tan rho by
        // (2/3)rho^3, about 1.8% at rho ~ 0.165, while the (*) inequality at N=20 is true
        // with a margin of only ~0.17%: the sufficient check then fails although the
        // inequality holds. The bound below overshoots by under 6e-6, and the round-up to
        // the 2^-40 grid keeps the upper-bound direction while capping denominators.
        Frac<BigNum> oneSixth(1,6);
        Frac<BigNum> oneOver120(1,120);
        Frac<BigNum> half(1,2);
        Frac<BigNum> rho2=rho*rho;
        Frac<BigNum> sinUpper=rho-rho*rho2*oneSixth+rho*rho2*rho2*oneOver120;
        Frac<BigNum> cosLower=one-rho2*half;
        Frac<BigNum> sRho=FA::dyadicBound(sinUpper/cosLower,40,1);
        check1 = ( (c0*sRho*sRho) < (one+a*a) );
        std::cout<<"C1"<<check1<<" ";
        Frac<BigNum> tLB=lowerBoundForTan2PiOverN( N, approximationOrder);
        Frac<BigNum> rhs2=tLB*(one+a*a-c0*sRho*sRho);
        rhs2/=(sRho+sRho);
        rhs2*=rhs2;
        int check2=(c0<rhs2);
        return check1*check2;
    }
template<typename BigNum>
int star2Check(BigNum N, BigNum s,
               const Frac<BigNum>& S,
               const Frac<BigNum>& T,
               const Frac<BigNum>& one){
    // sin^2\frac{2\pi}N*(1-S^2)^2T^2
    //  \ge
    // \sin^2\theta [(S^2+T^2)(1+T^2)-\sin^2\tfrac{2\pi}N\,(S^2+T^2)^2],
    // \theta=\frac{\pi}{N-s}
    // Define s2PiOverNLB to be the lower bound for \sin\frac{2\pi}{N}
    Frac<BigNum> oneSixth(1,6);
    Frac<BigNum> oneOver120(1,120);
    // Lower bound for sin(2pi/N): with xL = twoPiOverNLB <= 2pi/N < pi/2,
    //     xL - xL^3/6 <= sin(xL) <= sin(2pi/N)
    // (alternating series ending on a - term, then monotonicity of sin on [0,pi/2]).
    // The bound is positive on our range, so squaring preserves the direction, and it
    // is a LOWER bound, so it is rounded DOWN on the dyadic grid.
    Frac<BigNum> twoPiOverNLB(2*GL_PI_NUM_SMALL_LOW_PRECISION,GL_PI_DEN_LOW_PRECISION*N);
    Frac<BigNum> s2PiOverNLB=twoPiOverNLB-twoPiOverNLB*twoPiOverNLB*twoPiOverNLB*oneSixth;
    Frac<BigNum> s2PiOverNLBSq=FA::dyadicBound(s2PiOverNLB*s2PiOverNLB,40,0);
    // Sufficient to prove:
    // s2PiOverNLBSq*(1-S^2)^2T^2
    //  \ge
    // \sin^2\theta [(S^2+T^2)(1+T^2)-s2PiOverNLBSq\,(S^2+T^2)^2],
    //
    // Upper bound for sin(theta), theta = pi/(N-s) in [thetaLB, thetaUB]:
    //     sin(theta) <= theta - theta^3/6 + theta^5/120   (alternating series, ends on +)
    //                <= thetaUB - thetaLB^3/6 + thetaUB^5/120
    // (upper endpoint on + terms, lower endpoint on - terms). The cruder sin x <= x
    // overshoots by about theta^2/6 relative, which exceeds the (**) margins at N=20
    // (+0.87%) and N=40 (+0.09%): those checks then fail although (**) is true.
    // This is an UPPER bound, so its square is rounded UP on the dyadic grid.
    Frac<BigNum> thetaUB(GL_PI_NUM_SMALL_LOW_PRECISION+1,GL_PI_DEN_LOW_PRECISION*(N-s));
    Frac<BigNum> thetaLB(GL_PI_NUM_SMALL_LOW_PRECISION,GL_PI_DEN_LOW_PRECISION*(N-s));
    Frac<BigNum> thetaUB2=thetaUB*thetaUB;
    Frac<BigNum> thetaLB2=thetaLB*thetaLB;
    Frac<BigNum> sThetaUB=thetaUB-oneSixth*thetaLB*thetaLB2+oneOver120*thetaUB*thetaUB2*thetaUB2;
    Frac<BigNum> sThetaUBSq=FA::dyadicBound(sThetaUB*sThetaUB,40,1);
    // Sufficient to prove:
    // s2PiOverNLBSq*(1-S^2)^2T^2
    //  >
    // sThetaUBSq [(S^2+T^2)(1+T^2)-s2PiOverNLBSq\,(S^2+T^2)^2],
    // The step sin^2(theta)*bracket <= sThetaUBSq*bracket needs the bracket to be
    // nonnegative; certify that as part of the run rather than by inspection (it is
    // about +1.19 on our range, so this check never fails in practice).
    int check0=( s2PiOverNLBSq*(S*S+T*T) < one+T*T );
    Frac<BigNum> lhs=s2PiOverNLBSq*(one-S*S)*(one-S*S)*T*T;
    Frac<BigNum> rhs=sThetaUBSq*(S*S+T*T)*(one+T*T-s2PiOverNLBSq*(S*S+T*T));
    int check3=(rhs<lhs);
    return check0*check3;
}
    template<typename BigNum> int checkInequalities(){
        int success=1;
        Frac<BigNum> S(9,20);
        Frac<BigNum> T(31,64);
        Frac<BigNum> one(1,1);
        Frac<BigNum> a=S*T;
        Frac<BigNum> c0=(one-S*S)*(one+T*T);
        Frac<BigNum> PhiSq=(one-S*S);
        PhiSq*=PhiSq;
        PhiSq*=T*T;
        PhiSq/=(S*S+T*T);
        PhiSq/=one+T*T;
        Frac<BigNum> PsiSq=one+a*a;
        PsiSq*=PsiSq;
        PsiSq=c0/PsiSq;
        std::cout<<"S="<<S.toString()<<"\n";
        std::cout<<"T="<<T.toString()<<"\n";
        std::cout<<"a="<<a.toString()<<"\n";
        std::cout<<"c0="<<c0.toString()<<"\n";
        std::cout<<"PhiSq="<<PhiSq.toString()<<"\n";
        std::cout<<"PsiSq="<<PsiSq.toString()<<"\n";
        success*=check2PhiBiggerThan21Over20(PhiSq);
        std::cout<<"Check that 2Phi> 21/20: "<<success<<"\n";
        success*=checkPsiInvBiggerThan21Over20(PsiSq);
        std::cout<<"Check that Psi^{-1}> 21/20: "<<success<<"\n";
        
        std::cout<<"uBar checks:\n";
        BigNum N;BigNum s;
        N=32;s=1;
        std::cout<<"(N,s)=("<<FA::toString(N)<<","<<FA::toString(s)<<"): ";
        success*=checkTwoConditionsForUBar(N,s,c0,a,one);
        std::cout<<success<<"\n";
        N=48;s=2;
        std::cout<<"(N,s)=("<<FA::toString(N)<<","<<FA::toString(s)<<"): ";
        success*=checkTwoConditionsForUBar(N,s,c0,a,one);
        std::cout<<success<<"\n";
        std::cout<<"(*) and (**) checking\n";
        s=1;
        for(N=20;N<32;++N){
            std::cout<<"(N,s)=("<<FA::toString(N)<<","<<FA::toString(s)<<"): ";
            int r1=star1Check(N,s,c0,a,one,2);
            success*=r1;
            std::cout<<" (*) success: "<<r1<<" ";
            r1=star2Check(N,s,S,T,one);
            success*=r1;
            std::cout<<" (**) success: "<<r1<<"\n";
        }
        s=2;
        for(N=40;N<48;++N){
            std::cout<<"(N,s)=("<<FA::toString(N)<<","<<FA::toString(s)<<"): ";
            int r1=star1Check(N,s,c0,a,one,2);
            success*=r1;
            std::cout<<" (*) success: "<<r1<<" ";
            r1=star2Check(N,s,S,T,one);
            success*=r1;
            std::cout<<" (**) success: "<<r1<<"\n";
        }
        
        if(success==1){
            std::cout<<"All pinwheel checks passed.\n";
        }
        return success;
    }
}

#endif
