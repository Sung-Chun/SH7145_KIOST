/* 20220925 software standby for KIOST program */
//#define DEBUG
#include <mes2.h>
#include "SH7145.h"
#include "SH2MB.h"   /* SH2MB */
#include "define_com_port.h"
/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void software_standby(int present_sec,int wakeup_sec);

/* **** declaration external function ************* */
extern void alarm_reset(void);                  /* SH_RTC.c */
extern int  set_timer(int sleep_byou);          /* SH_RTC.c */
extern int  set_alarm_clock(int pc_byou);       /* SH_RTC.c */
extern int  read_alarm_clock(void);             /* SH_RTC.c */
extern int  read_timer(void);                   /* SH_RTC.c */
extern int  sec2date(unsigned int byou,int cal[]);
extern void sci_Open(int port,int speed);
extern void OS_terminal_open(void); /* SH2_serial_10MHz.c */
extern int  clock_pulse_generate(int freq,int flag);

extern int move_raw_files_to_usb(int num_to_move);

/* **** declaration external(global) variable ***** */
extern int  wakeup_interval,/* wakeup intervalÅiunit: sec) */
            t_freq_common;

void software_standby(int present_sec,int wakeup_sec){
   int alarm_byou,sleep_byou;
   unsigned short work;
   int cal[6];

      sleep_byou=wakeup_sec-present_sec;
      #ifdef DEBUG
         printf("wakeup_sec=%d\x0d\x0a",wakeup_sec);
         printf("present_sec=%d\x0d\x0a",present_sec);
         sec2date(present_sec,cal);
         printf("present_time=  %5d/%2d/%2d  %2d:%2d:%2d\x0d\x0a",
            cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
         sec2date(wakeup_sec,cal);
         printf("wakeup_time =  %5d/%2d/%2d  %2d:%2d:%2d\x0d\x0a",
            cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
      #endif
      if(sleep_byou<=30){
         printf("sleep time(sec)=%d\x0d\x0a",sleep_byou);
         printf("sleep time is too small,\x0d\x0a");
         printf(" then do not enter to sleep\x0d\x0a");
         return;
      }
      if(sleep_byou<255){
         /* set the second value you want to sleep to 8bit_timer */
         set_timer(sleep_byou);
         alarm_byou=present_sec+read_timer();      
      }
      else{ 
         set_alarm_clock(wakeup_sec); /* setting month:date:hour:min */
         alarm_byou=read_alarm_clock();
      }
      sleep_byou=alarm_byou-present_sec;
      sec2date(wakeup_sec,cal);
      printf("wakeup_time=   %5d/%2d/%2d  %2d:%2d:%2d\x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);
      printf("sleep time(sec)=%d\x0d\x0a",sleep_byou);
      if((sleep_byou > wakeup_interval) || (sleep_byou < 0)){
         printf("Something wrong\x0d\x0a");
         return;
      }

      /* Change the log folder name if rolling interval time elapsed. */
      roll_log_folder_name();
      /* Move raw files from record media to usb memory */
      move_raw_files_to_usb(5);

      /* Re-check the sleep time */
      present_sec = get_present_time();
      sleep_byou = wakeup_sec - present_sec;
      if(sleep_byou <= 30){
         printf("sleep time(sec)=%d\x0d\x0a",sleep_byou);
         printf("sleep time is too small,\x0d\x0a");
         printf(" then do not enter to sleep\x0d\x0a");
         return;
      }
      if(sleep_byou < 255){
         /* set the second value you want to sleep to 8bit_timer */
         set_timer(sleep_byou);
         alarm_byou = present_sec+read_timer();
      }
      else{
         set_alarm_clock(wakeup_sec); /* setting month:date:hour:min */
         alarm_byou = read_alarm_clock();
      }
      sleep_byou = alarm_byou - present_sec;
      if((sleep_byou > wakeup_interval) || (sleep_byou < 0)) {
         printf("Something wrong (after moving raw files)\x0d\x0a");
         return;
      }

      printf("Enter software_standby\x0d\x0a");
      /* prepare entering to software standby mode */
      work=TCSR;
      TCSR_W = 0xa51e;  /* WDT */  
      /* setting a waiting time until the clock stabiling when getting up */
      TCSR_W = 0x5aff;
      PBCR2 &= ~0x0800; /* IRQ3 usage setting */
      PBCR2 |=  0x0400;
      ICR1 |= 0x0010;   /* interruptat edge detection of IRQ3 */
      ICR2 &= ~0x0300;  /* interrupt at the fall of IRQ3 */
      SBYCR = 0x9e;     /* move software standby with sleep,wakeup by IRQ3 */
      asm(" SLEEP ");    

      /* after wakeup current program runs */
      /* Peripherals have been reset, need to be reconfigured */
      OS_terminal_open();           /* console */
      sci_Open(GPS_port,9600);      /* for GPS */
      sci_Open(CSAC_port,57600);    /* for CSAC */
      TCSR_W =(unsigned short) (0xa500 | work); /* restore WDT setting */ 
      clock_pulse_generate(2*t_freq_common,1);
      alarm_reset();
      printf("Wakeup from software standby\x0d\x0a");
      return;
}
