/* 内部クロック(20MHz)を用いて望みに近い周波数のクロックを作るprogram */
/* 引数0:MTUの設定無し 戻り値:Hz,clockの発振を開始しない */
/* 引数2:MTUの設定無し 戻り値:Hz*1000,clockの発振を開始しない */
/* 引数1:MTUの設定を行う 戻り値:Hz,clockの発振を開始する */

/* 出力はT-SH2MのCN3のコネクタのTIOC0_A(PE0)ピンから得る */
/* counter startがこの関数に入れてある */

/* Catsでの使用に関して説明
SH7145(SH2)はMTUとしてchannel_0,1,2,3,4の5channel内蔵している。
This program is using channel_0, channel_1, channel_2 and channel_4.
channel_1 and channel_2 are used by stop watch
channel_0 is used by synchronizing transmit data and D-FF trigger
channel_4 is used by quadrature demodulation and A/D trigger

それぞれのchannelはgeneral registerを複数持っている。
基本的に出力端子はこれらのgeneral regに付随して同じ数だけ存在する。
チャンネル4を例に取るとチャンネル4には4個のgeneral regがあり、出力端子は4個ある。
出力端子に望みの周波数のクロックを出力させるには、内部クロックを
チャンネル4のカウンタでカウントアップしgeneral regに設定した値と一致すれば
カウンタの値を0にクリアし同時にそのgeneral regに属する出力端子の信号を
トグル（分周）させることで得ることができる。各出力端子の信号は各々の
general regとの一致で出力をトグルできるが、カウンタをクリヤできるのは
4個のgeneral regのなかで1個である。
cos,sinと90度位相をずらしたクロックは次のようにして得ている。
TGRC_4のregでカウンタのクリヤとTIOC4C端子の出力をトグルさせ,
TGRD_4のregを使ってTIOC4D端子の出力をトグルさせることで得ている
*/

#include <mes2.h>
#include "SH7145.h"

/* Initial values of registers used here
   MSTCR2=c0f0
   PECRL2=aa00
   PEIORL=18
   TCR_0=0
   TIORH_0=0
   TMDR_0=c0
   */

/* **** Function prototype declaration ************ */
int clock_pulse_generate(int freq,int flag);

/* **** external(global) variable declaration ******* */
extern int richtig_freq1000;
int clock_pulse_generate(int freq,int flag){
    int n,true_freq;
    int base_freq;
    
      TSTR= 0x00;    /* counter stop */
      base_freq=20000000;  /* 20MHz */
      n=(int)(base_freq/(2*freq));   /* Toggleさせるので２倍 */
      true_freq=(int)(base_freq/(2*n));
      richtig_freq1000=(int)((double)base_freq*1000.0/(2.0*(double)n));
      if(flag==0){
         TSTR=0x00;return(true_freq);
         }
      if(flag==2){
         TSTR=0x00;return(richtig_freq1000);
         }
      /* TGRA_0のコンペアマッチでTCNT_0クリア・ 両端・  内部クロックカウント */
      TCR_0 = 0x38;           /* 001 11 000 */
      /* TGRC_4のコンペアマッチでTCNT_4クリア・ 両端・  内部クロックカウント */
      TCR_4 = 0xa8;           /* 101 11 000 */
    
      /* 入出力設定 */

      /* TIOC0A端子 コンペアマッチトグル出力 */
      TIORH_0 = 0x03;         /* 0000 0011 */
      /* TIOC4C,TIOC4D,端子 コンペアマッチトグル出力 */       
      TIORH_4 = 0x00;         /* 0000 0000 */
      TIORL_4 = 0x33;         /* 0011 0011 */

      /* counter 0と4を利用する */
      /* TIOC4C,TIOC4D,端子 出力許可 */
      TOER |= 0xf0;
      TGRA_0 =  n-1;      /* for sampling clock */
      TGRA_4  = 2*n-1;    /* TGRA_4を使用するのはA/D変換の起動に使うためである */
      TGRC_4  = 2*n-1;    /* for cos            */
      TGRD_4  = n;        /* for sin            */
      TSTR= 0x81;         /* カウンタスタート 10** *001 */
      return(true_freq);
   }
