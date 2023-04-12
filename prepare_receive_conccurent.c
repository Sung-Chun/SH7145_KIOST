/* 20210806 KIOST tomo.exe */
/* concurrent modeで使う変数を決定する */
/* concurrent modeでは1度決めれば同じ値が使われる */
/* A/D変換のclock数で変数の値を計算する */
/* tomo.cfgでextra_record lengthをmsecで与えている */
/* そのため受信待ち時間などの変数を受信クロックのクロック数に変換し与える */
/* 変数にcountがついているのは,この受信周波数のクロック個数を表す */

#include <stdlib.h>
#include <mes2.h>
#include "SH7145.h"

/* **** Function prototype declaration ************ */
int  prepare_receive_concurrent(void);

/* **** External function declaration ************* */
extern int  clock_pulse_generate(int freq,int flag);
extern void runlog(int code);

/* **** external(global) variable declaration ******* */
extern char station_name[16][20];
extern int
   total_st,               /* given at read_cfg */
   my_station,             /* given at read_cfg */
   mod_num,                /* given at read_cfg */
   code_repeat,            /* given at read_cfg, sound repeat number */
   receive_mode,           /* given at read_cfg */
   extra_record_width,     /* given at read_cfg (msec) */
   t_freq_common,          /* given at read_cfg,conccurentで使うTransmit freq */
   code_no[],              /* given at read_cfg */
   order[],
   tr_time[],              /* msec */
   code_length,            /* modification_set.cで計算されている */
   one_period_pair,        /* given experiment_set.c */
   sound_width_count;      /* given experiment_set.c */

/* The following variables are determined in this function */
extern int  /* determine in this function */
   true_sampling_freq,
   ad_start_count,
   ad_wait_count,       /* use at receive_raw.c */
   save_pair_no,
   ad_convert_pair_no,  /* (cos,sin) pair number */
   superimpose_count;   /* superimpose no */
extern short *p_data;

int prepare_receive_concurrent(void){
   int   st,top_runner,
         sound_wid,
         sample_duration_time,   /* A/D conversion length(msec) */
         ad_start=0;             /* time from ping(msec) */

      /* Transmit,demodulateに使うclock信号の出力を開始する */
      true_sampling_freq=clock_pulse_generate(2*t_freq_common,1);
      one_period_pair=mod_num*2*code_length;
      sound_wid = (int)sound_width_count*1000/true_sampling_freq;/* msec */

      printf("*** calculate receive conditions for concurrent mode *** \x0d\x0a");
      /* ********** determin ad_start_count ************ */
      /* ad_startは予想伝播時間のextra_record_width(msec)前から行う */
      /* ad_startがsound_width以下の場合sound_widthと等しい値になる */
      st=order[1];
      top_runner=tr_time[st];                            /* msec */
      ad_start=top_runner-extra_record_width;            /* msec */
      if(ad_start<=sound_wid) ad_start=sound_wid;
      ad_start_count=(int)(ad_start*true_sampling_freq/1000);
      ad_wait_count=ad_start_count-sound_width_count;
      if(ad_wait_count<0) ad_wait_count=0;

      /* **** calculate A/D conversion data no (cos,sin pair no) ***** */
      st=order[total_st-1];      /* 一番最後にやってくる局 */
      sample_duration_time=tr_time[st]+sound_wid+extra_record_width-ad_start;
      ad_convert_pair_no=(int)(sample_duration_time*true_sampling_freq/1000);

      /* **** calculate number to record in RAM and SD memory ***** */
      switch(receive_mode){
         case 0:
            p_data=(short *)malloc(ad_convert_pair_no*4+16);   /* 余分に取っておく */
            if(p_data==0){
               printf("Menmory allocation(p_data) can not work!\x0d\x0a");
               runlog(401);
               return(-1);           /* memory allocation error, このProgramの終了 */
            }
            superimpose_count=0;
            save_pair_no=ad_convert_pair_no;
            break;
         case 1:
            superimpose_count=(int)(ad_convert_pair_no/one_period_pair);
            if(ad_convert_pair_no%one_period_pair!=0) superimpose_count++;
            ad_convert_pair_no=superimpose_count*one_period_pair;
            p_data=(short *)malloc(one_period_pair*4+16);   /* 余分に取っておく */
            if(p_data==0){
               printf("Menmory allocation(p_data) can not work!\x0d\x0a");
               runlog(402);
               return(-1);           /* memory allocation error, このProgramの終了 */
            }
            save_pair_no=one_period_pair;
            break;
         default: break;
      }

      printf("sound_width(msec)= %d\x0d\x0a", sound_wid);
      printf("ad_start(msec)= %d\x0d\x0a",ad_start);
      printf("sample_duration(msec)=%d\x0d\x0a",sample_duration_time);
      printf("ad_start_count=%d ad_wait_count=%d\x0d\x0a",
         ad_start_count,ad_wait_count);
      printf("ad_convert_pair_no=%d\x0d\x0a",ad_convert_pair_no);
      switch(receive_mode){
         case 0:printf("normal reception\x0d\x0a");break;
         case 1:printf("superposition reception on ring buffer\x0d\x0a");
               break;
      }
      printf("save_pair_no=%d\x0d\x0a",save_pair_no);
      return(0);
}
