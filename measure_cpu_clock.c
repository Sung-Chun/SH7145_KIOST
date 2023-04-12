//#define DEBUG
/* CPU clock freq is measured by GPS */
/* GPS 1HzはPortE1に接続してある */
#include <mes2.h>
#include "SH7145.h"
#include "SH2MB.h"  /* SH2MB */
#include <macro.h>

/*****  このファイルでの関数プロトタイプ宣言  *******************************/
int measure_cpu_clock(void);

/*****  外部関数,外部変数宣言  **********************************************/
extern void serial_CSAC(int serial_CSAC_flag);        /* power_cntrl.c */
extern void time_information_log(int code,int value); /* log.c */

/*****  measure_cpu_clock  **************************************************/
/*                                                                          */
/* CSAC 1Hz(PE2)の1Hzの立ち上がり信号を使用してcpuのclockを計測する         */
/* 20MHz/16=1.5625MHz(T=0.6424us)のクロックを使用する                       */
/* 最大count0回GPGGAを受信し、1Hzの同期がとれていることを確認する           */
/****************************************************************************/
int measure_cpu_clock(void){
   int count;
   int i1,i2,bit0;

      serial_CSAC(1);
      bit0 = 0x0004;

      /* ここから時間差計測 */
      /* 割り込み全部禁止 */
     // INT_DISABLE();

         asm("stc sr,r0");
         asm("or #0xf0,r0");
         asm("ldc  r0,sr");

      TCR_1  = 0x07;             /* 000 00 111     TCNT_2のoverflowでカウント */          TCR_2  = 0x00;             /* 000 00 000     内部クロックでカウント */

      while((PEDRL & bit0)>0){;}   /* CSAC1HzがLowに成るまで待つ */
      TSTR |= 0x06;      /* 0x06 TCN1 & TCN2 count start */
      while((PEDRL & bit0)<1){;}   /* CSAC1HzがHighになるまで待つ */      
      TCNT_2 = 0;    
      TCNT_1 = 0;    
      while((PEDRL & bit0)>0){;}   /* CSAC1HzがLowに成るまで待つ */
      while((PEDRL & bit0)<1){;}   /* CSAC1HzがHighになるまで待つ */
      TSTR &= ~0x06;               /* ~0x06 TCN1 & TCN2 count stop */
      i2=TCNT_2;i1=TCNT_1;
      
      count=(i1<<16)+i2;         /* 注意 括弧が必要 */
      /* 割り込み全部許可 */
      //INT_ENABLE();

         asm("stc sr,r0");
         asm("and #0x0f,r0");
         asm("ldc r0,sr");

      sleep(-1);  /* OS切り替えのため必要 */
      sleep(100);
      printf("system freq = %d\x0d\x0a",count);
      time_information_log(101,count); 
      return(count);
}
