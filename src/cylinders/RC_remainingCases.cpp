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


#ifndef __INCL_RC_CPP
#define __INCL_RC_CPP
namespace RC{ 
    std::atomic<int> GL_RemVerificationError=0;
    template<typename BigNum> Frac<BigNum> calculate_rN(long n){
        Frac<BigNum> res;
        Frac<BigNum> Yn(710,113*n);
        Frac<BigNum> YnSq=Yn;
        YnSq*=Yn;
        Frac<BigNum> one(1,1);
        Frac<BigNum> oneOver360(1,360);
        Frac<BigNum> oneOver12(1,12);
        res=YnSq*oneOver360-oneOver12;
        res*=YnSq;
        res+=one;
        res*=YnSq;
        return res;
    }
    template<typename BigNum> struct Line{
    public:
        Frac<BigNum> p[3];
        Frac<BigNum> v[3];
        Line();
        void fromSevenIntegers(const std::vector<BigNum>& );
    };
    template<typename BigNum>
    Line<BigNum>::Line(){
        Frac<BigNum> one(1,1);
        Frac<BigNum> zero(0,1);
        p[0]=one;p[1]=zero;p[2]=0;
        v[0]=zero;v[1]=one;v[2]=zero;
    }
    template<typename BigNum> 
    void Line<BigNum>::fromSevenIntegers(const std::vector<BigNum>& v7){
        long sz=v7.size();
        if(sz!=7){GL_RemVerificationError=1;return;}
        BigNum sumSq= FA::multiplyBigNumbers(v7[0],v7[0]);
        sumSq+= FA::multiplyBigNumbers(v7[1],v7[1]);
        sumSq+= FA::multiplyBigNumbers(v7[2],v7[2]);
        if(sumSq!=FA::multiplyBigNumbers(v7[3],v7[3])){
            GL_RemVerificationError=2;return;
        }
        BigNum dotProductCheck=0,tmp;
        for(int i=0;i<3;++i){
            tmp=FA::multiplyBigNumbers(v7[i],v7[4+i]);
            dotProductCheck=FA::addBigNumbers(dotProductCheck,tmp);
        }
        if(dotProductCheck!=0){
            GL_RemVerificationError=3;return;
        }
        if(v7[3]<=0){
            GL_RemVerificationError=4;return;
        }
        if((v7[4]==0)&&(v7[5]==0)&&(v7[6]==0)){
            GL_RemVerificationError=5;return;
        }
        for(int i=0;i<3;++i){ 
            p[i]=Frac<BigNum>(v7[i],v7[3]); 
        } 
        for(int i=0;i<3;++i){ 
            v[i]=Frac<BigNum>(v7[4+i],1);  
        }
    }
    template<typename BigNum> Frac<BigNum> distSquare(const Line<BigNum>& ell1, const Line<BigNum>& ell2){
        Frac<BigNum> zero;
        std::vector<Frac<BigNum> > v1{zero,zero,zero}, v2{zero,zero,zero};
        std::vector<Frac<BigNum> > diff{zero,zero,zero};
        for(long i=0;i<3;++i){v1[i]=ell1.v[i];v2[i]=ell2.v[i];diff[i]=ell1.p[i]-ell2.p[i];}
        std::vector<Frac<BigNum> > cP=CPC::crossProduct(v1,v2);
        if((cP[0]==zero)&&(cP[1]==zero)&&(cP[2]==zero)){
            Frac<BigNum> dp=CPC::dotProduct(diff,v1);
            dp/=CPC::dotProduct(v1,v1);
            std::vector<Frac<BigNum> > distV{zero,zero,zero};
            for(long i=0;i<3;++i){
                distV[i]=diff[i]-dp*v1[i];
            }
            return CPC::dotProduct(distV,distV);
        }
        Frac<BigNum> dist=CPC::dotProduct(cP,diff);
        return (dist*dist)/CPC::dotProduct(cP,cP);
    }


    template<typename BigNum> 
    std::vector<Line<BigNum> > getLinesFromString(const std::string& in){
        std::vector<std::string> v=SF::stringToVector(in,CPC::GL_openTag, CPC::GL_closeTag);
        std::vector<Line<BigNum> > res;
        res.resize(v.size());
        for(long i=0;i<res.size();++i){
            res[i].fromSevenIntegers( CPC::getBNVector<BigNum>( SF::stringToVectorLong(v[i],CPC::GL_openTag,CPC::GL_closeTag) ) );
        }
        return res;
    }
 
    template<typename BigNum> int checkCase(long n){
        std::string fName="cWindows/cyl_window_N"+std::to_string(n+1)+".txt";
        std::vector<Line<BigNum> > lns=getLinesFromString<BigNum>( IOF::fileToString(fName) );
        Frac<BigNum> rN=FA::dyadicBound(calculate_rN<BigNum>(n),140,1);
        if(lns.size()!=n+1){
            GL_RemVerificationError=100*n+5;
            std::cout<<"Problem with "<<fName<<"\n";
            return 0;
        }
        if(GL_RemVerificationError!=0){
            std::cout<<"Problem with "<<fName<<" error code:"<<GL_RemVerificationError<<"\n";
            return 0;
        }
        for(long i=0;i<n;++i){
            for(long j=i+1;j<n+1;++j){
                if(FA::dyadicBound(distSquare(lns[i],lns[j]),140,0)<rN){
                    std::cout<<"-Failed at "<<n<<"\n";
                    return 0;
                }
            }
        }
        std::cout<<fName<<" success!\n";
        return 1;
    }
    template<typename BigNum> int checkCases(){
        int successPinwheel=PC::checkInequalities<BigNum>(); 
        if(successPinwheel==0){
            return 0;
        }
        int success=1;
        long n=9;
        while (n<21){
            success=checkCase<BigNum>(n);
            if(success==0){return 0;}
            ++n;
        }
        n=22;
        while(n<38){
            success=checkCase<BigNum>(n);
            if(success==0){return 0;}
            n+=2;
        }
        return 1;
    }
}

#endif
