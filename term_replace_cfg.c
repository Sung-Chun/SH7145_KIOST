/* Reload new cfg file */
/* 20220906 debug is not finish yet */
/* file name change from work.cfg to tomo.cfg */
/* �d�v cfg file��CRLF�ŏI�����Ă��� ��荞�ގ���CDLF�Ŏ�荞�܂Ȃ���΂Ȃ�Ȃ� */
/* Hyperterm��TextFile���M���s����Default�ł͕�����̍Ō��CR,LF��LF���Ȃ���� */
/* File-Property-Setting-ASCCII-Setup��Send line with line feed��I���� */
/* ��M�͊��荞�݂œ������Ă��邪�A��M���ڂ����������邱�Ƃ�����̂� */
/* ���M�͂P�s���邲�Ƃ�100msecdelay��ݒ肷��iHyperterm) */
#include <stdlib.h>
#include <string.h>
#include "SH7145.h"
#include <MES2.h>
#define  LF    0x0a

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int replace_cfg(void);

/* **** External function declaration ************* */
extern int  num_check(char *fin);   /* command_like.c */
extern int  copy(char *source, char *destination);
extern int  serRecvs(int port,char *s,char charactor,int n,int msec);

/* **** external(global) variable declaration ***** */
extern char record_media[];

int replace_cfg(){
   int i,j,k,m,st,flag;
   int ret=0;
   int fp;
   char bf[128],work_file[80];

      memset(work_file,'\0',80);
      strcpy(work_file,record_media);
      strcat(work_file,"work.cfg");
      fp=open(work_file,OptWrite);
      if(fp==-1){
         printf("%s can not open\x0d\x0a",work_file);
         return(1);
      }
      /* ��������cfg�t�@�C�����쐬����10�b�ԑ҂� */
      printf("Please input file\x0d\x0a");
      memset(bf,'\0',sizeof(bf));
      sleep(10000);
      ret=read(0,bf,80);             /* 1.my_station */      
      if(ret==-1){
         close(fp);
         return(2);
      }
      printf("%s",bf);
      fprintf(fp,"%s",bf);

      memset(bf,'\0',sizeof(bf));
      ret=read(0,bf,128);printf("%s",bf);/* 2.�Ǒ��� */
      if(ret==-1){
         close(fp);
         return(3);
      }
      fprintf(fp,"%s",bf);
      k=strlen(bf);
      for(i=0;i<k;++i){if((bf[i]<0x30) || (bf[i]>0x39)) break;}
      bf[i]=0;
      if(num_check(bf)==-1){
         printf("station number format error\x0d\x0a");
         close(fp);
         return(4);
      }
      st=atoi(bf);
      if((st<0) || (st>16)){
         printf("station number is mistaken\x0d\x0a");
         close(fp);
         return(4);
      }

      for(i=0;i<17;++i){
         memset(bf,'\0',128);
         ret=read(0,bf,128);printf("%s",bf);
         if(ret==-1){
            close(fp);
            return(i+5);
         }
         fprintf(fp,"%s",bf);
      }
      for(i=0;i<3;++i){ /* ������� */
         ret=read(0,bf,128);printf("%s",bf);
         if(ret==-1){
            close(fp);
            return(i+15);
         }
         k=strlen(bf);  /* NULL������������������ */
         for(j=0;j<k;++j){
            if((bf[j]>=0x30) && (bf[j]<=0x39)) break;
         }
         for(m=0;m<k-j+1;++m) bf[m]=bf[m+j];
         if((bf[4]!='-') || (bf[7]!='-') || (bf[10]!=' ')){flag=21;goto end1;}
         if((bf[13]!=':') || (bf[16]!=':')){flag=22;goto end1;}
         fprintf(fp,"%s",bf);    /* 2001-11-09 13:30:00 */
      }
      for(i=0;i<st;++i){
         memset(bf,'\0',128);
         ret=read(0,bf,128);
         if(ret==-1){flag=30;goto end1;}
         if(bf[0]!='{'){flag=31;goto end1;}
         printf("%s",bf);
         fprintf(fp,"%s",bf);    /* left brace */
         
         memset(bf,'\0',128);
         ret=read(0,bf,128);
         if(ret==-1){flag=32;goto end1;}
         printf("%s",bf);
         fprintf(fp,"%s",bf);    /* station name */
         
         ret=serRecvs(1,bf,LF,80,5000);printf("%s",bf);
         if(ret==-1){flag=33;goto end1;}
         fprintf(fp,"%s",bf);    /* code_no */
         
         memset(bf,'\0',128);
         ret=read(0,bf,128);printf("%s",bf);
         if(ret==-1){flag=34;goto end1;}
         fprintf(fp,"%s",bf);    /* latitude longitude */
         
         memset(bf,'\0',128);
         ret=read(0,bf,128);printf("%s",bf);
         if(ret==-1){flag=35;goto end1;}
         fprintf(fp,"%s",bf);    /* t_freq */
         
         memset(bf,'\0',128);
         ret=read(0,bf,128);printf("%s",bf);
         if(ret==-1){flag=37;goto end1;}
         if(bf[0]!='}'){flag=38;goto end1;}
         fprintf(fp,"%s",bf);    /* right brace */
      }  
      close(fp);
      memset(bf,'\0',80);
      strcpy(bf,record_media);
      strcat(bf,"tomo.cfg");
      flag=copy(work_file,bf);
      return(flag);
   end1:
      close(fp);
      return(flag);
}
