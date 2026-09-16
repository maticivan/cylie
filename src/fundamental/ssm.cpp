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

#ifndef _INCL_STsetmap_CPP
#define _INCL_STsetmap_CPP
#include<iostream>

namespace ssm{
  template<typename KEY> class set{
  private:
    class AVLNode{
    public:
      KEY value;
      long leftCount;
      long rightCount;
      long height;
      AVLNode* leftChild;
      AVLNode* rightChild;
      AVLNode(){
        leftChild=nullptr;
        rightChild=nullptr;
        leftCount=-17;
        rightCount=-17;
      }
      AVLNode(const KEY& k ) {
        value = k; leftChild = nullptr;
        rightChild = nullptr;
        leftCount=-17;
        rightCount=-17;
        height = 1;
      }
    }* _dRoot;
    long _size;
    long getHeight(AVLNode * );
    long hDiff(AVLNode * );
    long redoHeight(AVLNode * );
    AVLNode* rRotation(AVLNode* );
    AVLNode* lRotation(AVLNode* );
    AVLNode* balance(AVLNode* );
    AVLNode* insertInTree(AVLNode* , const KEY & , long & );
    AVLNode* getMinNode(AVLNode* );
    AVLNode* rewireMin(AVLNode* );
    AVLNode* removeFromTree(AVLNode* , const KEY& , long & );
    AVLNode* duplicateTree(AVLNode*);
    void clearAVLTree(AVLNode*);
    KEY avl_access(AVLNode* , const long & ) const;
  public:
    set();
    set(const set &);
    set& operator=(const set &);
    set(set&&) noexcept;
    set& operator=(set &&) noexcept;
    long size() const;
    long erase(const KEY & );
    long erase(const set & );
    long insert(const KEY & );
    long insert(const set &);
    set& operator+=(const KEY &);
    set& operator+=(const set &);
    set& operator-=(const KEY &);
    set& operator-=(const set &);
    std::pair<long,KEY> findIK(const KEY &, const long & = 0) const;
    long find(const KEY &, const long & = 0) const;
    long upperBound(const KEY &) const;
    void clear();
    KEY operator[](const long & ) const;
    virtual ~set();
  };
  template<typename KEY> long set<KEY>::getHeight(AVLNode * root){
    if(root==nullptr){
      return 0;
    }
    return root->height;
  }
  template<typename KEY> long set<KEY>::hDiff(AVLNode * root){
    return getHeight(root->rightChild)-getHeight(root->leftChild);
  }
  template<typename KEY> long set<KEY>::redoHeight(AVLNode * root){
    long lHeight = getHeight(root->leftChild);
    long rHeight = getHeight(root->rightChild);
    root->height = rHeight;
    if(lHeight>rHeight){root->height = lHeight;}
    root->height += 1;
    return root->height;
  }
  template<typename KEY>
  typename set<KEY>::AVLNode* set<KEY>::rRotation(AVLNode * root){
    AVLNode * other = root->leftChild;
    root->leftChild = other->rightChild;
    other->rightChild = root;
    long a,c;
    a= other->leftCount;
    c= root->rightCount;
    root->leftCount -= (a+1);
    other->rightCount += (c+1);
    redoHeight(root);
    redoHeight(other);
    return other;
  }
  template<typename KEY>
  typename set<KEY>::AVLNode * set<KEY>::lRotation(AVLNode * root){
    AVLNode * other = root->rightChild;
    root->rightChild = other->leftChild;
    other->leftChild = root;
    long a,c;
    a= other->rightCount;
    c=root->leftCount;
    root->rightCount -= (a+1);
    other->leftCount += (c+1);
    redoHeight(root);
    redoHeight(other);
    return other;
  }
  template<typename KEY>
  typename set<KEY>::AVLNode * set<KEY>::
  balance(AVLNode * root){
    redoHeight(root);
    if( hDiff(root)==2 ){
      if( hDiff(root->rightChild) < 0 ){
        root->rightChild = rRotation(root->rightChild);
      }
      return lRotation(root);
    }
    if( hDiff(root)==-2 ){
      if( hDiff(root->leftChild) > 0 ){
        root->leftChild = lRotation(root->leftChild);
      }
      return rRotation(root);
    }
    return root;
  }
  template<typename KEY>
  typename set<KEY>::AVLNode * set<KEY>::
  insertInTree(AVLNode * root, const KEY & _value, long & insSucc){
    if( root==nullptr ){
      insSucc=1;
      AVLNode *fR=new AVLNode (_value);
      fR->leftCount=0;
      fR->rightCount=0;
      return fR;
    }
    if( _value < root->value ){
      root->leftChild = insertInTree(root->leftChild,_value,insSucc);
      if(insSucc==1){
        root->leftCount+=1;
      }
    }
    else{
      if(root->value < _value ){
        root->rightChild = insertInTree(root->rightChild,_value,insSucc);
        if(insSucc==1){
          root->rightCount+=1;
        }
      }
      else{
        insSucc=0;
        return root;
      }
    }
    if(insSucc==1){
      return balance(root);
    }
    return root;
  }
  template<typename KEY>
  typename set<KEY>::AVLNode * set<KEY>::getMinNode(AVLNode * root){
    if(root->leftChild==nullptr){
      return root;
    }
    return getMinNode(root->leftChild);
  }
  template<typename KEY>
  typename set<KEY>::AVLNode * set<KEY>::rewireMin(AVLNode * root){
    if( root->leftChild==nullptr ){
      return root->rightChild;
    }
    root->leftChild = rewireMin(root->leftChild);
    root->leftCount -= 1;
    return balance(root);
  }
  template<typename KEY>
  typename set<KEY>::AVLNode *
        set<KEY>::removeFromTree(AVLNode * root, const KEY& _value,
           long & remSucc){
    if( root==nullptr ){
      remSucc=0;
      return nullptr;
    }
    if( _value < root->value ){
      root->leftChild = removeFromTree(root->leftChild,_value, remSucc);
      if(remSucc==1){
        root->leftCount -=1;
      }
    }
    else{
      if(root->value < _value ){
        root->rightChild = removeFromTree(root->rightChild,_value,remSucc);
        if(remSucc==1){
          root->rightCount -=1;
        }
      }
      else{
        AVLNode * lTemp = root->leftChild;
        AVLNode * rTemp = root->rightChild;
        long oldleftCount,oldrightCount;
        oldleftCount=root->leftCount;
        oldrightCount=root->rightCount;
        delete root;
        remSucc=1;
        if( rTemp == nullptr ){
          return lTemp;
        }
        AVLNode * rightMin = getMinNode(rTemp);
        rightMin->rightChild = rewireMin(rTemp);
        rightMin->leftChild = lTemp;
        rightMin->leftCount=oldleftCount;
        rightMin->rightCount=oldrightCount-1;
        remSucc=1;
        return balance(rightMin);
      }
    }
    return balance(root);
  }
  template<typename KEY> void set<KEY>::clearAVLTree(AVLNode *root){
    if(root!=nullptr){
      clearAVLTree(root->leftChild);
      clearAVLTree(root->rightChild);
      delete root;
    }
    return;
  }
  template<typename KEY>
  typename set<KEY>::AVLNode *
     set<KEY>::duplicateTree(AVLNode *root){
    AVLNode *newN;
    newN=nullptr;
    if(root!=nullptr){
      newN=new AVLNode;
      newN->value=root->value;
      newN->leftCount=root->leftCount;
      newN->rightCount=root->rightCount;
      newN->height=root->height;
      newN->leftChild = duplicateTree(root->leftChild);
      newN->rightChild= duplicateTree(root->rightChild);
    }
    return newN;
  }
  template<typename KEY> KEY
   set<KEY>::avl_access(AVLNode *root, const long &i) const{
    if(root==nullptr){
      KEY tempWithRandValue;
      return tempWithRandValue;
    }
    if(i==root->leftCount){
      return root->value;
    }
    if(i<root->leftCount){
      return avl_access(root->leftChild,i);
    }
    return avl_access(root->rightChild, i-root->leftCount-1);
  }
  template<typename KEY> set<KEY>::set(){
    _dRoot=nullptr;
    _size=0;
  }
  template<typename KEY> set<KEY>::set(const set<KEY>& _copyFrom){
    _dRoot=nullptr;
    _dRoot= duplicateTree(_copyFrom._dRoot);
    _size= _copyFrom._size;
  }
  template<typename KEY> set<KEY>::set(set<KEY>&& _moveFrom) noexcept{
    _size= _moveFrom._size;
    _moveFrom._size=0;
    _dRoot=_moveFrom._dRoot;
    _moveFrom._dRoot=nullptr;
  }
  template<typename KEY> set<KEY>&
    set<KEY>::operator=(const set<KEY>& _copyFrom){
    if(&_copyFrom!=this){
      clear();
      _dRoot= duplicateTree(_copyFrom._dRoot);
      _size= _copyFrom._size;
    }
    return *this;
  }
  template<typename KEY> set<KEY>&
     set<KEY>::operator=( set<KEY>&& _moveFrom) noexcept{
    if(&_moveFrom!=this){
      AVLNode* tRoot = _dRoot; _dRoot=_moveFrom._dRoot; _moveFrom._dRoot=tRoot;
      long tSize = _size; _size=_moveFrom._size; _moveFrom._size=tSize;
    }
    return *this;
  }
  template<typename KEY> long set<KEY>::size() const{
    return _size;
  }
  template<typename KEY> set<KEY>::~set(){
    clear();
  }
  template<typename KEY> void set<KEY>::clear(){
    _size=0;
    clearAVLTree(_dRoot);
    _dRoot=nullptr;
  }
  template<typename KEY> long set<KEY>::erase(const KEY & _value){
    long delSucc=0;
    _dRoot=removeFromTree(_dRoot,_value,delSucc);
    if(delSucc==1){
      --_size;
    }
    return delSucc;
  }
  template<typename KEY> long set<KEY>::insert(const KEY & _value){
    long addSucc=0;
    _dRoot=insertInTree(_dRoot,_value,addSucc);
    if(addSucc==1){
      ++_size;
    }
    return addSucc;
  }
  template<typename KEY> long set<KEY>::insert(const set<KEY> & _s2){
    long l2=_s2.size();
    long fR=0;
    for(int i=0;i<l2;++i){
      fR+= insert(_s2[i]);
    }
    return fR;
  }
  template<typename KEY> long set<KEY>::erase(const set<KEY> & _s2){
    long l2=_s2.size();
    long fR=0;
    for(int i=0;i<l2;++i){
      fR+= erase(_s2[i]);
    }
    return fR;
  }
  template<typename KEY> set<KEY>&
     set<KEY>::operator+=(const KEY & _value){
    insert(_value);
    return *this;
  }
  template<typename KEY> set<KEY>&
     set<KEY>::operator+=(const set<KEY> & _s){
    insert(_s);
    return *this;
  }
  template<typename KEY> set<KEY>&
     set<KEY>::operator-=(const KEY & _value){
    erase(_value);
    return *this;
  }
  template<typename KEY> set<KEY>&
     set<KEY>::operator-=(const set<KEY> & _s){
    erase(_s);
    return *this;
  }
  template<typename KEY> KEY set<KEY>::operator[](const long & i) const{
    if((i>-1)&&(i<_size)){
      return avl_access(_dRoot,i);
    }
    KEY irrelevantValue;
    return irrelevantValue;
  }
  template<typename KEY> std::pair<long,KEY>
  set<KEY>::findIK(const KEY & _v,
      const long & upperBoundInsteadOfNegOne) const{
    AVLNode* researcher= _dRoot;
    std::pair<long, KEY> found;
    found.first=-1;
    long discardedLeft=0;
    while( (researcher!=nullptr)&&(found.first==-1)){
      if(_v<researcher->value){
        researcher=researcher->leftChild;
      }
      else{
        if(researcher->value < _v){
          discardedLeft+=researcher->leftCount+1;
          researcher=researcher->rightChild;
        }
        else{
          found.first=discardedLeft+researcher->leftCount;
          found.second=researcher->value;
        }
      }
    }
    if(found.first*upperBoundInsteadOfNegOne==-1){
      found.first=discardedLeft;
    }
    return found;
  }
  template<typename KEY> long
  set<KEY>::find(const KEY & _v,
      const long & upperBoundInsteadOfNegOne) const{
     return findIK(_v,upperBoundInsteadOfNegOne).first;
  }
  template<typename KEY> long
    set<KEY>::upperBound(const KEY & _v) const{
    return find(_v,1);
  }
  template<typename KEY, typename VAL> class map{
  private:
    struct CompPair{
    public:
      KEY k;
      VAL v;
      int operator<(const CompPair& ) const;
    };
    set<CompPair> s;
  public:
    long size() const;
    long erase(const KEY & );
    long erase(const map & );
    long insert(const KEY & , const VAL &);
    long insert(const map &);
    std::pair<long,std::pair<KEY,VAL> >
    findIKV(const KEY & , const long & = 0) const;
    long find(const KEY &, const long & = 0) const;
    long upperBound(const KEY &) const;
    void clear();
    std::pair<KEY,VAL> operator[](const long & ) const;
  };
  template<typename KEY, typename VAL>
  int map<KEY,VAL>::CompPair::operator<(const CompPair& oth) const{
    if(k<oth.k){return 1;}
    return 0;
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::size() const{
    return s.size();
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::erase(const KEY & _k){
    CompPair p;
    p.k=_k;
    return s.erase(p);
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::erase(const map<KEY,VAL>& _m){
    return s.erase(_m.s);
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::insert(const KEY& _k, const VAL& _v){
    CompPair p;
    p.k=_k;p.v=_v;
    return s.insert(p);
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::insert(const map<KEY,VAL>& _m){
    return s.insert(_m.s);
  }
  template<typename KEY, typename VAL>
  std::pair<long, std::pair<KEY,VAL> >
  map<KEY,VAL>::findIKV(const KEY & _k,
  const long & upperBoundInsteadOfNegOne) const{
     CompPair p;
     p.k=_k;
     std::pair<long, CompPair> res=s.findIK(p,upperBoundInsteadOfNegOne);
     std::pair<long,std::pair<KEY,VAL> > retVal;
     retVal.first=res.first;
     retVal.second.first=res.second.k;
     retVal.second.second=res.second.v;
     return retVal;
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::find(const KEY & _k,
    const long & upperBoundInsteadOfNegOne) const{
    CompPair p;
    p.k=_k;
    return s.find(p,upperBoundInsteadOfNegOne);
  }
  template<typename KEY, typename VAL>
  long map<KEY,VAL>::upperBound(const KEY & _k) const{
    return find(_k,1);
  }
  template<typename KEY, typename VAL>
  void map<KEY,VAL>::clear(){
    s.clear();
  }
  template<typename KEY, typename VAL>
  std::pair<KEY,VAL> map<KEY,VAL>::operator[](const long & j) const{
    std::pair<KEY,VAL> res;
    CompPair p=s[j];
    res.first=p.k; res.second=p.v;
    return res;
  }
}
#endif
