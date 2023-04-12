/* 次回のping_byouの設定をPC内部時計を使用して行う　*/
/* start_time,wakeup_intervalから次回ping時刻(ping_byou)の計算をする   */
//#define DEBUG
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <MES2.h>
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int ping_time_schedule(int present_time);
int get_present_time(void);

/* **** External function declaration ************* */
extern int  ublox_GNRMC(int zeiten,int display);
extern void sci_Stop(int port);
extern void sci_Restart(int port);
extern int  CSAC_get_TOD(void);
extern int  read_rtc_clock(void);
extern int  sec2date(int byou,int cal[]);

/* **** external(global) variable declaration ***** */
extern int
      gps_operation,
      wakeup_interval,  /* wakeup interval（秒）            */
      start_time,
      end_time,
      burst_total_num,        /* バーストモードでの送信回数*/
      burst_interval;         /* バースト間隔*/
extern char bf[];      

int ping_time_schedule(int present_time){
   int   wakeup_num,burst_num,a_time;
   int   ping_byou,prev_wakeup;
   #ifdef DEBUG
      int   cal[6];
   #endif

      if(present_time > end_time){ /* 既に実験は終了 6時間後wakeupさせる */
         printf("Experiment is over\x0d\x0a");
         return(present_time+21600);      
      }

     if(present_time < start_time){ /* 実験はまだ始まっていない */
         printf("System is still waiting to start experiment\x0d\x0a");
         return(start_time); 
      }  

      wakeup_num=(int)((present_time-start_time)/wakeup_interval);
      if(burst_total_num*burst_interval==0){
         ping_byou=start_time+wakeup_interval*(wakeup_num+1);
      }
      else{
         prev_wakeup=start_time+wakeup_interval*wakeup_num;
         a_time=present_time-prev_wakeup;
         burst_num=(int)(a_time/burst_interval);
         if(burst_num<burst_total_num){
            ping_byou=prev_wakeup+burst_interval*burst_num+1;
         }
         else ping_byou=start_time+wakeup_interval*(wakeup_num+1);
      }
      #ifdef DEBUG
         /* ping_byou(次回音を発射させる時刻)が決まった */
         /* 2000.1.1からのwakeup_byouの値を時刻データに変換する */
         sec2date(ping_byou,cal);
         printf("Next ping_time=%4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
            cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
      #endif
      return(ping_byou);
}
int get_present_time(void){
   int present_sec=0;

      switch(gps_operation){
         case 0:
            sci_Restart(CSAC_port);
            present_sec=CSAC_get_TOD();
            sci_Stop(CSAC_port);
            break;
         case 1:
            sci_Restart(GPS_port);
            present_sec=ublox_GNRMC(1,0);
            sci_Stop(GPS_port);
            break;
         default: break;
      }
      if(present_sec==-1) present_sec=read_rtc_clock();
      return(present_sec);
}
