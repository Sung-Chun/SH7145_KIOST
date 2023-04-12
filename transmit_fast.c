/* 20210507 kcat装置用 CSACの10MHz利用 (1HzのみCSACまたはGPSを使用) */
/* 時間計測にはtimer1,2を直列,32bitのcounterを構成、20MHzクロックをカウント */

//#define  DEBUG
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <mes2.h>
#include <macro.h>   /* INT_ENABLE(),INT_DISABLE() */
#include "SH7145.h"
#include "SH2MB.h"   /* SH2MB */

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int transmit(int st,int code_length,int code_repeat,int mod_num);

/* **** External function declaration ************* */
extern void WDT(int flag);

/* **** external(global) variable declaration ***** */
extern int  m_seq[16][128];
extern int  gps_operation;
extern short *mcode;

int transmit(int st,int code_length,int code_repeat,int mod_num){
   int i,j,m;
   register short data0,data1;
   register short *work;

      INT_DISABLE();       /* NMI以外の割り込みを禁止 */
      PADRL |=  0x0300;    /* Tr0,Tr1=1 transmit off(Low active) */
   #ifdef DEBUG
      int i1,i2,i3,count;
      TSTR &= ~0x06;       /* timer1,2を利用する timer1,2 stop */
      TCR_1  = 0x07;       /* 000 00 111   TCNT_2のoverflowでカウント */
      TCR_2  = 0x00;       /* 000 00 000   内部クロック(20MHz)でカウント */
      TCNT_1 = 0;
      TCNT_2 = 0;          /* TCNT_1,TCNT_2の順にclearすること */
   #endif

      switch(gps_operation){
         case 0:  /* CSACの1Hz使用 (pulse width=100usec) */
            while((PEDRL & 0x4)!=0){;}    /* Lowに成るまで待つ */
            while((PEDRL & 0x4)==0){;}    /* Highに成るまで待つ */
            break;
         case 1:  /* GPSの1Hz使用 */
            while((PEDRL & 0x2)!=0){;}    /* Lowに成るまで待つ */
            while((PEDRL & 0x2)==0){;}    /* Highに成るまで待つ */
            break;
      }

      /* 1Hzに同期してクロックを作る */
      TCNT_0=0x0000;TCNT_4=0x0000;  /* timer0,4 restart */
      /* ******** ここから音を発射する ***************** */
      #ifdef DEBUG
         TSTR |= 0x06;              /* counter start */
      #endif
      for(i=0;i<code_repeat;i++){
         work=mcode;
         for(j=0;j<code_length;j++){
            data0=*work;++work;data1=*work;++work;
            for(m=0;m<mod_num;m++){
               while((PBDR & 0x10)==0){;} /*上がりを待つ*/
               while((PBDR & 0x10)!=0){;} /*下がりを待つ*/
               PADRL =data0;
               while((PBDR & 0x10)==0){;} /*立上がりを待つ*/
               while((PBDR & 0x10)!=0){;} /*立下がりを待つ*/
               PADRL =data1;
            }
         }        /* for(j=0;j<code_length;j++) end */
      }           /* for(i=0;i<code_repeat;i++) end */
      while((PBDR & 0x10)!=0){;} /*立下がりを待つ*/
      PADRL |=  0x0300;
      #ifdef DEBUG
         TSTR &= ~0x06;             /* counter stop */
         i1=TCNT_1;i2=TCNT_2;
         count=(i1<<16)+i2;         /* 注意 括弧が必要 */
         i3=(int)(count/20);        /* system clock=20MHz */
         INT_ENABLE() ;             /* すべての割り込みを許可 */
         return(i3);
      #endif
      INT_ENABLE() ;                /* すべての割り込みを許可 */
      return(0);
}
