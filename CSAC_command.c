/* 20220121 Cat3 mainboard */
//#define DEBUG
#define DEBUGstatus
#define DEBUGset_tod
//#define DEBUGsynch
#include <string.h>
#include <stdlib.h>
#include <MES2.h>
#include <macro.h>
#include "SH7145.h"
#include "SH2MB.h"  /* SH2MB */
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void CSAC_set_operating_mode(void);
int CSAC_set_TOD(int tod);
int CSAC_get_TOD(void);
int synchCSACtoGPS(void);
int CSAC_get_status(void);       /* Added 20180826 */
void CSAC_telemetry_initi(void); /* Added 20180826 */


/* **** External function declaration ************* */
extern void sci_Stop(int port);
extern void serPuts(int port,char *bf);
extern int  serSend(int port,char ch);
extern void serPuts(int port,char *bf);   /* Added 20180826 */
extern int  serRecvs(int port,char *s,char e,int n,int t);
extern void serClr(int port);
extern void WDT(int wdt_flag);   /* cpu_register_set.c */

void CSAC_set_operating_mode(void){
   int  i,ret;
    char b0[20];

      for(i=0;i<5;i++){
         switch(i){
            case 0: serPuts(CSAC_port,"!Ma\x0d\x0a");break;
            case 1: serPuts(CSAC_port,"!Ms\x0d\x0a");break;
            case 2: serPuts(CSAC_port,"!Md\x0d\x0a");break;
            case 3: serPuts(CSAC_port,"!Mu\x0d\x0a");break;
            case 4: serPuts(CSAC_port,"!Mc\x0d\x0a");break;
         }
         memset(b0,'\0',20);
         ret=serRecvs(CSAC_port,b0,0x0a,160,3000);
         #ifdef DEBUG
            printf("CSAC_op_mode ret=%d %s",ret,bf);
         #endif
      }
      return;
}

int CSAC_set_TOD(int tod){
    char b0[20],b1[40];
    int  tnum;

      memset(b0,'\0',20);
      memset(b1,'\0',40);

      sprintf(b0,"!TA%d\x0d\x0a",tod);
      serPuts(CSAC_port,b0);
      serRecvs(CSAC_port,b1,0x0a,39,3000);
      #ifdef DEBUGset_tod
         printf("enter CSAC_set_TOD %s",b0);
         printf(b1);
      #endif
      tnum=atol(b1+12); // Response format is "TimeOfDay = 11111111111[CR][LF]"
      if(tod-tnum) return(-1);
      else return(0);
}

int CSAC_get_TOD(void){
   int present_time;
   char  bf[128];
    
      memset(bf,'\0',128);
      serClr(CSAC_port);        /* 2013.11.28 added */
      serSend(CSAC_port,'T');
      serRecvs(CSAC_port,bf,0x0a,85,2000);
      #ifdef DEBUG
         printf("enter get_TOD %s\x0d\x0a",bf);
      #endif
      present_time=atol(bf);
      if(present_time>473040000) return(present_time);/* 4‰­7ç304–œ 2015”N */
      else  return(-1);
}

int synchCSACtoGPS(void){
   char  bf[128];
      WDT(1);
      memset(bf,'\0',128);
      serClr(CSAC_port);
      serSend(CSAC_port,'S');
      serRecvs(CSAC_port,bf,0x0A,5,5000);  //CASC replys after succesfull syncrhonization
      #ifdef DEBUGsynch
         printf("CSAC S command %s\x0d\x0a",bf);
      #endif
      if(bf[0]=='S') return(0);
      else return(-1);
}
int CSAC_get_status(void){
   char  b0[8],bf[160];
   int   ret;

      memset(b0,'\0',8);
      sprintf(b0,"!^\x0d\x0a");
      serClr(CSAC_port);
      serPuts(CSAC_port,b0);
      memset(bf,'\0',160);bf[0]=255;
      ret=serRecvs(CSAC_port,bf,0x0a,160,4000);
      #ifdef DEBUGstatus
         printf("CSAC_get_status ret=%d %s %s",ret,bf,b0);
      #endif
      b0[0]=bf[0];b0[1]=0;
      if((b0[0] >= '0') && (b0[0] <= '9')){
         ret=atoi(b0);           /* æ“ª‚Ì•¶Žš‚¾‚¯Žæ‚èo‚· */
         return(ret);
      }
      return((int)b0[0]);
}
void CSAC_telemetry_initi(void){
   int   ret;
   char  bf[160];

      serPuts(CSAC_port,"!M?\x0d\x0a");
      ret=serRecvs(CSAC_port,bf,0x0a,160,3000);
      #ifdef DEBUG
         printf("!M? ret=%d %s",ret,bf);
      #endif

      serPuts(CSAC_port,"!Ma\x0d\x0a");
      ret=serRecvs(CSAC_port,bf,0x0a,160,3000);
      #ifdef DEBUG
         printf("!Ma ret=%d %s",ret,bf);
      #endif

      serPuts(CSAC_port,"!MD\x0d\x0a");
      ret=serRecvs(CSAC_port,bf,0x0a,160,3000);
      #ifdef DEBUG
         printf("!MD ret=%d %s",ret,bf);
      #endif

      serPuts(CSAC_port,"!Mu\x0d\x0a");
      ret=serRecvs(CSAC_port,bf,0x0a,160,3000);
      #ifdef DEBUG
         printf("!Mu ret=%d %s",ret,bf);
      #endif

      serPuts(CSAC_port,"!U?\x0d\x0a");
      ret=serRecvs(CSAC_port,bf,0x0a,160,3000);
      #ifdef DEBUG
         printf("!U? ret=%d %s",ret,bf);
      #endif
     return;
}
