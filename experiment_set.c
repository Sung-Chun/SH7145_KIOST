/* 20220906 for KIOST */
//#define   DEBUG
/* Read cfg file and read M_code */
/* return cord                         */
/*      0: Normal return               */
/*     -1: bad configration            */
/*     -2: modification code error     */
/*     -3: Too long distance           */

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <MES2.h>

/* **** Function prototype declaration ************ */
int  experiment_set(char *cfg_file);
void display(void);

/* **** External function declaration ************* */
extern int  read_cfg_file(char *cfg_file);
extern int  clock_pulse_generate(int freq,int flag);
extern int  calc_distance(void);
extern int  arrival_order(void);
extern int  modification_code_set(int total_st,char *code_name);
extern void make_tr_data(int st,int code_length);
extern int  prepare_receive_concurrent(void);
extern void runlog(int);

/* **** external(global) variable declaration ******* */
extern char
      my_station_name[],
      station_name[16][20],
      code_name[];

extern int
      total_st,
      my_station,
      wakeup_interval,
      round_interval,
      extra_record_width,
      mod_num,
      code_repeat,
      code_no[],           /* given at read_cfg */
      code_length,         /* modification_set.cで計算されている */
      distance[],          /* my_stationからの距離(m) */
      tr_time[],           /* msec */
      order[];

/* The following variables are determined in this function */
extern int
      one_period_pair,     /* mod_num*2*code_length (experiment_set.c) */
      sound_width_count;

int experiment_set(char *cfg_file){
   int   ret,mseq_order;  

      ret=read_cfg_file(cfg_file);
      if(ret != 0){         /* 正常にcfgファイルを読み込んだら0を返す  */
         printf("The cfg file is bad(error no)=%d\x0d\x0a",ret);
         return(-1);
      }
      printf("***** read_cfg_file finished *****\x0d\x0a");
      printf("\x0d\x0a");
      ret=calc_distance();        /* calc_distance.c */
      if(ret != 0){
         printf("Please Change the Configuration!\x0d\x0a");
         runlog(231);return(ret);   /* 距離が遠すぎる場合は-3で戻る*/
      }
      arrival_order();

      mseq_order=modification_code_set(total_st,code_name);/* M系列コードの確認 */
      if(mseq_order==-1){
         printf("cannot open code file %s\x0d\x0a",code_name);
         return(-2);
      }
      make_tr_data(my_station,code_length);
      one_period_pair=mod_num*2*code_length;
      sound_width_count=code_repeat*one_period_pair;
      display();
      if(round_interval==0){
         ret=prepare_receive_concurrent();
         if(ret==-1){
            printf("prepare receive of concurrent mode is failure\x0d\x0a");
            return(-1);
         }
      }
      printf("****** Experiment prepare finished ****** \x0d\x0a");
      return(0);
}

void display(void){
   int   i,k;

      printf("\x0d\x0a");
      printf("**************************************\x0d\x0a");
      printf("My_station=%s code=%d total_st=%2d\x0d\x0a",
         my_station_name,code_no[my_station],total_st);

      printf("code name=%s code_digit=%d mod no=%d code repeat=%d\x0d\x0a",
         code_name,code_length,mod_num,code_repeat);

      printf("wakeup_int(min)=%d round_interval(sec)=%d extra_time(msec)=%d\x0d\x0a",
               (int)(wakeup_interval/60),round_interval,extra_record_width);

      /* 早く音が到達する順に表示させる */
      for(i=1;i<total_st;++i){
         k=order[i];
         printf("%2d st=%s code=%2d distance(m)=%5d Arrival(msec)=%5d\x0d\x0a",
            i,station_name[k],code_no[k],distance[k],tr_time[k]);
      }
      printf("\x0d\x0a");
}
