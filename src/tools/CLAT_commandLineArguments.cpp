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

#ifndef _INCL_CLAT_CPP
#define _INCL_CLAT_CPP

namespace CLAT{
    std::string to_string(const std::vector<std::string> &v){
      std::string res;
      res+="[";
      for(long i=0;i<v.size();++i){
        res+=v[i]+";";
      }
      res+="]";
      return res;
    }
    class CommandLineArgs{
        public:
        long silent;
        long helpNeeded;
        long cleanOutputFiles;
        long onlyClean;
        std::string inputFile;
        std::string outputFile;
        CommandLineArgs();
        std::string to_string() const;
    };
    CommandLineArgs::CommandLineArgs(){
        silent=0;
        helpNeeded=0;
        cleanOutputFiles=0;
    }
    std::string CommandLineArgs::to_string() const{
        std::string res;
        res+="Silent = "+std::to_string(silent)+"\n";
        res+="Help = "+std::to_string(helpNeeded)+"\n";
        res+="Clean = "+std::to_string(cleanOutputFiles)+"\n";
        res+="Only clean = "+std::to_string(onlyClean)+"\n";
        res+="Input = "+inputFile+"\n";
        res+="Output= "+outputFile+"\n";
        return res;
    }
    long isElement(const std::string& e, const std::vector<std::string>& v){
        long res=0;
        long sz=v.size();
        long i=0;
        while((i<sz)&&(res==0)){
            if(e==v[i]){
                res=1;
            }
            ++i;
        }
        return res;
    }
    long isSubset(const std::vector<std::string>& a, const std::vector<std::string> &b){
        long res=0;
        long i=0;
        long sz=a.size();
        while((i<sz)&&(res==0)){
            if(isElement(a[i],b)){
                res=1;
            }
            ++i;
        }
        return res;
    }
    std::string getFollowerOf(const std::vector<std::string>& a, const std::vector<std::string>& v){
        std::string res="notFound";
        long found=0;
        long sz=v.size();
        --sz;
        long i=0;
        while((found==0)&&(i<sz)){
            if(isElement(v[i],a)){
                found=1;res=v[i+1];
            }
            ++i;
        }
        return res;
    }
    std::string getFollowerOf(const std::string& a, const std::vector<std::string>& v){
        std::vector<std::string> av;
        av.resize(1);
        av[0]=a;
        return getFollowerOf(av,v);
    }
    CommandLineArgs getArguments(const std::vector<std::string>& v){
        CommandLineArgs res;
        std::vector<std::string> flags;flags.resize(2);
        flags[0]="-i";flags[1]="--input";
        res.inputFile=getFollowerOf(flags,v);
        flags[0]="-o";flags[1]="--output";
        res.outputFile=getFollowerOf(flags,v);
        flags[0]="-s";flags[1]="--silent";
        res.silent=isSubset(flags,v);
        flags[0]="-h";flags[1]="--help";
        res.helpNeeded=isSubset(flags,v);
        flags[0]="-c";flags[1]="--clean";
        res.cleanOutputFiles=isSubset(flags,v);
        flags[0]="-n";flags[1]="--onlyclean";
        res.onlyClean=isSubset(flags,v);
        if(res.outputFile=="notFound"){
            res.outputFile="file.out";
        }
        return res;
    }
    std::vector<std::string> singleElement(const std::string& in){
      std::vector<std::string> res;
      res.resize(1);
      res[0]=in;
      return res;
    }
    std::string convertLetterToCommand(const char& in){
      if(in=='i'){
        return "--input";
      }
      if(in=='o'){
        return "--output";
      }
      if(in=='s'){
        return "--silent";
      }
      if(in=='h'){
        return "--help";
      }
      if(in=='c'){
        return "--clean";
      }
      if(in=='n'){
        return "--onlyclean";
      }
      return "takeEverythingAfter";
    }
    std::vector<std::string> expandSingleItem(const std::string & in){
      std::vector<std::string> res;
      if(in.length()<2){
        return singleElement(in);
      }
      if((in[0]=='-')&&(in[1]=='-')){
        return singleElement(in);
      }
      if(in[0]!='-'){
        return singleElement(in);
      }
      long len=in.length()-1;
      res.resize(len);
      long i=0;
      long everythingOK=1;
      while((i<len)&&(everythingOK==1)){
        res[i]=convertLetterToCommand(in[i+1]);
        if( res[i]=="takeEverythingAfter"){
          everythingOK=0;
          res[i]="";
          for(long j=i;j<len;++j){
            res[i]+=in[j+1];
          }
          std::vector<std::string> resN;
          resN.resize(i+1);
          for(long j=0;j<i+1;++j){
            resN[j]=res[j];
          }
          res=resN;
        }
        ++i;
      }
      return res;
    }
    std::vector<std::vector<std::string> > expandSingleMinuses(const std::vector<std::string>& in){
      std::vector<std::vector<std::string> > expansion;
      long sz=in.size();
      if(sz>0){
        expansion.resize(sz);
        for(long i=0;i<sz;++i){
          expansion[i]=expandSingleItem(in[i]);
        }
      }
      return expansion;
    }
    long totalSize(const std::vector<std::vector<std::string> > &m){
      long sz=m.size();
      long total=0;
      for(long i=0;i<sz;++i){
        total+=m[i].size();
      }
      return total;
    }
    std::vector<std::string> removeDimension(const std::vector<std::vector<std::string> > &m){
      long tsz=totalSize(m);
      std::vector<std::string> res;
      res.resize(tsz);
      long c=0;
      long md=m.size();
      long mr;
      long i=0;long j;
      while(i<md){
        mr=m[i].size();
        j=0;
        while(j<mr){
          res[c]=m[i][j];
          ++j;++c;
        }
        ++i;
      }
      return res;
    }
    std::vector<std::string> treatSingleMinuses(const std::vector<std::string> & in){
      return removeDimension(expandSingleMinuses(in));
    }
    std::vector<std::string> argsInStrings(int argc, char **argv){
        std::vector<std::string> res;
        res.resize(argc-1);
        for(long i=0;i<argc-1;++i){
            res[i]=static_cast<std::string>(argv[i+1]);
        }
        return treatSingleMinuses(res);
    }
}
#endif
