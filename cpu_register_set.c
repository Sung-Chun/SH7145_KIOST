/* 20220217 prepare a signal to clear PIC counter */
/* 20180826 Cat3�ł�CSAC serial enable�M����+5V(Vcc)�ő�p */
/* CSAC clock enable��PIC�ō���Ă��� */
#include <MES2.h>
#include "SH7145.h"
#include "SH2MB.h"   /* SH2MB */

/* pio16_1�̂W�r�b�g���o�͂ɐݒ� */
/* d0=GPS 1Hz pulse gate signal output
   d1=PIC counter clear (20220217 at Skypark hotel)
   d2=do not use
   d3=do not use
   d4=Transducer receive�؂�ւ�switch
   d5=RaspberryPi power control
   d6=shutdown/PIC clear
   d7=do not use

   CSAC_1Hz��PortE2�ɐڑ����Ă���
   GPS_1Hz��PortE1�ɐڑ����Ă��� */

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void cpu_register_set(void);
void WDT(int wdt_flag);

void cpu_register_set(void){
      pio_cntl=2;    /* pio16_0=input, pio16_1=output */
      pio16_1=0x03;  /* GPS 1HZ enable,alive=1,Tr_receive relay off */
     
      /* Pin Function Controller */
      /* PortA�ݒ�  MES default PACRL1=0x15aa PACRL2=0xa155 PAIORL=0x0000 */
      /* PortA�ݒ�  MES default PACRH=0x5004 PAIORH=0x0000 */
      
      PACRH &= ~0x00f0;    /* PA18,PA19����o�̓|�[�g�Ŏg�p */
      PACRL1=0x1550;       /* control register R0R1 0101 0101 0000 R(reserve 0) */
      PACRL2=0xa145;       /* pin function set 1010 00R1 R100 R1R1 R(reserve 0) */
      PAIORL |= 0x0304;    /* PA2,PA8,PA9 output */
      PAIORH |= 0x04;      /* PA18�͏o�� (Zhu not use) */
      PAIORH &= ~0x08;     /* PA19 Rubidium lock monitor input */
      /* PA15     PA14      PA13      PA12      PA11     PA10     */
      /* CK(out)  RD(out)   WRH(out)  WRL(out)  CS1(out) CS0(out) */
      /* PA9      PA8       PA7       PA6       PA5      PA4       */
      /* PA9(out) PA8(out)  CS3(out)  CS2(out)  PA5(in)  TXD1(out) */
      /* PA3      PA2       PA1        PA0       */
      /* RXD1(in) PA2(out)  TXD0(out)  RXD0(in)  */
      /* PA8(out),PA9(out) are TR0,TR1 */
      /* PA2(out) is RS422 output enable */
      PADRL |= 0x0304;  /* pa9(Tr1)=pa8(Tr0)=1 transmit off(Low active) */
                        /* pa2=1 GPS RS422 driver out enable */

      /* PortB�ݒ�  MES default PBCR1=0x000a PBCR2=0xaf55 PBIOR=0x0004 */
      PBCR1 &= ~0x0008;    /* RRRR 00RR RRRR 0010-  R(reserve 0) -(No change) */
      PBCR2 &= ~0x0ff0;    /* ---- ---- ---- R-R-  R(reserve 0) */
      PBIOR =   0x0204;    /* PB4=input, PB9=output, PB2=out */      

      /* PB9       PB5         PB4         PB3         PB2      */
      /* PB9(out)  IRQ3(in)    PB4(in)     PB3(in/out) PB2(out) */
     
      /* PB9(out) is Analog power control*/
      /* IRQ3/PB5(in) is interrupt signal for wake up by RTC */
      /* PB4(in)  is sampling freq clock input*/
      /* PB3,2��RTC��i2C�Ŏg�p���Ă��邪�Ai2C�d�l */
      /* PB3(in/out) is RTC  (SDA) */
      /* PB2(out) is RTC  (SCL) */
      
      /*PB5��Rubidiumu�N���b�N�����1Hz���͂Ɏg�p2010.7.1*/
      /*PE13��CPLD�̃V���N���M���Ƃ��ďo�͂ɕύX2010.7.1*/
      
//    PEDRL= 0x0000;
      /* PortE�ݒ�  MES default PECRL1=0x0022 PECRL2=0xaa00 PEIORL=0x0018 */
      PECRL1 = 0x5022;  /* Cats��TIOC4D,TIOC4C,TXD2�[�q�g�p�ݒ� */
      /* 0101 0000 0010 0010-- -- -- -- -- -- */
      PECRL2 = 0xaa01;  /* Cats��RXD2,TIOC0A,PE1,PE2�[�q�g�p�ݒ� */
      /* 1010 1010 0000 0001 */
      PEIORL &= ~0x0006; /* PE1,2����͂ɐݒ� */
      //PEIORL |=  0xc001; /* TIOC4C,TIOC4D,TIOCA0���o�� 11-- ---- --00 0001 */
      PEIORL |=  0xe001;/*PE13��CPLD�̃V���N���M���Ƃ��ďo�͂ɕύX2010.7.1*/
      /* PE15         PE14       PE13       PE12       PE11     */
      /* TIOC4D(out) TIOC4C(out) PE13(in)   PE12(in)   PE11(in) */
      PEDRL &= ~0x2000;
      /* PE10         PE9        PE8        PE7        PE6        */
      /* TXD2(SCI)    PE9(in)    SCK2(SCI)  RXD2(SCI)  SCK3(TAC��) */
      
      /* PE5          PE4        PE3        PE2        PE1      PE0         */
      /* TXD3(TAC��) RXD3(TAC��) PE3(TAC��) PE2(CSAC)  PE1(GPS) TIOC0A(out) */

      /* TIOC4D(out) is cos4kHz clock output */
      /* TIOC4C(out) is sin4kHz clock output */
      /* PE1(in) is 1Hz clock input from GPS */
      /* PE2(in) is 1Hz clock input form CSAC */
      /* PE3,4,5,6��TAC�Ђ�SD�������J�[�h�Ɋ���t���Ďg���Ă��� */
      /* TIOC0A(out) is sampling freq clock output*/
      IPRH &= ~0x0fff;   /* WDT�̗D�揇�ʂ�14�Ԗ�(1)�ɐݒ� */
      IPRH |= 0x1000;
      IPRG |= 0xf000;    /* AD�ϊ��̗D�揇�ʂ�1�Ԗ�(15)�ɐݒ� */
      }

void WDT(int wdt_flag){
   switch(wdt_flag){
      case 0:
         TCSR_W = 0xa518;  /* timer disable */
         break;
      case 1:
         TCSR_W = 0xa53b;  /* timer enable T=1.6ms */
         break;
   }
}
