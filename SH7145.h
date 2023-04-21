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
#define TSTR (*(volatile unsigned char *)0xffff8240)     /*�^�C�}�X�^�[�g���W�X�^*/
#define TSYR (*(volatile unsigned char *)0xffff8241)     /*�^�C�}�V���N�����W�X�^*/
#define TOER (*(volatile unsigned char *)0xffff820a)     /*�^�C�}�A�E�g�v�b�g�}�X�^�C�l�[�u�����W�X�^*/
#define TOCR (*(volatile unsigned char *)0xffff820b)     /*�^�C�}�A�E�g�v�b�g�R���g���[�����W�X�^*/
#define TGCR (*(volatile unsigned char *)0xffff820d)     /*�^�C�}�Q�[�g�R���g���[�����W�X�^*/
#define TCDR (*(volatile unsigned short *)0xffff8214)    /*�^�C�}�����f�[�^���W�X�^*/
#define TDDR (*(volatile unsigned short *)0xffff8216)    /*�^�C�}�f�b�h�^�C���f�[�^���W�X�^*/
#define TCNTS (*(volatile unsigned short *)0xffff8220)   /*�^�C�}�T�u�J�E���^*/
#define TCBR (*(volatile unsigned short *)0xffff8222)    /*�^�C�}�����o�b�t�@���W�X�^*/

#define TCR_0 (*(volatile unsigned char *)0xffff8260)    /* �^�C�}�R���g���[�����W�X�^0*/
#define TMDR_0 (*(volatile unsigned char *)0xffff8261)   /* �^�C�}���[�h���W�X�^0*/
#define TIORH_0 (*(volatile unsigned char *)0xffff8262)  /* �^�C�}I/O�R���g���[�����W�X�^H0*/
#define TIORL_0 (*(volatile unsigned char *)0xffff8263)  /* �^�C�}I/O�R���g���[�����W�X�^L0*/
#define TIER_0 (*(volatile unsigned char *)0xffff8264)   /* �^�C�}�C���^�[���v�g�C�l�[�u�����W�X�^0*/
#define TSR_0 (*(volatile unsigned char *)0xffff8265)    /* �^�C�}�X�e�[�^�X���W�X�^0*/
#define TCNT_0 (*(volatile unsigned short *)0xffff8266)  /* �^�C�}�J�E���^0 */
#define TGRA_0 (*(volatile unsigned short *)0xffff8268)  /* �^�C�}�W�F�l�������W�X�^A0*/
#define TGRB_0 (*(volatile unsigned short *)0xffff826a)  /* �^�C�}�W�F�l�������W�X�^B0*/
#define TGRC_0 (*(volatile unsigned short *)0xffff826c)  /* �^�C�}�W�F�l�������W�X�^C0*/
#define TGRD_0 (*(volatile unsigned short *)0xffff826e)  /* �^�C�}�W�F�l�������W�X�^D0*/

#define TCR_1 (*(volatile unsigned char *)0xffff8280)    /*�^�C�}�R���g���[�����W�X�^1*/
#define TMDR_1 (*(volatile unsigned char *)0xffff8281)   /*�^�C�}���[�h���W�X�^1*/
#define TIOR_1 (*(volatile unsigned char *)0xffff8282)   /*�^�C�}I/O�R���g���[�����W�X�^1*/
#define TIER_1 (*(volatile unsigned char *)0xffff8284)   /*�^�C�}�C���^�[���v�g�C�l�[�u�����W�X�^1*/
#define TSR_1 (*(volatile unsigned char *)0xffff8285)    /*�^�C�}�X�e�[�^�X���W�X�^1*/
#define TCNT_1 (*(volatile unsigned short *)0xffff8286)  /*�^�C�}�J�E���^1*/
#define TGRA_1 (*(volatile unsigned short *)0xffff8288)  /*�^�C�}�W�F�l�������W�X�^A1*/
#define TGRB_1 (*(volatile unsigned short *)0xffff828a)  /*�^�C�}�W�F�l�������W�X�^B1*/

#define TCR_2 (*(volatile unsigned char *)0xffff82A0)    /*�^�C�}�R���g���[�����W�X�^2*/
#define TMDR_2 (*(volatile unsigned char *)0xffff82A1)   /*�^�C�}���[�h���W�X�^2*/
#define TIOR_2 (*(volatile unsigned char *)0xffff82A2)   /*�^�C�}I/O�R���g���[�����W�X�^2*/
#define TIER_2 (*(volatile unsigned char *)0xffff82A4)   /*�^�C�}�C���^�[���v�g�C�l�[�u�����W�X�^2*/
#define TSR_2 (*(volatile unsigned char *)0xffff82A5)    /*�^�C�}�X�e�[�^�X���W�X�^2*/
#define TCNT_2 (*(volatile unsigned short *)0xffff82A6)  /*�^�C�}�J�E���^2*/
#define TGRA_2 (*(volatile unsigned short *)0xffff82A8)  /*�^�C�}�W�F�l�������W�X�^A2*/
#define TGRB_2 (*(volatile unsigned short *)0xffff82Aa)  /*�^�C�}�W�F�l�������W�X�^B2*/

#define TCR_3 (*(volatile unsigned char *)0xffff8200)    /*�^�C�}�R���g���[�����W�X�^3*/
#define TMDR_3 (*(volatile unsigned char *)0xffff8202)   /*�^�C�}���[�h���W�X�^3*/
#define TIORH_3 (*(volatile unsigned char *)0xffff8204)  /*�^�C�}I/O�R���g���[�����W�X�^3*/
#define TIORL_3 (*(volatile unsigned char *)0xffff8205)  /*�^�C�}I/O�R���g���[�����W�X�^3*/
#define TIER_3 (*(volatile unsigned char *)0xffff8208)   /*�^�C�}�C���^�[���v�g�C�l�[�u�����W�X�^3*/
#define TSR_3 (*(volatile unsigned char *)0xffff822c)    /*�^�C�}�X�e�[�^�X���W�X�^3*/
#define TCNT_3 (*(volatile unsigned short *)0xffff8210)  /*�^�C�}�J�E���^3*/
#define TGRA_3 (*(volatile unsigned short *)0xffff8218)  /*�^�C�}�W�F�l�������W�X�^A3*/
#define TGRB_3 (*(volatile unsigned short *)0xffff821a)  /*�^�C�}�W�F�l�������W�X�^B3*/

#define TCR_4 (*(volatile unsigned char *)0xffff8201)    /*�^�C�}�R���g���[�����W�X�^4*/
#define TMDR_4 (*(volatile unsigned char *)0xffff8203)   /*�^�C�}���[�h���W�X�^4*/
#define TIORH_4 (*(volatile unsigned char *)0xffff8206)  /*�^�C�}I/O�R���g���[�����W�X�^4*/
#define TIORL_4 (*(volatile unsigned char *)0xffff8207)  /*�^�C�}I/O�R���g���[�����W�X�^4*/
#define TIER_4 (*(volatile unsigned char *)0xffff8209)   /*�^�C�}�C���^�[���v�g�C�l�[�u�����W�X�^4*/
#define TSR_4 (*(volatile unsigned char *)0xffff822d)    /*�^�C�}�X�e�[�^�X���W�X�^4*/
#define TCNT_4 (*(volatile unsigned short *)0xffff8212)  /*�^�C�}�J�E���^4*/
#define TGRA_4 (*(volatile unsigned short *)0xffff821c)  /*�^�C�}�W�F�l�������W�X�^A4*/
#define TGRB_4 (*(volatile unsigned short *)0xffff821e)  /*�^�C�}�W�F�l�������W�X�^B4*/
#define TGRC_4 (*(volatile unsigned short *)0xffff8228)  /*�^�C�}�W�F�l�������W�X�^A4*/
#define TGRD_4 (*(volatile unsigned short *)0xffff822a)  /*�^�C�}�W�F�l�������W�X�^B4*/

/*I/O �|�[�g���W�X�^*/
/*A �|�[�g*/
#define PADRH (*(volatile unsigned short *)0xffff8380)   /*�|�[�g�`�E�f�[�^���W�X�^H*/
#define PADRL (*(volatile unsigned short *)0xffff8382)   /*�|�[�g�`�E�f�[�^���W�X�^H*/
#define PAIORH (*(volatile unsigned short *)0xffff8384)  /*�|�[�g�`�EI/O���W�X�^H*/
#define PAIORL (*(volatile unsigned short *)0xffff8386)  /*�|�[�g�`�EI/O���W�X�^L*/
#define PACRH (*(volatile unsigned short *)0xffff8388)   /*�|�[�g�`�E�R���g���[�����W�X�^H*/
#define PACRL1 (*(volatile unsigned short *)0xffff838c)  /*�|�[�g�`�E�R���g���[�����W�X�^L1*/
#define PACRL2 (*(volatile unsigned short *)0xffff838e)  /*�|�[�g�`�E�R���g���[�����W�X�^L2*/

#define PBDR (*(volatile unsigned short *)0xffff8390)    /*�|�[�gB�E�f�[�^���W�X�^*/
#define PBIOR (*(volatile unsigned short *)0xffff8394)   /*�|�[�gB�EI/O���W�X�^*/
#define PBCR1 (*(volatile unsigned short *)0xffff8398)   /*�|�[�gB�E�R���g���[�����W�X�^*/
#define PBCR2 (*(volatile unsigned short *)0xffff839a)   /*�|�[�gB�E�R���g���[�����W�X�^*/


#define PEDRL (*(volatile unsigned short *)0xffff83b0)   /*�|�[�gE�E�f�[�^���W�X�^H*/
#define PECRL1 (*(volatile unsigned short *)0xffff83b8)  
#define PECRL2 (*(volatile unsigned short *)0xffff83ba)  
#define PEIORL (*(volatile unsigned short *)0xffff83b4)  /*�|�[�gE�E���W�X�^H*/
#define PFDR (*(volatile unsigned char *)0xffff83b3)     /*�|�[�gE�E�f�[�^���W�X�^H*/

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
   
/* ���荞�ݗD��x */
#define IPRA (*(volatile unsigned short *)0xffff8348) 
#define IPRB (*(volatile unsigned short *)0xffff834a) 
#define IPRC (*(volatile unsigned short *)0xffff834c) 
#define IPRD (*(volatile unsigned short *)0xffff834e) 
#define IPRE (*(volatile unsigned short *)0xffff8350) 
#define IPRF (*(volatile unsigned short *)0xffff8352)    /* WDT */ 
#define IPRG (*(volatile unsigned short *)0xffff8354) 
#define IPRH (*(volatile unsigned short *)0xffff8356)    /* SCI0 */ 
#define IPRI (*(volatile unsigned short *)0xffff835c)    /* SCI2 */

/*WDT �֘A*/
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
