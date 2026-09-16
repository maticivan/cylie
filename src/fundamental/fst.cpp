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

#ifndef __INCL_FST_CPP
#define __INCL_FST_CPP
#include<vector>
#include<stack>
#include<utility>
#include<stdexcept>
namespace fst{
template<typename TTT>
class stack{
private:
    long fastStorageCapacity=100;
    long fastStorageSz=0;
    std::vector<TTT> fastStorage;
    std::stack<TTT> slowStorage;
public:
    stack(long = 100);
    stack(const stack&);
    stack(stack&& );
    stack& operator=(const stack& );
    stack& operator=(stack &&);
    long getFastStorageCapacity() const;
    void setFastStorageCapacity(long );
    void push(const TTT&);
    void pop();
    void reverse();
    const TTT& top() const;
    bool empty() const;
    size_t size() const;
};
template<typename TTT> stack<TTT>::stack(long _fsc){
    if(_fsc<1){
        _fsc=100;
    }
    fastStorageCapacity=_fsc;
    fastStorageSz=0;
    fastStorage.resize(fastStorageCapacity);
}
template<typename TTT> long stack<TTT>::getFastStorageCapacity() const{return fastStorageCapacity;}
template<typename TTT> void stack<TTT>::setFastStorageCapacity(long _fsc){
    if(_fsc<1){
        _fsc=100;
    }
    if(_fsc==fastStorageCapacity){return;}
    fst::stack<TTT> sNew(_fsc);
    while(!(empty()) ){
        sNew.push(top());
        pop();
    }
    sNew.reverse();
    *this=std::move(sNew);
    return;
}
template<typename TTT> void stack<TTT>::push(const TTT& x){
    if(fastStorageSz<fastStorageCapacity){
        fastStorage[fastStorageSz]=x;
        ++fastStorageSz;
        return;
    }
    slowStorage.push(x);
}
template<typename TTT> void stack<TTT>::pop(){
    if(fastStorageSz<1){return;}
    if(!(slowStorage.empty())){
        slowStorage.pop();
        return;
    }
    --fastStorageSz;
}
template<typename TTT> void stack<TTT>::reverse(){
    fst::stack<TTT> sOld=*this;
    slowStorage = std::stack<TTT>();
    long i=0;
    while((i<fastStorageCapacity)&&(!(sOld.empty()))){
        fastStorage[i]=sOld.top();
        ++i;
        sOld.pop();
    }
    while(!(sOld.empty())){
        slowStorage.push(sOld.top());
        sOld.pop();
    }
}
template<typename TTT> const TTT& stack<TTT>::top() const{
    if(fastStorageSz<1){
        throw std::out_of_range("fst::stack::top() called on an empty stack");
    }
    if(!(slowStorage.empty())){return slowStorage.top();}
    return fastStorage[fastStorageSz-1];
}
template<typename TTT> bool stack<TTT>::empty() const{
    return (fastStorageSz==0);
}

template<typename TTT> size_t stack<TTT>::size() const{
    return fastStorageSz+slowStorage.size();
}
template<typename TTT> stack<TTT>::stack(const stack<TTT>& cf){
    fastStorageCapacity=cf.fastStorageCapacity;
    fastStorageSz=cf.fastStorageSz;
    fastStorage=cf.fastStorage;
    slowStorage=cf.slowStorage;
}
template<typename TTT> stack<TTT>::stack(stack<TTT>&& mf){
    fastStorageCapacity=mf.fastStorageCapacity;
    fastStorageSz=mf.fastStorageSz;
    fastStorage=std::move(mf.fastStorage);
    slowStorage=std::move(mf.slowStorage);
    mf.fastStorageSz=0;
    mf.fastStorageCapacity=1;
    std::vector<TTT> v1; v1.resize(1); mf.fastStorage=v1;
    std::stack<TTT> emptySt;mf.slowStorage=emptySt;
}
template<typename TTT> stack<TTT>& stack<TTT>::operator=(const stack<TTT>& cf){
    if(&cf==this){return *this;}
    fastStorageSz=cf.fastStorageSz;
    fastStorageCapacity=cf.fastStorageCapacity;
    fastStorage=cf.fastStorage;
    slowStorage=cf.slowStorage;
    return *this;
}
template<typename TTT> stack<TTT>& stack<TTT>::operator=(stack<TTT> && mf){
    if(&mf==this){return *this;}
    fastStorageSz=mf.fastStorageSz;
    fastStorageCapacity=mf.fastStorageCapacity;
    fastStorage=std::move(mf.fastStorage);
    slowStorage=std::move(mf.slowStorage);
    mf.fastStorageSz=0;
    mf.fastStorageCapacity=1;
    std::vector<TTT> v1; v1.resize(1); mf.fastStorage=v1;
    std::stack<TTT> emptySt;mf.slowStorage=emptySt;
    return *this;
}

}
#endif
