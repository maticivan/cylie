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


#ifndef _INCL_IF_IO_CPP
#define _INCL_IF_IO_CPP

extern char **environ;
namespace IOF{
struct WadminsGroup{
public:
  gid_t gid;
  WadminsGroup();
} GL_wadmins;
WadminsGroup::WadminsGroup(){
  gid = static_cast<gid_t>(-1);
  struct group grp;
  struct group *result = nullptr;
  long bufLen = sysconf(_SC_GETGR_R_SIZE_MAX);
  if(bufLen <= 0){ bufLen = 16384; }
  std::string buf(static_cast<size_t>(bufLen), '\0');
  int rc = getgrnam_r("wadmins", &grp,
                       &buf[0], static_cast<size_t>(bufLen),
                       &result);
  if(rc == 0 && result != nullptr){
    gid = result->gr_gid;
  }
}
  struct ImportantFolders{
  public:
    std::set<std::string> s;
    std::set<std::string> badStrings;
    std::string safeNameOfFolderThatDoesNotExist;
    ImportantFolders();
  } GL_folders;
  ImportantFolders::ImportantFolders(){
    badStrings.insert("/");
    badStrings.insert(".");
    badStrings.insert("\"");
    badStrings.insert("\n");
    badStrings.insert("\t");
    badStrings.insert("\'");
    badStrings.insert("@");
    badStrings.insert("!");
    badStrings.insert("~");
    badStrings.insert("`");
    badStrings.insert("$");
    badStrings.insert("%");
    badStrings.insert("^");
    badStrings.insert("\\");
    badStrings.insert("+");
    badStrings.insert(",");
    badStrings.insert(";");
    badStrings.insert(":");
    badStrings.insert("?");
    badStrings.insert("<");
    badStrings.insert(">");
  }
  int danger(const std::string & folderName){
    if(folderName==""){return 1;}
    if((GL_folders.s).find(folderName)!=(GL_folders.s).end()){
      return 1;
    }
    return 0;
  }
  std::string improveName(const std::string& _fName){
    std::string fName=BF::cleanAllSpaces(_fName);
    long len=fName.length();
    if((len>0)&&(fName[len-1]=='/')){
      std::string fNameOld=fName;
      --len;
      fName="";
      for(long i=0;i<len;++i){
        fName+=fNameOld[i];
      }
    }
    if(danger(fName)){
      fName=GL_folders.safeNameOfFolderThatDoesNotExist;
    }
    return fName;
  }
std::string nameOfFolder(const std::string & fullNameOfFile){
    std::string rnameOfFolder="";
    long sz=fullNameOfFile.size();
    while(sz>0){
        --sz;
        rnameOfFolder += fullNameOfFile[sz];
    }
    rnameOfFolder="!"+rnameOfFolder;
    rnameOfFolder=rnameOfFolder+"/";
    rnameOfFolder=(SF::eraseStuffBetween(rnameOfFolder,"!","/")).first;
    std::string nF="";
    sz=rnameOfFolder.size()-1;
    while(sz>0){
        --sz;
        nF +=rnameOfFolder[sz];
    }
    return nF;
}
int legalFileName(const std::string & in){
  if(MFRF::find(in,GL_folders.badStrings).second==-1){
    return 1;
  }
  return 0;
}
std::string justFileNameNoExtensionNoFolder(const std::string & nameOfFile,
                                            const std::string & beginningToIgnore=""){
    std::string folder=nameOfFolder(nameOfFile);
    std::string justName="";
    long pos=folder.size();
    long totalS=nameOfFile.size();
    while((pos<totalS)&&(nameOfFile[pos]!='.')){
        justName+=nameOfFile[pos];
        ++pos;
    }
    if(justName[0]=='/'){
        std::string temp=justName;
        justName="";
        pos=temp.size();
        while(pos>1){
            --pos;
            justName=temp[pos]+justName;
        }
    }
    pos=beginningToIgnore.size();
    if(pos>0){
        std::string temp2=justName;
        justName="";
        long sz=temp2.size();
        while(pos<sz){
            justName+=temp2[pos];
            ++pos;
        }
    }
    return justName;
}
std::string extensionOfFile(const std::string & fName){
  std::string foldN=nameOfFolder(fName);
  long pos=foldN.size();
  if((pos<fName.size())&&(fName[pos]=='/')){
    ++pos;
  }
  std::string nFWExt=justFileNameNoExtensionNoFolder(fName);
  pos+=nFWExt.size();
  if((pos<fName.size())&&(fName[pos]=='.')){
    ++pos;
  }
  long sz=fName.size();
  std::string fR="";
  while(pos<sz){
    fR+=fName[pos];
    ++pos;
  }
  return fR;
}
std::vector<std::string> selectFilesWithExtension(const std::vector<std::string>& vFiles,
                                                  const std::string & _extension,
                                                  const int & complement = 0){
  std::vector<std::string> fR;
  long num=0;
  long sz=vFiles.size();
  for(long i=0;i<sz;++i){
    if( (_extension==extensionOfFile(vFiles[i])) == 1-complement){++num;}
  }
  fR.resize(num);long counter=0;
  for(long i=0;i<sz;++i){
    if( (_extension==extensionOfFile(vFiles[i])) == 1-complement){
      fR[counter]=vFiles[i];++counter;
    }
  }
  return fR;
}
  int sys_deleteFolderAndSubfolders(const std::string &_folderName){
        std::string folderName=improveName(_folderName);
        std::error_code ec;
        std::experimental::filesystem::remove_all(folderName, ec);
        return 1;
  }
  int sys_deleteFile(const std::string & nameOfFile){
    std::map<std::string,std::string>::iterator it,itE;
    itE=GF::GL_OPENED_FILES.end();
    it=GF::GL_OPENED_FILES.find(nameOfFile);
    if(it!=itE){
      GF::GL_OPENED_FILES.erase(it);
    }
    std::error_code ec;
    std::experimental::filesystem::remove(nameOfFile, ec);
    return 1;
  }
std::vector<std::string> listFiles(const std::string &dirPath){
  std::vector<std::string> fR;
  long sz=0;
  for (const auto & entry : std::experimental::filesystem::directory_iterator(dirPath)){
    ++sz;
  }
  fR.resize(sz);
  long i=0;
  for (const auto & entry : std::experimental::filesystem::directory_iterator(dirPath)){
    fR[i]= entry.path();
    ++i;
  }
  return fR;
}
std::vector<std::string> listFiles(const std::string &dirPath,
                                   const std::set<std::string>
                                   &extensionsWithoutLeadingDotsToIgnore){
    std::vector<std::string> allF=listFiles(dirPath);
    std::stack<std::string> desired;
    for(long i=0;i<allF.size();++i){
        if(extensionsWithoutLeadingDotsToIgnore.find(
                extensionOfFile(allF[i])) ==
           extensionsWithoutLeadingDotsToIgnore.end()){
            desired.push(allF[i]);
        }
    }
    return SF::stackToVector(desired);
}
int sys_folderExists(const std::string &folderName){
  struct stat sb;
  if (stat(folderName.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
      return 1;
  }
  return 0;
}
int sys_clearFolder(const std::string &folderName,const std::string & extension="*"){
    if(folderName==""){return 0;}
    if(!sys_folderExists(folderName)){return 0;}
    std::vector<std::string> fList=listFiles(folderName);
    if(extension!="*"){
      std::string ext=extension;
      if((ext.length()>0)&&(ext[0]=='.')){
        ext=ext.substr(1);
      }
      fList=selectFilesWithExtension(fList,ext);
    }
    long sz=fList.size();
    for(long i=0;i<sz;++i){
      std::error_code ec;
      if(std::experimental::filesystem::is_regular_file(fList[i],ec)){
        sys_deleteFile(fList[i]);
      }
    }
    return 1;
  }
  int fileExists(const std::string &fN){
      std::ifstream fTest(fN);
      if(fTest.good()){
          return 1;
      }
      return 0;
  }
  std::string fileToString(const std::string &filename,const long & getFromDiskNotFromCache=0){
      if(getFromDiskNotFromCache==0){
        std::map<std::string,std::string>::iterator it,itE;
        itE=GF::GL_OPENED_FILES.end();
        it=GF::GL_OPENED_FILES.find(filename);
        if(it!=itE){
          return it->second;
        }
      }
      if(!fileExists(filename)){
        std::map<std::string,std::string>::iterator it,itE;
        itE=GF::GL_OPENED_FILES.end();
        it=GF::GL_OPENED_FILES.find(filename);
        if(it!=itE){
          GF::GL_OPENED_FILES.erase(it);
        }
        return("fileNotFound");
      }
      std::ifstream t(filename);
      t.seekg(0, std::ios::end);
      size_t size = t.tellg();
      std::string buffer(size, ' ');
      t.seekg(0);
      t.read(&buffer[0], size);
      if(GF::GL_OPENED_FILES.size()<GF::GL_MAXFILESINRAM){
        GF::GL_OPENED_FILES[filename]=buffer;
      }
      return buffer;
  }
  int sys_changeNameOfFile(const std::string & f1, const std::string & f2){
      if(fileExists(f2)){
        return 0;
      }
      if(!fileExists(f1)){
        return 1;
      }
      std::error_code ec;
      std::experimental::filesystem::rename(f1, f2, ec);
      fileToString(f1,1);//refreshing RAM
      fileToString(f2,1);//refreshing RAM
      return 1;
  }
int sys_changePermissions(const std::string & fileName, const std::string & permission="664"){
  std::error_code ec;
  std::experimental::filesystem::permissions(
      fileName,
      static_cast<std::experimental::filesystem::perms>(
          std::stoul(permission, nullptr, 8)),
      ec);
  return 1;
}
  int sys_copyFile(const std::string & originalFile, const std::string & newFile){
      std::error_code ec;
      std::experimental::filesystem::copy_file(
          originalFile, newFile,
          std::experimental::filesystem::copy_options::overwrite_existing,
          ec);
      return 1;
  }
  int sys_copyAllFilesButIgnoreThoseThatExist(const std::string & src, const std::string & dst){
    std::error_code ec;
    std::experimental::filesystem::copy(
        src, dst,
        std::experimental::filesystem::copy_options::recursive |
        std::experimental::filesystem::copy_options::skip_existing,
        ec);
    return 1;
  }
#ifndef IOF_NO_SPAWN
  int sys_runProgramNoOutput(const std::vector<std::string> & argv){
    if(argv.empty()){
      return 1;
    }
      std::vector<char*> cargv;
      cargv.resize(argv.size()+1);
      for(long i=0;i<argv.size();++i){
          cargv[i]=const_cast<char*>(argv[i].c_str());
      }
      cargv[argv.size()]=nullptr;
    posix_spawn_file_actions_t fa;
    posix_spawn_file_actions_init(&fa);
    posix_spawn_file_actions_addopen(&fa, 1, "/dev/null", O_WRONLY, 0);
    posix_spawn_file_actions_addopen(&fa, 2, "/dev/null", O_WRONLY, 0);
    pid_t pid;
    int rc = posix_spawnp(&pid, cargv[0], &fa, nullptr,
                           cargv.data(), ::environ);
    posix_spawn_file_actions_destroy(&fa);
    if(rc == 0){
      int status = 0;
      waitpid(pid, &status, 0);
    }
    return 1;
  }
  // Like sys_runProgramNoOutput, but with a wall-clock timeout. Returns:
  //   0  = child exited successfully (exit code 0)
  //   1  = child exited with nonzero status
  //   2  = wall-clock timeout fired; child was killed
  //   3  = posix_spawnp failed (could not spawn)
  // Uses a polling loop (~100ms granularity) since the existing IOF code
  // does not use signal handlers and this preserves that property.
  int sys_runProgramNoOutputWithTimeout(const std::vector<std::string> & argv, const long & timeoutSeconds){
    if(argv.empty()){
      return 3;
    }
    std::vector<char*> cargv;
    cargv.resize(argv.size()+1);
    for(long i=0;i<(long)argv.size();++i){
      cargv[i]=const_cast<char*>(argv[i].c_str());
    }
    cargv[argv.size()]=nullptr;
    posix_spawn_file_actions_t fa;
    posix_spawn_file_actions_init(&fa);
    posix_spawn_file_actions_addopen(&fa, 1, "/dev/null", O_WRONLY, 0);
    posix_spawn_file_actions_addopen(&fa, 2, "/dev/null", O_WRONLY, 0);
    pid_t pid;
    int rc = posix_spawnp(&pid, cargv[0], &fa, nullptr,
                          cargv.data(), ::environ);
    posix_spawn_file_actions_destroy(&fa);
    if(rc != 0){
      return 3;
    }
    long pollIntervalMicroseconds = 100000; // 100 ms
    long maxPolls = (timeoutSeconds * 1000000L) / pollIntervalMicroseconds;
    int status = 0;
    for(long i=0; i<maxPolls; ++i){
      pid_t w = waitpid(pid, &status, WNOHANG);
      if(w == pid){
        if(WIFEXITED(status) && WEXITSTATUS(status) == 0){
          return 0;
        }
        return 1;
      }
      if(w == -1){
        return 1;
      }
      usleep(pollIntervalMicroseconds);
    }
    // Timeout reached. Kill the child and reap it.
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    return 2;
  }
#endif
std::string sys_whoami(){
   uid_t uid = geteuid();
   struct passwd pwd;
   struct passwd *result = nullptr;
   long bufLen = sysconf(_SC_GETPW_R_SIZE_MAX);
   if(bufLen <= 0){ bufLen = 16384; }
   std::string buf(static_cast<size_t>(bufLen), '\0');
   int rc = getpwuid_r(uid, &pwd,
                       &buf[0], static_cast<size_t>(bufLen),
                       &result);
   if(rc == 0 && result != nullptr){
     return std::string(result->pw_name);
   }
   return "";
 }
 
int toFile(const std::string &filename, const  std::string  & s){
    std::string tmpName=filename+".tmp.x"+std::to_string(getpid())+"x";
    std::ofstream mfile;
    mfile.open(tmpName);
    if(!mfile){
      return 0;
    }
    mfile<<s;
    mfile.close();
    if(!mfile){
      sys_deleteFile(tmpName);
      return 0;
    }
    sys_changePermissions(tmpName);
    std::error_code ec;
    std::experimental::filesystem::rename(tmpName, filename, ec);
    if(ec){
      sys_deleteFile(tmpName);
      return 0;
    }
    std::map<std::string,std::string>::iterator itC=GF::GL_OPENED_FILES.find(filename);
    if(itC!=GF::GL_OPENED_FILES.end()){
      itC->second=s;
    }
    else if(GF::GL_OPENED_FILES.size()<GF::GL_MAXFILESINRAM){
      GF::GL_OPENED_FILES[filename]=s;
    }
    return 1;
}
  std::time_t timeOfCreation(const std::string & fName){
    std::experimental::filesystem::file_time_type lwt= std::experimental::filesystem::last_write_time(fName);
    std::time_t ttlwt= std::experimental::filesystem::file_time_type::clock::to_time_t(lwt);
    return ttlwt;
  }
  std::vector<std::time_t> timeOfCreation(const std::vector<std::string> & fNs){
    std::vector<std::time_t> fR;
    long sz=fNs.size();
    if(sz>0){
      fR.resize(sz);
      for(long i=0;i<sz;++i){
        fR[i]=timeOfCreation(fNs[i]);
      }
    }
    return fR;
  }
  std::string deleteOldFiles(const std::string & _folderName, const std::string & _extension, const long & defOfOld, const int & complement = 0){
    std::string fR="";
    std::vector<std::string> fList=listFiles( _folderName);
    fList=selectFilesWithExtension(fList,_extension,complement);
    long sz=fList.size();
    std::time_t crT;
    TMF::Timer tm;long age;long tmNow=tm.timeNow();
    for(long i=0;i<sz;++i){
      crT=timeOfCreation(fList[i]);
      age=tmNow-crT;
      if(age>defOfOld){
        sys_deleteFile(fList[i]);
        fR+="; "+fList[i];
      }
    }
    return fR;
  }
int sys_mkdir(const std::string &folderName){
    if(folderName==""){
      return 0;
    }
    if(sys_folderExists(folderName)){
      return 0;
    }
    std::error_code ec;
    std::experimental::filesystem::create_directory(folderName, ec);
    if(GL_wadmins.gid != static_cast<gid_t>(-1)){
      int chRes=chown(folderName.c_str(), static_cast<uid_t>(-1), GL_wadmins.gid);
    }
    std::experimental::filesystem::permissions(
        folderName,
        static_cast<std::experimental::filesystem::perms>(02774),
        ec);
    return 1;
  }
  int sys_createFolderIfDoesNotExist(const std::string &folderName,
                                     const std::string &fileToTestExistence="readme.txt",
                                     const std::string &contentOfFile="Do not edit this folder.",
                                     const std::string &anotherFileToAdd="noOtherFileToAdd",
                                     const std::string &contOther=" "){
    int fR=0;
    std::string tF=folderName;
    tF+="/"+fileToTestExistence;
    std::ifstream fTest(tF);
    if(!fTest.good()){
        sys_mkdir(folderName);
        toFile(tF,contentOfFile);
        if(anotherFileToAdd!="noOtherFileToAdd"){
          std::string tF2=folderName+"/"+anotherFileToAdd;
          toFile(tF2,contOther);
        }
        fR=1;
    }
    return fR;
  }
}
#endif
