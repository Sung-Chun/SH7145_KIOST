/* 20210507 kcat���u�p CSAC��10MHz���p (1Hz�̂�CSAC�܂���GPS���g�p) */
/* ���Ԍv���ɂ�timer1,2�𒼗�,32bit��counter���\���A20MHz�N���b�N���J�E���g */

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

      INT_DISABLE();       /* NMI�ȊO�̊��荞�݂��֎~ */
      PADRL |=  0x0300;    /* Tr0,Tr1=1 transmit off(Low active) */
   #ifdef DEBUG
      int i1,i2,i3,count;
      TSTR &= ~0x06;       /* timer1,2�𗘗p���� timer1,2 stop */
      TCR_1  = 0x07;       /* 000 00 111   TCNT_2��overflow�ŃJ�E���g */
      TCR_2  = 0x00;       /* 000 00 000   �����N���b�N(20MHz)�ŃJ�E���g */
      TCNT_1 = 0;
      TCNT_2 = 0;          /* TCNT_1,TCNT_2�̏���clear���邱�� */
   #endif

      switch(gps_operation){
         case 0:  /* CSAC��1Hz�g�p (pulse width=100usec) */
            while((PEDRL & 0x4)!=0){;}    /* Low�ɐ���܂ő҂� */
            while((PEDRL & 0x4)==0){;}    /* High�ɐ���܂ő҂� */
            break;
         case 1:  /* GPS��1Hz�g�p */
            while((PEDRL & 0x2)!=0){;}    /* Low�ɐ���܂ő҂� */
            while((PEDRL & 0x2)==0){;}    /* High�ɐ���܂ő҂� */
            break;
      }

      /* 1Hz�ɓ������ăN���b�N����� */
      TCNT_0=0x0000;TCNT_4=0x0000;  /* timer0,4 restart */
      /* ******** �������特�𔭎˂��� ***************** */
      #ifdef DEBUG
         TSTR |= 0x06;              /* counter start */
      #endif
      for(i=0;i<code_repeat;i++){
         work=mcode;
         for(j=0;j<code_length;j++){
            data0=*work;++work;data1=*work;++work;
            for(m=0;m<mod_num;m++){
               while((PBDR & 0x10)==0){;} /*�オ���҂�*/
               while((PBDR & 0x10)!=0){;} /*�������҂�*/
               PADRL =data0;
               while((PBDR & 0x10)==0){;} /*���オ���҂�*/
               while((PBDR & 0x10)!=0){;} /*���������҂�*/
               PADRL =data1;
            }
         }        /* for(j=0;j<code_length;j++) end */
      }           /* for(i=0;i<code_repeat;i++) end */
      while((PBDR & 0x10)!=0){;} /*���������҂�*/
      PADRL |=  0x0300;
      #ifdef DEBUG
         TSTR &= ~0x06;             /* counter stop */
         i1=TCNT_1;i2=TCNT_2;
         count=(i1<<16)+i2;         /* ���� ���ʂ��K�v */
         i3=(int)(count/20);        /* system clock=20MHz */
         INT_ENABLE() ;             /* ���ׂĂ̊��荞�݂����� */
         return(i3);
      #endif
      INT_ENABLE() ;                /* ���ׂĂ̊��荞�݂����� */
      return(0);
}
