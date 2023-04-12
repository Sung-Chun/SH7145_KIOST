/* 20220920 GPS makes time of CSAC synchronous */
/* alive signal is changed to low active (waiting high) */
/* ubloxGPSは閏秒の調整に時間が必要である */
/* CSACの時計をGPSに合わせる。正常に合わせたら/mmc/CSAC.txtファイルを作る */
/* CSAC_set_TODをどのtimingで行うべきか、debugがまだ完了していない */
/* 現在GPS 1Hzの立ち上がりでCSAC_set_TODを行っている */

#define  DEBUG
#include <macro.h>
#include "SH7145.h"
#include "SH2MB.h"  /* SH2MB */
#include <string.h>
#include "MES2.h"
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int adjust_clock_using_GPS(void);
int clock_diff(int display_flag);

/* **** External function declaration ************* */
extern int  ublox_GNRMC(int zeiten,int display);
extern void write_rtc_clock(int pc_byou); /* SH_RTC.c */
extern int  read_rtc_clock(void);
extern void sci_Stop(int port);
extern void sci_Restart(int port);
extern void serClr(int port);
extern int  serRecvs(int port,char *s,char charactor,int n,int msec);
extern void WDT(int wdt_flag);   /* register_set.c */
extern int  sec2date(int byou,int cal[]);
extern void CSAC_set_operating_mode(void);
extern int  CSAC_get_status(void);
extern int  CSAC_set_TOD(int tod);
extern int  CSAC_get_TOD(void);
extern int  synchCSACtoGPS(void);
extern void time_information_log(int value);

/* **** external(global) variable declaration ******* */
extern char record_media[];
extern int synchro_flag;

/**************************************************************/
int adjust_clock_using_GPS(void){
   int   ret,fp1,count,cnt0,cnt1,cnt2,cnt3;
   int   gps_sekunden,CSAC_sekunden,rtc_sekunden;  /* 2000.1.1からの経過秒 */
   int   cal[6];
   char  present_time[80],bf[100];
     
      WDT(1);
      sci_Stop(GPS_port);
      sci_Stop(CSAC_port);

      /* vorbereiten CSAC */
      sci_Restart(CSAC_port);
      CSAC_set_operating_mode();
      for(count=0;count<60;++count){
         ret=CSAC_get_status();
         if(ret==0) break;
         printf("%d %d CSAC is not ready yet\x0d\x0a",count,ret);
         sleep(5000);
      }
      if(count>=60){
         printf("CSAC isn't in the state which can be used\x0d\x0a");
         return(-1);
      }
      printf("CSAC is ready CSAC status: good\x0d\x0a");

      /* vorbereiten ubloxGPS */
      sci_Restart(GPS_port);
      cnt0=300;cnt1=301;cnt2=600;cnt3=150;

   #ifdef DEBUG
      cnt0=30;cnt1=31;cnt2=60;cnt3=15;
   #endif

      for(count=0;count<cnt0;count++){
         serRecvs(GPS_port,bf,0x0a,85,1200);
         printf("%d %s",count,bf);
      }
      /* **** PIC_counter_clear(20220920) by alive signal **** */
      pio16_1 &= ~0x02;sleep(50);pio16_1 |= 0x02;

      for(count=cnt1;count<cnt2;count++){
         serRecvs(GPS_port,bf,0x0a,85,1200);
         printf("%d %s",count,bf);
      }
      ublox_GNRMC(cnt3,1);  /* receive 150sec (10sec for debug) */
      sci_Stop(GPS_port);
      /* ******** ここまでに12.5分経過 ******** */

      sci_Restart(CSAC_port);
      while( (PEDRL & 0x0002) < 1 ){;} /* check GPS 1Hz pulse */
      while( (PEDRL & 0x0002) > 0 ){;}

      /* Hardware 1Hz of CSAC is syncheronized using GPS 1Hz */
      for(count=0;count<15;count++){   /* try 15 times until Erfolg */
         ret = synchCSACtoGPS();
         if(ret==0){break;}
         sleep(1000);
      }
      sleep(1000);
      /* ******** retry ************ */
      while((PEDRL & 0x0002)>0){;}   /* GPS: Lowになるまで待つ */
      while((PEDRL & 0x0002)<1){;}   /* GPS: Highになるまで待つ */

      for(count=0;count<10;count++){
         ret = synchCSACtoGPS();
         if(ret==0){break;}
         sleep(1000);
      }

      /* Software TOD of CSAC is syncheronized using GPS TOD */
      serClr(GPS_port);sci_Restart(GPS_port);
      for(count=0;count<10;count++){
         while((PEDRL & 0x0002)>0){;}   /* GPS: Lowになるまで待つ */
         while((PEDRL & 0x0002)<1){;}   /* GPS: Highになるまで待つ */
         gps_sekunden=ublox_GNRMC(1,0); /* 2000年1月1日からの経過秒数 */
         /* 20220904 Added following two command */
         /* CSAC_set_TODをどのtimingで行うべきか、debugがまだ完了していない */
         /* 現在GPS 1Hzの立ち上がりでCSAC_set_TODを行っている */
         while((PEDRL & 0x0002)>0){;}   /* GPS: Lowになるまで待つ */
         while((PEDRL & 0x0002)<1){;}   /* GPS: Highになるまで待つ */
         ret = CSAC_set_TOD(gps_sekunden+1); /* 2000年1月1日からの経過秒数 */
         write_rtc_clock(gps_sekunden+1);
         if(ret==0) break;
      }
      if(count>=10){
         printf("CSAC_set_TOD scheitern!\x0d\x0a");/* 失敗 */
         return(-1);
      }
      #ifdef DEBUG0
         for(count=0;count<10;++count){
            serClr(GPS_port);serClr(CSAC_port);
            while((PEDRL & 0x0002)>0){;}   /* GPS: Lowになるまで待つ */
            while((PEDRL & 0x0002)<1){;}   /* GPS: Highになるまで待つ */
            CSAC_sekunden=CSAC_get_TOD()-1;
            gps_sekunden=ublox_GNRMC(1,0);
            rtc_sekunden=read_rtc_clock();
            printf("GPS:  %d CSAC:  %d RTC:  %d\x0d\x0a",
               gps_sekunden,CSAC_sekunden,rtc_sekunden);
         }
      #endif

      while((PEDRL & 0x0002)>0){;}   /* GPS: Lowになるまで待つ */
      while((PEDRL & 0x0002)<1){;}   /* GPS: Highになるまで待つ */
      serClr(GPS_port);serClr(CSAC_port);
      CSAC_sekunden = CSAC_get_TOD();
      gps_sekunden=ublox_GNRMC(1,0);
//      rtc_sekunden=read_rtc_clock();
      rtc_sekunden=read_rtc_clock()-1; /* 20220915 */
      sec2date(gps_sekunden,cal);
      printf("CSAC calendar was synchronized by GPS\x0d\x0a");
      printf("present time  (GPS):  %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
      sec2date(CSAC_sekunden,cal);
      memset(present_time,'\0',80);
      sprintf(present_time,"present time (CSAC):  %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
      printf("%s",present_time);
      sec2date(rtc_sekunden,cal);
      printf("present time  (RTC):  %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);

      count=clock_diff(0);
      sci_Stop(GPS_port);
      memset(bf,'\0',sizeof(bf));
      strcpy(bf,record_media);
      strcat(bf,"CSAC.txt");
      fp1=open(bf,OptWrite);
      if(fp1>0){ 
         fprintf(fp1,"clock diff=%d %s",count,present_time);
         close(fp1);
      }
      synchro_flag=1;
      return(0);
}

/* GPS 1Hzの立ち上がり信号とcounterの1Hzの立ち上がり信号のずれ時間を計測する */
/* 25MHz/16=1.5625MHz(T=0.64us)のクロックを使用する */
int clock_diff(int display){
   int ret,i1,i2,count;

      sci_Restart(GPS_port);
      for(count=0;count<5;++count){
         ret=ublox_GNRMC(180,2);
         if(ret!=-1) break;
      }
      sci_Stop(GPS_port);
      if(ret==-1){
         printf("GPS is not available\x0d\x0a");
         return(-1000000);
      }

      /* ここから時間差計測 */
      INT_DISABLE();

      TCR_1  = 0x07;    /* 000 00 111     TCNT_2のoverflowでカウント */
      TCR_2  = 0x00;    /* 000 00 000     内部クロックでカウント */
      TCNT_1 = 0;    
      TCNT_2 = 0;

      while((PEDRL & 0x0002)>0){;}/* waiting for GPS low */
      TSTR |= 0x06;              /* counter start */  
      while((PEDRL & 0x0002)<1){;}/* waiting for GPS high */
      TCNT_2 = 0;TCNT_1 = 0;

      while((PEDRL & 0x0004)>0){;}/* waiting for SS low */
      while((PEDRL & 0x0004)<1){;}/* waiting for SS high */
      TSTR &= ~0x06;         /* ~0x06 TCN1 & TCN2 count stop */
      
      i1=TCNT_1;i2=TCNT_2;
      count=(i1<<16)+i2;         /* 注意 括弧が必要 */
      
      i1=(int)(count/20.0);  //count*1000000/20000000 (usec)
      if (i1>500000){
         i1 = i1-1000000;
      }
      INT_ENABLE();
      printf("Clock_diff(us)= %d\x0d\x0a",i1);
      time_information_log(i1);
      return(i1);
}
