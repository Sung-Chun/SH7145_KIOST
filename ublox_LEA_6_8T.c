/* 20180826 Cat3 kcat用 ublox LEA-6T GPS receiver */
/* 2012 4.24 $GPRMC or $GNRMCコマンドのみ使用する */

//#define DEBUG
#define  DEBUG_0
#include <string.h>
#include <stdlib.h>
#include <mes2.h>
#include "SH2MB.h"      /* TAC-SH2MB */
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int ublox_GNRMC(int zeiten,int display);

/* GPRMCで時刻情報を入手し、gps_byouを戻す。                      */
/* displayによって画面出力するかどうかを決定する                  */
/* display=0 何も表示しない                                       */
/* display=1 同期がとれていれば現在時刻(TOD of GPS)を表示         */
/*           同期がとれていなければ、GNRMCコマンド情報を表示      */
/* display=2 GNRMCコマンド情報を表示,Availableになればreturnする  */
/* 衛星と同期がとれればgps_sekundenを持ってreturnする             */
/* zeiten>0の場合は衛星と同期がとれるまでzeiten回数受信を繰り返す */
/* zeiten=0,1の場合は1回でreturnする                              */
/* 衛星と同期がとれておればgps秒を戻り値、そうでない場合は-1      */

/* **** External function declaration ************* */
extern int  date2sec(int time1[],int date1[]);
extern int  serRecvs(int port,char *s,char charactor,int n,int msec);
extern void serClr(int port);
extern void WDT(int flag);          /* register.c */

/* **** external(global) variable declaration ******* */
extern char str_latitude[],str_longitude[];

int ublox_GNRMC(int zeiten,int display){
   int   count,i,j,j1,len,synchronisieren=0,gps_sekunden,retry;
   int   kanma[15];
   char  cf[85],year[5],month[5],day[5],hour[5],min[5],sec[5];
   int   time1[3],date1[3];
   char  df[5],bf[128];
   
   serClr(GPS_port);       /* これが必ず必要である 2011.11.30 */
   for(count=0;count<zeiten;++count){
      retry=4;
      memset(bf,0,strlen(bf));
      while(retry){
         serRecvs(GPS_port,bf,0x0a,85,1200);
         strcpy(cf,bf);cf[6]='\0';
         df[0]=cf[3];df[1]=cf[4];df[2]=cf[5];df[3]=0;
         if(strcmp("RMC",df)==0) break;
         retry--;
      }
      
      if(retry<0) return(-1);     

      /* GPRMCデータを取り出す */
      len=strlen(bf);
      j=0;
      for(i=0;i<len;++i){     /* カンマ位置の検出 */
         if(bf[i]==','){kanma[j]=i+1;j++;}
      }
      j1=kanma[0];
      hour[0]=bf[j1];hour[1]=bf[j1+1];hour[2]='\0';
      min[0]=bf[j1+2];min[1]=bf[j1+3];min[2]='\0';
      sec[0]=bf[j1+4];sec[1]=bf[j1+5];sec[2]='\0';

      j1=kanma[1];synchronisieren=0;
      if(bf[j1]=='A') synchronisieren=1;
         
      j1=kanma[2];
      for(i=0;i<=8;i++) str_latitude[i]=bf[j1+i];               
      str_latitude[9]=bf[kanma[3]];str_latitude[10]='\0';
         
      j1=kanma[4];         
      for(i=0;i<=9;i++) str_longitude[i]=bf[j1+i];
      str_longitude[10]=bf[kanma[5]];
      str_longitude[11]='\0';
               
      j1=kanma[8];               
      day[0]=bf[j1];day[1]=bf[j1+1];day[2]='\0';
      month[0]=bf[j1+2];month[1]=bf[j1+3];month[2]='\0';
      year[0]='2';year[1]='0';
      year[2]=bf[j1+4];year[3]=bf[j1+5];year[4]='\0';
      #ifdef DEBUG
         if(synchronisieren==1) printf("GPS available\x0d\x0a");
         else printf("GPS not available\x0d\x0a");
         printf("%s %s %s %s %s %s\x0d\x0a"
            ,year,month,day,hour,min,sec);
         printf("latitude:%s longitude:%s\x0d\x0a",str_latitude,str_longitude);
      #endif
      if(display==1){
         if(synchronisieren==1){
            printf("%s %s %s %s %s %s\x0d\x0a"
               ,year,month,day,hour,min,sec);
         }
         else{
            printf("%d %s",count,bf);
         }
      }
      if(display==2){
         printf("%d %s",count,bf);
         if(synchronisieren==1) return(0);
      }
   }     /* for(count=0;;) end */
   if(synchronisieren==1){
      time1[0]=atoi(hour);time1[1]=atoi(min);time1[2]=atoi(sec);
      date1[0]=atoi(year);date1[1]=atoi(month);date1[2]=atoi(day);
      gps_sekunden=date2sec(time1,date1);
      return(gps_sekunden);
   }
   return(-1);
}
