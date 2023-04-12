/*Serial Communication resister*/
#define SMR0 (*(volatile unsigned char *)0xffff81a0)
#define BRR0 (*(volatile unsigned char *)0xffff81a1)
#define SCR0 (*(volatile unsigned char *)0xffff81a2)
#define TDR0 (*(volatile unsigned char *)0xffff81a3)
#define SSR0 (*(volatile unsigned char *)0xffff81a4)
#define RDR0 (*(volatile unsigned char *)0xffff81a5)
#define SDCR0 (*(volatile unsigned char *)0xffff81a6)
#define SMR1 (*(volatile unsigned char *)0xffff81b0)
#define BRR1 (*(volatile unsigned char *)0xffff81b1)
#define SCR1 (*(volatile unsigned char *)0xffff81b2)
#define TDR1 (*(volatile unsigned char *)0xffff81b3)
#define SSR1 (*(volatile unsigned char *)0xffff81b4)
#define RDR1 (*(volatile unsigned char *)0xffff81b5)
#define SDCR1 (*(volatile unsigned char *)0xffff81b6)
#define SMR2 (*(volatile unsigned char *)0xffff81c0)
#define BRR2 (*(volatile unsigned char *)0xffff81c1)
#define SCR2 (*(volatile unsigned char *)0xffff81c2)
#define TDR2 (*(volatile unsigned char *)0xffff81c3)
#define SSR2 (*(volatile unsigned char *)0xffff81c4)
#define RDR2 (*(volatile unsigned char *)0xffff81c5)
#define SDCR2 (*(volatile unsigned char *)0xffff81c6)

/*Timer Resister*/
#define TSTR (*(volatile unsigned char *)0xffff8240)     /*タイマスタートレジスタ*/
#define TSYR (*(volatile unsigned char *)0xffff8241)     /*タイマシンクロレジスタ*/
#define TOER (*(volatile unsigned char *)0xffff820a)     /*タイマアウトプットマスタイネーブルレジスタ*/
#define TOCR (*(volatile unsigned char *)0xffff820b)     /*タイマアウトプットコントロールレジスタ*/
#define TGCR (*(volatile unsigned char *)0xffff820d)     /*タイマゲートコントロールレジスタ*/
#define TCDR (*(volatile unsigned short *)0xffff8214)    /*タイマ周期データレジスタ*/
#define TDDR (*(volatile unsigned short *)0xffff8216)    /*タイマデッドタイムデータレジスタ*/
#define TCNTS (*(volatile unsigned short *)0xffff8220)   /*タイマサブカウンタ*/
#define TCBR (*(volatile unsigned short *)0xffff8222)    /*タイマ周期バッファレジスタ*/

#define TCR_0 (*(volatile unsigned char *)0xffff8260)    /* タイマコントロールレジスタ0*/
#define TMDR_0 (*(volatile unsigned char *)0xffff8261)   /* タイマモードレジスタ0*/
#define TIORH_0 (*(volatile unsigned char *)0xffff8262)  /* タイマI/OコントロールレジスタH0*/
#define TIORL_0 (*(volatile unsigned char *)0xffff8263)  /* タイマI/OコントロールレジスタL0*/
#define TIER_0 (*(volatile unsigned char *)0xffff8264)   /* タイマインターラプトイネーブルレジスタ0*/
#define TSR_0 (*(volatile unsigned char *)0xffff8265)    /* タイマステータスレジスタ0*/
#define TCNT_0 (*(volatile unsigned short *)0xffff8266)  /* タイマカウンタ0 */
#define TGRA_0 (*(volatile unsigned short *)0xffff8268)  /* タイマジェネラルレジスタA0*/
#define TGRB_0 (*(volatile unsigned short *)0xffff826a)  /* タイマジェネラルレジスタB0*/
#define TGRC_0 (*(volatile unsigned short *)0xffff826c)  /* タイマジェネラルレジスタC0*/
#define TGRD_0 (*(volatile unsigned short *)0xffff826e)  /* タイマジェネラルレジスタD0*/

#define TCR_1 (*(volatile unsigned char *)0xffff8280)    /*タイマコントロールレジスタ1*/
#define TMDR_1 (*(volatile unsigned char *)0xffff8281)   /*タイマモードレジスタ1*/
#define TIOR_1 (*(volatile unsigned char *)0xffff8282)   /*タイマI/Oコントロールレジスタ1*/
#define TIER_1 (*(volatile unsigned char *)0xffff8284)   /*タイマインターラプトイネーブルレジスタ1*/
#define TSR_1 (*(volatile unsigned char *)0xffff8285)    /*タイマステータスレジスタ1*/
#define TCNT_1 (*(volatile unsigned short *)0xffff8286)  /*タイマカウンタ1*/
#define TGRA_1 (*(volatile unsigned short *)0xffff8288)  /*タイマジェネラルレジスタA1*/
#define TGRB_1 (*(volatile unsigned short *)0xffff828a)  /*タイマジェネラルレジスタB1*/

#define TCR_2 (*(volatile unsigned char *)0xffff82A0)    /*タイマコントロールレジスタ2*/
#define TMDR_2 (*(volatile unsigned char *)0xffff82A1)   /*タイマモードレジスタ2*/
#define TIOR_2 (*(volatile unsigned char *)0xffff82A2)   /*タイマI/Oコントロールレジスタ2*/
#define TIER_2 (*(volatile unsigned char *)0xffff82A4)   /*タイマインターラプトイネーブルレジスタ2*/
#define TSR_2 (*(volatile unsigned char *)0xffff82A5)    /*タイマステータスレジスタ2*/
#define TCNT_2 (*(volatile unsigned short *)0xffff82A6)  /*タイマカウンタ2*/
#define TGRA_2 (*(volatile unsigned short *)0xffff82A8)  /*タイマジェネラルレジスタA2*/
#define TGRB_2 (*(volatile unsigned short *)0xffff82Aa)  /*タイマジェネラルレジスタB2*/

#define TCR_3 (*(volatile unsigned char *)0xffff8200)    /*タイマコントロールレジスタ3*/
#define TMDR_3 (*(volatile unsigned char *)0xffff8202)   /*タイマモードレジスタ3*/
#define TIORH_3 (*(volatile unsigned char *)0xffff8204)  /*タイマI/Oコントロールレジスタ3*/
#define TIORL_3 (*(volatile unsigned char *)0xffff8205)  /*タイマI/Oコントロールレジスタ3*/
#define TIER_3 (*(volatile unsigned char *)0xffff8208)   /*タイマインターラプトイネーブルレジスタ3*/
#define TSR_3 (*(volatile unsigned char *)0xffff822c)    /*タイマステータスレジスタ3*/
#define TCNT_3 (*(volatile unsigned short *)0xffff8210)  /*タイマカウンタ3*/
#define TGRA_3 (*(volatile unsigned short *)0xffff8218)  /*タイマジェネラルレジスタA3*/
#define TGRB_3 (*(volatile unsigned short *)0xffff821a)  /*タイマジェネラルレジスタB3*/

#define TCR_4 (*(volatile unsigned char *)0xffff8201)    /*タイマコントロールレジスタ4*/
#define TMDR_4 (*(volatile unsigned char *)0xffff8203)   /*タイマモードレジスタ4*/
#define TIORH_4 (*(volatile unsigned char *)0xffff8206)  /*タイマI/Oコントロールレジスタ4*/
#define TIORL_4 (*(volatile unsigned char *)0xffff8207)  /*タイマI/Oコントロールレジスタ4*/
#define TIER_4 (*(volatile unsigned char *)0xffff8209)   /*タイマインターラプトイネーブルレジスタ4*/
#define TSR_4 (*(volatile unsigned char *)0xffff822d)    /*タイマステータスレジスタ4*/
#define TCNT_4 (*(volatile unsigned short *)0xffff8212)  /*タイマカウンタ4*/
#define TGRA_4 (*(volatile unsigned short *)0xffff821c)  /*タイマジェネラルレジスタA4*/
#define TGRB_4 (*(volatile unsigned short *)0xffff821e)  /*タイマジェネラルレジスタB4*/
#define TGRC_4 (*(volatile unsigned short *)0xffff8228)  /*タイマジェネラルレジスタA4*/
#define TGRD_4 (*(volatile unsigned short *)0xffff822a)  /*タイマジェネラルレジスタB4*/

/*I/O ポートレジスタ*/
/*A ポート*/
#define PADRH (*(volatile unsigned short *)0xffff8380)   /*ポートＡ・データレジスタH*/
#define PADRL (*(volatile unsigned short *)0xffff8382)   /*ポートＡ・データレジスタH*/
#define PAIORH (*(volatile unsigned short *)0xffff8384)  /*ポートＡ・I/OレジスタH*/
#define PAIORL (*(volatile unsigned short *)0xffff8386)  /*ポートＡ・I/OレジスタL*/
#define PACRH (*(volatile unsigned short *)0xffff8388)   /*ポートＡ・コントロールレジスタH*/
#define PACRL1 (*(volatile unsigned short *)0xffff838c)  /*ポートＡ・コントロールレジスタL1*/
#define PACRL2 (*(volatile unsigned short *)0xffff838e)  /*ポートＡ・コントロールレジスタL2*/

#define PBDR (*(volatile unsigned short *)0xffff8390)    /*ポートB・データレジスタ*/
#define PBIOR (*(volatile unsigned short *)0xffff8394)   /*ポートB・I/Oレジスタ*/
#define PBCR1 (*(volatile unsigned short *)0xffff8398)   /*ポートB・コントロールレジスタ*/
#define PBCR2 (*(volatile unsigned short *)0xffff839a)   /*ポートB・コントロールレジスタ*/


#define PEDRL (*(volatile unsigned short *)0xffff83b0)   /*ポートE・データレジスタH*/
#define PECRL1 (*(volatile unsigned short *)0xffff83b8)  
#define PECRL2 (*(volatile unsigned short *)0xffff83ba)  
#define PEIORL (*(volatile unsigned short *)0xffff83b4)  /*ポートE・レジスタH*/
#define PFDR (*(volatile unsigned char *)0xffff83b3)     /*ポートE・データレジスタH*/

/*AD converter*/
#define ADDR0 (*(volatile unsigned short *)0xffff8420)   
#define ADDR1 (*(volatile unsigned short *)0xffff8422)
#define ADDR2 (*(volatile unsigned short *)0xffff8424)
#define ADDR3 (*(volatile unsigned short *)0xffff8426)
#define ADDR4 (*(volatile unsigned short *)0xffff8428)
#define ADDR5 (*(volatile unsigned short *)0xffff842a)
#define ADDR6 (*(volatile unsigned short *)0xffff842c)
#define ADDR7 (*(volatile unsigned short *)0xffff842e)
#define ADCSR_0 (*(volatile unsigned char *)0xffff8480)  
#define ADCSR_1 (*(volatile unsigned char *)0xffff8481)  
#define ADCR_0 (*(volatile unsigned char *)0xffff8488)   
#define ADCR_1 (*(volatile unsigned char *)0xffff8489)
#define ADTSR (*(volatile unsigned char *)0xffff87F4) 
   
/* 割り込み優先度 */
#define IPRA (*(volatile unsigned short *)0xffff8348) 
#define IPRB (*(volatile unsigned short *)0xffff834a) 
#define IPRC (*(volatile unsigned short *)0xffff834c) 
#define IPRD (*(volatile unsigned short *)0xffff834e) 
#define IPRE (*(volatile unsigned short *)0xffff8350) 
#define IPRF (*(volatile unsigned short *)0xffff8352)    /* WDT */ 
#define IPRG (*(volatile unsigned short *)0xffff8354) 
#define IPRH (*(volatile unsigned short *)0xffff8356)    /* SCI0 */ 
#define IPRI (*(volatile unsigned short *)0xffff835c)    /* SCI2 */

/*WDT 関連*/
#define TCSR (*(volatile unsigned char *)0xffff8610)
#define TCSR_W (*(volatile unsigned short *)0xffff8610)
#define TCNT (*(volatile unsigned char *)0xffff8611)
#define RSTCSR_W (*(volatile unsigned short *)0xffff8612)
#define RSTCSR (*(volatile unsigned char *)0xffff8613)

#define ICR1 (*(volatile unsigned short *)0xffff8358)
#define ICR2 (*(volatile unsigned short *)0xffff8366)

/* Standby */
#define SBYCR (*(volatile unsigned char *)0xffff8614)
#define SYSCR (*(volatile unsigned char *)0xffff8618)
#define MSTCR1 (*(volatile unsigned short *)0xffff861c)
#define MSTCR2 (*(volatile unsigned short *)0xffff861e)
