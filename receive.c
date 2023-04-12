/* 20210806 revised */
/* enter here with MES_OS stopped(with WDT(0)) */
/* receive_mode=0:after saving data of AD conv. data are shifting 6bits */
/* receive_mode=1:just after AD conv. data are shifting 6bits */

//#define DEBUG
#include <mes2.h>
#include "SH7145.h"
#include "SH2MB.h"

/* **** Function prototype declaration ************ */
int receive(int wait_count,int ad_convert_pair_no);
void AD_ini(void);

/* **** External function declaration ************* */
extern void Amp_power(int on_off);              /* power_cntl.c */
extern void receive_relay_switch(int on_off);   /* power_cntl.c */
extern void WDT(int flag);                      /* register_set.c */

/* **** external(global) variable declaration ***** */
extern short *p_data;
extern int
   round_interval,
   gps_operation,
   receive_mode,     /* nornal receive(0) or ring buffer pileup receive(1) */
   one_period_pair;  /* experiment_set.c */
#ifdef DEBUG
   extern int  sampling_freq1000,cpu_system_clock;
#endif

void AD_ini(void){   /* A/D start by MTU channel_4 clock */
    /* set ADTSR(ADTrigerSelectRegister),ADControlRegister */      
      MSTCR2 &= ~0x0030;/* standby mode reset */
      ADTSR  = 0x0a;    /* AD0 and AD1 are triggered by MTU */
      ADCR_0 = 0x67;
      ADCR_1 = 0x67;
      ADCSR_0 =0x08;    /* AN0 select,Single mode,without ADFlag interrupt */
      ADCSR_1 =0x08;    /* AN4 select,Single mode AD1 set */
      IPRH  &= 0x0fff;  /* WDT priority is set 3(13)に設定 */
      IPRH |= 0xd000;   /* AD convert priority set highest 1(15) */
      IPRG |= 0xf000;
      TIER_0 = 0x40;    /* AD conversion stop now */
}

/* **** receive_raw ************************************************* */
/* before call this function you must output 8kHz,4kHz(cos,sin) clock */
/* ****************************************************************** */
int receive(int wait_count,int ad_convert_pair_no){
   int   i;
   unsigned short sdata;
   short *p;
   short *p_end;

      WDT(0);
      Amp_power(1);
      receive_relay_switch(1);
      AD_ini();
      p=p_data;p_end=p_data+one_period_pair*2;   /* cos,sin total no */
   #ifdef DEBUG
      int i1,i2,stop_watch_value,timer12_count,estimate_receive_width;
      char bf[80];
      double delta_t0,delta_t1;
      TSTR &= ~0x06;    /* MTU,1,2 counter stop */
      TCR_1  = 0x07;    /* 000 00 111   countup by overflow of TCNT_2 */
      TCR_2  = 0x00;    /* 000 00 000   countup by inner clock */
      delta_t0=(double)(1000000.0/(double)cpu_system_clock);
      TCNT_1 = 0;
      TCNT_2 = 0;
   #endif

      TSTR |= 0x81;  /* MTU channel 0 and 4 counter start */
      ADCR_0 = 0xe7; /* Enable AD conv. start by MTU trigger */
      ADCR_1 = 0xe7;
      TIER_4 = 0xc0; /* Allow AD conv. request by compare match of TGRA_4 */

      if(round_interval>0){                     /* 1Hzだけ待つ */
         switch(gps_operation){
            case 0:  /* CSACの1Hz使用 (pulse width=100usec) */
               while((PEDRL & 0x0004)>0){;}     /* CSAC,Lowになるまで待つ */
               while((PEDRL & 0x0004)<1){;}     /* CSAC,Highになるまで待つ */
               break;
            case 1: /* GPSの1Hz使用 */
               while((PEDRL & 0x0002)>0){;}     /* GPS,Lowになるまで待つ */
               while((PEDRL & 0x0002)<1){;}     /* GPS,Highになるまで待つ */
               break;
         }
      }

      /* AD convert but do not store memory. Wait to save memory.  */
      for(i=wait_count;i>0;--i){
         while((ADCSR_0 & 0x80)==0){;}
         ADCSR_0 &= (~0x80);
         ADCSR_1 &= (unsigned char)(~0x80);  /* Clear ADF(A/D end flag) */
      }
      /* store memory from here */
      p=p_data;
      #ifdef DEBUG
         TSTR |= 0x06;        /* stop watch start: MTU1,2 counter start */
      #endif
      switch(receive_mode){
         case 0:/* normal reception */
            for(i=ad_convert_pair_no;i>0;--i){
               while((ADCSR_0 & 0x80)==0){;}
               *(p++)=(short)(ADDR0);   /* cos */
               *(p++)=(short)(ADDR4);   /* sin */
               ADCSR_0 &= (unsigned char)(~0x80);
               ADCSR_1 &= (unsigned char)(~0x80);  /* Clear ADF(A/D end flag) */
            }
            break;
         case 1:/* Superposition reception on ring buffer */
            for(i=ad_convert_pair_no;i>0;--i){
               while((ADCSR_0 & 0x80)==0){;}
               *(p++)+=(short)((unsigned short)ADDR0>>6);   /* cos */
               *(p++)+=(short)((unsigned short)ADDR4>>6);   /* sin */
               ADCSR_0 &= (unsigned char)(~0x80);
               ADCSR_1 &= (unsigned char)(~0x80);  /* Clear ADF(A/D end flag) */
               if(p>=p_end) p=p_data;        /* ring buffer */
            }
           break;
         default: break;
      }
      ADCR_0 = 0x67; /* stop AD convert by MTU */
      ADCR_1 = 0x67;
      TIER_4 = 0x40; /* stop TGRA_4 compare match */
      Amp_power(0);  /* 20210808 これが抜けていた */
      #ifdef DEBUG
         TSTR &= ~0x06;             /* timer12_counter stop */
         i1=TCNT_1;i2=TCNT_2;
         timer12_count=(i1<<16)+i2;
         stop_watch_value=(int)((double)timer12_count*delta_t0);
         delta_t1=1000000.0/(((double)sampling_freq1000)/1000.);/* usec */
         estimate_receive_width=(int)((double)ad_convert_pair_no*delta_t1);
         WDT(1);
         printf("cpu_system_clock=%d period(ns)=%d\x0d\x0a",
            (int)cpu_system_clock,(int)(delta_t0*1000.0));
         printf("sampling_freq=%d period(us)=%d\x0d\x0a",
            (int)sampling_freq1000/1000,(int)delta_t1);
         printf("ad_convert_pair_no=%d\x0d\x0a",ad_convert_pair_no);
         printf("estimate_receive_width(us)=%d measured width(us)=%d\x0d\x0a",
            estimate_receive_width,stop_watch_value);
         WDT(0);
      #endif
      if(receive_mode==0){  /* AD value are shift 6 bits */
         p=p_data;
         for(i=ad_convert_pair_no*2;i>0;--i){
            sdata=(unsigned short)*p;
            *(p++) = sdata>>6;    /* Logical shift */
         }
      }
      WDT(1);
      #ifdef DEBUG
         return(stop_watch_value);
      #endif
      return(0);
   }
