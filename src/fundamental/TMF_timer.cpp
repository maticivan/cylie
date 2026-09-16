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


#ifndef _INCL_TIMER_CPP
#define _INCL_TIMER_CPP

namespace TMF{
  const long GLOBAL_NUM_DAYS_IN_WEEK=7;
  const long GLOBAL_NUM_MONTHS_IN_YEAR=12;
  const std::string GLOBAL_DAYS_OF_WEEK_A[GLOBAL_NUM_DAYS_IN_WEEK] = {
     "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  const std::string GLOBAL_MONTHS_A[GLOBAL_NUM_MONTHS_IN_YEAR] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep","Oct","Nov","Dec"};
  const long GLOBAL_DAYS_IN_MONTHS_IN_NONLEAP_YEAR[12]={
          31,28,31,30,31,30,31,31,30,31,30,31
        };
  long numDaysInYear(const long & y){
    if( y%4!=0){
      return 365;
    }
    if(y%400==0){
      return 366;
    }
    if(y%100==0){
      return 365;
    }
    return 366;
  }
  long getDaysSinceYYYY(const std::vector<long> &_t,const long & _yS=1900){
    // (_t[0], _t[1])=(yr, mo)
    // y is a year >=1600
    // m is the number of months passed in the year y; m\in{0,1,2,...,11}
    // _t[2] is days passed after (yr,mo) time mark
    long ySince=_yS;
    if(ySince%100!=0){
      ySince=(ySince/100)*100;
    }
    if(_t.size()<3){return -1;}
    long y=_t[0];
    long m=_t[1];
    long d=_t[2];
    long gregorianAdjust=0;

    for(long adjuster=ySince;adjuster<y;adjuster+=100){
        if((adjuster%100==0)&&(adjuster%400!=0)){gregorianAdjust-=1;}
    }
    long numFeb29PrevYears=(y-ySince)/4+gregorianAdjust+1;
    if((y-ySince)%4==0){--numFeb29PrevYears;}
    d+= (y-ySince)*365+numFeb29PrevYears;
    if(m>0){
      d+=31;
    }
    if(m>1){
      d+=28;
      if( numDaysInYear(y)==366){
        d+=1;
      }
    }
    for(long j=2;j<11;++j){
      if(m>j){
        d+=GLOBAL_DAYS_IN_MONTHS_IN_NONLEAP_YEAR[j];
      }
    }
    return d;
  }
  long getSecondsSinceYYYY(const std::vector<long> & _t,const long & _yS=1900){
    // _t has the form (y,m,d,hr,min,sec);
    if(_t.size()<6){return -1;}
    return ((getDaysSinceYYYY(_t,_yS)*24+_t[3])*60+_t[4])*60+_t[5];
  }
  std::vector<long> daysSinceYYYYToYMD(const long & _d, const long & _yS=1900){
     std::vector<long> fR;
     fR.resize(3);
     long ySince=_yS;
     if(ySince%100!=0){
       ySince=(ySince/100)*100;
     }
     fR[0]=ySince;fR[1]=0;fR[2]=0;
     long bigYearBlockNDays=400*365+97;
     fR[0] += 400 *(_d/bigYearBlockNDays);
     long d=_d%bigYearBlockNDays;
     while(d>numDaysInYear(fR[0])){
       d-=numDaysInYear(fR[0]);
       fR[0]+=1;
     }
     if(d>31){
       fR[1] +=1;
       d-=31;
     }
     else{
       fR[2]=d;
       return fR;
     }
     if(numDaysInYear(fR[0])==365){
       if(d>28){
         d-=28;
         fR[1] +=1;
       }
       else{
         fR[2]=d;
         return fR;
       }
     }
     else{
       if(d>29){
         d-=29;
         fR[1]+=1;
       }
       else{
         fR[2]=d;
         return fR;
       }
     }
     for(long j=2;j<11;++j){
        if(d>GLOBAL_DAYS_IN_MONTHS_IN_NONLEAP_YEAR[j]){
         d-=GLOBAL_DAYS_IN_MONTHS_IN_NONLEAP_YEAR[j];
         fR[1]+=1;
        }
        else{
          fR[2]=d;
          return fR;
        }
     }
     fR[2]=d;
     return fR;
  }
std::string secToDHMS(long sT){
    std::string res;
    long sec=sT%60;
    sT/=60;
    long min=sT%60;
    sT/=60;
    long hr=sT%24;
    sT/=24;
    long d=sT;
    if(d<100){res+=" ";}
    if(d<10){res+=" ";}
    res+=std::to_string(d);
    res+="days and ";
    if(d==0){res="";}
    if(hr<10){res+="0";}
    res+=std::to_string(hr);
    res+=":";
    if(min<10){res+="0";}
    res+=std::to_string(min);
    res+=":";
    if(sec<10){res+="0";}
    res+=std::to_string(sec);
    return res;
}
  std::vector<long> standardizeTimeVector(const std::vector<long> & _t, const long & _yS=1900){
    // input is a vector of  components (Yr, Mo, Dy, Hr, Min, Sec)
    // where Yr and Mo are correct, however (Dy, Hr, Min, Sec) may be unusual as a result of addition of dates
    // output will clear it so that Mo\in[0,11], Dy - appropriate for the month and year;
    // Hr\in [0,23]; Min\in [0,59]; Sec\in[0,59];
    if(_t.size()!=6){
      std::vector<long> empty;empty.resize(0);return empty;
    }
    std::vector<long> tmp=_t;
    if(tmp[5]>59){
      tmp[4] += (tmp[5])/60;
      tmp[5] = (tmp[5])%60;
    }
    if(tmp[4]>59){
      tmp[3] += (tmp[4])/60;
      tmp[4] = (tmp[4])%60;
    }
    if(tmp[3]>24){
      tmp[2] += (tmp[3])/24;
      tmp[3] = (tmp[3])%24;
    }
    long ySince=_yS;
    if(ySince%100!=0){
      ySince=(ySince/100)*100;
    }
    long daysSinceYYYY = getDaysSinceYYYY(tmp,ySince);
    std::vector<long> ymd=daysSinceYYYYToYMD(daysSinceYYYY,ySince);
    tmp[0]=ymd[0];tmp[1]=ymd[1];tmp[2]=ymd[2];
    return tmp;
  }
  std::vector<long> stGMT_to_timeVectorTMF(const std::string & _input){
    // returns (Yr, Mo, Dy, Hr, Min, Sec)
    // input should be of the form
    // Wed, 19 Aug 2020 13:59:14 GMT
    // 01234567890123456789012345678
    std::vector<long> fR;
    fR.resize(6);
    for(long i=0;i<6;++i){fR[i]=-1;}
    std::vector<long> signalForBadInput=fR;
    if(_input.length() < 25){return signalForBadInput;}
    std::string tmp;
    tmp="";
    tmp+=_input[5];
    tmp+=_input[6];
    fR[2]=BF::stringToInteger(tmp);
    if((fR[2]<0)||(fR[2]>31)){return signalForBadInput;}
    tmp="";
    tmp+=_input[8];
    tmp+=_input[9];
    tmp+=_input[10];
    long i=0;
    while(i<GLOBAL_NUM_MONTHS_IN_YEAR){
      if(tmp==GLOBAL_MONTHS_A[i]){
        fR[1]=i;
        i=GLOBAL_NUM_MONTHS_IN_YEAR+1;
      }
      ++i;
    }
    if((fR[1]<0)||(fR[1]>GLOBAL_NUM_MONTHS_IN_YEAR)){return signalForBadInput;}
    tmp="";
    tmp+=_input[12];
    tmp+=_input[13];
    tmp+=_input[14];
    tmp+=_input[15];
    fR[0]=BF::stringToInteger(tmp);
    if(fR[1]<0){return signalForBadInput;}
    tmp="";
    tmp+=_input[17];
    tmp+=_input[18];
    fR[3]=BF::stringToInteger(tmp);
    if((fR[3]<0)||(fR[3]>23) ){return signalForBadInput;}
    tmp="";
    tmp+=_input[20];
    tmp+=_input[21];
    fR[4]=BF::stringToInteger(tmp);
    if((fR[4]<0)||(fR[4]>59)){return signalForBadInput;}
    tmp="";
    tmp+=_input[23];
    tmp+=_input[24];
    fR[5]=BF::stringToInteger(tmp);
    if((fR[5]<0)||(fR[5]>59)){return signalForBadInput;}
    return fR;
  }

  class Timer{
  private:
    long timerStart;
    long timerEnd;
    long timerInProgress;
      long timerStartSec;
      long timerEndSec;
  public:
    Timer(const long & =0);
    void start();
    void end();
    long timeNow(const long & = 1000000000) const;
    double getTime() const;
      long getTimeSec() const;
      std::string getTimeString() const;
    std::string dayOfTheWeekAbbreviation(const long &) const;
    std::string monthAbbreviation(const long &) const;
    std::vector<long> timeNowVector() const;
      long timeNowSec() const;
    // returns (year,month,day,hour,minute,second,dayOfTheWeek)
    // month \in {0,1,..., 11}
    // day \in {1,2,..., NumberOfDaysInTheMonth}
    // hour \in {0,1,...,23}
    // minute \in {0,1,...,59}
    // second \in {0,1,...,59}
    // dayOfTheWeek \in {0,1,...,6}

    std::string cookieExpiration(const std::vector<long> &) const;
    // the vector in the argument must have 6 elements and first two must be 0

    std::string cookieExpiration(const long &, const long &, const long &, const long &) const;
    // arguments: 1) days; 2) hours; 3) minutes; 4) seconds;
    // arguments are added to the current time to obtain the expiration of cookie
    // days, hours, minutes, and seconds must be non-negative integers - they can be large

    std::string timeYYYYMMDD() const;
    std::string timeYYYYMMDD(const std::vector<long> &) const;
    std::string timeYYYYMM() const;
    std::string timeYYYYMM(const std::vector<long> &) const;
    std::string timeYYYY() const;
    std::string timeYYYY(const std::vector<long> &) const;
    std::string timeString() const;
    std::string timeString(const std::vector<long> &) const;
    std::vector<long> addTime(const std::vector<long> & , const std::vector<long> & ) const;
  };
  Timer::Timer(const long & _inProgress){
    timerInProgress=_inProgress;
    if(timerInProgress==1){
      timerStart=std::chrono::high_resolution_clock::now().time_since_epoch().count();
        timerStartSec=timeNowSec();
    }
  }
  void Timer::start(){
    timerInProgress=1;
    timerStart=std::chrono::high_resolution_clock::now().time_since_epoch().count();
      timerStartSec=timeNowSec();
  }
  void Timer::end(){
    timerInProgress=2;
    timerEnd=std::chrono::high_resolution_clock::now().time_since_epoch().count();
      timerEndSec=timeNowSec();
  }
  double Timer::getTime() const{
    if(timerInProgress!=2){
      return -1.0;
    }
    double fR= double(timerEnd)-double(timerStart);
    fR/= 1000000.0;
    return fR;
  }
long Timer::getTimeSec() const{
  if(timerInProgress!=2){
    return -1.0;
  }
  return timerEndSec-timerStartSec;
}
std::string Timer::getTimeString() const{
    if(timerInProgress!=2){
      return "NA";
    }
    return secToDHMS(timerEndSec-timerStartSec);
}
  long Timer::timeNow(const long &precision) const{
    long fR= std::chrono::high_resolution_clock::now().time_since_epoch().count();
    return fR/precision;
  }
  std::string Timer::dayOfTheWeekAbbreviation(const long & _wd) const{
    long wd=_wd;
    while(wd<0){wd+=GLOBAL_NUM_DAYS_IN_WEEK;}
    return GLOBAL_DAYS_OF_WEEK_A[wd% GLOBAL_NUM_DAYS_IN_WEEK];
  }
  std::string Timer::monthAbbreviation(const long & _m) const{
    long m=_m;
    while(m<0){m+=GLOBAL_NUM_MONTHS_IN_YEAR;}
    return GLOBAL_MONTHS_A[m%GLOBAL_NUM_MONTHS_IN_YEAR];
  }
  std::vector<long> Timer::addTime(const std::vector<long> & _t7, const std::vector<long> & _add6) const{
    // _t has 7 components:
    // _t[0]=yr, _t[1]=mo, _t[2]=d, _t[3]=hr, _t[4]=m, _t[5]=sec, _t[6]=dayOfTheWeek (range 0-6)
    // return has also 7 components
    // _add has 6 components but yr and mo must be 0.; it does not have dayOfTheWeek
    std::vector<long> empty;
    empty.resize(0);
    if((_t7.size()!=7)||(_add6.size()!=6)){
      return empty;
    }
    if((_add6[0]!=0)||(_add6[1]!=0)){
      return empty;
    }
    if(_t7[0]<1700){return empty;}
    if((_t7[1]<0)||(_t7[1]>GLOBAL_NUM_MONTHS_IN_YEAR-1)){return empty;}
    if(_t7[2]<0){return empty;}
    if(_t7[3]<0){return empty;}
    if(_t7[4]<0){return empty;}
    if(_t7[5]<0){return empty;}
    if((_t7[6]<0)||(_t7[6]>GLOBAL_NUM_DAYS_IN_WEEK-1)){return empty;}
    if(_add6[2]<0){return empty;}
    if(_add6[3]<0){return empty;}
    if(_add6[4]<0){return empty;}
    if(_add6[5]<0){return empty;}
    long carryOverDays=0;
    long sec=_t7[5]+_add6[5];
    long min=_t7[4]+_add6[4];
    long hr=_t7[3]+_add6[3];
    if(sec>59){
      min += sec/60;
      sec = sec%60;
    }
    if(min>59){
      hr += min/60;
      min = min%60;
    }
    if(hr>24){
      carryOverDays= hr/24;
      hr = hr%24;
    }
    std::vector<long> fR;
    fR=_t7;
    fR[6]=(_t7[6]+carryOverDays)%GLOBAL_NUM_DAYS_IN_WEEK;
    std::vector<long> temp;
    temp.resize(6);
    for(long i=0;i<6;++i){temp[i]=fR[i];}
    for(long i=2;i<6;++i){temp[i]+=_add6[i];}
    temp=standardizeTimeVector(temp);
    for(long i=0;i<6;++i){fR[i]=temp[i];}
    return fR;
  }
  std::vector<long> Timer::timeNowVector() const{
    std::vector<long> fR;
    fR.resize(7);
    std::time_t rawTime;
    time(&rawTime);
    struct tm *ptm;
    ptm=gmtime(&rawTime);
    fR[3]=ptm->tm_hour;
    fR[4]=ptm->tm_min;
    fR[5]=ptm->tm_sec;
    fR[2]=ptm->tm_mday;
    fR[1]=ptm->tm_mon;
    fR[0]=ptm->tm_year+1900;
    fR[6]=ptm->tm_wday;
    return fR;
  }
long Timer::timeNowSec() const{return getSecondsSinceYYYY(timeNowVector());}
  std::string Timer::timeString(const std::vector<long> &v) const{
    std::string fR="";
    if(v.size()!=7){
      return "";
    }
    fR+=dayOfTheWeekAbbreviation(v[6]);
    fR+=", ";
    fR+=BF::padded(v[2],10,"0");
    fR+=" "+monthAbbreviation(v[1])+" "+std::to_string(v[0])+" ";
    fR+=BF::padded(v[3],10,"0");
    fR+=":";
    fR+=BF::padded(v[4],10,"0");
    fR+=":";
    fR+=BF::padded(v[5],10,"0");
    fR+=" GMT";
    return fR;
  }
  std::string Timer::timeString() const{
    return timeString(timeNowVector());
  }
  std::string Timer::timeYYYYMMDD(const std::vector<long> & v) const{
    std::string fR=std::to_string(v[0]);
    fR+=BF::padded(v[1]+1,10,"0");
    fR+=BF::padded(v[2],10,"0");
    return fR;
  }
  std::string Timer::timeYYYYMMDD() const{return timeYYYYMMDD(timeNowVector());}
  std::string Timer::timeYYYYMM(const std::vector<long> & v) const{
    std::string fR=std::to_string(v[0]);
    fR+=BF::padded(v[1]+1,10,"0");
    return fR;
  }
  std::string Timer::timeYYYYMM() const{return timeYYYYMM(timeNowVector());}
  std::string Timer::timeYYYY(const std::vector<long> & v) const{
    return std::to_string(v[0]);
  }
  std::string Timer::timeYYYY() const{return timeYYYY(timeNowVector());}
  std::string Timer::cookieExpiration(const std::vector<long> & v) const{
    if(v.size()!=6){
      return "";
    }
    std::vector<long> tn=timeNowVector();
    std::vector<long> te=addTime(tn,v);
    return timeString(te);
  }
  std::string Timer::cookieExpiration(const long & _d, const long & _h, const long & _m, const long & _s) const{
    std::vector<long> v;
    v.resize(6);v[0]=0;v[1]=0;v[2]=_d;v[3]=_h;v[4]=_m;v[5]=_s;
    return cookieExpiration(v);
  }
  std::string sanitizeTimeString(const std::string & ts){
    std::vector<long> tv=stGMT_to_timeVectorTMF(ts);
    if(tv[0]<0){
      return "Wed, 19 Aug 2020 13:59:14 GMT";
    }
    return ts;
  }
}
#endif
