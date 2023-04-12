/* **************************************************************
 *  西暦年は2018年というように4桁で与える
 *************************************************************** */
//#define DEBUG
#include <MES2.h>

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int date2sec(int time1[],int date1[]);
int sec2date(int byou,int cal[]);

/* **** External function declaration ************* */
extern void WDT(int flag);

/* 現在の日時を2000年1月1日午前0時からの経過秒で表す */
static int daybyou=86400L;  /* 24*3600 */

int date2sec(int time1[],int date1[]){
      int   year1,mon1,day1,leap,leap_flag;
      int  day_no=0,byou;
      year1=date1[0];mon1=date1[1];day1=date1[2];

      WDT(0);
      switch(mon1){
         case  1:day_no=0;break;
         case  2:day_no=31;break;
         case  3:day_no=59;break;
         case  4:day_no=90;break;
         case  5:day_no=120;break;
         case  6:day_no=151;break;
         case  7:day_no=181;break;
         case  8:day_no=212;break;
         case  9:day_no=243;break;
         case  10:day_no=273;break;
         case  11:day_no=304;break;
         case  12:day_no=334;break;
         default:break;
      }      
      day_no += ((int)(year1-2000)*365+(int)(day1-1));/* 月は1日から始まる */
      /* 閏年の補正をする  2100年１月まで有効 */
      leap=(int)((year1-2000)/4)+1;  /* プラス１したのは2000年は閏年であるから */
      leap_flag=(year1-2000)%4;
      if(leap_flag!=0) day_no+=leap;
      else if(leap_flag==0){
         if(mon1<=2) day_no=day_no+(leap-1);
         else day_no+=leap;
      }
      byou=(unsigned int)day_no*daybyou+(unsigned int)time1[0]*3600+
         (unsigned int)time1[1]*60+(unsigned int)time1[2];
      WDT(1);
      return(byou);
}

/* 2000.1.1からの秒の値を時刻データに変換する */
short leap_interval=365*3+366;

int sec2date(int byou,int cal[]){
   int   day_long,daybyou;
   int   i,year1,day1,day0,year,month=1,day=1,leap,leap_flag;
   int   hour,min,sec;
      
      WDT(0);
      #ifdef DEBUG
         WDT(1);
         printf("sec2date byou=%d\x0d\x0a",byou);
      #endif

      day_long=(int)(byou/86400)+1;  /* 日は１から始まる */
      daybyou=(int)(byou%86400);

      #ifdef DEBUG
         printf("sec2date day_long=%d daybyou=%d\x0d\x0a",day_long,daybyou);
      #endif

      leap_flag=0;
      leap=(int)(day_long/(int)leap_interval)+1;  /* 2000year is leap */
      day1=((int)day_long)%leap_interval;

      #ifdef DEBUG
         printf("sec2date leap=%d day1=%d\x0d\x0a",leap,day1);
      #endif

      if(day1==0){
         year1=0;
         day0=365;
         goto g0;
      }

      if((day1>0) && (day1<367)){
         year1=1;
         leap_flag=1;
         day0=day1;
         goto g0;
      }
      else if((day1>=367) && (day1<732)){
         year1=2;
         day0=day1-366;
         goto g0;
      }
      else if((day1>=732) && (day1< 1097)){
         year1=3;
         day0=day1-731;
         goto g0;
      }
      else if((day1>=1097) && (day1<leap_interval)){
         year1=4;
         day0=day1-1096;
      }
      else{ /* error */
         for(i=0;i<6;++i) cal[i]=0;
         WDT(1);
         return(-1);
      }
   g0:
      year=1999+(leap-1)*4+year1;
      #ifdef DEBUG
         printf("sec2date year1=%d\x0d\x0a",year1);
      #endif

      if((day0>0) && (day0<=31)){  /* １月 */
         month=1;
         day=day0;
         goto g1;
      }
      if((day0>31) && (day0<=59)){  /* 2月 */
         month=2;
         day=day0-31;
         goto g1;
      }
      if((day0==60) && (leap_flag==1)){      
         month=2;
         day=29;
         goto g1;
      }
      if((leap_flag==1) && (day0>60)) day0=day0-1;

      if((day0>59) && (day0<=90)){  /* 3 */
         month=3;
         day=day0-59;
         goto g1;
      }
      if((day0>90) && (day0<=120)){ /* 4 */
         month=4;
         day=day0-90;
         goto g1;
      }
      if((day0>120) && (day0<=151)){ /* 5 */
         month=5;
         day=day0-120;
         goto g1;
      }
      if((day0>151) && (day0<=181)){ /* 6 */
         month=6;
         day=day0-151;
         goto g1;
      }
      if((day0>181) && (day0<=212)){ /* 7 */
         month=7;
         day=day0-181;
         goto g1;
      }
      if((day0>212) && (day0<=243)){ /* 8 */
         month=8;
         day=day0-212;
         goto g1;
      }
      if((day0>243) && (day0<=273)){ /* 9 */
         month=9;
         day=day0-243;
         goto g1;
      }
      if((day0>273) && (day0<=304)){ /* 10 */
         month=10;
         day=day0-273;
         goto g1;
      }
      if((day0>304) && (day0<=334)){ /* 11 */
         month=11;
         day=day0-304;
         goto g1;
         }
      if((day0>334) && (day0<=365)){ /* 12 */
         month=12;
         day=day0-334;
         }
   g1:
      hour=(int)(daybyou/3600);
      min=(int)((daybyou%3600)/60);
      sec=(int)(daybyou%60);
      /* year,month,day,hour,min,secが求められた */
      cal[0]=year;cal[1]=month;cal[2]=day;
      cal[3]=hour;cal[4]=min;cal[5]=sec;
      WDT(1);
      return(0);
   }
