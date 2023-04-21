/* 20210806 KIOST tomo.exe */
/* concurrent mode�Ŏg���ϐ������肷�� */
/* concurrent mode�ł�1�x���߂�Γ����l���g���� */
/* A/D�ϊ���clock���ŕϐ��̒l���v�Z���� */
/* tomo.cfg��extra_record length��msec�ŗ^���Ă��� */
/* ���̂��ߎ�M�҂����ԂȂǂ̕ϐ�����M�N���b�N�̃N���b�N���ɕϊ����^���� */
/* �ϐ���count�����Ă���̂�,���̎�M���g���̃N���b�N����\�� */

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
   t_freq_common,          /* given at read_cfg,conccurent�Ŏg��Transmit freq */
   code_no[],              /* given at read_cfg */
   order[],
   tr_time[],              /* msec */
   code_length,            /* modification_set.c�Ōv�Z����Ă��� */
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

      /* Transmit,demodulate�Ɏg��clock�M���̏o�͂��J�n���� */
      true_sampling_freq=clock_pulse_generate(2*t_freq_common,1);
      one_period_pair=mod_num*2*code_length;
      sound_wid = (int)sound_width_count*1000/true_sampling_freq;/* msec */

      printf("*** calculate receive conditions for concurrent mode *** \x0d\x0a");
      /* ********** determin ad_start_count ************ */
      /* ad_start�͗\�z�`�d���Ԃ�extra_record_width(msec)�O����s�� */
      /* ad_start��sound_width�ȉ��̏ꍇsound_width�Ɠ������l�ɂȂ� */
      st=order[1];
      top_runner=tr_time[st];                            /* msec */
      ad_start=top_runner-extra_record_width;            /* msec */
      if(ad_start<=sound_wid) ad_start=sound_wid;
      ad_start_count=(int)(ad_start*true_sampling_freq/1000);
      ad_wait_count=ad_start_count-sound_width_count;
      if(ad_wait_count<0) ad_wait_count=0;

      /* **** calculate A/D conversion data no (cos,sin pair no) ***** */
      st=order[total_st-1];      /* ��ԍŌ�ɂ���Ă���� */
      sample_duration_time=tr_time[st]+sound_wid+extra_record_width-ad_start;
      ad_convert_pair_no=(int)(sample_duration_time*true_sampling_freq/1000);

      /* **** calculate number to record in RAM and SD memory ***** */
      switch(receive_mode){
         case 0:
            p_data=(short *)malloc(ad_convert_pair_no*4+16);   /* �]���Ɏ���Ă��� */
            if(p_data==0){
               printf("Menmory allocation(p_data) can not work!\x0d\x0a");
               runlog(401);
               return(-1);           /* memory allocation error, ����Program�̏I�� */
            }
            superimpose_count=0;
            save_pair_no=ad_convert_pair_no;
            break;
         case 1:
            superimpose_count=(int)(ad_convert_pair_no/one_period_pair);
            if(ad_convert_pair_no%one_period_pair!=0) superimpose_count++;
            ad_convert_pair_no=superimpose_count*one_period_pair;
            p_data=(short *)malloc(one_period_pair*4+16);   /* �]���Ɏ���Ă��� */
            if(p_data==0){
               printf("Menmory allocation(p_data) can not work!\x0d\x0a");
               runlog(402);
               return(-1);           /* memory allocation error, ����Program�̏I�� */
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
