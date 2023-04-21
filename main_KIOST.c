/* �؍��C�m������ �K���X���ƃA���~�ψ��e�� tomo.exe with CSAC (MES2.5R19) */
/* WARMUP_TIME��40����90�ɂ����B���R��mount USB���s���ɒ������Ԃ��o�߂��� */
/* you can use USB or SD memory */
/* When using a USB memory,please use Elecom MF-SUB32GBK */
/* gps_operation is also possible */
/* gps_operation is also possible */
/* �e�ǂ��������𔭎�(round_robin)�܂��͓���(conccurent)�ɉ��𔭎˂��� */
/* RasPi not built in */
/* Connect a new Bleutooth BLE to the console */
/* GPS receiver is built outside of Aluminium housing(kcat1,2) */
/* GPS receiver is built in the glass sphere(kcat3,4) */
/* Use the console terminal as a monitor for kcat1,2 */
/* The system clock of the CPU is CSAC 10MHz */
/* The transmit and receive clock are created from the CPU clock */
/* time variables are the number of clocks used by the CPU */
/* MES_OS is stoped in the time critical part of the program */

/* Automatically sync CSAC clock if CSAC.txt does not exist */
/* CSAC.txt is created when the CSAC clock is synchronized */
/* At the beginning of the experiment, delete old CSAC.txt */
/* user mode�ɓ���΁AE command��CSAC��delete���o���� */
/* receive relay switch��analog power�ƓƗ��ɐ���ł���悤�ɂ��� */

//#define DEBUG
#define VERSION "KIOST tomo.exe debug Ver=20220925"
#define WARMUP_TIME 90        /* 20220915 change (sec) USB�g�p */
#define PREPARATION_TIME 5    /* countdown���O�܂łɋ�����鎞��(sec ) */
#define ON  1
#define OFF 0

#include <MES2.h>
#include <string.h>
#include "SH7145.h"
#include "SH2MB.h"  /* SH2MB */
#include "define_com_port.h"
#include "TOMO.h" /* define global variable */

/* **** External function declaration ************* */
extern void alarm_reset(void);            /* SH_RTC.c */
extern void cpu_register_set(void);
extern int  default_set(void);            /* default_set.c */
extern int  get_present_time(void);       /* ping_time_schedule.c */
extern void openning_mes(int present_sec,int battery_low);  /* log.c */
extern int  experiment_set(char *cfg_file);
extern void gps_power(int on_off);
extern int  user_mode(int wait,int src);
extern int  adjust_clock_using_GPS(void);
extern void write_rtc_clock(int pc_byou); /* SH_RTC.c */
extern int  sec2date(int byou,int cal[]);
extern void shutdown(int next_wakeup_sec);
extern void shutdown8hour(void);
extern void software_standby(int present_sec,int wakeup_sec);
extern int  ping_time_schedule(int present_time);
extern int  round_robin(int ping_byou);
extern int  concurrent(int ping_byou);
extern void sci_Restart(int port);
extern int  CSAC_get_TOD(void);
extern void runlog(int code);
/*******************************/

int main(void){
   char cfg_file[80];           /* working */
   int count,ret, 
      battery_low,
      ping_byou=0,
      next_wakeup_sec,
      cal[6],                 /* Calender working */
      present_sec;

//      printf("r,1\x0d\x0a");  /* BLE initialize */
      printf("\x0d\x0a***** KIOST Tomography program start *****\x0d\x0a");
      strcpy(version,VERSION);
      /* PIC��PowerOnReset���J��Ԃ��̂ŁA�ŏ���alarm_reset���K�v */
      alarm_reset();          /* SH2_RTC.c */   
      cpu_register_set();

      /* ************************************** */
      /* serial open,����CSAC��GPS�Ǝ��Ԃ��킹����Ă��邩���肷�� */
      synchro_flag=default_set();

      memset(cfg_file,0,sizeof(cfg_file));
      strcpy(cfg_file,record_media);
      strcat(cfg_file,"tomo.cfg");
      printf("cfg_file=%s\x0d\x0a",cfg_file);

      /* present_sec��2000�N1��1������̌o�ߕb�� */
      present_sec=get_present_time();
      battery_low=(pio16_0 & 0x0080);  /* XPA7��PIC�����Battery_Low���ڑ� */
      #ifdef DEBUG
         battery_low=0;
      #endif
      openning_mes(present_sec,0); /* openning message display */
      if(battery_low!=0){
         runlog(9);
         shutdown8hour();
      }
      count=0;
   again:
      ret=experiment_set(cfg_file);/* read tomo.cfg & read Code */
      if(ret<0){
         printf("experiment_set failed:ret code=%d\x0d\x0a",ret);
         ret=user_mode(30,2);
         if(ret==100){
            runlog(1);
            /* ��������������Ă���OS�ɖ߂� */
            if(p_data>0) free((char *)p_data);
            exit(1);
         }   /* Program �I�� shell�ɖ߂� */
         count++;
         if(count<3) goto again;
         printf("System will shutdown\x0d\x0a");
         runlog(2);shutdown8hour();
      }
      if(gps_operation==1) gps_power(ON);
      ret=user_mode(30,0);         /* user mode�ɓ��� 10�b�Ԃ�Break���͑҂� */
      if(ret==100){
         runlog(3);
         /* ��������������Ă���OS�ɖ߂� */
         if(p_data>0) free((char *)p_data);
         exit(1);
      }   /* Program �I�� shell�ɖ߂� */
      if(gps_operation==1) goto next1;

      /* ����CSAC��GPS�Ǝ��ԍ��킹����Ă��Ȃ��ꍇ�͎��ԍ��킹���� */
      sci_Restart(CSAC_port);
      if(synchro_flag==1) goto next0;
      gps_power(ON);
      printf("CSAC is synchroning with GPS!\x0d\x0a");
      ret=adjust_clock_using_GPS();
      gps_power(OFF);
      if(ret<0){
         printf("CSAC could not be synchronized with GPS!\x0d\x0a");
         printf("System will shutdown\x0d\x0a");
         runlog(4);shutdown8hour();
      }               
   next0:
      sci_Restart(CSAC_port);
      for(count=0;count<15;++count){
         ret=CSAC_get_TOD();
         printf("CSAC check i=%d ret=%d\x0d\x0a",count,ret);
         if(ret != -1){
            write_rtc_clock(ret);
            break;
         }
      }
   next1:      
      sleep(100);
loop0:
      /* **** PIC_counter_clear(20220921) by alive signal **** */
      pio16_1 &= ~0x02;sleep(50);pio16_1 |= 0x02;
      /* �����o���������ǂ������肷�� */
      printf("\x0d\x0a****** Ping time schedule *****\x0d\x0a");
      present_sec=get_present_time();
      sec2date(present_sec,cal);         
      printf("present_time=  %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);

      ping_byou=ping_time_schedule(present_sec);
      sec2date(ping_byou,cal);
      printf("Next ping_time=%4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);

      if(ping_byou<(present_sec+PREPARATION_TIME)){
         printf("too short time\x0d\x0a");
         runlog(5);
         goto next2;
      }
      if(ping_byou>(present_sec+WARMUP_TIME+10)){
         printf("not yet wakeup time\x0d\x0a");
         runlog(6);
         goto next2;
      }

      /* Sound Transmit and Receive start */
      if(round_interval==0){
         concurrent(ping_byou);
      }
      else{
         round_robin(ping_byou);
      }
   next2:
      /* wakeup�����̌��� */
      printf("****** Ending process ******\x0d\x0a");
      present_sec=get_present_time();
      sec2date(present_sec,cal);         
      printf("present_time=  %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);

      ping_byou=ping_time_schedule(present_sec);
      sec2date(ping_byou,cal);
      printf("Next ping_time=%4d %2d %2d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);

      switch(gps_operation){
         case 0:
            next_wakeup_sec=ping_byou-WARMUP_TIME;
            shutdown(next_wakeup_sec);
            break;
         case 1:
            next_wakeup_sec=ping_byou-30;
            software_standby(present_sec,next_wakeup_sec);
            break;
      }
      goto loop0;
      /* ��������������Ă���OS�ɖ߂� */
      if(p_data>0) free((char *)p_data);
}     /* main end */
