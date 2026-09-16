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


#ifndef _INCL_RCPF_CPP
#define _INCL_RCPF_CPP

namespace RNDF{
  //long GL_randSeed= std::chrono::high_resolution_clock::now().time_since_epoch().count();
  long GL_randSeed= 100;//intentionally deterministic seed.
  std::mt19937 GL_mt_randIF(GL_randSeed);
  long  mTwist(const long & uniformLimit=1000000){
    std::uniform_int_distribution<long> uInt(0,uniformLimit-1);
    return uInt(GL_mt_randIF);
  }

std::vector<long> genMTwistPermutation(const long & l){
// returns a random permutation of {0,1,...,l-1}
  std::vector<long> fR;
  if(l<1){
    return fR;
  }
  std::vector<long> v;
  fR.resize(l);
  v.resize(l);
  long rN;
  for(long i=0;i<l;++i){
    v[i]=i;
  }
  long i=l;long j=0;
  while(i>0){
    rN=mTwist(i);
    --i;
    fR[j]=v[rN];
    if(rN!=i){
      v[rN]=v[i];
    }
    ++j;
  }
  return fR;
}
}
#endif
