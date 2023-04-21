/* wakeup_interval is in minutes but converted to seconds after reading */
#define   DEBUG
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <mes2.h>
/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int  read_cfg_file(char *cfg_file);
int  date_time_set(char *fin,int *date1,int  *time1,int flag);

/* **** External function declaration ************* */
extern void runlog(int);                               /* runlog.c           */
extern int  date2sec(int time1[],int date1[]);         /* date_sec_convert.c */
extern int  fgets(char *fin,int k,int fp);             /* command_like.c     */
extern int  num_check(char *fin);                      /* command_like.c     */

/* **** external(global) variable declaration ******* */
extern char
      my_station_name[],
      code_name[],
      latitude[16][40],longitude[16][40],
      station_name[16][20];
extern int
      total_st,               /* ���ǐ�                                       */
      my_station,             /* ���ǔԍ� ex.0                                */
      extra_record_width,     /* ���̒����v���X�ǂꂾ����M���邩             */
      t_freq_common,
      t_freq[],               /* transmit frequency                           */
      sound_speed,            /* reference sound speed                        */
      gps_operation,
      mod_num,                /* �ϒ����x(wave no/digit)                      */
      code_repeat,            /* �R�[�h�̌J��Ԃ���                         */
      receive_mode,           /* nornal receive(0) or ring buffer pileup receive(1) */
      wakeup_interval,        /* wakeup interval�i��)                         */
      round_interval,         /* round interval(�b)                           */
//      calc_start,           /* ���֌v�Z������X�^�[�g���ԂɊ֌W�����l     */
//      calc_length,          /* ���֌v�Z�����鎞�Ԓ���(ms)                 */
      code_no[16],            /* �ő�16 station���p�ӂ���                     */
      tr_time[16],            /* my_station����̉��̓`�d�\�z����(msec)       */
      test_time,              /* �e�X�g�����ˎ���                             */
      start_time,             /* �����J�n����                                 */
      end_time,               /* �����I������                                 */
      burst_total_num,        /* �o�[�X�g���[�h�ł̑��M��*/
      burst_interval;         /* �o�[�X�g�Ԋu*/

static char f[25][80],fin[80];

/***** read_cfg_file **********************************************************/
/* 2008.5.                                                                    */
/* cfg�t�@�C�����J���A�����ݒ���s�����ϐ��ɒl���i�[����                    */
/* ���� �R�[�h��12���܂ł̔z���p�ӂ����B12���ȏ�͔z��𑝂₷�K�v������    */
/* return= 0 normally return                                                  */
/* return=error_no                                                            */
/******************************************************************************/
int read_cfg_file(char *cfg_file){
   int   i,j,k,m,st_no,error_no,time1[3],date1[3];
   int   fp;
   char  bf[128];

      fp=open(cfg_file,OptRead);
      if(fp==-1){
         printf("%s cannot open\x0d\x0a",cfg_file);
         runlog(201);return(-1);
      }
      /* cfg file�ɂ͓r���ŃX�y�[�X����(����)������̂Œ��� */
      for(i=0;i<22;++i){
         fgets(fin,80,fp);
         printf("%s",fin);
         for(j=0;j<80;++j){      /* �R�����g��0d,0a�������Ȃ� */
            if((fin[j] < 0x20) || (fin[j] ==';')){fin[j]=0;break;}
         }
         /* ���̃X�y�[�X�������������� */
         k=strlen(fin);  /* NULL������������������ */
         for(j=0;j<k;++j){
            if(fin[j] != 0x20) break;
         }
         for(m=0;m<k-j+1;++m) bf[m]=fin[m+j];
         strcpy(f[i],bf);
      }       /* �擪��22�̃V�X�e���ݒ�f�[�^�ǂݍ��ݏI�� */

      i=strlen(f[0]);         /* ���ꕶ������菜�� */
      for(j=0;j<i;++j){
         if(f[0][j]<=0x20) break;
      }
      f[0][j]='\0';  /* ���s�����������ăA�X�L�[�Y�ɂ��� */
      strcpy(my_station_name,f[0]);          /* my station name ex.S1   */
      /* total station number */
      if(num_check(f[1])==-1){runlog(202);error_no=202;goto error1;}
      total_st=atoi(f[1]);
      /* wakeup�Ԋu(��) */
      if(num_check(f[2])==-1){runlog(203);error_no=203;goto error1;}
      wakeup_interval=atoi(f[2])*60; /*�b�̒l�ɕϊ�����*/
      if(wakeup_interval<=0) {runlog(204);error_no=204;goto error1;}
      /* round interval(sec) */
      if(num_check(f[3])==-1){runlog(205);error_no=205;goto error1;}
      round_interval=atoi(f[3]);
      i=round_interval*total_st;
      if(i>=wakeup_interval){runlog(221);error_no=221;goto error1;}         

      /* Transmit_freqency(Hz) */
      if(num_check(f[4])==-1){runlog(206);error_no=206;goto error1;}
      t_freq_common=atoi(f[4]);

      /* code name*/
      i=strlen(f[5]);
      for(j=0;j<i;++j){if(f[5][j]<=0x20) break;}
      f[5][j]='\0';  /* ���s�����������ăA�X�L�[�Y�ɂ��� */
      strcpy(code_name,f[5]);

      if(num_check(f[6])==-1){runlog(207);error_no=207;goto error1;}
      mod_num=atoi(f[6]);     /* �ϒ����x */

      if(num_check(f[7])==-1){runlog(208);error_no=208;goto error1;}
      code_repeat=atoi(f[7]); /* code repeat count */

      /* receive_mode nornal receive or ring buffer pileup receive */
      if(f[8][0]=='N' || f[8][0]=='n' || (f[8][0]=='0')) receive_mode=0;/* nornal receive */
      else receive_mode=1;             /* superimpose in ring buffer */

      if(num_check(f[9])==-1){runlog(210);error_no=210;goto error1;}
      extra_record_width=atoi(f[9]);/* ���̎������Ԃ̑O��ɋL�^����M���� */

//      if(num_check(f[10])==-1){runlog(211);error_no=211;goto error1;}
//      calc_length=atoi(f[10]);/* ���֌v�Z���Ԓ�(msec) */

//      if(num_check(f[12])==-1){runlog(213);error_no=213;goto error1;}
//      calc_start=atoi(f[12]);/* ���֌v�Z�J�n���� */

      if(num_check(f[13])==-1){runlog(214);error_no=214;goto error1;}
      sound_speed=atoi(f[13]);/* sound speed (m/s) */

      if(f[14][0]=='G' || f[14][0]=='g' || f[14][0]=='1') gps_operation=1;
      else gps_operation=0;

      if(num_check(f[16])==-1){runlog(216);error_no=216;goto error1;}
      burst_total_num=atoi(f[16]);

      if(num_check(f[17])==-1){runlog(217);error_no=217;goto error1;}
      burst_interval=atoi(f[17])*60; //minutes to second
      i=burst_total_num*burst_interval;
      if(i>=wakeup_interval){runlog(222);error_no=222;goto error1;}

      /* �����X�^�[�g�����ƏI������ */
      f[19][19]='\0';
      strcpy(fin,f[19]);  /* Format 1998-11-09 13:30:00 */
      i=date_time_set(fin,date1,time1,0);
      if(i!=0){runlog(223);error_no=223;goto error1;}
      test_time=date2sec(time1,date1);/* ���ϐ� 2000.1.1����̌o�ߕb */
      f[20][19]='\0';
      strcpy(fin,f[20]);  /* Format 1998-11-09 13:30:00 */
      i=date_time_set(fin,date1,time1,0);
      if(i!=0){runlog(224);error_no=224;goto error1;}
      start_time=date2sec(time1,date1);/* ���ϐ� 2000.1.1����̌o�ߕb */
      f[21][19]='\0';
      strcpy(fin,f[21]);  /* Format 1998-11-09 13:30:00 */
      i=date_time_set(fin,date1,time1,0);
      if(i!=0){runlog(225);error_no=225;goto error1;}
      end_time=date2sec(time1,date1);/* ���ϐ� 2000.1.1����̌o�ߕb */


   #ifdef DEBUG
//      printf("test_time =%s\x0d\x0a",f[19]);     
//      printf("start_time=%s\x0d\x0a",f[20]);
//      printf("end_time  =%s\x0d\x0a",f[21]);
   #endif
      sleep(1000);   /* ��ʂ��X�N���[�����Ȃ��悤��1�b�҂� */

      my_station=-1;
      for(st_no=0;st_no<total_st;++st_no){
         for(;;){
            fgets(fin,80,fp);
            if(fin[0]==0) goto end1;
            if(fin[0]=='{') break;  /* left brace catch */
         }
         fgets(fin,80,fp);        /*station name*/
         fin[strlen(fin)-2]='\0';/* ���s�����܂œǂݍ���ł���̂ŉ��s���������� */
         if(strcmp(my_station_name,fin)==0) my_station=st_no;
         strcpy(station_name[st_no],fin);
      #ifdef DEBUG
         printf("station_name=%s\x0d\x0a",station_name[st_no]);
      #endif
         
         fgets(fin,80,fp);/*code ��荞�� */
         for(j=0;j<80;++j){
            if((fin[j] < 0x20) | (fin[j] ==';')){fin[j]=0;break;}
         }
         if(num_check(fin)==-1){runlog(218);error_no=218;goto error1;}
         sscanf(fin,"%d\x0d\x0a",&code_no[st_no]);
         
         fgets(fin,80,fp);/*latitude ��荞�� */
         i=0;
         for(j=0;j<(int)strlen(fin);++j){
            if(fin[j]>=0x30 && fin[j]<=0x39){latitude[st_no][i]=fin[j];i++;}
            if(fin[j]=='-' || fin[j]=='.'){latitude[st_no][i]=fin[j];i++;}
            if(fin[j]=='N' || fin[j]=='S'){
               latitude[st_no][i]=fin[j];i++;j++;
               latitude[st_no][i]='\0';break;
            }
         }
         i=0;/*longitude ��荞��  */
         for(k=j;k<(int)strlen(fin);++k){
            if(fin[k]=='-' || fin[k]=='.'){longitude[st_no][i]=fin[k];i++;}
            if(fin[k]>=0x30 && fin[k]<=0x39){longitude[st_no][i]=fin[k];i++;}
            if(fin[k]=='E' || fin[k]=='W'){
               longitude[st_no][i]=fin[k];i++;
               longitude[st_no][i]='\0';break;
            }
         }
      #ifdef DEBUG
         printf("lon=%s ,lat=%s\x0d\x0a",longitude[st_no],latitude[st_no]);
      #endif
/*----------------------------------------------------------------------------*/
/* cfg�t�@�C���̂��̕����ɂ͉����̎��g���������Ă���  */
/* �ǖ��ɈقȂ�p�����[�^������΂����ɏ����B                             */
         fgets(fin,80,fp);    /* t_freq */
         for(j=0;j<80;++j){      /* �R�����g��0d,0a�������Ȃ� */
            if((fin[j] < 0x20) || (fin[j] ==';')){fin[j]=0;break;}
         }
         if(num_check(fin)==-1){runlog(219);error_no=219;goto error1;}
         sscanf(fin,"%d\x0d\x0a",&t_freq[st_no]); /* ���g���g�� */
/*----------------------------------------------------------------------------*/
         fgets(fin,80,fp);
         if(fin[0]!='}'){runlog(226);error_no=226;goto error1;}  /* right brace catch? */
      }   /* for(st=;;) _no end */
   end1:
      if(st_no==0){runlog(227);error_no=227;goto error1;}
      if(total_st!=st_no){runlog(228);error_no=228;goto error1;}
      if(my_station==-1){runlog(229);error_no=229;goto error1;}
      close(fp);  /* cfg�͐���ɓǂݍ��ݏI�� */
      return(0);
   error1:
      printf("configuration format error\x0d\x0a");
      close(fp);return(error_no);
}
/***** date_time_set.c *******************************************************/
/* cfg�t�@�C����start���Ԃ�end���Ԃ�ǂݍ���Ńt�H�[�}�b�g�𐮂���           */
/*****************************************************************************/
int date_time_set(char *fin,int *date1,int  *time1,int flag){
   int   j;
   char  k,bf[80];

      if((fin[4]!='-') || (fin[7]!='-') || (fin[10]!=' ')){
         return(-1);
      }
      if((fin[13]!=':') || (fin[16]!=':')){
         return(-2);
      }
      /* isdigit�͐����ł����0�ȊO�̒l��߂� */
      for(j=0;j<4;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j]=(unsigned char)k;
         else return(-3);
      }
      bf[4]=0;date1[0]=atoi(bf); /* �N */
      for(j=5;j<7;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j-5]=(unsigned char)k;
         else return(-4);
      }
      bf[2]=0;date1[1]=atoi(bf); /* �� */
      for(j=8;j<10;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j-8]=(unsigned char)k;
         else return(-5);
      }
      bf[2]=0;date1[2]=atoi(bf); /* �� */
      for(j=11;j<13;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j-11]=(unsigned char)k;
         else return(-6);
      }
      bf[2]=0;time1[0]=atoi(bf); /* �� */
      for(j=14;j<16;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j-14]=(unsigned char)k;
         else return(-7);
      }
      bf[2]=0;time1[1]=atoi(bf); /* �� */
      for(j=17;j<19;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j-17]=(unsigned char)k;
         else return(-9);
      }
      bf[2]=0;time1[2]=atoi(bf); /* �b */
      if(flag==0) return(0);
      for(j=20;j<22;++j){
         k=fin[j];
         if(isdigit(k)!=0) bf[j-20]=(unsigned char)k;
         else{bf[j-20]=0;break;}
      }
      j=atoi(bf);
      return(j);
}
