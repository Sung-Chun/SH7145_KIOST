/* �����ɉ��g�𔭎˂���A���g�̑��M��Ɏ�M���s�� */
/* transmit()�֐��̈��������̃v���O�����ƈقȂ钍�� */
/* ���g���M�ォ��AD�ϊ��J�n�܂ł̃J�E���g�������߂� */
#define DEBUG
#include <string.h>     /* memset() */
#include <stdlib.h>
#include <mes2.h>
#include "SH7145.h"
/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int concurrent(int ping_byou);

/* **** External function declaration ************* */
extern int  wait_time(int wait_byou);
extern void transmit(int my_st,int code_len,int times,int mod_num);
extern void receive(int ad_wait_count,int ad_convert_pair_no);
extern int  save_raw_data(int ping_byou);
extern void runlog(int); 
extern void WDT(int flag);
extern int read_rtc_clock(void);

/* **** external(global) variable declaration ***** */
extern char station_name[16][20];
extern int
      my_station,
      ad_start_count,
      ad_wait_count,
      ad_convert_pair_no,
      code_length,
      mod_num,             /* �ϒ����x(wave no/digit) */
      code_repeat,
      receive_mode,
      one_period_pair;     /* mod_num*2*code_length (experiment_set.c) */
extern short *p_data;

int concurrent(int ping_byou){
   int ret;

      if(receive_mode==1){
         memset(p_data,0,one_period_pair*4);
      }

      printf("***** Countdown *****\x0d\x0a");
      /* ***** wait_time()�ŉ��g���ˎ����܂ő҂�******* */
      ret=wait_time(ping_byou);
      if(ret != 0) return(-1);
      /* ********���g���M�܂��͎�M�ɓ��� ************* */
      transmit(my_station,code_length,code_repeat,mod_num);
      WDT(0);
      receive(ad_wait_count,ad_convert_pair_no);
      WDT(1);
      save_raw_data(ping_byou);
      printf("save raw data end\x0d\x0a");      
   return(0);
}
