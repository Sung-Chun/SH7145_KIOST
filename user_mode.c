/* 20220919 user mode */
/* devised to avoid delay writing of USB memory when executing DeleteFIle */
/* SH2/7145 external host interface program */
/* serial console is used(OS already has open the serial console) */
/* return code=0 正常終了 return to main() and the program continues */
/* return code=-1 no reply: return to main() and the program continues */
/* return code=100 return main() and terminate the program */
/* use \x0d\x0a instead of \n to absorb the difference in terminal soft */

#include <string.h>
#include "SH7145.h"
#include <MES2.h>
#include "define_com_port.h"

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int   user_mode(int wait,int src);  /* 1 wait=5sec */
int   break_check(void);
char  catch_CR(int wait_sec);       /* console terminal */
int   file_out(int flag);
void  display_menu(void);

/* **** External function declaration ************* */
extern void gps_power(int on_off);
extern int  ublox_GNRMC(int zeiten,int display);
extern int  adjust_clock_using_GPS(void);
extern int  clock_diff(int flag);   /* adjust_clock_using_GPS.c */
extern int  CSAC_get_TOD(void);
extern int  sec2date(int byou,int cal[]);

extern void Amp_power(int on_off);  /* power_cntrl.c */
extern void transmit(int st,int code_length,int times,int mod_num);
extern int  clock_pulse_generate(int freqg,int flag);

extern void sci_Restart(int port);  /* serial.c */
extern int  serRecvs(int port,char *s,char charactor,int n,int msec);
extern void sci_Stop(int port);
extern void serClr(int port);

extern void set_rtc_clock(void);    /* SH_RTC.c */
extern int  replace_cfg(void);      /* term_replace_cfg.c */
//extern int  replace_prog(void);
extern void WDT(int on_off_flag);   /* cpu_register_set.c */
extern void runlog(int code);       /* Added 20220919 */

/* **** external(global) variable declaration ******* */
extern int     code_length,code_repeat,mod_num,my_station,gps_operation;
extern int     t_freq_common,synchro_flag;
extern char    record_media[];


static char command_prompt[]=
   "\x0d\x0aPlease input command(a b c d e g h j k p r s t x z)\x0d\x0a";
static int j_command_flag;
int user_mode(int wait,int src){  /* 1 waitは1秒 */
   int   i,fp,CSAC_sekunden,ret=0;
   int   cal[6];
   char  c1,bf[128];

      WDT(1);
      switch(src){
         case  0:strcpy(bf,"\x0d\x0a*** Planning & Data accessing ***\x0d\x0a");
                  break;
         case  1:strcpy(bf,"\x0d\x0a*** Normally measuring ***\x0d\x0a");break;
         case  2:strcpy(bf,"\x0d\x0a*** cfg file bad ***\x0d\x0a");break;
      }
      printf(bf);

      printf("Press y key if you want to enter user mode\0x20...\0x20");
      ret=break_check();/* 約10秒間の判定 */
      if(ret !=1 ){ 
         printf("\x0d\x0a");
         return(-1);          /* 応答なしで戻る */
      }
      printf("\x0d\x0a Break signal is catched\x0d\x0a");
      sleep(300);
      /* 通信の確立終了 */
      display_menu();
      gps_power(1);j_command_flag=0;

      for(;;){
         printf(command_prompt);
         c1=catch_CR(wait);            /* wait enter key for wait_sec */
         if(c1==-1) return(-1);        /* time over */
         if(c1==0x1b) continue;
         switch(c1){
            case  'a':
            case  'A':  /* new cfg create */
               printf("Please send text file\x0d\x0a");
               ret=replace_cfg();
               if(ret != 0){
                  printf("\x0d\x0atomo.cfg file can not reloaded\x0d\x0a");
                  printf("return=%d\x0d\x0a",ret);
                  break;   /* 失敗 */
               }
               printf("\x0d\x0aNew tomo.cfg file is re-loaded\x0d\x0a\x0d\x0a");
               printf("Now PC is Resetting\x0d\x0a");
               TCSR_W=0xa51b;
               RSTCSR_W=0x5a5f;
               TCSR_W=0x5af0;
               TCSR_W=0xa57b;
               sleep(1000);
               break; //case a,A

            case  'b':
            case  'B':    /* retrieve cfg file */
               file_out(0);
               break;

            case  'c':
            case  'C':    /* GPS check */
               printf("Please connect GPS receiver\x0d\x0a");
               sci_Restart(GPS_port);
               for(i=0;i<60;i++){
                  serRecvs(GPS_port,bf,0x0a,85,1200);
                  printf("%d %s",i,bf);
               }
               ublox_GNRMC(30,1);
               sci_Stop(GPS_port);
               break;

            case  'd':
            case  'D':  /* retrieve runlog.txt */
               file_out(1);
               break;

            case  'e':
            case  'E':  /* delete CSAC.txt file */
               printf("CSAC.txt is deleted! Are you sure? (y/n)\x0d\x0a");
               c1=catch_CR(10);
               if((c1==0x59) || (c1==0x79)){
                  memset(bf,'\0',80);
                  strcpy(bf,record_media);
                  strcat(bf,"CSAC.txt");
                  printf("DeleteFile=%s\x0d\x0a",bf);
                  fp=open(bf,OptRead);
                  if(fp!=-1){
                     ret=DeleteFile(bf);
                     close(fp);sleep(500);
                     ret=DeleteFile(bf);
                     if(ret!=-1) printf("%s is deleted!\x0d\x0a",bf);
                  }
                  /* Bestatigen Sie das Loschen */
                  fp=open(bf,OptRead);
                  if(fp==-1){
                     printf("%s does not exist\x0d\x0a",bf);
                     synchro_flag=0;
                     runlog(801);   /* Added to avoid delay write USB 20220919 */
                  }
                  if(fp!=-1){
                     close(fp);
                     printf("caution!!! CSAC.txt could not be deleted\x0d\x0a");
                  }
               }
               break;

            case  'g':
            case  'G':  /* delete runlog.txt file */
               printf("runlog.txt is deleted! Are you sure? (y/n)\x0d\x0a");
               c1=catch_CR(10);
               if((c1==0x59) || (c1==0x79)){
                  memset(bf,'\0',80);
                  strcpy(bf,record_media);
                  strcat(bf,"runlog.txt");
                  DeleteFile(bf);
                  runlog(802);   /* Added to avoid delay write USB 20220919 */
               }
               break;

            case  'h':
            case  'H':  /* Help Command */
               display_menu();
               break;

            case  'j':
            case  'J': /* meassure inner clock error */
               if(gps_operation==1){
                  printf("CSAC is not available because GPS operation\x0d\x0a");
                  break;
               }
               printf("measure 1PPS differences between GPS and CSAC\x0d\x0a");
               if(j_command_flag==1) goto skip0;
               sci_Restart(GPS_port);
               for(i=0;i<120;i++){
                  ublox_GNRMC(10,2);
               }
               for(i=0;i<30;i++){
                  ublox_GNRMC(10,1);
               }
               sci_Stop(GPS_port);
               j_command_flag=1;
            skip0:
               sleep(500);
               clock_diff(1);
               break;

            case  'k':
            case  'K':
               if(gps_operation==1){
                  printf("CSAC is not available because GPS operation\x0d\x0a");
                  break;
               }
               sci_Restart(CSAC_port);
               for(i=0;i<15;i++){
                  CSAC_sekunden=CSAC_get_TOD();
                  sec2date(CSAC_sekunden,cal);
                  printf("present time (CSAC):  %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
                     cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
               }
               sci_Stop(CSAC_port);
               break;

            case  'p':
            case  'P':    /* Transmit sound immediately */
               WDT(1);
               Amp_power(0);
               clock_pulse_generate(t_freq_common*2,1);
               TSTR= 0x81;         /* カウンタスタート 1000 0001 */
               transmit(my_station,code_length,code_repeat,mod_num);
               WDT(1);
               TSTR= 0x00;    /* counter stop */
               break;

            case  'r':
            case  'R':  /* Reset system immediately */
               printf("System is reset! Are you sure? (y/n)\x0d\x0a");
               c1=catch_CR(10);
               if((c1==0x59) || (c1==0x79)){
                  printf("Now PC is Resetting\x0d\x0a");
                  TCSR_W=0xa51b;
                  RSTCSR_W=0x5a5f;
                  TCSR_W=0x5af0;
                  TCSR_W=0xa57b;
                  sleep(1000);
               }  
               break;

            case  's':
            case  'S':/* ChipScaleAtomicClock is adjusted by GPS(synchronize) */
               printf("CSAC is synchronizing with GPS\x0d\x0a");
               ret=adjust_clock_using_GPS();
               if(ret != 0) printf("Could not adjust becouse bad GPS,CSAC\x0d\x0a");
               break;

            case  't':
            case  'T':  /* PC time adjust */
               set_rtc_clock();
               break;

            case  'x':
            case  'X':
               j_command_flag=0;
               return(0);  /* 正常に終了しリターンする */
               break;

            case  'Z':
            case  'z':
               printf("Program stop! Are you sure? (y/n)\x0d\x0a");
               c1=catch_CR(10);
               if((c1==0x59) || (c1==0x79)){
                  printf("Good bye\x0d\0a0");
                  gps_power(0);
                  return(100);
               }
               break;

         }  /* switch end */
      }  /* for(;;) end */
}

/* Watch the y key for 10 seconds */
int break_check(void){
   char ch;
   int i,n;

   n=18500; /* This value is detrmined by debug program */
   for(i=0; i<n; i++){
      if(read(1, &ch, 1)){
         if((ch == 'y')||(ch=='Y')) return(1);
      }
   }
   return(-1);
}

char catch_CR(int wait){   /* console terminal */
   int   i,n;
   char  c,bf[8];
      
   bf[0]=0x01;
   n =1300*wait;  /* if wait=5, waiting time is about 5 sec */
   for(i = 0;i < n;i++) {
      if(read(1, &c, 1)){
         bf[1]=bf[0];bf[0]=c;
         write(1, &c , 1);
         if(c==0x0d) return(bf[1]);  /* CR code catch */
      }
   }
   return(-1);
}  

/* 配列の最後には自動的に0(ASCIIZ文字)が付け加えられたものが格納される */
/* strcpy や "abc" も自動的にASCIIZにしてくれる */
static char greeting[]="****** KIOST system Ver 3.1(user mode) *****\x0d\x0a",
      msg1[]="a: Re_load new cfg file      b: retrieve current cfg file\x0d\x0a",
      msg2[]="c: GPS check                 d: retrieve runlog.txt\x0d\x0a",
      msg3[]="e: delete CSAC.txt file      f: reserved\x0d\x0a",
      msg4[]="g: delete runlog&work file   h: Help command\x0d\x0a",
      msg5[]="i: reserved                  j: measure clock error\x0d\x0a",
      msg6[]="k: TOD of CSAC check         m: reserved\x0d\x0a",
      msg7[]="p: Transmit test sound       r: system reset immediately\x0d\x0a",
      msg8[]="s: CSAC adjust using GPS     t: Adjust PC clock\x0d\x0a",
      msg9[]="x: return to a measurment    z: Program stop!\x0d\x0a";

void display_menu(void){
      printf(greeting);
      printf(msg1);printf(msg2);printf(msg3);printf(msg4);
      printf(msg5);printf(msg6);printf(msg7);printf(msg8);
      printf(msg9);
}

int file_out(int flag){
   int   fp=0;
   int   i=0;
   char  bf[80];

      memset(bf,'\0',sizeof(bf));
      strcpy(bf,record_media);
      switch(flag){
         case    0:
            strcat(bf,"tomo.cfg");
            fp=open(bf,OptRead);
            if(fp==-1){
               printf("%s can not open\x0d\x0a",bf);
               return(-1);
            }
            break;
         case    1:
            strcat(bf,"runlog.txt");
            fp=open(bf,OptRead);
            if(fp==-1){
               printf("%s can not open\x0d\x0a",bf);
               return(-1);
            }
            break;
         default:
            break;
      }   /* switch(flag) 終了 */

      while(1){      /* =0 not file end, !=0 file end */
         memset(bf,'\0',sizeof(bf));
         i=read(fp,bf,80);
         if(i<=0) break;
         printf("%s",bf);
      }
      close(fp);
      return(0);
}
