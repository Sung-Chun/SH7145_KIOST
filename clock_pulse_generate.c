/* �����N���b�N(20MHz)��p���Ė]�݂ɋ߂����g���̃N���b�N�����program */
/* ����0:MTU�̐ݒ薳�� �߂�l:Hz,clock�̔��U���J�n���Ȃ� */
/* ����2:MTU�̐ݒ薳�� �߂�l:Hz*1000,clock�̔��U���J�n���Ȃ� */
/* ����1:MTU�̐ݒ���s�� �߂�l:Hz,clock�̔��U���J�n���� */

/* �o�͂�T-SH2M��CN3�̃R�l�N�^��TIOC0_A(PE0)�s�����瓾�� */
/* counter start�����̊֐��ɓ���Ă��� */

/* Cats�ł̎g�p�Ɋւ��Đ���
SH7145(SH2)��MTU�Ƃ���channel_0,1,2,3,4��5channel�������Ă���B
This program is using channel_0, channel_1, channel_2 and channel_4.
channel_1 and channel_2 are used by stop watch
channel_0 is used by synchronizing transmit data and D-FF trigger
channel_4 is used by quadrature demodulation and A/D trigger

���ꂼ���channel��general register�𕡐������Ă���B
��{�I�ɏo�͒[�q�͂�����general reg�ɕt�����ē������������݂���B
�`�����l��4���Ɏ��ƃ`�����l��4�ɂ�4��general reg������A�o�͒[�q��4����B
�o�͒[�q�ɖ]�݂̎��g���̃N���b�N���o�͂�����ɂ́A�����N���b�N��
�`�����l��4�̃J�E���^�ŃJ�E���g�A�b�v��general reg�ɐݒ肵���l�ƈ�v�����
�J�E���^�̒l��0�ɃN���A�������ɂ���general reg�ɑ�����o�͒[�q�̐M����
�g�O���i�����j�����邱�Ƃœ��邱�Ƃ��ł���B�e�o�͒[�q�̐M���͊e�X��
general reg�Ƃ̈�v�ŏo�͂��g�O���ł��邪�A�J�E���^���N�����ł���̂�
4��general reg�̂Ȃ���1�ł���B
cos,sin��90�x�ʑ������炵���N���b�N�͎��̂悤�ɂ��ē��Ă���B
TGRC_4��reg�ŃJ�E���^�̃N������TIOC4C�[�q�̏o�͂��g�O������,
TGRD_4��reg���g����TIOC4D�[�q�̏o�͂��g�O�������邱�Ƃœ��Ă���
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
      n=(int)(base_freq/(2*freq));   /* Toggle������̂łQ�{ */
      true_freq=(int)(base_freq/(2*n));
      richtig_freq1000=(int)((double)base_freq*1000.0/(2.0*(double)n));
      if(flag==0){
         TSTR=0x00;return(true_freq);
         }
      if(flag==2){
         TSTR=0x00;return(richtig_freq1000);
         }
      /* TGRA_0�̃R���y�A�}�b�`��TCNT_0�N���A�E ���[�E  �����N���b�N�J�E���g */
      TCR_0 = 0x38;           /* 001 11 000 */
      /* TGRC_4�̃R���y�A�}�b�`��TCNT_4�N���A�E ���[�E  �����N���b�N�J�E���g */
      TCR_4 = 0xa8;           /* 101 11 000 */
    
      /* ���o�͐ݒ� */

      /* TIOC0A�[�q �R���y�A�}�b�`�g�O���o�� */
      TIORH_0 = 0x03;         /* 0000 0011 */
      /* TIOC4C,TIOC4D,�[�q �R���y�A�}�b�`�g�O���o�� */       
      TIORH_4 = 0x00;         /* 0000 0000 */
      TIORL_4 = 0x33;         /* 0011 0011 */

      /* counter 0��4�𗘗p���� */
      /* TIOC4C,TIOC4D,�[�q �o�͋��� */
      TOER |= 0xf0;
      TGRA_0 =  n-1;      /* for sampling clock */
      TGRA_4  = 2*n-1;    /* TGRA_4���g�p����̂�A/D�ϊ��̋N���Ɏg�����߂ł��� */
      TGRC_4  = 2*n-1;    /* for cos            */
      TGRD_4  = n;        /* for sin            */
      TSTR= 0x81;         /* �J�E���^�X�^�[�g 10** *001 */
      return(true_freq);
   }
