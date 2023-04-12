/* 2013 11.25 speed upのための工夫をした */
/* 出力データをあらかじめheap領域に格納しpointer変数をregister変数とした */
/* GPSのみ使用,CPLDで作ったクロックは使わない */
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <mes2.h>
#include <macro.h>
#include "SH7145.h"
#include "SH2MB.h"  /* SH2MB */

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void make_tr_data(int st,int code_length);

/* **** external(global) variable declaration ***** */
extern int m_seq[16][128];
extern short *mcode;

void make_tr_data(int st,int code_length){
   int j,k,n,s,total_byte;
   int   out_data0,out_data1,out_data2;
   register short *work;
   
      out_data0=(PADRL &= ~0x0300);
      out_data1=(out_data0 | 0x0100);/*---- --01 ---- ----*/
      out_data2=(out_data0 | 0x0200);/*---- --10 ---- ----*/
      PADRL |=  0x0300;   /* Tr0,Tr1=1 transmit off */

      total_byte=code_length*4+10;     /* 余分に取っておく */
      mcode=(short *)malloc(total_byte);
      memset(mcode, 0, total_byte);
         
      s=(int)((code_length+1)/32);
      work=mcode;
      for(j=0;j<s;j++){
         for(k=0;k<32;k++){
            if(k==31 && j==(s-1)) continue;/* 最後の余分なdigitを省く */
            n=m_seq[st][j]>>k;
            n &= 1;
            if(n==1){
               *work=out_data1;++work;
               *work=out_data2;++work;
            }
            else{
               *work=out_data2;++work;
               *work=out_data1;++work;
            }
         }
      }
}
