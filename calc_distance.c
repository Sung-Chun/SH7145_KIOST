//#define   DEBUG
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <mes2.h>
#define  MAX_DISTANCE 145000.0   /* 145km */

/***** �֐��v���g�^�C�v�錾 ***************************************************/
int      calc_distance(void);
void     arrival_order(void);
double   kakudo(char *kaku);

/***** External Function ***********************************************************/
extern void runlog(int);                               /* runlog.c           */
extern int  date2sec(int time1[],int date1[]);         /* date_sec_convert.c  */
extern int  num_check(char *fin);                      /* command_like.c      */

/***** Global variables ***********************************************************/
extern char
      latitude[16][40],longitude[16][40],
      station_name[16][20],
      bf[120];
extern int
      total_st,               /* ���ǐ� */
      my_station,             /* ���ǔԍ� ex.0*/
      sound_speed,            /* reference sound speed */
      distance[16],           /* my_station����̋���(m) */
      tr_time[16],            /* my_station����̉��̓`�d�\�z����(msec) */
      order[16];

static double  
      radius=(double)6378136.0,   /* �n�����a(m) */
      pai=3.141592;
static char kaku[20];

/***** calc_distance **********************************************************/
/* ���̋ǂƂ̊Ԃ̋����̌v�Z������                                             */
/* ������ MAX_DISTANCE ���ł����0�AMAX_DISTANCE�ȏ�ł����-3��Ԃ�          */
/******************************************************************************/
int calc_distance(void){
   int      i,too_far_distance=0;
   double   lat0,lat1,lon0,lon1,diff_lat,diff_lon,dx,dy;

      strcpy(kaku,latitude[my_station]);
      lat0=kakudo(kaku);        /* latitude */
      strcpy(kaku,longitude[my_station]);
      lon0=kakudo(kaku);       /* longitude */
      for(i=0;i<total_st;++i){
         too_far_distance=0;
         strcpy(kaku,latitude[i]);
         lat1=kakudo(kaku);
         strcpy(kaku,longitude[i]);
         lon1=kakudo(kaku);
         diff_lat=lat1-lat0;
         diff_lon=lon1-lon0;
         dx=radius*cos(pai*(lat0+lat1)/2.0/(double)10800)*diff_lon*pai/(double)10800;
         dy=radius*pai*diff_lat/(double)10800;
         distance[i]=(int)(sqrt(dx*dx+dy*dy));  /* distance(m) */
         if(distance[i]>MAX_DISTANCE){
            too_far_distance=-3;
            runlog(251);
         }
         tr_time[i]=(int)(1000*distance[i]/sound_speed);   /* msec */
         #ifdef   DEBUG
            printf(bf,"%d %s distance(m)=%10d tr_time(ms)=%10d\x0d\x0a"
               ,i,station_name[i],distance[i],tr_time[i]);
         #endif
      }
      distance[my_station]=(double)0.0;tr_time[my_station]=0;
      return(too_far_distance);
}

/* calc_distance()�Ōv�Z�������ϐ�tr_time[](���B�\�z����)���g�p����� */
/* tr_time[]�̈����͋ǔԍ��ł��� */
/* order[0]=my_station��tr_time[0]=0�ł��� */
/* order[1]�͈�ԍŏ��ɉ�������Ă���ǔԍ�,order[2]��2�Ԗڂɉ�������Ă���ǔԍ� */
void arrival_order(void){
   int   i,j,imin=0;
   int   travel_time,tr[16];

      /* �������B���鏇��order[]�����߂� */
      for(i=0;i<total_st;++i) tr[i]=tr_time[i];
      for(j=0;j<total_st;++j){
         travel_time=(int)100000;
         for(i=0;i<total_st;++i){   /* �ŏ��l�𔲂��o�� */
            if(travel_time>tr[i]){
               travel_time=tr[i];
               imin=i;
            }
         }
         tr[imin]=(int)100000;
         order[j]=imin;
      }
      return;
}

/* GPS receiver ���ʃf�[�^�̕\�� 12345.678E �� 123�x45.678��EAST */
/* ���̒l�𕪂̒l�Ŗ߂� */
double  kakudo(char *kaku){
   unsigned int   i,degree;
   unsigned char  c1;
   double   fugou,degree_min,min;

      fugou=1.0;
      i=strlen(kaku);
      c1=kaku[i];
      if((c1=='S') || (c1=='s')) fugou=-1.0;
      if((c1=='W') || (c1=='w')) fugou=-1.0;
      if((c1>0x2f) && (c1<0x39)) i++;  /* N or E is omitted */
      kaku[i]='\0';
      min=atof(kaku);
      degree=(unsigned int)(min/100);
      min-=(double)(degree*100);
      degree_min=fugou*((double)degree*60.0+min);
      return(degree_min);
}
