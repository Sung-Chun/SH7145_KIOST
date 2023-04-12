/* 20180810 Cat3 mainboard change program */

/* 与えられた時刻（秒）まで待つ 時刻は2000.1.1からの経過秒           */
/* return(0) 正常終了,return(-1,-2) 異常終了                         */
/* 時間待ちはcount(sec)回数まで                                      */

//#define DEBUG
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mes2.h>
#include "SH7145.h"
#include "SH2MB.h"   /* SH2MB */
#include <macro.h>
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int wait_time(int wait_byou);

/* **** External function declaration ************* */
extern int  read_rtc_clock(void);
extern int  CSAC_set_TOD(int tod);
extern int  ublox_GNRMC(int zeiten,int display);
extern void write_rtc_clock(int pc_byou); /* SH_RTC.c */
extern void runlog(int code);
extern void sci_Stop(int port);
extern void sci_Restart(int port);
extern void WDT(int wdt_flag);                  /* register_set.c */
extern int  CSAC_get_TOD(void);

/* **** external(global) variable declaration ******* */
extern int  wakeup_interval;  /* unit:second */
extern int  gps_operation;

int wait_time(int wait_byou){
   int i,count,diff,CSAC_sec,negative,rtc_sekunden,gps_sec;
   int pre_diff=0;

      /* **** PIC_counter_clear(20220920) by alive signal **** */
      pio16_1 &= ~0x02;sleep(50);pio16_1 |= 0x02;
      #ifdef DEBUG
         printf("wait_byou=%d\x0d\x0a",wait_byou);
      #endif
      count=wakeup_interval;
      if(count>300) count=300;
      negative=0;

      if(gps_operation==1) goto gps_operation0;
      INT_DISABLE();      
      while((PEDRL & 0x4)!=0) ;  /* CSAC 1Hz Lowになるまで待つ */
      while((PEDRL & 0x4)==0) ;  /* CSAC 1Hz Highになるまで待つ */
      TSTR= 0x81;                /* カウンタスタート 1000 0001 */
      WDT(1);INT_ENABLE();

      sci_Restart(CSAC_port);
      /* 20201217 added CSAC check */
      CSAC_sec=CSAC_get_TOD();
      diff=wait_byou-CSAC_sec;
      if(diff>1000000){
         rtc_sekunden=read_rtc_clock();
         CSAC_set_TOD(rtc_sekunden-1); /* 2000年1月1日からの経過秒数 */
         runlog(601);
      }
      for(i=0;i<count;++i){
         CSAC_sec=CSAC_get_TOD();
         diff=wait_byou-CSAC_sec;
         if(diff==0){
            sci_Stop(CSAC_port);
            write_rtc_clock(CSAC_sec); /* 1sec passes */
            return(0);   /* 時刻到来 case 0 を抜け出す */
         }
         printf("wait_time(CSAC) %d\x0d\x0a",diff);
         if(diff>0) negative=0;
         else{
            negative++;
            if(negative>2) return(-1);
         }
      }
      runlog(602);
      return(-1);        /* 時刻を逃した */

   /* ********* 20210507 Added gps_operation ********* */
   gps_operation0:
      INT_DISABLE();      
      while((PEDRL & 0x2)!=0) ;  /* GPS 1Hz Lowになるまで待つ */
      while((PEDRL & 0x2)==0) ;  /* GPS 1Hz Highになるまで待つ */
      TSTR= 0x81;                /* カウンタスタート 1000 0001 */
      WDT(1);INT_ENABLE();

      sci_Restart(GPS_port);
      /* 20201217 added CSAC check */
      for(i=0;i<count;++i){
         gps_sec=ublox_GNRMC(1,0);
//         if(gps_sec==-1) continue;
         diff=wait_byou-gps_sec;
         if(diff==0){
            sci_Stop(GPS_port);
            write_rtc_clock(gps_sec); /* 1sec passes */
            return(0);   /* 時刻到来 case 0 を抜け出す */
         }
         printf("wait_time(GPS) %d\x0d\x0a",diff);
         if(pre_diff==1){
            sci_Stop(GPS_port);
            return(0);   /* 時刻到来 case 0 を抜け出す */
         }
         pre_diff=diff;
         if(diff>0) negative=0;
         else{
            negative++;
            if(negative>2) return(-1);
         }
      }
      runlog(603);
      return(-1);        /* 時刻を逃した */
}
