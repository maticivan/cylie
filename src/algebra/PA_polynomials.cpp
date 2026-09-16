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

#ifndef _INCL_PA_CPP
#define _INCL_PA_CPP
namespace PA{
std::atomic<int> GL_errorWithMonomials=0;
template<typename TTT> Monomial<TTT> monConst(long c){
  TTT numConst;numConst=c;
  Monomial<TTT> m1(numConst);
  return m1;
}
template<typename TTT> Polynomial<TTT> polConst(long c){
  Polynomial<TTT> p1;
  p1+=monConst<TTT>(c);
  return p1;
}
    template<typename BigNum>
    std::string printDiadicRational(const Frac<BigNum>& x){
        return x.toString();
    }
    EvaluationConstants::EvaluationConstants(){
      uint64_t detector1=128;
      overflowDetector=0;
      uint64_t eraserBase=0,eraser1=255;
      for(long i=0;i<8;++i){
        overflowDetector+=detector1;
        detector1*=256;
        eraserBase+=eraser1;
        eraser1*=256;
      }
      eraser1=255;
      for(long i=0;i<8;++i){
        expEraser[i]=eraserBase-eraser1;
        eraser1*=256;
      }
    }
    template<typename TTT>
    Monomial<TTT>::Monomial(){
        c=0;exps8=0;
    }
uint64_t assignExponentToVariable(const std::string& variable, long exp){
    if(exp<=0){
        return 0;
    }
    if(exp>=127){
        GL_errorWithMonomials=1;
        return 0;
    }
    if(variable.size()!=3){
        GL_errorWithMonomials=1;
        return 0;
    }
    if(! ((variable[0]=='p')||(variable[0]=='q')) ){
        GL_errorWithMonomials=1;
        return 0;
    }
    if(variable[1]!='_'){
        GL_errorWithMonomials=1;
        return 0;
    }
    if((variable[2]<'0')||(variable[2]>'3')){GL_errorWithMonomials=1;return 0;}
    size_t lastComponent=static_cast<size_t>(variable[2]-'0'); 
    uint64_t mult=1;
    if(variable[0]=='p'){
        for(size_t j=0;j<4;++j){mult*=256;}
    }
    for(size_t j=0;j<lastComponent;++j){mult*=256;}
    return exp*mult;
}
  template<typename TTT>
  Monomial<TTT>::Monomial(const TTT& coefficient, const std::string& variable, long exponent){
    c=coefficient;
    exps8=0;
    if(exponent<0){exponent=0;}
    if(exponent>0){ 
      exps8=assignExponentToVariable(variable,exponent);
    }
  }
  template<typename TTT> 
  std::string Monomial<TTT>::toString(int skipCoefficient) const{
      TTT zero;zero=0;
      std::string res;
      if(c==zero){return res;}
      res+=printDiadicRational(c);
    if(skipCoefficient){res="";}
    std::string prefix="q_";
    uint64_t remExp=exps8;
    for(long i=0;i<8;++i){
      if(i==4){prefix="p_";}
      if(remExp%256!=0){
        res+="\\cdot "+prefix+std::to_string(i%4)+"^{"+std::to_string(remExp%256)+"}";
      }
      remExp/=256;
    }
    return res;
  }
  template<typename TTT>
  int Monomial<TTT>::operator<(const Monomial<TTT>& oth) const{
    return (exps8 < oth.exps8);
  } 
  template<typename TTT>
  TTT power(TTT res, const TTT& x, long e){
      TTT multiplier=x;
      while(e>0){
  	    if(e%2){res*=multiplier;}
  	    e/=2;
  	    multiplier*=multiplier;  
      }
	    return res;
   }
  template<typename TTT>
  TTT power(const TTT& x, long e){
      TTT res; res=1;
      return power(res,x,e);
   }
  long getExponent(uint64_t exps8, const std::string& vName){
    if(vName.size()!=3){return -1;}
    if(vName[1]!='_'){return -1;}
    if((vName[0]!='q')&&(vName[0]!='p')){return -1;}
    if((vName[2]<'0')||(vName[2]>'3')){return -1;}
    size_t pos=vName[2]-'0';
    if(vName[0]=='p'){pos+=4;}
    for(long i=0;i<pos;++i){exps8/=256;}
    return exps8%256;
  }
  template<typename TTT>
  Monomial<TTT> Monomial<TTT>::evaluate(const std::map<std::string,TTT>& varVal) const{
    Monomial<TTT> zero,res;
      TTT zeroNum;zeroNum=0;
    if(c==zeroNum){return zero;}
    res=*this;
    typename std::map<std::string,TTT>::const_iterator it=varVal.begin();
    std::map<std::string,long>::const_iterator itVEFinder;
    long currentExp;size_t eraserPos;
    while(it!=varVal.end()){
      currentExp=getExponent(exps8,it->first);
      if(it->second==zeroNum){
        if(currentExp>0){return zero;}
      }
      else{
        if(currentExp>0){
          res.c*=power(it->second,currentExp);
          eraserPos=it->first[2]-'0';
          if(it->first[0]=='p'){eraserPos+=4;}
          res.exps8 &= GL_EConstants.expEraser[eraserPos];
        }
      }
      ++it;
    }
    return res;
  }
  template<typename TTT>
  Monomial<TTT>& Monomial<TTT>::operator*=(const Monomial<TTT>& oth){
    c*=oth.c;
      TTT zero;
      zero=0;
    if(c==zero){
      exps8=0;
    }
    else{
      if( (exps8&GL_EConstants.overflowDetector) || (oth.exps8&GL_EConstants.overflowDetector)){
        FA::GL_OVERFLOW=1;
      }
      exps8+=oth.exps8;
      if(exps8&GL_EConstants.overflowDetector){
        FA::GL_OVERFLOW=1;
      }
    }
    return *this;
  }
  template<typename TTT>
  Monomial<TTT>& Monomial<TTT>::operator*=(const TTT& oth){
    Monomial<TTT> othM(oth);
    return operator*=(othM);
  }
  template<typename TTT>
  Monomial<TTT> operator*(const Monomial<TTT>& a, const Monomial<TTT>& b){
    Monomial<TTT> res=a;
    res*=b;
    return res;
  }

    template<typename TTT> Polynomial<TTT>::Polynomial(){ 
    }
  template<typename TTT> Polynomial<TTT>::Polynomial(TTT c){
    if(c.getNum()!=0){
      Monomial<TTT> m(c);
      terms.insert(m);
    }
  }
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator+=(const Polynomial<TTT>& oth){
    for(long i=0;i<(oth.terms).size();++i){
      long j=terms.find((oth.terms)[i]);
      if(j<0){
        terms.insert((oth.terms)[i]);
      }
      else{
        Monomial<TTT> tOld=terms[j];
        Monomial<TTT> tToAdd=(oth.terms)[i];
        terms.erase(tOld);
        tOld.c+=tToAdd.c;
        if(tOld.c.getNum()!=0){
          terms.insert(tOld);
        }
      }
    }
    return *this;
  }
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator*=(const TTT& m){
      TTT zeroNum;
      zeroNum=0;
    if(m==zeroNum){
      terms.clear();
      return *this;
    }
    ssm::set<Monomial<TTT> > termsNew;
    for(long i=0;i<terms.size();++i){
      Monomial<TTT> t=terms[i];
      t.c *= m;
      termsNew.insert(t);
    }
    terms=std::move(termsNew);
    return *this;
  }
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator-=(const Polynomial<TTT>& oth){
    Polynomial<TTT> q=oth;
    TTT minus1;minus1=-1;
    q*=minus1;
    (*this)+=q;
    return *this;
  }  
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator+=(const Monomial<TTT>& m){
      TTT zero;zero=0;
    if(m.c==zero){return *this;}
    Polynomial<TTT> q;
    q.terms.insert(m);
    return operator+=(q);
  }  
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator-=(const Monomial<TTT>& m){
      TTT zero;zero=0;
    if(m.c==zero){return *this;}
    Polynomial<TTT> q;
    q.terms.insert(m);
    return operator-=(q);
  }  
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator*=(const Monomial<TTT>& m){
      TTT zero;zero=0;
    if(m.c==zero){
      terms.clear();
      return *this;
    }
    ssm::set<Monomial<TTT> > termsNew;
    for(long i=0;i<terms.size();++i){
      Monomial<TTT> t=terms[i];
      t *= m;
      termsNew.insert(t); 
    }
    terms=std::move(termsNew);
    return *this; 
  }
  template<typename TTT>
  Polynomial<TTT>& Polynomial<TTT>::operator*=(const Polynomial<TTT>& oth){
    Polynomial<TTT> result;
    Polynomial<TTT> current;
    for(long i=0;i<oth.terms.size();++i){
      current.terms=terms;
      current*=oth.terms[i];
      result+=current;
    }
    terms=result.terms;
    return *this;
  } 
  template<typename TTT>
  Polynomial<TTT> Polynomial<TTT>::evaluate(const std::map<std::string,TTT>& varVal) const{
    Polynomial<TTT> res;
    for(long i=0;i<terms.size();++i){
      res+=terms[i].evaluate(varVal);
    }
    return res;
  }
  template<typename TTT>
  std::string Polynomial<TTT>::toString() const{
    std::string res;
    int printed=0;long charactersPrintedInLine=0;std::string newestAddition;
      TTT zero;
      zero=0;
    for(long i=0;i<terms.size();++i){
      Monomial<TTT> t=terms[i];
      if(charactersPrintedInLine>100){
        res+="\\\\\n&\\quad ";
        charactersPrintedInLine=0;
      }
      if((printed)&&(t.c>zero)){
        res+="+";
      }
      printed=1;
      newestAddition=t.toString();
      res+=newestAddition;
      charactersPrintedInLine+=newestAddition.size();
    }
    return res;
  } 
  template<typename TTT>
  long maxVarDegree(const PA::Monomial<TTT>& m){
    long mdeg=0;
    uint64_t remExp=m.exps8;
    uint64_t currentExp;
    for(long i=0;i<8;++i){
      currentExp=remExp%256;
      if(currentExp>mdeg){mdeg=currentExp;}
      remExp/=256;
    }
    return mdeg;
  }
  template<typename TTT>
  long maxVarDegree(const PA::Polynomial<TTT>& p){
    long mdeg=0;long cd;
    for(long i=0;i<p.terms.size();++i){
      cd=maxVarDegree(p.terms[i]);
      if(cd>mdeg){mdeg=cd;}
    }
    return mdeg;
  }
  template<typename TTT> ssm::set<std::string> allVariables(const PA::Monomial<TTT>& m){
    ssm::set<std::string> avr;
    uint64_t remExp=m.exps8;
    std::string vName="q_";
    for(long i=0;i<8;++i){
      if(i==4){vName="p_";}
      if(remExp%256>0){
        avr.insert(vName+std::to_string(i%4));
      }
      remExp/=256;
    }
    return avr;
  }
  template<typename TTT>
  ssm::set<std::string> allVariables(const PA::Polynomial<TTT>& p){
    ssm::set<std::string> avr;
    for(long i=0;i<p.terms.size();++i){
      avr+=allVariables(p.terms[i]);
    }
    return avr;
  }
  /*
  template<typename TTT>
  long degreeOf(const PA::Monomial<TTT>& m,const std::string& vN){
    std::map<std::string,long>::const_iterator it=m.vExp.find(vN);
    if(it==m.vExp.end()){return 0;}
    return it->second;
  }*/
  template<typename TTT>
  Polynomial<TTT> polynomialToPower(const Polynomial<TTT>& base, long exponent){
    Polynomial<TTT> res=polConst<TTT>(1);
    return power(res,base,exponent);
  }
  template<typename TTT>
  Polynomial<TTT> substitute(const Monomial<TTT>& m,
                             const std::map<std::string,Polynomial<TTT> >& substituteMap){
    Polynomial<TTT> res;
    TTT numOne,numZero;
    numOne=1;numZero=0;
      Monomial<TTT> mOne(numOne);
    if(m.c==numZero){return res;}
    res+=mOne;
    uint64_t remExp=m.exps8;
    size_t currentExp;
    typename std::map<std::string,Polynomial<TTT> >::const_iterator itS;
    std::string prefix="q_",vName;
    for(long i=0;i<8;++i){
      currentExp=remExp%256;
      if(i==4){prefix="p_";}
      if(currentExp>0){
        vName=prefix+std::to_string(i%4);
        itS=substituteMap.find(vName);
        if(itS==substituteMap.end()){
          Monomial<TTT> tmp(numOne,vName,currentExp);
          res*=tmp;
        }
        else{
          res*=polynomialToPower(itS->second,currentExp);
        }
      }
      remExp/=256;
    }
    res*=m.c;
    return res;
  }
  template<typename TTT>
  Polynomial<TTT> substitute(const Polynomial<TTT>& p, const std::map<std::string,Polynomial<TTT> >& substituteMap){
    Polynomial<TTT> res;
    for(long i=0;i<p.terms.size();++i){ 
      res+=substitute(p.terms[i],substituteMap);
    }
    return res;
  }
  template<typename TTT>
  int equal(const Polynomial<TTT>& a, const Polynomial<TTT>& b){
    if(a.terms.size()!=b.terms.size()){return 0;}
    for(long i=0;i<a.terms.size();++i){
      Monomial<TTT> aM=a.terms[i];
      Monomial<TTT> bM=b.terms[i];
      if( !(aM.c==bM.c) ){return 0;}
      if(aM.exps8!=bM.exps8){return 0;}
    }
    return 1;
  }
  template<typename TTT>
  std::vector<std::vector<std::vector<TTT> > > polMatrix(const PA::Polynomial<TTT>& p){
    long mdeg=maxVarDegree(p);
    std::vector<TTT> p1;
    std::vector<std::vector<TTT> > p2;
    std::vector<std::vector<std::vector<TTT> > > p3;
    p1.resize(mdeg+1);
    p2.resize(mdeg+1);
    p3.resize(mdeg+1);
    for(long i=0;i<mdeg+1;++i){
      p2[i]=p1;
    }
    for(long i=0;i<mdeg+1;++i){
      p3[i]=p2;
    }
    ssm::set<std::string> avr=allVariables(p); 
    long i0,i1,i2;
    for(long i=0;i<p.terms.size();++i){
      PA::Monomial<TTT> mi=p.terms[i];
      i0=getExponent(mi.exps8,avr[0]);
      i1=getExponent(mi.exps8,avr[1]);
      i2=getExponent(mi.exps8,avr[2]);
      if((i0>-1)&&(i1>-1)&&(i2>-1)){
        p3[i0][i1][i2]=mi.c;
      }
      else{
        GL_errorWithMonomials=1;
      }
    }
    return p3;
  }

}

#endif
