/* 20220223 Cat3 Mainboard使用 */
/* Cat3は本来常時GPS使用のMainboard用に設計 GPSをSCI0に割り付け */
/* default_set.cでは装置固有のパラメーターを設定する */
/* CSAC使用の装置ではCSACのsynchronizeが既に行われているかチェックする */
//#define  DEBUG
#include <string.h>
#include <MES2.h>
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int default_set(void);
int determine_record_media(void);

/* **** External function declaration ************* */
extern void eri_0(void);
extern void rxi_0(void);
extern void eri_2(void);
extern void rxi_2(void);
//extern void adi0(void); 20210519

extern void Amp_power(int on_off);
extern void sci_Open(int port,int speed);
extern void sci_Restart(int port);
extern void sci_Stop(int port);
extern void gps_power(int on_off);
extern int  CSAC_get_status(void);
extern int  ublox_GNRMC(int check,int display);
extern void write_rtc_clock(int pc_byou); /* SH_RTC.c */

/* **** external(global) variable declaration ******* */
extern int gps_operation;
extern char record_media[];

int default_set(void){
   int fp,ret;
   char  bf[80];

      /* interruput vector setting */
      set_handler(128,eri_0 );   /* GPS_port */
      set_handler(129,rxi_0 );
      set_handler(168,eri_2 );   /* CSAC_port */
      set_handler(169,rxi_2 );

      gps_operation=0;  /* temporarily set */
      sci_Open(GPS_port,9600);
      sci_Open(CSAC_port,57600);
      Amp_power(0);
      sci_Stop(CSAC_port);sci_Stop(GPS_port);
      sleep(1000);
      printf("\n");

      /* determin record media */
      ret=determine_record_media();
      if(ret!=0){
         printf("record media is missing. Program stop\x0d\x0a");
         exit(0);
      }
      printf("record media is %s\x0d\x0a",record_media);

      /* 既にCSACがGPSと時間あわせされているか判定する */
      printf("CSAC.txt is now checking...\x0d\x0a");
      memset(bf,'\0',80);
      strcpy(bf,record_media);
      strcat(bf,"CSAC.txt");

      fp=open(bf,OptRead);
      if(fp != -1){
         close(fp);
         printf("CSAC was synchronized already\x0d\x0a");
         return(1);  /* CSAC synchronized_flag */
      }
      printf("CSAC is not yet synchronized with GPS!\x0d\x0a");
      printf("GPS receiver is checking now!\x0d\x0a");
      gps_operation=1;     /* temporarily set */
      gps_power(1);
      sci_Restart(GPS_port);
      printf("GPS available is now checking...\x0d\x0a");
      ret=ublox_GNRMC(600,2); /* 同期が取れ次第returnする */
      if(ret==-1){
         printf("Program stop because GPS receiver is bad\x0d\x0a");
         exit(0);
      }
      write_rtc_clock(ret);
      sci_Stop(GPS_port);
      return(0);
}
int determine_record_media(void){
   int   ret;
   char  bf[40];

      printf("\x0d\x0a");
      printf("****** determine record media ******** \x0d\x0a");
      memset(record_media,0,8);
      printf("SD  memory check\x0d\x0a");
      eject("mmc0");ret=mount("mmc0");sleep(500);/* SD memory */
      if(ret!=-1){
         strcpy(record_media,"/mmc0/");
         return(0);
      }
      printf("USB memory check\x0d\x0a");
      memset(bf,0,sizeof(bf));
      strcpy(bf,"/spc0/tomo.exe");
      #ifdef DEBUG
         strcpy(bf,"/spc0/d.exe");
      #endif
      printf("check file=%s\x0d\x0a",bf);
      ret=open(bf,OptRead);
      if(ret!=-1){
         strcpy(record_media,"/spc0/");
         close(ret);
         return(0);
      }
      printf("can not find record media\x0d\x0a",ret);
      return(-1);
}
