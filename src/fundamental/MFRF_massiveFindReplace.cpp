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

#ifndef _INCL_MFRF_massiveFindReplace_CPP
#define _INCL_MFRF_massiveFindReplace_CPP
namespace MFRF{
  char toLowerCase(const char & x){
      char fR=x;
      if((x>='A')&&(x<='Z')){
          fR=fR-'A'+'a';
      }
      return fR;
  }
  std::string toLowerCase(const std::string & input){
      std::string output=input;
      long len = input.length();
      for(long i=0;i<len;++i){
          output[i]=toLowerCase(input[i]);
      }
      return output;
  }
  template<typename CCC,typename SSS>
  struct MassiveSearchTreeNode{
  public:
      CCC mKey;
      int isLeaf; int isRoot;
      SSS externalKeyIfLeaf;
      long height;
      MassiveSearchTreeNode(const long & = 0);
      std::map<CCC, MassiveSearchTreeNode<CCC,SSS>*> children;
  };
  template<typename CCC, typename SSS>
  MassiveSearchTreeNode<CCC,SSS>::MassiveSearchTreeNode(const long & _h){
      isLeaf=0; isRoot=0; height=_h;
  }
  template<typename CCC, typename SSS>
  MassiveSearchTreeNode<CCC,SSS>* deleteEntireTree(MassiveSearchTreeNode<CCC,SSS>* aRoot){
      if(aRoot==nullptr){return nullptr;}
      typename std::map<CCC,MassiveSearchTreeNode<CCC,SSS>*>::iterator it,itE;
      itE=(aRoot->children).end();
      it=(aRoot->children).begin();
      while(it!=itE){
          it->second=deleteEntireTree(it->second);
          ++it;
      }
      delete aRoot;
      return nullptr;
  }
  template<typename CCC, typename SSS>
  MassiveSearchTreeNode<CCC,SSS>* deleteEveryTreeInMap(std::map<CCC,MassiveSearchTreeNode<CCC,SSS>* >& mRoots){
      typename std::map<CCC,MassiveSearchTreeNode<CCC,SSS>*>::iterator it,itE;
      itE=mRoots.end();
      it=mRoots.begin();
      while(it!=itE){
          it->second=deleteEntireTree(it->second);
          ++it;
      }
      return nullptr;
  }
  template<typename CCC,typename SSS>
  MassiveSearchTreeNode<CCC,SSS>* addToTree(MassiveSearchTreeNode<CCC,SSS>* aOldRoot, const SSS& toAdd, const long& start, const int& _isRoot, const CCC& _mKey){
      if(start>toAdd.size()){return nullptr;}
      MassiveSearchTreeNode<CCC,SSS>* aNewRoot=aOldRoot;
      if(aNewRoot==nullptr){
          aNewRoot=new MassiveSearchTreeNode<CCC,SSS>(start);
          aNewRoot->isRoot=_isRoot;
          aNewRoot->mKey=_mKey;
      }
      if(aNewRoot->isLeaf){
          return aNewRoot;
      }
      if(start==toAdd.size()){
          aNewRoot->isLeaf=1;
          aNewRoot->externalKeyIfLeaf=toAdd;
          deleteEveryTreeInMap(aNewRoot->children);
          return aNewRoot;
      }
      MassiveSearchTreeNode<CCC,SSS>* addressOfAdding=nullptr;
      typename std::map<CCC,MassiveSearchTreeNode<CCC,SSS>* >::iterator it;
      it=(aNewRoot->children).find(toAdd[start]);
      if(it!=(aNewRoot->children).end()){
          addressOfAdding=it->second;
      }
      (aNewRoot->children)[toAdd[start]]=addToTree(addressOfAdding,toAdd,start+1,0,toAdd[start]);
      return aNewRoot;
  }
  MassiveSearchTreeNode<char,std::string>* createMassSearchTree(const std::map<std::string,std::string> & kRM){
      MassiveSearchTreeNode<char,std::string>* aRes=nullptr;
      std::map<std::string,std::string>::const_iterator it,itE;
      it=kRM.begin();
      itE=kRM.end();
      while(it!=itE){
          aRes=addToTree(aRes,it->first,0,1,'#');
          ++it;
      }
      return aRes;
  }
  std::pair<std::string,long> find(const std::string& x, MassiveSearchTreeNode<char,std::string>* aRoot,const int& caseSensitive){
      std::pair<std::string,long> result;
      result.second=-1;
      long mainCounter=0;
      std::map<MassiveSearchTreeNode<char,std::string>* ,long> activeKeys, nextActiveKeys;
      std::map<MassiveSearchTreeNode<char,std::string>* ,long>::iterator it,itE;
      long sz=x.size();
      int reachedTheEndOfOneOfTheKeys=0;
      std::string keyThatWasRecognized;
      std::map<char, MassiveSearchTreeNode<char,std::string>*>::const_iterator itCh,itChE;
      std::map<std::string,std::string>::const_iterator itSSM;
      char nextChar;
      while((mainCounter<sz)&&(reachedTheEndOfOneOfTheKeys==0)){
          activeKeys[aRoot]=0;
          reachedTheEndOfOneOfTheKeys=0;
          itE=activeKeys.end();
          it=activeKeys.begin();
          nextChar=x[mainCounter];
          if(caseSensitive==0){
            nextChar=toLowerCase(nextChar);
          }
          while((it!=itE)&&(reachedTheEndOfOneOfTheKeys==0)){
              itChE=((it->first)->children).end();
              itCh=((it->first)->children).find(nextChar);
              if(itCh!=itChE){
                nextActiveKeys[itCh->second]=(it->second)+1;
                if((itCh->second)->isLeaf){
                  reachedTheEndOfOneOfTheKeys=1;
                  keyThatWasRecognized=(itCh->second)->externalKeyIfLeaf;
                }
              }
              ++it;
          }
          if(reachedTheEndOfOneOfTheKeys){
            result.first=keyThatWasRecognized;
            result.second=mainCounter-keyThatWasRecognized.size()+1;
          }
          else{
            activeKeys=std::move(nextActiveKeys);
          }
          ++mainCounter;
      }
      return result;
  }
  std::string findAndReplace(const std::string& x, const std::map<std::string,std::string> & keyReplacementMap, MassiveSearchTreeNode<char,std::string>* aRoot,const int& caseSensitive){
      std::string result;
      long mainCounter=0;
      long lastCopied=-1;
      std::map<MassiveSearchTreeNode<char,std::string>* ,long> activeKeys, nextActiveKeys;
      std::map<MassiveSearchTreeNode<char,std::string>* ,long>::iterator it,itE;
      long sz=x.size();
      long maxLoss,maxKeep;
      int reachedTheEndOfOneOfTheKeys;
      std::string keyThatWasRecognized;
      std::map<char, MassiveSearchTreeNode<char,std::string>*>::const_iterator itCh,itChE;
      std::map<std::string,std::string>::const_iterator itSSM;
      long lastWriting=0;
      char nextChar;
      while(mainCounter<sz){
          maxLoss=0;maxKeep=0;activeKeys[aRoot]=0;
          reachedTheEndOfOneOfTheKeys=0;
          itE=activeKeys.end();
          it=activeKeys.begin();
          nextChar=x[mainCounter];
          if(caseSensitive==0){
            nextChar=toLowerCase(nextChar);
          }
          while((it!=itE)&&(reachedTheEndOfOneOfTheKeys==0)){
              itChE=((it->first)->children).end();
              itCh=((it->first)->children).find(nextChar);
              if(itCh!=itChE){
                nextActiveKeys[itCh->second]=(it->second)+1;
                if((it->second)+1>maxKeep){
                  maxKeep=(it->second)+1;
                }
                if((itCh->second)->isLeaf){
                  reachedTheEndOfOneOfTheKeys=1;
                  keyThatWasRecognized=(itCh->second)->externalKeyIfLeaf;
                  maxKeep=(it->second)+1;
                }
              }
              else{
                if((it->second)+1>maxLoss){
                  maxLoss=(it->second)+1;
                }
              }
              ++it;
          }
          if(reachedTheEndOfOneOfTheKeys){
            nextActiveKeys.clear();
            maxLoss=0;
            itE=activeKeys.end();
            it=activeKeys.begin();
            while(it!=itE){
              if((it->second)+1>maxLoss){
                maxLoss=(it->second)+1;
              }
              ++it;
            }
          }
          if(maxLoss>maxKeep){
            lastWriting=mainCounter-maxKeep+1;
            for(long i=mainCounter-maxLoss+1;i<mainCounter-maxKeep+1;++i){
              result+=x[i];
            }
          }
          if(reachedTheEndOfOneOfTheKeys){
            itSSM=keyReplacementMap.find(keyThatWasRecognized);
            if(itSSM!=keyReplacementMap.end()){
              result+=itSSM->second;
            }
            lastWriting=mainCounter+1;
          }
          activeKeys=std::move(nextActiveKeys);
          ++mainCounter;
      }
      for(long i=lastWriting;i<sz;++i){
        result+=x[i];
      }
      return result;
  }
  std::string findAndReplace(const std::string& inputWord,const std::map<std::string,std::string> & _keyReplacementMap, const int& caseSensitive=0){
      if((inputWord.length()<1)||(_keyReplacementMap.size()<1)){
        return inputWord;
      }
      std::map<std::string,std::string> keyReplacementMap;
      if(caseSensitive){
        keyReplacementMap=_keyReplacementMap;
      }
      else{
        std::map<std::string,std::string>::const_iterator it,itE;
        itE=_keyReplacementMap.end();
        it=_keyReplacementMap.begin();
        std::string lCase;
        while(it!=itE){
          lCase=toLowerCase(it->first);
          if(lCase!=(it->second)){
            keyReplacementMap[lCase]=it->second;
          }
          ++it;
        }
      }
      if(keyReplacementMap.size()<1){
        return inputWord;
      }
      MassiveSearchTreeNode<char,std::string>* aRoot=createMassSearchTree(keyReplacementMap);
      std::string result=findAndReplace(inputWord,keyReplacementMap,aRoot,caseSensitive);
      deleteEntireTree(aRoot);
      return result;
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::set<std::string> & _keys, const int& caseSensitive=0){
    // Result is the pair.
    //        The second component is -1, if none of the elements
    //        of the set _keys was a substring of inputWord
    //        Otherwise, the second component is the first index i
    //        at which an element of the set starts as a substring
    //        of inputWord. The first component is the exact element
    //        of the set that appears as the substring.
      if((inputWord.length()<1)||(_keys.size()<1)){
        std::pair<std::string,long> empty;
        empty.second=-1;
        return empty;
      }
      std::map<std::string,std::string> keyReplacementMap;
      std::set<std::string>::const_iterator it,itE;
      itE=_keys.end();
      it=_keys.begin();
      std::string nextKey;
      while(it!=itE){
        nextKey=*it;
        if(caseSensitive==0){
          nextKey=toLowerCase(nextKey);
        }
        keyReplacementMap[nextKey]="";
        ++it;
      }
      MassiveSearchTreeNode<char,std::string>* aRoot=createMassSearchTree(keyReplacementMap);
      std::pair<std::string,long> result=find(inputWord,aRoot,caseSensitive);
      deleteEntireTree(aRoot);
      return result;
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0,const std::string _key1, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    allStrs.insert(_key1);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0,const std::string _key1, const std::string _key2, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    allStrs.insert(_key1);
    allStrs.insert(_key2);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0,const std::string _key1, const std::string _key2, const std::string _key3, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    allStrs.insert(_key1);
    allStrs.insert(_key2);
    allStrs.insert(_key3);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0,const std::string _key1, const std::string _key2, const std::string _key3, const std::string _key4, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    allStrs.insert(_key1);
    allStrs.insert(_key2);
    allStrs.insert(_key3);
    allStrs.insert(_key4);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0,const std::string _key1, const std::string _key2, const std::string _key3, const std::string _key4, const std::string _key5, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    allStrs.insert(_key1);
    allStrs.insert(_key2);
    allStrs.insert(_key3);
    allStrs.insert(_key4);
    allStrs.insert(_key5);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::pair<std::string,long> find(const std::string& inputWord,const std::string _key0,const std::string _key1, const std::string _key2, const std::string _key3, const std::string _key4, const std::string _key5, const std::string _key6, const int& caseSensitive=0){
    std::set<std::string> allStrs;
    allStrs.insert(_key0);
    allStrs.insert(_key1);
    allStrs.insert(_key2);
    allStrs.insert(_key3);
    allStrs.insert(_key4);
    allStrs.insert(_key5);
    allStrs.insert(_key6);
    return find(inputWord,allStrs,caseSensitive);
  }
  std::map<std::string,std::string> toMap(const std::vector<std::string>& _k, const std::vector<std::string>& _v){
    std::map<std::string,std::string> res;
    long sz=_k.size();
    long sz2=_v.size();
    if(sz2<sz){sz=sz2;}
    for(long i=0;i<sz;++i){res[_k[i]]=_v[i];}
    return res;
  }
  std::map<std::string,std::string> toMap(const std::vector<std::pair<std::string, std::string> >& v, const int& direction){
    std::map<std::string,std::string> res;
    long sz=v.size();
    for(long i=0;i<sz;++i){
      if(direction==-1){
        res[v[i].second]=v[i].first;
      }
      else{
        res[v[i].first]=v[i].second;
      }
    }
    return res;
  }
  std::string findAndReplace(const std::string& inputWord,const std::vector<std::string>& keys, const std::vector<std::string>& vals, const int& caseSensitive=0){
    return findAndReplace(inputWord,toMap(keys,vals),caseSensitive);
  }
  void addToReplMap(std::map<std::string,std::string>& m, const std::string& k, const std::string& v){
    if((k!="")&&(k!=v)){
      m[k]=v;
    }
  }
  std::string findAndReplace(const std::string& inputWord,
                             const std::string& k00, const std::string& v00,
                             const std::string& k01, const std::string& v01="",
                             const std::string& k02="", const std::string& v02="",
                             const std::string& k03="", const std::string& v03="",
                             const std::string& k04="", const std::string& v04="",
                             const std::string& k05="", const std::string& v05="",
                             const std::string& k06="", const std::string& v06="",
                             const std::string& k07="", const std::string& v07="",
                             const std::string& k08="", const std::string& v08="",
                             const std::string& k09="", const std::string& v09="",
                             const std::string& k10="", const std::string& v10=""
                            ){
    std::map<std::string,std::string> replMap;
    addToReplMap(replMap,k00,v00);
    addToReplMap(replMap,k01,v01);
    addToReplMap(replMap,k02,v02);
    addToReplMap(replMap,k03,v03);
    addToReplMap(replMap,k04,v04);
    addToReplMap(replMap,k05,v05);
    addToReplMap(replMap,k06,v06);
    addToReplMap(replMap,k07,v07);
    addToReplMap(replMap,k08,v08);
    addToReplMap(replMap,k09,v09);
    addToReplMap(replMap,k10,v10);
    return findAndReplace(inputWord,replMap);
  }
}
#endif
