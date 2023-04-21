/* 2020925 shutdown, wakeup program */
/* SH2MB�̓d����f���� */
/* shutdown�M����pio16_1��Bit6���g���� */
/* wakeup�M����RTC��interrupt�M�����g���� */
/* RTC��interrupt signal��alarm_clear(RTC)�R�}���h��clear����� */
/* 20130831 shutdown�����Ȃ��ꍇ��PIC��wakeup�M���𑗂� */
/* 20130927 �U��wakeup�M���p���X����3�b����5�b�ɕύX */
/* RTC�ւ�sleep�����鎞�Ԃ�255�b�ȉ��Ȃ�Ζ]�݂̕b�Ԃ�^���� */
/* RTC�ւ�256�b�ȏ�Ȃ�Ύ���N���������Ύ��Ԃ𕪂܂ł̒P�ʂŗ^���� */
/* wakeup interval��256�b�����Ȃ��WARMUP_TIME���ԑO�ɋN�������� */
/* 256�b�ȏ�̏ꍇ�͕��̒l�Ő؂�グ����(ex80�b�͂Q���O) */

#include <mes2.h>
#include "SH7145.h"
#include "SH2MB.h"
#define NOSHUTDOWN  90  /* 20220914 change 30 to 90 */

/* SH2MB CN1 pin17(XPB6)��shutdown�p�̐M���[�q(Cat3 mainboard) */
/* pio16_1��16�r�b�g���o�͂ɐݒ� */
/* d0=GPS 1Hz display enable
   d1=PIC counter clear
   d2=no use
   d3=no use
   d4=Transducer receive�؂�ւ�relay switch
   d5=RaspberryPi power control
   d6=shutdown
   d7=no use

   GPS 1Hz��PortE1�ɐڑ����Ă���
   calender 1Hz=PE2 */
/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void shutdown(int wakeup_byou);
void shutdown8hour(void);

/* **** External function declaration ************* */
extern int  set_timer(int shutdown_sec);       /* SH2_RTC.c */
extern int  set_alarm_clock(int pc_byou);    /* SH2_RTC.c */
extern int  read_timer(void);                /* SH2_RTC.c */
extern int  read_alarm_clock(void);          /* SH2_RTC.c */
extern int  read_rtc_clock(void);
extern int  sec2date(int byou,int cal[]);
extern void Amp_power(int flag);             /* power_cntl.c */

extern int  wakeup_interval;/* wakeup interval�iunit: sec) */

void shutdown(int wakeup_byou){
   int shutdown_sec,cal[6];
   int   alarm_byou,present_time;
   long k;

      present_time=read_rtc_clock();
      shutdown_sec=(int) wakeup_byou-present_time;

      if(shutdown_sec<=(NOSHUTDOWN)){
         printf("shutdown_sec=%d\x0d\x0a",shutdown_sec);
         printf("shutdown time is too small,\x0d\x0a");
         printf(" then do not enter to shutdown\x0d\x0a");
         return;
      }

      if(shutdown_sec<255){        /* sleep���������b�l��8bit_timer�ɃZ�b�g */
         set_timer(shutdown_sec);  /* sleep_sec��ARTC���犄�荞�ݐM�����o�� */
         alarm_byou=present_time+read_timer();      
      }else{ 
         set_alarm_clock(wakeup_byou); /* ��Ύ��� date:hour:min ���Z�b�g���� */
         alarm_byou=read_alarm_clock();
      }
      shutdown_sec=(int) alarm_byou-present_time;
      printf("***********************************\x0d\x0a");
      printf("shutdown_sec=%d\x0d\x0a",shutdown_sec);
      printf("***********************************\x0d\x0a");
      if((shutdown_sec > wakeup_interval) || (shutdown_sec < 0)){
         printf("Something wrong\x0d\x0a");
         return;
      }
      (void)sec2date(present_time,cal);
      printf("present time=    %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",cal[0],cal[1],
               cal[2],cal[3],cal[4],cal[5]);
      (void)sec2date(alarm_byou,cal);
      printf("next wakeup time=%4d %2d %2d  %2d:%02d:%02d\x0d\x0a",cal[0],cal[1],
               cal[2],cal[3],cal[4],cal[5]);

      printf("Enter to shutdown\x0d\x0a");
      eject("mmc0");eject("rom0");
      for(k=0;k<400;++k){
         Amp_power(0);
         pio16_1=0x40;  /* PIC will cut power for SH2 after catching this */
         sleep(1000);
      }
      return;
}
//#define DEBUG
/* 8���Ԑ�܂�shutdown������ */
/* PIC�v���O�����̒���9���Ԍo�߂����1�xwakeup����l�ɂ��Ă��� */
void shutdown8hour(void){
   int present_time,next_wakeup_time;
   #ifdef DEBUG
      int cal[6];
   #endif

      present_time=read_rtc_clock();
      next_wakeup_time=present_time+8*3600;/* 8���Ԑ�܂�shutdown������ */
      #ifdef DEBUG
         (void)sec2date(present_time,cal);
         printf("present time=    %4d %2d %2d  %2d:%02d:%02d\x0d\x0a",cal[0],cal[1],
               cal[2],cal[3],cal[4],cal[5]);
         (void)sec2date(next_wakeup_time,cal);
         printf("next wakeup time=%4d %2d %2d  %2d:%02d:%02d\x0d\x0a",cal[0],cal[1],
               cal[2],cal[3],cal[4],cal[5]);
      #endif
      shutdown(next_wakeup_time);/* wakeup after one month */
}
   
