/* 20180810 Cat3 mainboard */
/* CSAC用の57600bpsを忘れるな！ */
/* 2012 4.23 CPU=10MHz  Max BaudRate=57600BPS */
/* SCI2(CSAC),SCI0(ubloxGPS) 割り込み受信でbuffer内に納めるprogram  */
/* SCI1はsystemで使用(console) */
/* 配列変数の関係でSCI0=port0,SCI2=port1と呼ぶ */
/* 使用する前に各ポートの信号線の設定cpu_register_set()が必要である */
/* ファイルの受信はHypertermから、一気に送られるのでファイルの大きさのバッファが必要であった */
/* cats.cfgの大きさが約2kB弱あるのでバッファーの大きさは2kB必要であった */

   /* interruput vector setting が必要 
      set_handler(128,eri_0 );
      set_handler(129,rxi_0 );
      set_handler(168,eri_2 );
      set_handler(169,rxi_2 ); */


#include <mes2.h>
#include "SH7145.h"
#define BUFLEN 2048
  
extern void WDT(int flag);

void  sci_Open(int port,int speed);
void  sci_Stop(int port);
void  sci_Restart(int port);
int   serRecv(int port);
int   serRecvw(int port,int time);
int   serRecvs(int port,char *s,char charactor,int n,int msec);
void  serSend(int port,char ch);
void  serPuts(int port,char *str);
int   serNrd(int port);
void  serClr(int port);
void  OS_terminal_open(void);

/* MES2.4R4 */
void  eri_0(void) __attribute__((interrupt_handler));
void  rxi_0(void) __attribute__((interrupt_handler));
void  eri_2(void) __attribute__((interrupt_handler));
void  rxi_2(void) __attribute__((interrupt_handler));

static unsigned char ibuf[2][BUFLEN];        /* 入力バッファ   */
static unsigned char *ibufp1[2], *ibufp2[2]; /* in,out pointer     */
static unsigned char *ibufe[2];              /* end of ibuf[]  */
static int     nrecv[2];                     /* # of data available */
unsigned char transfer_bps(int bps);

void sci_Open(int port,int bps){
   unsigned char baud_rate_reg;
   volatile int i;   
      baud_rate_reg=transfer_bps(bps);
      switch(port){
         case 0:
            SDCR0= 0xf2;
            SCR0 = 0x00;            /* RIE,Te,Re=0 disable */
            SMR0 = 0x00;
            BRR0 = baud_rate_reg;            
            break;
         case 1:
            SDCR2= 0xf2;
            SCR2 = 0x00;            /* RIE,Te,Re=0 */
            SMR2 = 0x00;
            BRR2 = baud_rate_reg;
            break;
         }
      for(i=0;i<30000;++i);         /* wait */
      ibufp1[port]=ibufp2[port]=ibuf[port];  /* memory addressを格納 */
      ibufe[port]=ibuf[port] + BUFLEN;
      nrecv[port]=0;
      
      switch(port){
         case 0:
            SCR0 = 0x70;      /* RIE,Te,Re=1 enable */
            break;
         case 1:
            SCR2 = 0x70;      /* RIE,Te,Re=1 enable */
            break;
      }
      
      IPRF=(unsigned short)((IPRF & 0xff0f) | (14<<4)) ;/* SCI0 IPRF7-4 <= 14 */   
      IPRI=(unsigned short)((IPRI & 0x0fff) | (15<<12));/* SCI2 IPRI15-12 <= 15 */
      IPRH=(unsigned short)((IPRH & 0x0fff) | (1 << 12));/* WDT IPRH15-12 <= 1 */
}
         
unsigned char transfer_bps(int bps){
   unsigned short brr;
      switch(bps){
         case   4800:brr=129;break; /*  4800 bps */
         case   9600:brr=64;break;  /*  9600 bps */
         case  19200:brr=32;break;  /* 19200 bps */
         case  38400:brr=15;break;  /* 38400 bps */
         case  57600:brr=10;break; 
//         case 115200:brr=6; break;/* cpu clock=10MHzでは115200 bpsは無理 */
         default:   brr=15;
         }
      return((unsigned char)brr);
}

void sci_Stop(int port){
      switch(port){
         case 0:
            SCR0 = 0x00;      /* RIE,Te,Re=0 disable */
            break;
         case 1:
            SCR2 = 0x00;      /* RIE,Te,Re=0 disable */
            break;
      }
}      
         
void sci_Restart(int port){
      serClr(port);  
      switch(port){
         case 0:
            SCR0 = 0x70;      /* RIE,Te,Re=1 enable */
            break;
         case 1:
            SCR2 = 0x70;      /* RIE,Te,Re=1 enable */
            break;
      }
}  
void serClr(int port){
      ibufp1[port]=ibufp2[port]=ibuf[port];
      ibufe[port]=ibuf[port]+BUFLEN;
      nrecv[port]=0;
}

int serRecv(int port){
   unsigned char ch;
   int r;

      if (nrecv[port]>0) {
         ch = *ibufp2[port];
         r=(unsigned int)ch;
         if (++ibufp2[port]==ibufe[port]) ibufp2[port]= ibuf[port];
         switch(port){
            case 0:
               SCR0 = 0x30;      /* RIE=0,Te,Re=1 disable */
               nrecv[0]--;
               SCR0 = 0x70;      /* RIE,Te,Re=1 enable */
               break;
            case 1:
               SCR2 = 0x30;      /* RIE=0,Te,Re=1 disable */
               nrecv[1]--;
               SCR2 = 0x70;      /* RIE,Te,Re=1 enable */
               break;
         }
      }
      else r = -1;
      return r;
}
      
int serRecvw(int port,int msec){ /* msec unit is 1msec */
   int   c,i;
   volatile int k;
   
      c = serRecv(port);
      if(c!=-1) return(c);
      for(i=0;i<msec;++i){    /* 時間待ち */
         for(k=0;k<1250;++k); /* 1msec wait */
         c = serRecv(port);
         if(c != -1){
            return(c);
         }
      }
      return(-1);
}      
/* 1byteでもmsec待って入力がなければ-1で戻る(n*msec待つことはしない) */
int serRecvs(int port,char *s,char e,int n,int msec){
   int   i, c;
   char  j;
   
      WDT(0);
      for (i = 0;i<n;++i ) {
         if ((c = serRecvw(port,msec)) == -1){
            WDT(1);
            return(-1);
         }
         j=(char)c;
         *s=j;
         s++;
         if (j ==e) break;   /* 文字コードがeと等しければ終わる */
      }
      if (i < n) *s = '\0';
      WDT(1);      
      return(i+1);
}
      
int serNrd(int port){
      return nrecv[port];
}  

void serSend(int port,char ch){
   char flag;
      switch(port){
         case 0:
            jp0:
               flag=SSR0;
               if((flag & 0x80)==0) goto jp0;
               TDR0 =ch;
               SSR0 &= 0x7f;
            break;
         case 1:
            jp1:
               flag=SSR2;
               if((flag & 0x80)==0) goto jp1;
               TDR2 =ch;
               SSR2 &= 0x7f;
            break;
         }
}
  
void serPuts(int port,char *str){
   char flag;
      while(*str){
         switch(port){
            case 0:
               jp2:
                  flag=SSR0;
                  if((flag & 0x80)==0) goto jp2;
                  TDR0 =*str++;
                  SSR0 &=0x7f;
                  break;
            case 1:
               jp3:
                  flag=SSR2;
                  if((flag & 0x80)==0) goto jp3;
                  TDR2 =*str++;
                  SSR2 &=0x7f;
                  break;
            }     /* switch end */
         }        /* while end */   
}

void OS_terminal_open(void){   /* OS terminal用 Baud rate=115.2kbpsに固定している */
      SDCR1= 0xf2;             /* after sleep SCI port must rearrange */
      SMR1 = 0;
      BRR1 = 15;  /* 38400bps */
      SCR1 = 0x30;
      SSR1 = 0x84;
}
             
/* *******  interrupt handler   ********* */
//#pragma interrupt
void eri_0(void){    /* (eri0(vect=128)) */
      SSR0 &= 0xc7;  /* clear error flag */
}

//#pragma interrupt
void rxi_0(void){    /* (rxi0(vect=129)) */
      *ibufp1[0] =RDR0; /* ここで文字が外部から入力される */
      if (++ibufp1[0]== ibufe[0]) ibufp1[0]= ibuf[0];/* ring buffer構造 */
      nrecv[0]++;
      SSR0 &= 0xbf;  /* clear RDRF of SSR */
}
//#pragma interrupt
void eri_2(void){    /* (eri2(vect=168)) */
      SSR2 &= 0xc7;  /* clear error flag */
}

//#pragma interrupt
void rxi_2(void){    /* (rxi0(vect=169)) */
      *ibufp1[1] =RDR2; /* ここで文字が外部から入力される */
      if (++ibufp1[1]== ibufe[1]) ibufp1[1]= ibuf[1];/* ring buffer構造 */
      nrecv[1]++;
      SSR2 &= 0xbf;  /* clear RDRF of SSR */
}
