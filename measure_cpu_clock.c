//#define DEBUG
/* CPU clock freq is measured by GPS */
/* GPS 1Hz��PortE1�ɐڑ����Ă��� */
#include <mes2.h>
#include "SH7145.h"
#include "SH2MB.h"  /* SH2MB */
#include <macro.h>

/*****  ���̃t�@�C���ł̊֐��v���g�^�C�v�錾  *******************************/
int measure_cpu_clock(void);

/*****  �O���֐�,�O���ϐ��錾  **********************************************/
extern void serial_CSAC(int serial_CSAC_flag);        /* power_cntrl.c */
extern void time_information_log(int code,int value); /* log.c */

/*****  measure_cpu_clock  **************************************************/
/*                                                                          */
/* CSAC 1Hz(PE2)��1Hz�̗����オ��M�����g�p����cpu��clock���v������         */
/* 20MHz/16=1.5625MHz(T=0.6424us)�̃N���b�N���g�p����                       */
/* �ő�count0��GPGGA����M���A1Hz�̓������Ƃ�Ă��邱�Ƃ��m�F����           */
/****************************************************************************/
int measure_cpu_clock(void){
   int count;
   int i1,i2,bit0;

      serial_CSAC(1);
      bit0 = 0x0004;

      /* �������玞�ԍ��v�� */
      /* ���荞�ݑS���֎~ */
     // INT_DISABLE();

         asm("stc sr,r0");
         asm("or #0xf0,r0");
         asm("ldc  r0,sr");

      TCR_1  = 0x07;             /* 000 00 111     TCNT_2��overflow�ŃJ�E���g */          TCR_2  = 0x00;             /* 000 00 000     �����N���b�N�ŃJ�E���g */

      while((PEDRL & bit0)>0){;}   /* CSAC1Hz��Low�ɐ���܂ő҂� */
      TSTR |= 0x06;      /* 0x06 TCN1 & TCN2 count start */
      while((PEDRL & bit0)<1){;}   /* CSAC1Hz��High�ɂȂ�܂ő҂� */      
      TCNT_2 = 0;    
      TCNT_1 = 0;    
      while((PEDRL & bit0)>0){;}   /* CSAC1Hz��Low�ɐ���܂ő҂� */
      while((PEDRL & bit0)<1){;}   /* CSAC1Hz��High�ɂȂ�܂ő҂� */
      TSTR &= ~0x06;               /* ~0x06 TCN1 & TCN2 count stop */
      i2=TCNT_2;i1=TCNT_1;
      
      count=(i1<<16)+i2;         /* ���� ���ʂ��K�v */
      /* ���荞�ݑS������ */
      //INT_ENABLE();

         asm("stc sr,r0");
         asm("and #0x0f,r0");
         asm("ldc r0,sr");

      sleep(-1);  /* OS�؂�ւ��̂��ߕK�v */
      sleep(100);
      printf("system freq = %d\x0d\x0a",count);
      time_information_log(101,count); 
      return(count);
}
