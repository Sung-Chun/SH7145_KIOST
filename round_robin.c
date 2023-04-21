/* ���E���h���r�����ɉ��g�̑���M���s�� */
/* token�������Ă���ǂ����𔭎˂��� */
/* round_robin�̏ꍇ��Tr_freq���ǂɂ��قȂ��Ă��ǂ� */            
/* tomo.cfg��extra_record length��msec�ŗ^���Ă��� */
/* ���̂��ߎ�M�҂����ԂȂǕϐ�����M�N���b�N�̃N���b�N���ŗ^���� */

#define DEBUG
#include <string.h>     /* memset() */
#include <stdlib.h>
#include "SH7145.h"
#include <MES2.h>

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int round_robin(int ping_byou);

/* **** External function declaration ************* */
extern void WDT(int flag);
extern void runlog(int);                  /* log.c */
extern int  wait_time(int wait_byou);
extern void transmit(int st,int code_length,int times,int mod_num);
extern void receive(int count,int ad_convert_pair_no);
extern int  clock_pulse_generate(int freq,int flag);
extern int  save_raw_data(int ping_byou);

/* **** external(global) variable declaration ***** */
extern char station_name[16][20],bf[];
extern int
   total_st,               /* given at read_cfg */
   my_station,             /* given at read_cfg */
   round_interval,
   mod_num,
   code_length,
   code_repeat,            /* given at read_cfg, sound repeat number */
   receive_mode,           /* given at read_cfg */
   extra_record_width,     /* given at read_cfg (msec) */
   t_freq[],               /* given at read_cfg,conccurent�Ŏg��Transmit freq */
   code_no[],
   order[],
   tr_time[],              /* msec */
   sound_width_count;      /* sound_width��clock�ŃJ�E���g������ */

/* The following variables are determined in this function */
extern int  /* determin in this function */
   true_sampling_freq,
   one_period_pair,     /* mod_num*2*code_length */
   ad_start_count,ad_wait_count,
   ad_wait_count,       /* use at receive_raw.c */
   save_pair_no,
   ad_convert_pair_no,  /* (cos,sin) pair number */
   superimpose_count;
      
extern short *p_data;

int round_robin(int ping_byou){
   int
      token,ret,station,
      top_runner=0,
      transmit_freq,
      ad_start,            /* msec */
      sound_wid,           /* msec */
      sample_duration_time;

      for(station=0;station<total_st;++station){
         token=station; /* �����o�����������ǔԍ� */
         transmit_freq=t_freq[token];
         /* Transmit,demodulate�Ɏg��clock�M���̏o�͂��J�n���� */
         true_sampling_freq=clock_pulse_generate(2*transmit_freq,1);       
         sound_wid = (int)sound_width_count*1000/true_sampling_freq;/* msec */

         printf("****************************************\x0d\x0a");
         if(my_station==token){     /* ���𑗐M����ǂł��� */
            printf("My station will transmit sound\t");
            printf("  sound_width(ms)=%d\x0d\x0a",sound_wid);
         }
         else{                      /* ������M����ǂł��� */
            printf("My station will receive sound\x0d\x0a");
            printf("*** calculate receive conditions for round_robin mode *** \x0d\x0a");
            /* ************* ad_start(msec)�̌�������� *************** */
            /* ad_start  (msec) �\�z�`�d���Ԃ�extra_record_width�O����s�� */
            /* ad_start_count  ping_time����̃N���b�N���̌v�Z */      
            /* ad_wait_count   A/D�ϊ��܂ő҂�����N���b�N���̌v�Z */      
            top_runner=tr_time[token];
            ad_start=top_runner-extra_record_width;   /* msec */
            if(ad_start<0) ad_start=0;
            ad_start_count=ad_wait_count=(int)(ad_start*true_sampling_freq/1000);
            /* **** calculate A/D conversion data no (cos,sin pair no) ***** */
            sample_duration_time=sound_wid+extra_record_width*2;   /* msec */
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
                  memset(p_data,0,one_period_pair*4);
                  break;
               default: break;
            }

            printf("Sound station=%s code=%2d sound_width(msec)=%d\x0d\x0a"
               ,station_name[token],code_no[token],sound_wid);
            printf("expected arrival time(msec)=%d ad_start(msec)=%d\x0d\x0a"
               ,tr_time[token],ad_start);
            printf("sample_duration(msec)=%d\x0d\x0a",sample_duration_time);
            printf("ad_start_count=%d ad_wait_count=%d\x0d\x0a",
               ad_start_count,ad_wait_count);
            printf("ad_convert_pair_no=%d\x0d\x0a",ad_convert_pair_no);
            switch(receive_mode){
               case 0:printf("normal reception\x0d\x0a");break;
               case 1:printf("superposition reception on ring buffer\x0d\x0a");
                     break;
            }
            printf("save_pair_no=%d\r",save_pair_no);
         }  /* else end */

      /* ***** wait_time()�ŉ��g���ˎ����܂ő҂�******* */
         printf("***** Countdown *****\x0d\x0a");
         ret=wait_time(ping_byou);
      /* ********���g���M�܂��͎�M�ɓ��� ************* */
         if(ret==0){
            if(my_station==token){
               /* transmit1()���Ŏ���1�b�̗����オ��܂ő҂̂� */
               /* ���g���ˎ�����ping_byou+1�ƂȂ� */
               transmit(my_station,code_length,code_repeat,mod_num);
               printf("Sound was tranmitted\x0d\x0a\x0d\x0a");
            }
            else{
               receive(ad_wait_count,ad_convert_pair_no);
               save_raw_data(ping_byou);
               printf("save raw data end\x0d\x0a");
            }
         }
         else{    /* wait_time()���s */
            if(station != 0){
               printf("countdown failed\x0d\x0a");
               runlog(414);
            }
         }
         ping_byou += round_interval;
         if(p_data!=0) free((char *)p_data);   /* ����������� */
      }  /* for(station=0;station<total_st;++station) end */
      WDT(1);
      return(0);
}
