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

#ifndef _INCL_SF_STRINGS_CPP
#define _INCL_SF_STRINGS_CPP

namespace SF{
  template<class T1, class T2>
  class TwoValues{
  public:
    T1 x;
    T2 y;
    long operator<(const TwoValues &) const;
  };
  template<class T1, class T2>
  long TwoValues<T1,T2>::operator<(const TwoValues<T1,T2> & oth) const{
    if(x<oth.x){
      return 1;
    }
    if(x>oth.x){
      return 0;
    }
    if(y<oth.y){
      return 1;
    }
    return 0;
  }
  template<typename T1, typename T2>
  void sortWithFollower(std::vector<T1> & v1, std::vector<T2> & v2){
    long sz=v1.size();
    if(v2.size()==sz){
      std::vector<TwoValues<T1,T2> > w;
      w.resize(sz);
      for(long i=0;i<sz;++i){
        (w[i]).x=v1[i];
        (w[i]).y=v2[i];
      }
      std::sort(w.begin(),w.end());
      for(long i=0;i<sz;++i){
        v1[i]=(w[i]).x;
        v2[i]=(w[i]).y;
      }
    }
  }
  int isLetterOrBS(const char & c){
    if( (c>='a') && (c<='z')){return 1;}
    if( (c>='A') && (c<='Z')){return 1;}
    if(c=='\\'){return 1;}
    return 0;
  }
  std::string removeStartingCharacters(const std::string& in, char ch){
    std::string out;
    long pos=0;
    while((pos<in.size())&&(in[pos]==ch)){++pos;}
    while(pos<in.size()){out+=in[pos];++pos;}
    return out;
  }
  long getPositionOfFirstLetterOrBS(const std::string & st, const long & start=0){
    long len=st.length();
    long res=-1;
    long i=start;
    while((i<len)&&(!isLetterOrBS(st[i]))){
      ++i;
    }
    if((i<len)&&(isLetterOrBS(st[i]))){
      res=i;
    }
    return res;
  }
  bool caseInsensitiveComp(char a, char b){
    return (MFRF::toLowerCase(a)==MFRF::toLowerCase(b));
  }
  std::pair<std::string, int> getEverythingBefore(const std::string & src,
                                                  long & pos,
                                                  const std::string &endSt,
                                                  const int & caseSensitive = 0){
      //
      // If you do the following
      // std::string src="012345678901234567890123456789";
      // long pos=7;
      // std::pair<std::string,int> res=SF::getEverythingBefore(src,pos,"5678");
      // then you will have res.first="78901234" and pos=19
      std::string::const_iterator srcIt=src.begin(),j;
      srcIt+=pos;
      if(caseSensitive==0){
        j=std::search(srcIt,src.end(),endSt.begin(),endSt.end(),caseInsensitiveComp);
      }
      else{
        j=std::search(srcIt,src.end(),endSt.begin(),endSt.end());
      }
      if(j==src.end()){
        pos=src.length();
        return std::make_pair(src,0);
      }
      std::string fR;
      while(srcIt!=j){
        fR+=*srcIt; ++srcIt;++pos;
      }
      pos+=endSt.length();
      return std::make_pair(fR,1);
  }
  long firstContainsTheOther(const std::string &st1,const std::string &st2,const int & caseSensitive=0){
    long pos=0;
    std::pair<std::string,int> aD=getEverythingBefore(st1,pos,st2,caseSensitive);
    return aD.second;
  }
  std::pair<std::string,int> extract(const std::string &  ,
                                     long &  ,
                                     const std::string & ,
                                     const std::string & ,
                                     const int &  );
  std::pair<std::string,int> extrStrong(const std::string & src,
                                        long & pos,
                                        const std::string &startString,
                                        const std::string &endString,
                                        const int & caseSensitive = 0){
      std::pair<std::string,int> fR;
      fR.first=""; fR.second=0;
      if( (startString=="") || (endString=="")){return fR;}
      if(startString==endString){
        return extract(src,pos,startString,endString,caseSensitive);
      }
      long saveOldPos=pos;
      std::pair<std::string,int> thrashSt=getEverythingBefore(src,pos,startString,caseSensitive);
      std::pair<std::string,int> beforeNextStart,beforeNextEnd;
      if(thrashSt.second==1){
          long depth=1;
          long posAfterStart;
          long posAfterEnd;
          while(depth>0){
            posAfterStart=pos;
            posAfterEnd=pos;
            beforeNextStart=getEverythingBefore(src,posAfterStart,startString,caseSensitive);
            beforeNextEnd=getEverythingBefore(src,posAfterEnd,endString,caseSensitive);
            if((beforeNextStart.second==1)&&(beforeNextEnd.second==1)){
              if(posAfterStart<posAfterEnd){
                fR.first+=beforeNextStart.first+startString;
                pos=posAfterStart;
                ++depth;
              }
              else{
                --depth;
                fR.first+=beforeNextEnd.first;
                if(depth>0){
                  fR.first+=endString;
                }
                pos=posAfterEnd;
              }
            }
            else{
              if(beforeNextEnd.second==1){
                --depth;
                fR.first+=beforeNextEnd.first;
                if(depth>0){
                  fR.first+=endString;
                }
                pos=posAfterEnd;
              }
              else{
                fR.second=0;
                fR.first="";
                return fR;
              }
            }
          }
          fR.second=1;
      }
      if(fR.second==0){
          fR.first="";
          pos=saveOldPos;
      }
      return fR;
  }
  std::pair<std::string,int> extract(const std::string & src,
                                     long & pos,
                                     const std::string &startString,
                                     const std::string &endString,
                                     const int & caseSensitive = 0){

      if(startString!=endString){
        return extrStrong(src,pos,startString,endString,caseSensitive);
      }
      std::pair<std::string,int> fR;
      fR.first="";
      fR.second=0;
      std::pair<std::string,int> thrashSt=getEverythingBefore(src,pos,startString,caseSensitive);
      if(thrashSt.second==1){
          fR=getEverythingBefore(src,pos,endString,caseSensitive);
      }
      if(fR.second==0){
          fR.first="";
      }
      return fR;
  }
  std::pair<std::string,int> extractAndReplace(const std::string &  ,
                                               long &  ,
                                               const std::string & ,
                                               const std::string & ,
                                               const int &  ,
                                               const std::string &  );

  std::pair<std::string,int> extrReplStrong(const std::string & src,
                                            long & pos,
                                            const std::string &startString,
                                            const std::string &endString,
                                            const int & caseSensitive = 0,
                                            const std::string & replaceWith=""){
      if(startString==endString){
        return extractAndReplace(src,pos,startString,endString,caseSensitive,replaceWith);
      }
      std::pair<std::string,int> fR;
      fR.first="";
      fR.second=0;
      std::pair<std::string,int> thrashSt=getEverythingBefore(src,pos,startString,caseSensitive);
      std::pair<std::string,int> beforeNextStart,beforeNextEnd;
      if(thrashSt.second==1){
          long depth=1;
          long posAfterStart;
          long posAfterEnd;
          while(depth>0){
            posAfterStart=pos;
            posAfterEnd=pos;
            beforeNextStart=getEverythingBefore(src,posAfterStart,startString,caseSensitive);
            beforeNextEnd=getEverythingBefore(src,posAfterEnd,endString,caseSensitive);
            if((beforeNextStart.second==1)&&(beforeNextEnd.second==1)){
              if(posAfterStart<posAfterEnd){
                fR.first+=beforeNextStart.first+startString;
                pos=posAfterStart;
                ++depth;
              }
              else{
                --depth;
                fR.first+=beforeNextEnd.first;
                if(depth>0){
                  fR.first+=endString;
                }
                pos=posAfterEnd;
              }
            }
            else{
              if(beforeNextEnd.second==1){
                --depth;
                fR.first+=beforeNextEnd.first;
                if(depth>0){
                  fR.first+=endString;
                }
                pos=posAfterEnd;
              }
              else{
                fR.second=0;
                fR.first="";
                return fR;
              }
            }
          }
          fR.second=1;
      }
      if(fR.second==0){
          fR.first=src;
          return fR;
      }
      fR.first=thrashSt.first;
      fR.first+=replaceWith;
      long posSave=(fR.first).length();
      long sz=src.length();
      while(pos<sz){
        fR.first += src[pos];
        ++pos;
      }
      pos=posSave;
      return fR;
  }
  std::pair<std::string,int> extractAndReplace(const std::string & src,
                                               long & pos,
                                               const std::string &startString,
                                               const std::string &endString,
                                               const int & caseSensitive = 0,
                                               const std::string & replaceWith=""){
    if(startString!=endString){
      return extrReplStrong(src,pos,startString,endString,caseSensitive,replaceWith);
    }
    std::pair<std::string,int> fR;
    fR.first="";
    fR.second=0;
    std::pair<std::string,int> thrashSt=getEverythingBefore(src,pos,startString,caseSensitive);
    if(thrashSt.second==1){
        fR=getEverythingBefore(src,pos,endString,caseSensitive);
    }
    if(fR.second==0){
        fR.first="";
        return fR;
    }
    fR.first=thrashSt.first;
    fR.first+=replaceWith;
    long posSave=(fR.first).length();
    long sz=src.length();
    while(pos<sz){
      fR.first += src[pos];
      ++pos;
    }
    pos=posSave;
    return fR;
  }
  std::pair<std::string,int> eraseStuffBetween(const std::string & src,
                                               const std::string &startString,
                                               const std::string &endString,
                                               const long & cpos = 0,
                                               const int & caseSensitive = 0){

      long pos=cpos;
      return extractAndReplace(src,pos,startString,endString,caseSensitive,"");
  }
  std::string genRandomValueIfNecessary(const std::string &input){
    long pos;
    std::pair<std::string,int> allD;
    pos=0;allD=extract(input,pos,"_randomCode_","_/randomCode_");
    if(allD.second==0){
      return input;
    }
    return "109809709";//placeholder instead of random number generator
  }
  std::string replaceVariablesWithValues(const std::string & input,
                                         const std::string & sepVarB, const std::string & sepVarE,
                                         const std::map<std::string,std::string> & varsToVals){
    if(varsToVals.size()<1){return input;}
    std::string output=input;
    std::string currentVar,currentVal;
    long sepLens=sepVarB.length()+sepVarE.length();
    long currentValLen;
    std::map<std::string,std::string>::const_iterator it,itE;
    itE=varsToVals.end();
    long pos;
    std::pair<std::string,int> allD;
    pos=0;allD=extract(output,pos,sepVarB,sepVarE);
    std::string newOutput;
    while(allD.second==1){
      currentVar=allD.first;
      it=varsToVals.find(currentVar);
      if(it!=itE){
        pos-=(sepLens+currentVar.length());
        if(pos<0){pos=0;}
        newOutput="";
        for(long i=0;i<pos;++i){
          newOutput+=output[i];
        }
        currentVal=it->second;
        allD=extractAndReplace(output,pos,sepVarB,sepVarE,1,currentVal);
        if(allD.second==1){
          output=newOutput+allD.first;
        }
      }
      allD=extract(output,pos,sepVarB,sepVarE);
    }
    return output;
  }
  template<typename T>
  void flipTheStack(std::stack<T>& s){
    std::stack<T> t;
    t=std::move(s);
    while(!t.empty()){
      s.push(t.top());
      t.pop();
    }
  }

  template<typename TTT>
  std::stack<TTT> clearStack(std::stack<TTT> & v){
    if(v.empty()){
      return v;
    }
    v.pop();
    return clearStack(v);
  }
  template<typename TTT>
  std::vector<TTT> stackToVector(std::stack<TTT> & st){//returns flipped stack as a vector
    std::vector<TTT> fR;
    long sz=st.size();
    if(sz>0){
      fR.resize(sz);
      for(long i=0;i<sz;++i){
        fR[sz-i-1]=st.top();
        st.pop();
      }
    }
    return fR;
  }
  template<typename TTT>
  std::set<TTT> vectorToSet(const std::vector<TTT> &_v){
    std::set<TTT> fR;
    long sz=_v.size();
    for(long i=0;i<sz;++i){
      fR.insert(_v[i]);
    }
    return fR;
  }
  template<typename TTT>
  std::vector<TTT> setToVector(const std::set<TTT> &_s){
    std::vector<TTT> fR;
    long sz=_s.size();
    fR.resize(sz);
    typename std::set<TTT>::const_iterator it=_s.begin(),itE=_s.end();
    long i=0;
    while(it!=itE){
      fR[i]=*it;
      ++it;++i;
    }
    return fR;
  }
  std::string reverseString(const std::string& input){
    std::string result;
    long i=input.size();
    while(i>0){
      --i;
      result+=input[i];
    }
    return result;
  }
  std::string findAndReplace(const std::string &input,
                          const std::string &_searchFor,
                          const std::vector<std::string> &_replaceWith,
                          const int &all=1,
                          const int &caseSensitive=0){
      // Searches input for _searchFor; The first one is replaced by _replaceWith[0],
      // the second occurrence with _replaceWith[1], ..., the _replaceWith.size()-th ocurrence
      // with _replaceWith[0] again and it cycles
      if(input.length()<1){return input;}
      if(_searchFor.length()<1){return input;}
      std::string searchFor=_searchFor;
      std::vector<std::string> replaceWith=_replaceWith;
      if(replaceWith.size()==0){
        replaceWith.resize(1);replaceWith[0]="";
      }
      long ctrReplaceWith=0;
      long lenReplW=replaceWith.size();
      std::string endS="*j!Z8)#";//GF::GL_HIDING_STRING_SF;
      long pos=0;
      std::pair<std::string,int> allD;
      std::string output="",nBefore;
      long len=input.length();
      long lsf=searchFor.length();
      long lp1=len+1;
      long oldPos;
      while(pos<len){
        oldPos=pos;
        allD=getEverythingBefore(input,pos,searchFor,caseSensitive);
        if(allD.second==1){
          output += allD.first;
          if(pos<lp1){
            //Found the real occurrence of searchFor
            // not the fake one that we added to the end
            output+=replaceWith[ctrReplaceWith];
            if(lenReplW>1){
              ++ctrReplaceWith;
              if(ctrReplaceWith==lenReplW){
                ctrReplaceWith=0;
              }
            }
            if(all==0){
              oldPos=pos;
            }
          }
        }
        if((allD.second==0)||(all==0)){
          pos=len;
          for(long i=oldPos;i<pos;++i){
            output+=input[i];
          }
        }
      }
      return output;
  }
  std::string findAndReplace(const std::string &input,
                          const std::string &searchFor,
                          const std::string &_replaceWith,
                          const int &all=1,
                          const int &caseSensitive=0){
      if(searchFor==_replaceWith){
        return input;
      }
      if(input.length()<1){return input;}
      std::vector<std::string> replw;
      replw.resize(1);
      replw[0]=_replaceWith;
      return findAndReplace(input,searchFor,replw,all,caseSensitive);
  }
  long getFirstPosition(const std::string &input,
                        const long & _pos,
                        const std::string &searchFor,
                        const int & caseSensitive=0){
    long pos=_pos;
    std::pair<std::string,int> allD=getEverythingBefore(input,pos,searchFor,caseSensitive);
    if(allD.second==0){
      return input.length();
    }
    return pos-searchFor.length();
  }
  void updateFirstPositions(std::vector<long>& fPositions,
                            const std::string &input,
                            const long &_pos,
                            const std::vector<std::string> &searchFor,
                            const int& caseSensitive=0){
    long ln=searchFor.size();
    if((ln==fPositions.size())&&(ln>0)){
      for(long i=0;i<ln;++i){
        if( fPositions[i]<_pos ){
          fPositions[i]=getFirstPosition(input,_pos,searchFor[i],caseSensitive);
        }
      }
    }
  }
  std::pair<long,long> minAndMinimizer(const std::map<long,long> & m){
    std::pair<long,long> res;
    res.first=-1;res.second=-1;
    long sz=m.size();
    if(sz<1){return res;}
    return *(m.begin());
  }
  std::map<long,long> firstPosMap(const std::vector<long> & v){
    std::map<long,long> res;
    long i=v.size();
    while(i>0){
      --i;
      res[v[i]]=i;
    }
    return res;
  }
  std::string findAndReplaceSecureButNotVeryFast(const std::string &input,
                             const std::vector<std::string> &searchFor,
                             const std::vector<std::string> &replaceWith,
                             const int &caseSensitive=0){
      long lenSF=searchFor.size();
      if((lenSF<1)||(replaceWith.size()!=lenSF)){
        return input;
      }
      std::vector<long> fPositions;
      fPositions.resize(lenSF);
      for(long i=0;i<lenSF;++i){
        fPositions[i]=-1;
      }
      std::pair<long,long> minsV;
      long pos=0;
      updateFirstPositions(fPositions,input,pos,searchFor,caseSensitive);
      std::map<long,long> fPMap=firstPosMap(fPositions);
      long sz=input.length();
      std::string output;
      while(pos<sz){
        minsV=minAndMinimizer(fPMap);
        while(pos<minsV.first){
          output+=input[pos];
          ++pos;
        }
        if((minsV.second>-1)&&(minsV.first<sz)){
          output+=replaceWith[minsV.second];
          pos+=searchFor[minsV.second].length();
        }
        fPMap.erase(fPMap.begin());
        fPMap[getFirstPosition(input,pos, searchFor[minsV.second],caseSensitive)]=minsV.second;
      }
      return output;
  }
  std::string findAndReplace(const std::string &input,
                             const std::vector<std::string> &searchFor,
                             const std::vector<std::string> &replaceWith,
                             const int &caseSensitive=0){
      // This function trusts that
      // 1) searchFor[i] appears only once for every i.
      // 2) searchFor[i] appears before searchFor[j] if i<j
      // If 1) or 2) fails, then the function will findAndReplaceSecureButNotVeryFast.
      long lenSF=searchFor.size();
      if((lenSF<1)||(replaceWith.size()!=lenSF)){
        return input;
      }
      std::vector<long> fPositions;
      fPositions.resize(lenSF);
      for(long i=0;i<lenSF;++i){
        fPositions[i]=-1;
      }
      std::pair<long,long> minsV;
      long pos=0;
      updateFirstPositions(fPositions,input,pos,searchFor,caseSensitive);
      long trustworthy=1;
      long i=1;
      while((i<lenSF)&&(trustworthy==1)){
        if(fPositions[i-1]>=fPositions[i]){
          trustworthy=0;
        }
        ++i;
      }
      if(trustworthy==0){
        return findAndReplaceSecureButNotVeryFast(input,searchFor,replaceWith,caseSensitive);
      }
      long sz=input.length();
      std::string output;
      i=0;
      while((pos<sz)&&(i<lenSF)){
        minsV.first=fPositions[i];
        minsV.second=i;
        while(pos<fPositions[i]){
          output+=input[pos];
          ++pos;
        }
        if(fPositions[i]<sz){
          output+=replaceWith[i];
          pos+=searchFor[i].length();
        }

        ++i;
      }
      while(pos<sz){
        output+=input[pos];
        ++pos;
      }
      return output;
  }
  long countInString(const std::string & _allItems,
                                    const std::string & _nextB="_n_",
                                    const std::string & _nextE="_/n_",
                                    const std::string & _attMustHave="!*!"){
    long fR=0;
    long pos=0;
    std::pair<std::string,int> nextSt=extract(_allItems,pos,_nextB,_nextE);
    while(nextSt.second==1){
      if(_attMustHave!="!*!"){
        fR+=firstContainsTheOther(nextSt.first,_attMustHave);
      }
      else{
        ++fR;
      }
      nextSt=extract(_allItems,pos,_nextB,_nextE);
    }
    return fR;
  }
  std::string vectorToString(const std::vector<std::string> & v,
                            const std::string & _nextB="_n_",
                            const std::string & _nextE="_/n_"){
    std::string fR="";
    long sz=v.size();
    for(long i=0;i<sz;++i){
      fR+=_nextB+v[i]+_nextE;
    }
    return fR;
  }
  std::string vectorToString(const std::vector<long> & v,
                            const std::string & _nextB="_n_",
                            const std::string & _nextE="_/n_"){
    std::string fR="";
    long sz=v.size();
    for(long i=0;i<sz;++i){
      fR+=_nextB+std::to_string(v[i])+_nextE;
    }
    return fR;
  }
  long copyTheRestOfTheStringAndGiveUp(const std::string& in, const long& oldPosSave, std::string& out,const int& copyIsNecessary){
    long len=in.length();
    if(copyIsNecessary==0){
      return len;
    }
    long pos=oldPosSave;
    while(pos<len){
      out+=in[pos];
      ++pos;
    }
    return pos;
  }
  std::pair<std::stack<std::string>,std::string>
                  stringToStackAndRemoveItems(const std::string & _allItems,
                                              const std::string & _nextB="_n_",
                                              const std::string & _nextE="_/n_",
                                              const int & removalIsNecessary=1,
                                              const std::string & _attMustHave="!*!",
                                              const std::string& prefixToReplace="",
                                              const std::string& suffixToReplace="",
                                              const long& counterStart=-1){
    std::pair<std::stack<std::string>,std::string> fR;
    long pos=0; std::pair<std::string,int> allD;
    long cntr=counterStart;
    long len=_allItems.length(); long openTagLen=_nextB.length();
    std::string stBefore,nextSt;
    long oldPosSave;
    while(pos<len){
      oldPosSave=pos;
      allD=getEverythingBefore(_allItems,pos,_nextB);
      if(allD.second==0){
        pos=copyTheRestOfTheStringAndGiveUp(_allItems,oldPosSave,fR.second,removalIsNecessary);
      }
      else{
        pos-=openTagLen;
        stBefore=allD.first;
        allD=extract(_allItems,pos,_nextB,_nextE);
        if(allD.second==0){
          pos=copyTheRestOfTheStringAndGiveUp(_allItems,oldPosSave,fR.second,removalIsNecessary);
        }
        else{
          nextSt=allD.first;
            if(removalIsNecessary){
                fR.second+=stBefore;
                if(prefixToReplace!=""){
                    fR.second+=prefixToReplace;
                    if(cntr>-1){
                        fR.second+=std::to_string(cntr);
                        ++cntr;
                    }
                    fR.second+=suffixToReplace;
                }
            }
          if((_attMustHave=="!*!") || (findAndReplace(nextSt,_attMustHave,"")!=nextSt) ){
            (fR.first).push(nextSt);
          }
        }
      }
    }
    return fR;
  }
  std::pair<std::vector<std::string>,std::string>
                  stringToVectorAndRemoveItems(const std::string & _allItems,
                                               const std::string & _nextB="_n_",
                                               const std::string & _nextE="_/n_",
                                               const int & removalIsNecessary=1,
                                               const std::string & _attMustHave="!*!"){
    std::pair<std::vector<std::string>,std::string> fR;
    std::pair<std::stack<std::string>,std::string> tmpStack;
    tmpStack=stringToStackAndRemoveItems(_allItems,_nextB,_nextE,removalIsNecessary,_attMustHave);
    fR.second=tmpStack.second;
    fR.first=stackToVector(tmpStack.first);
    return fR;
  }
  std::vector<std::string> stringToVector(const std::string & _allItems,
                                          const std::string & _nextB="_n_",
                                          const std::string & _nextE="_/n_",
                                          const std::string & _attMustHave="!*!"){
    return stringToVectorAndRemoveItems(_allItems,_nextB,_nextE,0,_attMustHave).first;
  }
std::vector<long> stringToVectorLong(const std::string & _allItems,
                                     const std::string & _nextB="_n_",
                                     const std::string & _nextE="_/n_",
                                     const std::string & _attMustHave="!*!"){
    std::vector<std::string> vStr=stringToVectorAndRemoveItems(_allItems,_nextB,_nextE,0,_attMustHave).first;
    std::vector<long> res;
    if(vStr.size()==0){return res;}
    res.resize(vStr.size());
    for(long i=0;i<vStr.size();++i){
        res[i]=BF::stringToInteger(vStr[i]);
    }
    return res;
}
  std::stack<std::string> stringToStack(const std::string & _allItems,
                                        const std::string & _nextB="_n_",
                                        const std::string & _nextE="_/n_",
                                        const std::string & _attMustHave="!*!"){
    return stringToStackAndRemoveItems(_allItems,_nextB,_nextE,0,_attMustHave).first;
  }
  std::vector<std::string> stringToVectorSimpleSeparator(const std::string& _nAnswers, const std::string& separator=";"){
      std::string saB="_n!-!*_", saE="_/n!-!*_";
      return stringToVector(saB+
                            findAndReplace(_nAnswers,separator,saE+saB)+
                            saE,saB,saE);
  }
  std::vector<std::vector<std::string> > stringToMatrix(const std::string & _qs,
                                                        const std::string & _nextB,
                                                        const std::string & _nextE,
                                                        const std::string & _sepInsideB,
                                                        const std::string & _sepInsideE){
    std::vector<std::vector<std::string> > fR;
    long sz=countInString(_qs,_nextB,_nextE);
    if(sz==0){
      return fR;
    }
    fR.resize(sz);
    long pos=0;
    std::string individualLine;
    for(long i=0;i<sz;++i){
      individualLine=extract(_qs,pos,_nextB,_nextE).first;
      fR[i]=stringToVector(individualLine,_sepInsideB,_sepInsideE);
    }
    return fR;
  }
  long getLegalCharacters(std::map<char,long> &fR1, std::map<long,char> &fR2){
   char c;
   long toAdd=0;
   for(c='0';c<='9';++c){
     fR1[c]=toAdd;
     fR2[toAdd]=c;
     ++toAdd;
   }
   for(c='a';c<='z';++c){
     fR1[c]=toAdd;
     fR2[toAdd]=c;
     ++toAdd;
   }
   for(c='A';c<='Z';++c){
     fR1[c]=toAdd;
     fR2[toAdd]=c;
     ++toAdd;
   }
   return toAdd;
  }
  class TranslatorCharToLong{
  private:
    long num;
    std::map<char,long> cToL;
    std::map<long,char> lToC;
  public:
    TranslatorCharToLong();
    long getLong(const char &) const;
    char getChar(const long &) const;
    long getNum() const;
  };
  TranslatorCharToLong::TranslatorCharToLong(){
    num=getLegalCharacters(cToL,lToC);
  }
  long TranslatorCharToLong::getLong(const char & _c) const{
    std::map<char,long>::const_iterator it,itE;
    itE=cToL.end();
    it=cToL.find(_c);
    long fR=-1;
    if(it!=itE){
      fR=it->second;
    }
    return fR;
  }
  char TranslatorCharToLong::getChar(const long & _l) const{
    std::map<long,char>::const_iterator it,itE;
    itE=lToC.end();
    it=lToC.find(_l);
    char fR='0';
    if(it!=itE){
      fR=it->second;
    }
    return fR;
  }
  long TranslatorCharToLong::getNum() const{
    return num;
  }
  TranslatorCharToLong GLOBAL_TRANSLATOR_CHAR_LONG;
  long charToNum(const char & x){
    return GLOBAL_TRANSLATOR_CHAR_LONG.getLong(x);
  }
  std::string numToStr(const long & _l){
    long l=_l%256;
    std::string fR="a";
    char c=GLOBAL_TRANSLATOR_CHAR_LONG.getChar(l);
    fR[0]=c;
    return fR;
  }
  std::vector<long> strToVectInt(const std::string &_s){
    std::vector<long> fR;
    long sz=_s.size();
    fR.resize(sz);
    long i=0;
    while(i<sz){
      fR[i]= charToNum(_s[i]);
      ++i;
    }
    return fR;
  }
  std::string vectIntToStr(const std::vector<long> &_c){
    long i=0;
    long lC=_c.size();
    std::string c;
    while(i<lC){
      c+=numToStr(_c[i]);
      ++i;
    }
    return c;
  }
  std::vector<long> getAdditionVector6(){
    std::vector<long> aV6;
    aV6.resize(6);
    aV6[0]=7;aV6[1]=9;aV6[2]=3;aV6[3]=8;aV6[4]=7;aV6[5]=4;
    return aV6;
  }
  std::string combineTwoWords(const std::string &_a, const std::string &_b){
    // lengths of _a and _b must be smaller than or equal to 999
    long numChr=GLOBAL_TRANSLATOR_CHAR_LONG.getNum();
    std::vector<long> aV6=getAdditionVector6();
    std::string __c;
    std::string __b=_b;
    if(__b.length()<3){
      __b="000"+__b;
    }
    std::string __a=_a;
    if(__a.length()<3){
      __a="000"+__a;
    }
    std::vector<long> a,b;
    a=strToVectInt(__a);b=strToVectInt(__b);
    std::vector<long> c;
    long lA,lB,lC;
    lA=a.size();lB=b.size();
    std::string __slA=BF::padded(lA,100,"0");
    std::string __slB=BF::padded(lB,100,"0");
    std::vector<long> slA=strToVectInt(__slA);
    std::vector<long> slB=strToVectInt(__slB);
    lC=lA+lB+6;
    c.resize(lC);
    long i=1;
    c[0]=(a[0]+b[0])%numChr;
    while(i<3){
      c[i]=(a[i]+b[i]+c[i-1])%numChr;++i;
    }
    c[3]=(c[2]+c[1]+slA[0]+aV6[0])%numChr;
    c[4]=(c[3]+c[1]+slB[0]+aV6[1])%numChr;
    c[5]=(c[4]+c[1]+slA[1]+aV6[2])%numChr;
    c[6]=(c[5]+c[1]+slB[1]+aV6[3])%numChr;
    c[7]=(c[6]+c[1]+slA[2]+aV6[4])%numChr;
    c[8]=(c[7]+c[1]+slB[2]+aV6[5])%numChr;
    i=9;
    long posB=lB;
    while(posB>0){
      --posB;
      c[i]=(c[i-1]+b[posB])%numChr;
      ++i;
    }
    posB=3;
    long posA=3;
    while(posA<lA){
      c[i]=(c[i-1]+b[posB]+a[posA])%numChr;
      ++i;++posA;++posB;
      if(posB>=lB){
        posB=0;
      }
    }
    return vectIntToStr(c);
  }
  long makeItBiggerThanOrEqual0(long x){
    long numChr=GLOBAL_TRANSLATOR_CHAR_LONG.getNum();
    while(x<0){
      x+=numChr;
    }
    return x;
  }
  std::pair<std::string,std::string> oneWordToTwoWords(const std::string &_st){
    std::vector<long> aV6=getAdditionVector6();
    long lenC=_st.length();
    if(lenC<9){
      return std::pair<std::string,std::string>("notFound","notFound");
    }
    std::string __a,__b;
    std::vector<long> a,b,c,slA,slB;
    c=strToVectInt(_st);
    slA.resize(3);slB.resize(3);
    slA[0]=makeItBiggerThanOrEqual0(c[3]-c[2]-c[1]-aV6[0]);
    slB[0]=makeItBiggerThanOrEqual0(c[4]-c[3]-c[1]-aV6[1]);
    slA[1]=makeItBiggerThanOrEqual0(c[5]-c[4]-c[1]-aV6[2]);
    slB[1]=makeItBiggerThanOrEqual0(c[6]-c[5]-c[1]-aV6[3]);
    slA[2]=makeItBiggerThanOrEqual0(c[7]-c[6]-c[1]-aV6[4]);
    slB[2]=makeItBiggerThanOrEqual0(c[8]-c[7]-c[1]-aV6[5]);
    long lA,lB;
    lA=BF::stringToInteger(vectIntToStr(slA));
    lB=BF::stringToInteger(vectIntToStr(slB));
    long posC=9;
    long posB=lB;
    a.resize(lA);b.resize(lB);
    while(posB>0){
      --posB;
      b[posB]=makeItBiggerThanOrEqual0(c[posC]-c[posC-1]);
      ++posC;
    }
    long posA=3;
    posB=3;
    a[0]=makeItBiggerThanOrEqual0(c[0]-b[0]);
    a[1]=makeItBiggerThanOrEqual0(c[1]-c[0]-b[1]);
    a[2]=makeItBiggerThanOrEqual0(c[2]-c[1]-b[2]);
    while(posA<lA){
      a[posA]=makeItBiggerThanOrEqual0(c[posC]-c[posC-1]-b[posB]);
      ++posA;++posC;++posB;
      if(posB>=lB){
        posB=0;
      }
    }
    __a=vectIntToStr(a);
    __b=vectIntToStr(b);
    return std::pair<std::string,std::string>(__a,__b);
  }
  std::string getElFromMapOrNotFoundMessage(const std::map<std::string,std::string> & _mm, const std::string & _q, const std::string & _mnf){
    std::map<std::string,std::string>::const_iterator it,itE=_mm.end();
    it=_mm.find(_q);
    if(it!=itE){
      return it->second;
    }
    return _mnf;
  }
  std::vector<std::string> removeFromSeq(const std::vector<std::string> &in, const std::string & toRemove){
    std::vector<std::string> out;
    long sz=in.size();
    long i=0;
    while((i<sz)&&(in[i]!=toRemove)){
      ++i;
    }
    if(i==sz){
      return in;
    }
    out.resize(sz-1);
    if(sz==1){
      return out;
    }
    for(long j=0;j<i;++j){
      out[j]=in[j];
    }
    ++i;
    while(i<sz){
      out[i-1]=in[i];
    }
    return out;
  }
  void assignValueFromMap(const std::map<std::string,std::string>& mM, const std::string &key, std::string& valAssignIfInMap){
    std::map<std::string,std::string>::const_iterator it=mM.find(key);
    if(it!=mM.end()){
      valAssignIfInMap=it->second;
    }
  }
  void assignValueFromMap(const std::map<std::string,std::string>& mM, const std::string &key, std::vector<std::string>& valAssignIfInMap){
    std::map<std::string,std::string>::const_iterator it=mM.find(key);
    if(it!=mM.end()){
      valAssignIfInMap=stringToVector(it->second,"_n*_","_/n*_");
    }
  }
  void assignValueFromMap(const std::map<std::string,std::string>& mM,
                          const std::string &key,
                          long& valAssignIfInMap,
                          long leftEndPointOfAcceptableRange=-1,
                          long rightEndPointOfAcceptableRange=-1){
    std::map<std::string,std::string>::const_iterator it=mM.find(key);
    if(it!=mM.end()){
        long v=BF::stringToInteger(it->second);
        if( ((leftEndPointOfAcceptableRange<0)||(v>=leftEndPointOfAcceptableRange))
              &&
            ((rightEndPointOfAcceptableRange<0)||(v<=rightEndPointOfAcceptableRange)) ){
            valAssignIfInMap=v;
        }
    }
  }
  void varValPairs(const std::string & input,
                                                const std::string & sepVarValB, const std::string & sepVarValE,
                                                const std::string & sepVarB,const std::string & sepVarE,
                                                const std::string & sepValB, const std::string &sepValE,
                                                std::map<std::string,std::string> & mM){
    std::vector<std::string> indItems=stringToVector(input,sepVarValB,sepVarValE);
    long sz=indItems.size();
    long pos;std::pair<std::string,int> allD;
    std::string val,var;
    for(long i=0;i<sz;++i){
      pos=0;allD=extract(indItems[i],pos,sepVarB,sepVarE);
      if(allD.second==1){
        var=allD.first;
        pos=0;allD=extract(indItems[i],pos,sepValB,sepValE);
        if(allD.second==1){
          val=allD.first;
          mM[var]= genRandomValueIfNecessary( val);
        }
      }
    }
  }
  std::pair<std::string,int> replaceAllOuterLayerSeparators(const std::string &input,
                                                            const std::string & _cOpen,
                                                            const std::string &_cClose,
                                                            const std::string & _rOpen,
                                                            const std::string &_rClose,
                                                            const std::map<std::string,std::string> & permanentChangesForProtection ,
                                                            const int & caseSensitive =0){
    std::string output="";
    std::pair<std::string,int> fR;fR.second=1;
    long pos=0,oldPos,newPos;
    std::pair<std::string,int> allD,allDB;
    allD=extract(input,pos,_cOpen,_cClose,caseSensitive);
    if(allD.second==0){
      fR.first=input;
      return fR;
    }
    oldPos=0;
    int keepGoing=1;
    std::string replacingPart;
    long szV=permanentChangesForProtection.size();
    while(keepGoing==1){
      replacingPart= allD.first;
      if(szV>0){
        replacingPart=MFRF::findAndReplace(replacingPart,permanentChangesForProtection,caseSensitive);
      }
      allDB=getEverythingBefore(input,oldPos,_cOpen,caseSensitive);
      if(allDB.second==1){
        output+=allDB.first;
        output+=_rOpen+replacingPart+_rClose;
        oldPos=pos;
        allD=extract(input,pos,_cOpen,_cClose,caseSensitive);
        if(allD.second==0){
          keepGoing=0;
        }
      }
      else{
        fR.second=0;keepGoing=0;
      }
    }
    long ln=input.length();
    while(oldPos<ln){
      output+=input[oldPos];
      ++oldPos;
    }
    fR.first=output;
    return fR;
  }
  std::string getTagInteriorAndRemoveTag(std::string& source,const std::string& openT, const std::string& closeT){
    long pos; std::pair<std::string,int> allD;
    pos=0;allD=extract(source,pos,openT,closeT);
    if(allD.second==0){
      return "";
    }
    std::string interior=allD.first;
    source=findAndReplace(source,openT+interior+closeT,"",0);
    return interior;
  }
  std::set<std::string> stringToSet(const std::string & everything, const std::string & sepKeyB,const std::string & sepKeyE){
    std::set<std::string> fR;
    std::vector<std::string> keys=stringToVector(everything,sepKeyB,sepKeyE);
    long sz=keys.size();
    for(long i=0;i<sz;++i){
      fR.insert(keys[i]);
    }
    return fR;
  }
  ssm::set<std::string> stringToSsms(const std::string & everything, const std::string & sepKeyB,const std::string & sepKeyE){
      ssm::set<std::string> fR;
      std::vector<std::string> keys=stringToVector(everything,sepKeyB,sepKeyE);
      long sz=keys.size();
      for(long i=0;i<sz;++i){
          fR.insert(keys[i]);
      }
      return fR;
  }
  std::set<std::string> stringToSetSimpleSeparator(const std::string& _in, const std::string& separator=";"){
      std::string saB="_n!-!*_", saE="_/n!-!*_";
      return stringToSet(saB+
                         findAndReplace(_in,separator,saE+saB) +
                         saE,saB,saE);
  }
  ssm::set<std::string> stringToSsmsSimpleSeparator(const std::string& _in, const std::string& separator=";"){
    std::string saB="_n!-!*_", saE="_/n!-!*_";
    return stringToSsms(saB+
                       findAndReplace(_in,separator,saE+saB) +
                       saE,saB,saE);
  }
  std::string setToString(const std::set<std::string> & s, const std::string & sepKeyB,const std::string & sepKeyE ,const std::string &formattingSeparator=""){
    std::string fR="";
    std::set<std::string>::const_iterator it=s.begin(),itE=s.end();
    while(it!=itE){
      fR+=sepKeyB+(*it)+sepKeyE+formattingSeparator;
      ++it;
    }
    return fR;
  }
  std::map<std::string,std::string> stringToMap(const std::string & everything, const std::string & sepKeyB,const std::string & sepKeyE,const std::string & sepValB,const std::string & sepValE){
    std::map<std::string,std::string> fR;
    int stillWorking=1;
    std::pair<std::string,int> allD; long pos=0;
    std::string nextKey;
    while(stillWorking){
      allD=extract(everything,pos,sepKeyB,sepKeyE);
      if(allD.second==1){
        nextKey=allD.first;
        allD=extract(everything,pos,sepValB,sepValE);
        if(allD.second==1){
          fR[nextKey]=allD.first;
        }
        else{
          stillWorking=0;
        }
      }
      else{
        stillWorking=0;
      }
    }
    return fR;
  }
  std::string mapToString(const std::map<std::string,std::string> & m,const std::string &beginSeparator,const std::string &endSeparator, const std::string & sepKeyB,const std::string & sepKeyE,const std::string & sepValB,const std::string & sepValE){
    std::string fR="";
    std::map<std::string,std::string>::const_iterator it=m.begin(),itE=m.end();
    while(it!=itE){
      fR+=beginSeparator+sepKeyB+(it->first)+sepKeyE+sepValB+(it->second)+sepValE+endSeparator;
      ++it;
    }
    return fR;
  }
std::string mapToString(const std::map<std::string,long> & m,const std::string &beginSeparator,const std::string &endSeparator, const std::string & sepKeyB,const std::string & sepKeyE,const std::string & sepValB,const std::string & sepValE){
  std::string fR="";
  std::map<std::string,long>::const_iterator it=m.begin(),itE=m.end();
  while(it!=itE){
    fR+=beginSeparator+sepKeyB+(it->first)+sepKeyE+sepValB+std::to_string(it->second)+sepValE+endSeparator;
    ++it;
  }
  return fR;
}
  std::string mapToString(const std::map<std::string,std::string> & m, const std::string & sepKeyB,const std::string & sepKeyE,const std::string & sepValB,const std::string & sepValE,const std::string &formattingSeparator = ""){
    return mapToString(m,"",formattingSeparator,sepKeyB,sepKeyE,sepValB,sepValE);
  }
  std::string updateVarsWithVals(const std::string & input, const std::map<std::string,std::string> & sr, const std::string & sB, const std::string &sE){
    std::string output=input;
    std::map<std::string,std::string>::const_iterator it,itE;
    it=sr.begin();itE=sr.end();
    std::map<std::string,std::string> replMap;
    while(it!=itE){
      replMap[sB+(it->first)+sE]=sB+(it->second)+sE;
      ++it;
    }
    output=MFRF::findAndReplace(output,replMap);
    return output;
  }
  template<typename TTT>
  std::vector<TTT> oneElementVector(const TTT& el){
    std::vector<TTT> fR;fR.resize(1);fR[0]=el;
    return fR;
  }
  std::vector<std::string> getLines(const std::string& input, char lineSeparator='\n'){
    std::stack<std::string> linesStack;
    std::string nextLine;
    long sz=input.length();
    long i=0;
    while(i<sz){
      if(input[i]==lineSeparator){
        linesStack.push(nextLine);
        nextLine="";
      }
      else{
        nextLine+=input[i];
      }
      ++i;
    }
    if(nextLine!=""){
      linesStack.push(nextLine);
    }
    return stackToVector(linesStack);
  }
  std::string incrementString(const std::string & x){
      std::string res=x;
      long cS=res.size();
      long success=0;--cS;
      char zeroM1='0'-1;
      char nineP1='9'+1;
      char aM1='a'-1;
      char zP1='z'+1;
      while((success==0)&&(cS>-1)){
          if((res[cS]>zeroM1)&&(res[cS]<'9')){
              res[cS]=res[cS]+1;success=1;
          }
          if((success==0)&&(res[cS]=='9')){
              res[cS]='a';success=1;
          }
          if((success==0)&&(res[cS]>aM1)&&(res[cS]<zP1)){
              if(res[cS]<'z'){
                  res[cS]=res[cS]+1;
                  success=1;
              }
              else{
                  res[cS]='0';
              }
          }
          cS--;
      }
      if(success==0){
          res='1'+res;
      }
      return res;
  }
  std::string findYForNonSubstring(const std::string & x, const std::string & start ="aaa", const std::string &beg="b", const std::string &end="e"){
    //The function will find a string y >= start for which (beg+y+end) is not a substring of x
    std::string y=start;
    while(findAndReplace(x,beg+y+end,"")!=x){
      y=incrementString(y);
    }
    return y;
  }
  std::string findNonSubstring(const std::string &x, const std::string & start ="aaa", const std::string &beg="b",const std::string &end="e"){
    //The function will find a string z that starts with beg, ends with end, is not substring of x,
    //and the middle part y of z (obtained after removing beg and end) is >=start
    return beg+findYForNonSubstring(x,start,beg,end)+end;
  }
  void extendMapWithSubMapOfAnotherMap(std::map<std::string,std::string>& dest, const std::map<std::string,std::string> &src, const std::string & keyPrefix, const std::string & valPrefix, const long & maxNum){
    long i=0;
    std::string key,val;
    std::map<std::string,std::string>::const_iterator itSrc,itSrcE;
    itSrcE=src.end();
    while(i<maxNum){
      itSrc=src.find(keyPrefix+std::to_string(i));
      if(itSrc!=itSrcE){
        key=itSrc->second;
        itSrc=src.find(valPrefix+std::to_string(i));
        if(itSrc!=itSrcE){
          val=itSrc->second;
          dest[key]=val;
        }
      }
      ++i;
    }
  }
  std::string forwardValVarPairs(const std::map<std::string,std::string> & src, const std::string & keyPrefix, const std::string & valPrefix, const long &maxNum){
    std::map<std::string,std::string> kvPairs;
    extendMapWithSubMapOfAnotherMap(kvPairs,src,keyPrefix,valPrefix,maxNum);
    std::map<std::string,std::string>::const_iterator it,itE;
    itE=kvPairs.end();
    it=kvPairs.begin();
    std::string res="";
    long i=0;
    while(it!=itE){
      res+="&"+keyPrefix+std::to_string(i)+"="+it->first;
      res+="&"+valPrefix+std::to_string(i)+"="+it->second;
      ++it;
      ++i;
    }
    return res;
  } 
  template<typename T>
  void assignToConst(const T& receiver, const T& sender){
    *(T*)(&receiver)=sender;
  } 
std::string removeFirstCharacter(const std::string& _in){ 
  std::string out;
  for(long i=1;i<_in.length();++i){
    out+=_in[i];
  }
  return out;
}
void extractComments(std::string& answ, std::string& eC, std::string& eW, std::string& eAll){
    std::map<std::string,std::string> kvMap;
    kvMap["[rc]"]="[r]c";
    kvMap["[rw]"]="[r]w";
    kvMap["[/rc]"]="[/r]";
    kvMap["[/rw]"]="[/r]";
    kvMap["[r]"]="[r]a";
    std::string a2=MFRF::findAndReplace(answ,kvMap);
    std::vector<std::string> allC=SF::stringToVector(a2,"[r]","[/r]");
    kvMap.clear();
    for(long i=0;i<allC.size();++i){
      kvMap["[r]"+allC[i]+"[/r]"]="";
      if(allC[i].length()>1){
        if(allC[i][0]=='c'){
          eC+="[r]"+removeFirstCharacter(allC[i])+"[/r]";
        }
        if(allC[i][0]=='w'){
          eW+="[r]"+removeFirstCharacter(allC[i])+"[/r]";
        }
        if(allC[i][0]=='a'){
          eAll+="[r]"+removeFirstCharacter(allC[i])+"[/r]";
        }
      }
    }
    answ=MFRF::findAndReplace(a2,kvMap);
} 
  std::string insertReasonableLineBreaks(const std::string& in){
    long startConvertingSpaceToLineBreak=80;
    long startConvertingAnythingToLineBreak=100;
    std::string out;
    long i,cLine;
    i=0;
    long sz=in.length();
    cLine=0;
    while(i<sz){
      if((in[i]==' ')||(in[i]=='\t')||(in[i]=='\n')||(static_cast<long>(in[i])==13)){
          if(cLine>startConvertingSpaceToLineBreak){
            out+="\n";
            cLine=0;
          }
          else{
            out+=in[i];
            ++cLine;
          }
      }
      else{
        if(cLine>startConvertingAnythingToLineBreak){
          out+="\n";
          cLine=0;
        }
        ++cLine;
        out+=in[i];
      }
      ++i;
    }
    return out;
  }
  class LSPair{
  public:
    long l;
    std::string s;
    int operator<(const LSPair& )const;
  };
  int LSPair::operator<(const LSPair& b) const{
    if(l<b.l) return 1;
    if(l>b.l) return 0;
    if(s<b.s) return 1;
    return 0;
  }
  LSPair toLSPair(const std::string& s){
    LSPair res;
    res.l=s.size();
    res.s=s;
    return res;
  }
std::string unrollRepetitiveLoop(const std::string& _in,
                                 const std::string& nonPaddingPrefix="!NP!"){
    std::string out;
    std::vector<std::string> comm=stringToVector(_in,"_n*_","_/n*_");
    if(comm.size()<4){
        return _in;
    }
    std::string textTemplate=comm[0];
    std::string variable=comm[1];
    long startValue=BF::stringToInteger(comm[2]);
    long endValue=BF::stringToInteger(comm[3]);
    long paddingNumber=10;
    if(comm.size()>4){
        paddingNumber=BF::stringToInteger(comm[4]);
    }
    std::string tmpText;
    for(long i=startValue;i<endValue;++i){
        tmpText=findAndReplace(textTemplate,nonPaddingPrefix+variable,std::to_string(i));
        out+=findAndReplace(tmpText,variable,BF::padded(i,paddingNumber,"0"));
    }
    return out;
}
std::string addOnlyNewItems(const std::string& _in,
                            const std::string& _n,
                            const std::string& oT, const std::string& cT ){
    std::string out=_in;
    std::set<std::string> sAll=stringToSet(_in,oT,cT);
    std::set<std::string> sNew=stringToSet(_n,oT,cT);
    std::set<std::string>::const_iterator itN=sNew.begin();
    while(itN!=sNew.end()){
        if(sAll.find(*itN)==sAll.end()){
            out+=oT+*itN+cT+"\n";
            sAll.insert(*itN);
        }
        ++itN;
    }
    return out;
}
std::string prepareNewContent(const std::string& _in, const std::string& cOT, const std::string& cCT,
                              const std::string& toAdd, const std::string& oT, const std::string& cT){
    std::vector<std::string> ocV=SF::stringToVector(_in,cOT,cCT);
    std::string oldContent;
    if(ocV.size()>0){
        oldContent=ocV[0];
    }
    return addOnlyNewItems(oldContent,toAdd,oT,cT);
}
    std::string updateRepetitiveText(const std::string& text,
                                     const std::string& _tO="[loop]",
                                     const std::string& _tC="[/loop]"){
        std::map<std::string,std::string> krMap;
        std::vector<std::string> allLoops=stringToVector(text,_tO,_tC);
        if(allLoops.size()<1){return text;}
        for(long i=0;i<allLoops.size();++i){
            krMap[_tO+allLoops[i]+_tC]=unrollRepetitiveLoop(allLoops[i]);
        }
        return MFRF::findAndReplace(text,krMap);
    }
std::pair<std::string,std::string> parameterExtraction(const std::string& _in,
                                                       const std::string& openTag,
                                                       const std::string& closeTag,
                                                       const std::string& _default=""){
    long pos; std::pair<std::string,int> allD;
    std::pair<std::string,std::string> res;
    res.first=_default;res.second=_in;
    pos=0;allD=extract(_in,pos,openTag,closeTag);
    if(allD.second==1){
        res.first=allD.first;
        res.second=findAndReplace(res.second,openTag+allD.first+closeTag,"");
    }
    return res;
}
std::vector<long> cutVector(const std::vector<long>& v, long sz){
    std::vector<long> res;
    res.resize(sz);
    for(long i=0;i<sz;++i){
        res[i]=0;
        if(i<v.size()){
            res[i]=v[i];
        }
    }
    return res;
}
std::vector<long> tagIndicesForBalanceCheck(const std::string &input, const std::vector<std::string> & oT, const std::vector<std::string> & cT){
    std::map<std::string,std::string> replMap;
    std::string oSep="~__**!!__";
    std::string cSep="~/__**!!__";
    for(long i=0;i<oT.size();++i){
        replMap[oT[i]]=oSep+std::to_string(i+1)+cSep;
    }
    for(long i=0;i<cT.size();++i){
        replMap[cT[i]]=oSep+"-"+std::to_string(i+1)+cSep;
    }
    return stringToVectorLong(MFRF::findAndReplace(input,replMap),oSep,cSep);
}
int tagCountMatching(const std::vector<long>& tags){
    std::map<long,long> counters;
    std::map<long,long>::const_iterator it;
    long absI;long currentVal;
    for(long i=0;i<tags.size();++i){
        absI=tags[i];
        if(absI<0){absI*=-1;}
        it=counters.find(absI);
        if(it==counters.end()){
            counters[absI]=tags[i]/absI;
        }
        else{
            currentVal=it->second+tags[i]/absI;
            if(currentVal==0){
                counters.erase(it->first);
            }
            else{
                counters[it->first]=currentVal;
            }
        }
    }
    return (counters.size()==0);
}
int tagBalanceMatching(const std::vector<long>& tags){
    std::stack<long> tagStack;
    int violationHappened=0;
    long i=0; long sz=tags.size();
    while((violationHappened==0)&&(i<sz)){
        if(tags[i]>0){
            tagStack.push(tags[i]);
        }
        else{
            if((tagStack.empty())||(tagStack.top()!=-tags[i])){
                violationHappened=1;
            }
            else{
                tagStack.pop();
            }
        }
        ++i;
    }
    return ((violationHappened==0)&&(tagStack.empty()));
}
int matchingTags(const std::string &input,
                   const std::vector<std::string> & oT,
                   const std::vector<std::string> & cT,
                   const std::string& algorithm="count"){
      std::vector<long> tagVector = tagIndicesForBalanceCheck(input,oT,cT);
      if((algorithm.size()>0) &&
         ( (algorithm[0]=='b') || (algorithm[0]=='B') )
         ){
          // the desired algorithm is specified to be
          // balanceMatching
          return tagBalanceMatching(tagVector);
      } 
      return tagCountMatching(tagVector);
}
}
#endif
