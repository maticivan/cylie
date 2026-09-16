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


#ifndef __INCL_CC_CPP
#define __INCL_CC_CPP
#include <csignal>
#include <cstdio>
#include <cstdlib>
namespace CC{
// Set to 0 to refuse certificates that carry no SOS records at all, as the plan block
// prescribes for the end of the transition. While it is 1, such files are still proved by the
// old box route, so the two routes can be compared on the same file (plan item T1).
long GL_allowLegacyBoxRoute=1;
std::atomic<int> GL_stopRequested=0;
extern "C" void requestStop(int){
    // First ctrl+C: ask the worker threads to stop after the box they are on;
    // the state is then saved and the program exits cleanly.
    // Second ctrl+C: give up on the graceful exit and terminate immediately.
    if(GL_stopRequested==1){std::_Exit(130);}
    GL_stopRequested=1;
} 
template<typename BigNum>
Frac<BigNum> power(Frac<BigNum> x, long a){
    Frac<BigNum> res(1,1);
    for(long i=0;i<a;++i){res*=x;}
    return res;
}
template<typename BigNum>
Frac<BigNum> findConservativeMax(long a, long b, long c,
                                 Frac<BigNum> coeff,
                                 const CPC::Box<Frac<BigNum> >& box){
    Frac<BigNum> xV[2], yV[2],zV[2];
    xV[0]=power(box.ls[0],a);xV[1]=power(box.hs[0],a);
    yV[0]=power(box.ls[1],b);yV[1]=power(box.hs[1],b);
    zV[0]=power(box.ls[2],c);zV[1]=power(box.hs[2],c);
    Frac<BigNum> extreme1=xV[0]*yV[0]*zV[0];
    Frac<BigNum> extreme2=extreme1;
    Frac<BigNum> tmp;
    for(long i=0;i<2;++i){
        for(long j=0;j<2;++j){
            for(long k=0;k<2;++k){
                tmp=xV[i]*yV[j]*zV[k];
                if(tmp<extreme1){extreme1=tmp;}
                if(extreme2<tmp){extreme2=tmp;}
            }
        }
    }
    extreme1*=coeff;extreme2*=coeff;
    if(extreme1<extreme2){return extreme2;}
    return extreme1;
}
template<typename BigNum>
Frac<BigNum> findConservativeBound(const std::vector<std::vector<std::vector<Frac<BigNum> > > > &mP,
                             const CPC::Box<Frac<BigNum> >& b,
                             int boundTypePm1){
    Frac<BigNum> zero(0,1);
    Frac<BigNum> res=zero;
    long maxDegree=mP.size();
    for(long i=0;i<maxDegree;++i){
        for(long j=0;j<maxDegree;++j){
            for(long k=0;k<maxDegree;++k){
                if(!(mP[i][j][k]==zero)){
                    if(boundTypePm1<0){
                        Frac<BigNum> mpijk=mP[i][j][k];
                        mpijk.setNum(-mpijk.getNum());
                        res-=findConservativeMax(i,j,k,mpijk,b);
                    }
                    else{
                        res+=findConservativeMax(i,j,k,mP[i][j][k],b);
                    }
                }
            }
        }
    }
    return res;
}
template<typename BigNum>
int whichInequalityCanBeGuaranteedOnBox(const std::vector<std::vector<std::vector<Frac<BigNum> > > > &mP,
                                        const CPC::Box<Frac<BigNum> >& b){
    // returns 1 if mP>=0 is guaranteed on the box
    // returns -1 if mP<0 is guaranteed on the box
    // returns 0 if the algorithm can guarantee neither mP>=0 nor mP<0
    Frac<BigNum> conservativeUpperBound=findConservativeBound(mP,b,1);
    Frac<BigNum> conservativeLowerBound=findConservativeBound(mP,b,-1);
    if(conservativeLowerBound.getNum()>=0){return 1;}
    if(conservativeUpperBound.getNum()<0){return -1;}
    return 0;
}
template<typename BigNum>
int succeedOnOneBox(const std::vector<std::vector<std::vector<Frac<BigNum> > > > &mGh,
                    const std::vector<std::vector<std::vector<Frac<BigNum> > > > &mUh,
                    const std::vector<std::vector<std::vector<Frac<BigNum> > > > &mQh,
                    const CPC::Box<Frac<BigNum> >& b){
    if(whichInequalityCanBeGuaranteedOnBox(mUh,b)==-1){return 1;}
    if(whichInequalityCanBeGuaranteedOnBox(mQh,b)==-1){return 2;}
    if(whichInequalityCanBeGuaranteedOnBox(mGh,b)== 1){return 3;}
    return 0;
}
template<typename BigNum>
void runTestP(long threadId, long* nThreads,
              fst::stack<CPC::Box<Frac<BigNum> > >* sBoxes,
              long* numStacksForOneEll,
              fst::stack<CPC::Box<Frac<BigNum> > >* a_r_B,
              long* a_forTotalCounters,
              int* a_forSuccess,
              std::vector<std::vector<std::vector<Frac<BigNum> > > >* a_mGh,
              std::vector<std::vector<std::vector<Frac<BigNum> > > >* a_mUh,
              std::vector<std::vector<std::vector<Frac<BigNum> > > >* a_mQh,
              long* a_giveUpCounter,
              long* a_skipTxtOutput){
    long ellShiftStacks=0;
    long stackToWork;
    long writingPosition=threadId;
    int& ind_success=*a_forSuccess;
    long giveUpCounter;
    int successInd;
    CPC::Box<Frac<BigNum> > currentBox;
    for(long ell=0;ell<4;++ell){
        long& totalCounter=a_forTotalCounters[writingPosition];
        stackToWork=threadId;
        giveUpCounter=*a_giveUpCounter;
        while(stackToWork<*numStacksForOneEll){
            fst::stack<CPC::Box<Frac<BigNum> > >& remaining=sBoxes[stackToWork+ellShiftStacks];
            while((remaining.size()>0)&&(giveUpCounter>0)&&(GL_stopRequested==0)){
                    --giveUpCounter;currentBox=remaining.top();
                    successInd=succeedOnOneBox(a_mGh[ell],a_mUh[ell],a_mQh[ell],currentBox);
                    if(successInd!=0){
                        remaining.pop();
                        ++totalCounter;
                        currentBox.inequalityCode=successInd;
                        if(*a_skipTxtOutput==0){
                            (a_r_B[writingPosition]).push(currentBox);
                        }
                    }
                    else{
                        CPC::removeTopThenSplitAndAddToStack(remaining);
                    }
                }
                if(remaining.size()!=0){
                    ind_success=0;
                }
            stackToWork+=*nThreads;
        }
        ellShiftStacks+=*numStacksForOneEll;
        writingPosition+=*nThreads;
    }
}
std::string initialCommentBoxes(){
    std::string res;
    res+="This file contains the partition of the three-dimensional cube\n";
    res+="centered at origin. For each box in the partition, we write \n";
    res+="1: If we can guarantee that U_h<0\n";
    res+="2: If we can guarantee that Q_h<0\n";
    res+="3: If we can guarantee that G>=0\n";
    return res;
}
template<typename BigNum>
std::string printSetup(const CPC::TestSetup<BigNum> &ts){
    std::string res;
    for(long l=0;l<ts.array_TPolynomials.size();++l){
        long ell=l+1;
        int matrixStarted;
        res+="\\section{Term $\\ell="+std::to_string(ell)+"$}\n";
        res+="\\subsection{Matrix $D_"+std::to_string(ell)+"$}\n";
        res+="We will write the matrix $D^{-1}$ instead of $D$. The matrices $D$ and $D^{-1}$ ";
        res+="are diagonal, so one is easy to construct from the other. The matrix $D^{-1}$ is ";
        res+="more convenient because the denominators of diagonal terms are powers of $2$. Therefore, ";
        res+="their binary expressions are finite and there is no rounding in computer storage.";
        res+="\\[D_"+std::to_string(ell)+"^{-1}=\\left[\\begin{array}{";
        for(long i=0;i<2*ell+1;++i){res+="c";}
        res+="}\n";
        matrixStarted=0;
        for(long i=0;i<2*ell+1;++i){
            if(matrixStarted){res+="\\\\\n";}
            matrixStarted=1;
            int rowStarted=0;
            for(long j=0;j<2*ell+1;++j){
                if(rowStarted){
                    res+=" & ";
                }
                rowStarted=1;
                if(i==j){
                    res+=PA::printDiadicRational(ts.array_invWDiagonals[l][i]);
                }
                else{
                    res+=std::to_string(0);
                }
            }
        }
        res+="\n\\end{array}\\right].\\]\n";
        res+="\\subsection{Matrix $C_"+std::to_string(ell)+"$}\n";
        res+="\\[C_"+std::to_string(ell)+"=\\left[\\begin{array}{";
        for(long i=0;i<2*ell+1;++i){res+="c";}
        res+="}\n";
        matrixStarted=0;
        for(long i=0;i<2*ell+1;++i){
            if(matrixStarted){res+="\\\\\n";}
            matrixStarted=1;
            int rowStarted=0;
            for(long j=0;j<2*ell+1;++j){
                if(rowStarted){
                    res+=" & ";
                }
                rowStarted=1;
                res+=PA::printDiadicRational(ts.array_C[l][i][j]);
            }
        }
        res+="\n\\end{array}\\right].\\]\n";
        res+="\\subsection{Matrix $T_{h}^"+std::to_string(ell)+"(q_0,q_1,q_2,q_3)$}\n";
        for(long i=0;i<2*ell+1;++i){
            matrixStarted=1;
            for(long j=0;j<2*ell+1;++j){
                res+="\\begin{align*}(T_{h}^"+std::to_string(ell)+")_{"+std::to_string(i+1)+","+std::to_string(j+1)+"}&=";
                res+=(ts.array_TPolynomials[l][i][j]).toString();
                res+="\\end{align*}\n";
            }
        }
    }
    res+="\\section{Polynomial $U_h$}\n";
    res+="\\begin{align*}U_h(q_0,q_1,q_2,q_3)&="+ts.pol_Uh.toString()+"\\end{align*}\n";
    res+="\\section{Polynomial $Q_h$}\n";
    res+="\\begin{align*}Q_h(q_0,q_1,q_2,q_3)&="+ts.pol_Qh.toString()+"\\end{align*}\n";
    res+="\\section{Polynomial $G$}\n";
    res+="We store $denominator(M)*G$ instead of $G$ in the memory because $denominator(M)*G$ can have all of its coefficients stored with full precision, without rounding. Recall that computers work in binary and for them, $\\frac15$ is an infinite expression that must be rounded. Computers can only tolerate denominators that are powers of two.\n";
    res+="\\begin{align*}denominator(M)*G(q_0,q_1,q_2,q_3)&="+ts.pol_G.toString()+"\\end{align*}\n";
    return res;
}
template<typename BigNum>
std::string polXYZSummary(const std::string& polName,const PA::Polynomial<Frac<BigNum> >& pol){
    std::string res;
    ssm::set<std::string> avr= PA::allVariables(pol);
    std::map<std::string,std::string> replMap;
    if(avr.size()!=3){return "Error with polynomial " +polName+ ".\n";}
    std::string vNames[]={"x","y","z"};
    for(long i=0;i<3;++i){
        replMap[avr[i]]=vNames[i];
    }
    res+="\\subsection{Polynomial $"+polName+"(x,y,z)$}\n";
    res+="\\begin{align*}"+polName+"(x,y,z)&="+MFRF::findAndReplace(pol.toString(),replMap)+"\\end{align*}";
    return res;
}
template<typename BigNum>
void splitUntilAcceptable(fst::stack<CPC::Box<Frac<BigNum> > >& allBoxes,
                          const CPC::Box<Frac<BigNum> >& boxToSplit,
                          Frac<BigNum> targetSize){
    if( !(targetSize<boxToSplit.hs[0]-boxToSplit.ls[0])){
        allBoxes.push(boxToSplit);
        return;
    }
    fst::stack<CPC::Box<Frac<BigNum> > > oneSplit(8);
    oneSplit.push(boxToSplit);
    CPC::removeTopThenSplitAndAddToStack(oneSplit);
    while(!oneSplit.empty()){
        splitUntilAcceptable(allBoxes,oneSplit.top(),targetSize);
        oneSplit.pop();
    }
}
template<typename BigNum>
std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long> splitIntoBoxes(int exponentOfDenominator,int numCopies){
    // .second of the return value will be the number of stacks in one of the numCopies
    // .first will be an array of stacks of length (.second)*numCopies
    std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long> res;
    CPC::Box<Frac<BigNum> > bigBox;
    Frac<BigNum> targetSize(1);Frac<BigNum> oneHalf(1,2);
    long numInOneCopy=1;
    for(int j=0;j<exponentOfDenominator;++j){
        targetSize*=oneHalf;
        numInOneCopy*=8;
    }
    numInOneCopy*=8;
    res.second=numInOneCopy;
    long total=numInOneCopy*numCopies;
    res.first=new fst::stack<CPC::Box<Frac<BigNum> > >[total];
    fst::stack<CPC::Box<Frac<BigNum> > > allBoxes(numInOneCopy);
    splitUntilAcceptable(allBoxes,bigBox,targetSize);
    std::vector<long> permutation=RNDF::genMTwistPermutation(numInOneCopy);
    long i=0;
    while(i<numInOneCopy){
        long j=permutation[i];
        while(j<total){
            ((res.first)[j]).push(allBoxes.top());
            j+=numInOneCopy;
        }
        allBoxes.pop();
        ++i;
    }
    return res;
}
// ------------------------------------------------------------------ saved state
//
// After every attempt, the remaining boxes are saved to scDest_boxes.txt and the
// progress counters to scDest_counters.txt. The next run loads the saved boxes
// instead of generating them with splitIntoBoxes.
//
// Every coordinate that ever appears in a box is dyadic: the initial cube has
// corners at +-1 and boxes are only ever halved. A coordinate is therefore saved
// as "num/k", meaning num/2^k, with both integers in plain decimal. This is
// exact: the saved state reloads to bit-identical Frac values.
//
// A box is saved as [[l0][h0]][[l1][h1]][[l2][h2]][code], the same shape that
// the partition printout in resB uses. A stack is [box][box]...[box]. The file
// scDest_boxes.txt is [numStacksForOneEll][total][stack]...[stack].
//
// The file scDest_counters.txt is [c0][c1][c2][c3][v0][v1][v2][v3]: the number
// of certified boxes and the remaining (not yet certified) volume, per value of
// ell. On load, the volume of the loaded boxes is recomputed and compared, per
// ell, with the saved v_ell; any mismatch means the boxes file does not describe
// the same region it described when it was saved, and the program refuses to
// resume. This is what makes a truncated or corrupted saved state fail loudly
// instead of producing a vacuous success over a partial cover of the cube.
template<typename BigNum>
BigNum stringToBigNum(const std::string& _in, int* a_ok){
    BigNum res=0;BigNum ten=10;
    long pos=0;long len=_in.length();
    int neg=0;
    while((pos<len)&&((_in[pos]==' ')||(_in[pos]=='\n')||(_in[pos]=='\t'))){++pos;}
    if((pos<len)&&(_in[pos]=='-')){neg=1;++pos;}
    if(pos>=len){*a_ok=0;return res;}
    while(pos<len){
        char c=_in[pos];
        if((c<'0')||(c>'9')){*a_ok=0;return res;}
        BigNum digit=c-'0';
        res=FA::addBigNumbers(FA::multiplyBigNumbers(res,ten),digit);
        ++pos;
    }
    if(neg){BigNum zero=0;res=FA::subBigNumbers(zero,res);}
    return res;
}
template<typename BigNum>
std::string dyadicToString(const Frac<BigNum>& x, int* a_ok){
    if(!x.isDyadic()){*a_ok=0;return "0/0";}
    return FA::toString(x.getNum())+"/"+std::to_string(x.getLog2Den());
}
template<typename BigNum>
Frac<BigNum> dyadicFromString(const std::string& _in, int* a_ok){
    Frac<BigNum> res(0,1);
    long len=_in.length();long slashPos=-1;
    for(long i=0;i<len;++i){
        if(_in[i]=='/'){slashPos=i;break;}
    }
    if((slashPos<1)||(slashPos==len-1)){*a_ok=0;return res;}
    BigNum num=stringToBigNum<BigNum>(_in.substr(0,slashPos),a_ok);
    int okExp=1;
    BigNum kBig=stringToBigNum<BigNum>(_in.substr(slashPos+1),&okExp);
    if((okExp==0)||(kBig<0)||(kBig>100000)){*a_ok=0;return res;}
    long k=static_cast<long>(kBig);
    res=Frac<BigNum>(num,1);
    Frac<BigNum> oneHalf(1,2);
    for(long i=0;i<k;++i){res*=oneHalf;}
    return res;
}
template<typename BigNum>
Frac<BigNum> boxVolume(const CPC::Box<Frac<BigNum> >& b){
    Frac<BigNum> res=b.hs[0]-b.ls[0];
    res*=(b.hs[1]-b.ls[1]);
    res*=(b.hs[2]-b.ls[2]);
    return res;
}
template<typename BigNum>
std::string boxToString(const CPC::Box<Frac<BigNum> >& b, int* a_ok){
    std::string res;
    for(long i=0;i<3;++i){
        res+="[["+dyadicToString(b.ls[i],a_ok)+"][";
        res+=dyadicToString(b.hs[i],a_ok)+"]]";
    }
    res+="["+std::to_string(b.inequalityCode)+"]";
    return res;
}
template<typename BigNum>
CPC::Box<Frac<BigNum> > getBoxFromString(const std::string & _in, int* a_ok){
    CPC::Box<Frac<BigNum> > res;
    std::vector<std::string> v=SF::stringToVector(_in,"[","]");
    if(v.size()<3){*a_ok=0;return res;}
    for(long i=0;i<3;++i){
        std::vector<std::string> lh=SF::stringToVector(v[i],"[","]");
        if(lh.size()!=2){*a_ok=0;return res;}
        res.ls[i]=dyadicFromString<BigNum>(lh[0],a_ok);
        res.hs[i]=dyadicFromString<BigNum>(lh[1],a_ok);
        if(!(res.ls[i]<res.hs[i])){*a_ok=0;return res;}
    }
    if(v.size()>3){res.inequalityCode=static_cast<int>(BF::stringToInteger(v[3]));}
    return res;
}
template<typename BigNum>
fst::stack<CPC::Box<Frac<BigNum> > > getStackFromString(const std::string& _in,
                                                        Frac<BigNum>* a_volume,
                                                        int* a_ok){
    fst::stack<CPC::Box<Frac<BigNum> > > res;
    std::vector<std::string> v=SF::stringToVector(_in,"[","]");
    for(long i=0;i<v.size();++i){
        CPC::Box<Frac<BigNum> > b=getBoxFromString<BigNum>(v[i],a_ok);
        if(*a_ok==0){return res;}
        *a_volume+=boxVolume(b);
        res.push(b);
    }
    return res;
}
template<typename BigNum>
std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long> loadBoxesFromString( const std::string& _in,
                                                                           Frac<BigNum>* a_remVol,
                                                                           int* a_ok){
    std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long> res;
    res.first=nullptr;
    res.second=0;
    Frac<BigNum> zero(0,1);
    for(long ell=0;ell<4;++ell){a_remVol[ell]=zero;}
    std::vector<std::string> v0=SF::stringToVector(_in,"[","]");
    if(v0.size()<3){*a_ok=0;return res;}
    long total=BF::stringToInteger(v0[1]);
    if(total!=v0.size()-2){*a_ok=0;return res;}
    res.second=BF::stringToInteger(v0[0]);
    if((res.second<1)||(total!=4*res.second)){*a_ok=0;res.second=0;return res;}
    res.first=new fst::stack<CPC::Box<Frac<BigNum> > >[total];
    for(long i=0;i<total;++i){
        (res.first)[i]=getStackFromString<BigNum>(v0[i+2],a_remVol+(i/res.second),a_ok);
    }
    if(*a_ok==0){
        delete[] res.first;
        res.first=nullptr;
        res.second=0;
    }
    return res;
}
template<typename BigNum>
int saveBoxesToFile(const std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long>& bSplit,
                    const std::string& fileName,
                    Frac<BigNum>* a_remVol){
    // Fills a_remVol[0..3] with the remaining volume per ell as a side product,
    // so that saveCountersToFile can record it. Draining a copy of each stack
    // leaves the originals untouched.
    long numStacksForOneEll=bSplit.second;
    long total=4*numStacksForOneEll;
    Frac<BigNum> zero(0,1);
    for(long ell=0;ell<4;++ell){a_remVol[ell]=zero;}
    int ok=1;
    std::string res;
    res+="["+std::to_string(numStacksForOneEll)+"]\n";
    res+="["+std::to_string(total)+"]\n";
    for(long i=0;i<total;++i){
        res+="[";
        fst::stack<CPC::Box<Frac<BigNum> > > cpy=(bSplit.first)[i];
        while(!cpy.empty()){
            CPC::Box<Frac<BigNum> > b=cpy.top();
            cpy.pop();
            a_remVol[i/numStacksForOneEll]+=boxVolume(b);
            res+="["+boxToString(b,&ok)+"]\n";
        }
        res+="]\n";
    }
    if(ok==0){return 0;}
    return IOF::toFile(fileName,res);
}
template<typename BigNum>
int saveCountersToFile(const long* aTotalCounters, long nThreads,
                       const Frac<BigNum>* a_remVol,
                       const std::string& fileName){
    int ok=1;
    std::string res;
    for(long ell=0;ell<4;++ell){
        long cEll=0;
        for(long i=0;i<nThreads;++i){cEll+=aTotalCounters[ell*nThreads+i];}
        res+="["+std::to_string(cEll)+"]\n";
    }
    for(long ell=0;ell<4;++ell){
        res+="["+dyadicToString(a_remVol[ell],&ok)+"]\n";
    }
    if(ok==0){return 0;}
    return IOF::toFile(fileName,res);
}
template<typename BigNum>
long* loadCountersFromFile(const std::string& _in, long nThreads,
                           Frac<BigNum>* a_savedRemVol,
                           int* a_ok){
    // Returns a zeroed 4*nThreads array with the carried per-ell counts seeded
    // into slot ell*nThreads, which is where thread 0 accumulates; this makes
    // the carried counts survive a change in the number of threads.
    long* res=new long[4*nThreads];
    for(long i=0;i<4*nThreads;++i){res[i]=0;}
    std::vector<std::string> v=SF::stringToVector(_in,"[","]");
    if(v.size()!=8){*a_ok=0;return res;}
    for(long ell=0;ell<4;++ell){
        long cEll=BF::stringToInteger(v[ell]);
        if(cEll<0){*a_ok=0;return res;}
        res[ell*nThreads]=cEll;
        a_savedRemVol[ell]=dyadicFromString<BigNum>(v[4+ell],a_ok);
    }
    return res;
}
template<typename BigNum>
void reDistributeStacks(fst::stack<CPC::Box<Frac<BigNum> > >* sArray,long len){
    fst::stack<CPC::Box<Frac<BigNum> > > allBoxes(1000);
    for(long i=0;i<len;++i){
        while(sArray[i].size()>0){
            allBoxes.push(sArray[i].top());
            sArray[i].pop();
        }
    }
    long i=0;
    while(allBoxes.size()>0){
        sArray[i].push(allBoxes.top());
        allBoxes.pop();
        ++i;
        i%=len;
    }
    return;
}
template<typename BigNum>
void reDistributeStacks(std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long>& bSplit,int numCopies){
    // bSplit.second is ALREADY the number of stacks in ONE copy (one chart);
    // the array has numCopies*bSplit.second stacks in total. Boxes must never
    // cross a chart boundary: a box redistributed into another chart would be
    // tested against the wrong polynomials, and the saved-state volume check
    // would not detect it.
    if(numCopies<1){return;}
    long numInOneCopy=bSplit.second;
    long shift=0;
    for(long i=0;i<numCopies;++i){
        reDistributeStacks(bSplit.first+shift,numInOneCopy);
        shift+=numInOneCopy;
    }
    return;
}
template<typename BigNum>
std::string printStackSizes(std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long>& bSplit,int numCopies){
    if(numCopies<1){return "";}
    std::string res;
    long numInOneCopy=bSplit.second;
    long shift=0;
    for(long i=0;i<numCopies;++i){
        for(long j=0;j<numInOneCopy;++j){
            res+= std::to_string((bSplit.first)[shift+j].size())+" ";
            if(j%20==19){res+="\n";}
        }
        res+="\n\n";
        shift+=numInOneCopy;
    }
    return res;
}
// The theorem, printed from the verified records only (targetBound, tauSqNum/Den), never from
// any text in the file. Called exactly once, immediately before a success return, so that it is
// the last thing printed before main()'s closing lines and the PASS line.
template<typename BigNum>
BigNum exactSqrtOrZero(const BigNum& n){
    // returns r with r*r==n if n is a perfect square, else 0 (n>0 assumed)
    if(n<=0){return 0;}
    BigNum lo=0,hi=n; BigNum one=1;
    while(lo<hi){
        BigNum mid=(lo+hi)/2;
        if(mid*mid<n){lo=mid+one;}else{hi=mid;}
    }
    return (lo*lo==n)?lo:BigNum(0);
}
template<typename BigNum>
std::string omegaDescription(const CPC::TestSetup<BigNum>& ts,int plainOmega=0){
    BigNum A=ts.tauSq.getNum(), B=ts.tauSq.getDen();
    BigNum a=exactSqrtOrZero(A), b=exactSqrtOrZero(B);
    double tauD=std::sqrt(std::stod(FA::toString(A))/std::stod(FA::toString(B)));
    double omD=tauD/(2.0-tauD);
    char buf[80]; std::snprintf(buf,sizeof(buf),"%.12f",omD);
    std::string res;
    if(a!=0&&b!=0){
        // tau = a/b exactly, omega = a/(2b-a)
        BigNum on=a, od=2*b-a;
        if(plainOmega==1){
            return FA::toString(on)+"/"+FA::toString(od);
        }
        res="omega = "+FA::toString(on)+"/"+FA::toString(od)+" = "+std::string(buf)+"...";
        res+="  (tau = "+FA::toString(a)+"/"+FA::toString(b)+")";
    }
    else{
        res="omega = tau/(2-tau) = "+std::string(buf)+"...  (tau = sqrt("+FA::toString(A)+"/"+FA::toString(B)+"))";
    }
    return res;
}
template<typename BigNum>
void printTheoremStatement(const CPC::TestSetup<BigNum>& ts){
    std::string omegaDesc=omegaDescription(ts);
    std::cout<<"\nTHEOREM. No more than "<<ts.targetBound<<" pairwise non-overlapping infinite circular\n";
    std::cout<<"cylinders of radius "<<omegaDesc<<"\n";
    std::cout<<"can simultaneously touch the unit ball.\n";
    std::cout<<"In the notation of the paper: ("<<ts.targetBound<<","<<omegaDescription(ts,1)<<")\\in\\Pcyl\n";
    std::cout<<"Canonical certificate name: cyl_N"<<ts.targetBound<<"_omega";
    {
        BigNum A=ts.tauSq.getNum(), B=ts.tauSq.getDen();
        BigNum a=exactSqrtOrZero(A), b=exactSqrtOrZero(B);
        if(a!=0&&b!=0){ std::cout<<FA::toString(a)<<"_"<<FA::toString(2*b-a); }
        else{ std::cout<<"sqrt"<<FA::toString(A)<<"_"<<FA::toString(B); }
    }
    std::cout<<".txt\n";
    if(CPC::GL_skipMultVerification||CPC::GL_skipOrthVerification){
        std::cout<<"(Conditional: skipMultVerification/skipOrthVerification is \"yes\", so the table\n";
        std::cout<<"identities that check would establish are assumed, not verified.)\n";
    }
}
template<typename BigNum>
int mainWorkFromMap(const std::map<std::string,std::string> & mainMap, const std::string& scDest){
    CPC::TestSetup<BigNum> ts=CPC::getFromMap<BigNum>(mainMap);
    if(ts.success==0){std::cout<< "Failed.\n";return 0;}
    // Hypothesis (H6), G>=0 on K_h, is established either by the sum-of-squares identity or,
    // for certificates emitted before the SOS records existed, by box subdivision. A file that
    // carries any SOS record is verified only by the SOS route: falling back for a file with a
    // missing or damaged SOS record would let a tampered certificate reach the PASS line by
    // another path.
    if(SOSC::isSOSCertificate(mainMap)){
        int sosSuccess=SOSC::verifyCertificate(mainMap,ts);
        std::string resSOS=printSetup(ts);
        resSOS+=SOSC::printSOSSummary(mainMap,ts);
        IOF::toFile(scDest+".tex",resSOS);
        if(sosSuccess==0){std::cout<<"Failed.\n";}
        else{printTheoremStatement(ts);}
        return sosSuccess;
    }
    if(GL_allowLegacyBoxRoute==0){
        std::cout<<"This is a pre-SOS certificate: it carries no sum-of-squares records.\n";
        std::cout<<"Re-emit it with the generator; certificates are never patched by hand.\n";
        std::cout<<"Failed.\n";
        return 0;
    }
    std::cout<<"Pre-SOS certificate: hypothesis (H6) will be checked by box subdivision.\n";
    long nThreads=std::thread::hardware_concurrency();
    if(nThreads==0){nThreads=1;}
    std::cout<<"Partitioning the box and verifying the inequalities. This may take a while.\n";
    if(nThreads==1){
        std::cout<<"Your CPU has only one core. This may take up to 20 minutes.\n";
    }
    else{
        std::cout<<"Your CPU has multiple cores. I'll use them.\nNumber of threads that ";
        std::cout<<"are available to me is ";
        std::cout<<nThreads<<"\n";
    }
    TMF::Timer tm;
    tm.start();
    std::cout<<"Allocating memory.\n";
    std::string res,resB;
    long totalCounter=0;
    res+=printSetup(ts);
    resB+=initialCommentBoxes();
    std::vector<std::vector<std::vector<Frac<BigNum> > > >* mGh = new std::vector<std::vector<std::vector<Frac<BigNum> > > >[4];
    std::vector<std::vector<std::vector<Frac<BigNum> > > >* mUh = new std::vector<std::vector<std::vector<Frac<BigNum> > > >[4];
    std::vector<std::vector<std::vector<Frac<BigNum> > > >* mQh = new std::vector<std::vector<std::vector<Frac<BigNum> > > >[4];
    PA::Polynomial<Frac<BigNum> >* a_pUh3 = new PA::Polynomial<Frac<BigNum> >[4];
    PA::Polynomial<Frac<BigNum> >* a_pQh3 = new PA::Polynomial<Frac<BigNum> >[4];
    PA::Polynomial<Frac<BigNum> >* a_pG3  = new PA::Polynomial<Frac<BigNum> >[4];
    for(long i=0;i<4;++i){
        std::map<std::string,Frac<BigNum> > vEval;
        vEval["q_"+std::to_string(i)]=1;
        a_pUh3[i]=ts.pol_Uh.evaluate(vEval);
        a_pQh3[i]=ts.pol_Qh.evaluate(vEval);
        a_pG3[i]=ts.pol_G.evaluate(vEval);
        mGh[i]=PA::polMatrix(a_pG3[i]);
        mUh[i]=PA::polMatrix(a_pUh3[i]);
        mQh[i]=PA::polMatrix(a_pQh3[i]);
    }
    //presplitting to side  1/4 is lossless: an instrumented run of the adaptive algorithm showed
    //the largest box ever certified has side 1/4 on every face, so the adaptive tree contains the
    //uniform depth-3 tree.
        std::string boxesName=scDest+"_boxes.txt";
        std::string countersName=scDest+"_counters.txt";
        std::pair<fst::stack<CPC::Box<Frac<BigNum> > >*,long> bSplit;
        bSplit.first=nullptr;bSplit.second=0;
        long* aTotalCounters=nullptr;
        if(IOF::fileExists(boxesName)||IOF::fileExists(countersName)){
            std::cout<<"Found saved state. Loading "<<boxesName<<" instead of generating boxes.\n";
            int ok=1;
            if(!(IOF::fileExists(boxesName)&&IOF::fileExists(countersName))){ok=0;}
            Frac<BigNum> loadedRemVol[4];
            Frac<BigNum> savedRemVol[4];
            if(ok){
                bSplit=loadBoxesFromString<BigNum>(IOF::fileToString(boxesName),loadedRemVol,&ok);
                aTotalCounters=loadCountersFromFile<BigNum>(IOF::fileToString(countersName),nThreads,savedRemVol,&ok);
            }
            if(ok){
                for(long ell=0;ell<4;++ell){
                    if(!(loadedRemVol[ell]==savedRemVol[ell])){ok=0;}
                }
            }
            if((FA::GL_OVERFLOW)||(FA::GL_DIVBY0)){ok=0;}
            if((ok==0)||(bSplit.first==nullptr)){
                std::cout<<"The saved state is damaged or incomplete: the volume of the loaded\n";
                std::cout<<"boxes does not match the volume recorded at the time of saving.\n";
                std::cout<<"Refusing to resume. Delete "<<boxesName<<" and\n";
                std::cout<<countersName<<" to restart the verification from the beginning.\n";
                delete[] bSplit.first;
                delete[] aTotalCounters;
                delete[] mGh;delete[] mUh;delete[] mQh;
                return 0;
            }
            if(CPC::GL_skipTxtOutput==0){
                CPC::GL_skipTxtOutput=1;
                std::cout<<"Note: the box-by-box printout cannot include boxes certified in\n";
                std::cout<<"previous runs, so txt output is turned off for this resumed run.\n";
            }
            long carried=0;
            for(long i=0;i<4*nThreads;++i){carried+=aTotalCounters[i];}
            std::cout<<"Resuming. Boxes certified in previous runs: "<<carried<<".\n";
        }
        else{
            bSplit=splitIntoBoxes<BigNum>(2,4);
            aTotalCounters=new long[4*nThreads];
            for(long i=0;i<4*nThreads;++i){aTotalCounters[i]=0;}
        }
    std::thread* allTh=new std::thread[nThreads];
    fst::stack<CPC::Box<Frac<BigNum> > >* a_Res_B=nullptr;
    if(CPC::GL_skipTxtOutput==0){
        a_Res_B=new fst::stack<CPC::Box<Frac<BigNum> > >[4*nThreads];
        for(long i=0;i<4*nThreads;++i){
            a_Res_B[i].setFastStorageCapacity(200000);
        }
    }
    int* aSuccesses=new int[nThreads];
    long session=0; 
    long maxNumSessions=10000;long maxWorkInSession=10000;
    int success=0;
    int arithmeticTrouble=0;
    tm.end();
    std::cout<<"Memory allocation done. Took "<<tm.getTimeSec()<<" seconds.\n";
    std::signal(SIGINT,requestStop);
    while((success==0)&&(session<maxNumSessions)&&(GL_stopRequested==0)){
        ++session;
        tm.start();
        reDistributeStacks(bSplit,4);
        for(long i=0;i<nThreads;++i){aSuccesses[i]=1;}
        for(long i=0;i<nThreads;++i){
            allTh[i]=std::thread(
                                    runTestP<BigNum>,
                                    i,&nThreads,
                                    bSplit.first,
                                    &(bSplit.second),
                                    a_Res_B,
                                    aTotalCounters,aSuccesses+i,
                                    mGh, mUh, mQh,&maxWorkInSession,
                                    &(CPC::GL_skipTxtOutput)
                                );
        }
        success=1;
        for(long i=0;i<nThreads;++i){
            allTh[i].join();
            success*=aSuccesses[i];
        }
        long doneSoFar=0;
        for(long i=0;i<4*nThreads;++i){doneSoFar+=aTotalCounters[i];}
        std::cout<<"Attempt "<<session<<" finished. Certified boxes so far: "<<doneSoFar;
        tm.end();
        std::cout<<".\nThis attempt took "<<tm.getTimeSec()<<" seconds. ";
        if(success==1){
            std::cout<<"SUCCESS! Cleaning up.";
        }
        else{
            std::cout<<"Still not done.";
        }
        std::cout<<"\n";
        if((FA::GL_OVERFLOW)||(FA::GL_DIVBY0)||(PA::GL_errorWithMonomials)){
            arithmeticTrouble=1;
            success=0;
            std::cout<<"WARNING: an arithmetic flag was raised during this attempt. The\n";
            std::cout<<"results of this attempt are not trustworthy and will NOT be saved.\n";
            std::cout<<"The saved state on disk is from the last attempt that finished\n";
            std::cout<<"cleanly. This certificate may need a wider integer type.\n";
            break;
        }
        if(success==0){
            Frac<BigNum> remVol[4];
            int okSave=saveBoxesToFile(bSplit,boxesName,remVol);
            okSave*=saveCountersToFile<BigNum>(aTotalCounters,nThreads,remVol,countersName);
            if(okSave){
                std::cout<<"State saved.\n";
            }
            else{
                std::cout<<"WARNING: saving the state failed. If the run is interrupted now,\n";
                std::cout<<"it will resume from the previous save, or from the beginning.\n";
            }
        }
    }
    tm.start();
    if(success){
        for(long i=0;i<nThreads;++i){
            for(long ell=0;ell<4;++ell){
                totalCounter+=aTotalCounters[i+ell*nThreads];
            }
        }
        std::remove(boxesName.c_str());
        std::remove(countersName.c_str());
    }
    else if(arithmeticTrouble){
        std::cout<<"Stopped because of an arithmetic flag. Nothing from the failed\n";
        std::cout<<"attempt was saved.\n";
    }
    else if(GL_stopRequested){
        std::cout<<"Interrupted by the user. The state is saved; running the same\n";
        std::cout<<"command again will resume the verification where it stopped.\n";
    }
    else{
        std::cout<<"Giving up after "<<maxNumSessions<<" attempts.\n";
    }
    long base=0;
    for(long ell=0;ell<4;++ell){
        resB+="\n\n\n*** Substitution q_"+std::to_string(ell)+"=1 ***\n\n";
        res+="\\section{Substitution $q_"+std::to_string(ell)+"=1$"+"}\n";
        res+=polXYZSummary("U_h",a_pUh3[ell]);
        res+=polXYZSummary("Q_h",a_pQh3[ell]);
        res+=polXYZSummary("G",a_pG3[ell]);
        if(CPC::GL_skipTxtOutput==0){
            long counterPartition=0;
            ssm::set<CPC::Box<Frac<BigNum> > > sAll;
            for(long i=0;i<nThreads;++i){
                while(!((a_Res_B[base+i]).empty())){
                    sAll.insert((a_Res_B[base+i]).top());
                    (a_Res_B[base+i]).pop();
                }
                counterPartition+=aTotalCounters[base+i];
            }
            for(long i=0;i<sAll.size();++i){
                CPC::Box<Frac<BigNum> > currentBox=sAll[i];
                resB+="[";
                for(long i=0;i<3;++i){
                    resB+="[["+PA::printDiadicRational(currentBox.ls[i])+"][";
                    resB+=PA::printDiadicRational(currentBox.hs[i])+"]]";
                }
                resB+="["+std::to_string(currentBox.inequalityCode)+"]";
                resB+="]\n";
            }
            resB+="Total number of boxes in the partition = "+std::to_string(counterPartition)+"\n\n";
        }
        base+=nThreads;
    }
    delete[] aSuccesses;
    delete[] aTotalCounters;
    if(CPC::GL_skipTxtOutput==0){delete[] a_Res_B;}
    delete[] allTh;
    delete[] bSplit.first;
    delete[] mGh;
    delete[] mUh;
    delete[] mQh;
    IOF::toFile(scDest+".tex",res);
    if(CPC::GL_skipTxtOutput==0){IOF::toFile(scDest+".txt",resB);}
    tm.end();
    std::cout<<"Cleaning and summary took "<<tm.getTimeSec()<<" seconds.\n";
    std::cout<< "Success = "+std::to_string(success)
            +"\nTotal number of certified boxes: "
            +std::to_string(totalCounter)+"\n";
    if(success){printTheoremStatement(ts);}
    return success;
}
int mainWork(const std::string& _srcTxt, const std::string& scDest){
    std::map<std::string,std::string> mainMap=SF::stringToMap(_srcTxt,"[key]","[/key]","[value]","[/value]");
    long L=CPC::getNumber(mainMap,"Lmax");
    if(L<1){std::cout<<"Failed. No Lmax in the source file.\n";return 0;}
    if(SOSC::isSOSCertificate(mainMap)){
        // The SOS stage multiplies deposited Gram entries (measured up to 2^61 on the R4one
        // certificate) by generator coefficients and accumulates over the basis, so the width
        // depends on k as well as on L, and __int128 is not safe even at L=4. Everything from
        // here on therefore runs at Int256 or wider.
        long k=CPC::getNumber(mainMap,"sosExponentK");
        if(L+k<9){return mainWorkFromMap<Int256>(mainMap,scDest);}
        return mainWorkFromMap<Int512>(mainMap,scDest);
    }
    if(L<6){return mainWorkFromMap<__int128>(mainMap,scDest);}
    if(L<9){return mainWorkFromMap<Int256>(mainMap,scDest);}
    return mainWorkFromMap<Int512>(mainMap,scDest);
}
}

#endif
