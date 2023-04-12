/* RTC-8564NB リアルタイムクロックモジュール */
/* RTCはI2Cで接続してある */
/* I2C address=RTC_WRITE   0xa2 */
/* I2C address=RTC_READ    0xa3 */

#include <string.h>
#include <stdlib.h>
#include <mes2.h>
#include "SH7145.h"
/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void  set_timer(int sleep_byou);
int   set_rtc_clock(void);
int   read_rtc_clock(void);
void  write_rtc_clock(int pc_byou);
int   read_alarm_clock(void);
int   set_alarm_clock(int pc_byou);
void  alarm_reset(void);

/* このモジュール内だけで使用する関数群 */
static void _wait();
static void start_i2c(); 
static void stop_i2c();
static int writebyte(int data);
static int readbyte();
static void nak();
static int send_ctrl(int ctrl);
int rtc_read(int addr);
int rtc_write(int addr,int data);
static unsigned int get_value(int reg);
static unsigned int set_value(int value);
static int num_check(char *fin);
int    time1[3],date1[3],week;

/* **** External function declaration ************* */
extern void WDT(int flag);
extern int date2sec(int time1[],int date1[]);
extern int sec2date(int byou,int cal[]);
extern int gets(char *bf);          /* command_like.c */
extern int num_check(char *fin);    /* command_like.c */

void  set_timer(int sleep_byou){ /* sleep_sec後、RTCから割り込み信号を出させる */
      rtc_write(1, 0x01);        /*Interrupt ON*/
      rtc_write(15,sleep_byou);  /* Timer count set*/
      rtc_write(14,0x82);        /* Timer start*/
      return;
}

int read_timer(void){
      return(rtc_read(15));
}
         
int set_rtc_clock(void){
   int   i,j=0,date1[3],time1[3];
   int   pc_byou;
   char  fin[6],bf[80];

      printf("Please input for ex. 2001-06-24 09:23:15\r");
      j=gets(bf); /* string input */
      if(j==-1) return(-1);
      fin[0]=bf[0];fin[1]=bf[1];fin[2]=bf[2];fin[3]=bf[3];fin[4]=0;
      if(num_check(fin)==-1) goto end1;
      i=atoi(fin);date1[0]=i;   /* year */
      fin[0]=bf[5];fin[1]=bf[6];fin[2]=0;
      if(num_check(fin)==-1) goto end1;
      i=atoi(fin);date1[1]=i;   /* month */
      fin[0]=bf[8];fin[1]=bf[9];fin[2]=0;
      if(num_check(fin)==-1) goto end1;
      i=atoi(fin);date1[2]=i;   /* day */

      fin[0]=bf[11];fin[1]=bf[12];fin[2]=0;
      if(num_check(fin)==-1) goto end1;
      i=atoi(fin);time1[0]=i;   /* hour */
      fin[0]=bf[14];fin[1]=bf[15];fin[2]=0;
      if(num_check(fin)==-1) goto end1;
      i=atoi(fin);time1[1]=i;   /* minute */
      fin[0]=bf[17];fin[1]=bf[18];fin[2]=0;
      if(num_check(fin)==-1) goto end1;
      i=atoi(fin);time1[2]=i+1; /* second */

      pc_byou=date2sec(time1,date1);
      write_rtc_clock(pc_byou);
      return(0);
   end1:
      printf("%s Clock format error\r",bf);
      return(-1);
}

int read_rtc_clock(void){
   int pc_byou;
      date1[0] = get_value(rtc_read(8))+2000;   /* 年 00,01,02,... */
      date1[1] = get_value(rtc_read(7) & 0x1f); /* 月 */
      week = get_value(rtc_read(6) & 0x07);     /* 曜日（使用しない） */
      date1[2] = get_value(rtc_read(5) & 0x3f); /* 日 */
      time1[0] = get_value(rtc_read(4) & 0x3f); /* 時 */
      time1[1] = get_value(rtc_read(3) & 0x7f); /* 分 */
      time1[2] = get_value(rtc_read(2) & 0x7f); /* 秒 */
      pc_byou=date2sec(time1,date1);            /* 2000.1.1からの経過秒 */
      return(pc_byou);
      }

void write_rtc_clock(int pc_byou){
   int   cal[6];
   int   yearL2;
      sec2date(pc_byou,cal);          
      yearL2=((unsigned int)(cal[0]%100))&0xff;
      rtc_write(8, set_value(yearL2));    /* 年 00,01,02,... */
      rtc_write(7, set_value(cal[1]));    /* 月 */
      rtc_write(5, set_value(cal[2]));    /* 日 */
      rtc_write(4, set_value(cal[3]));    /* 時 */
      rtc_write(3, set_value(cal[4]));    /* 分 */
      rtc_write(2, set_value(cal[5]));    /* 秒 */
      return;
}

/* set_alarm_clockは将来の日,時,分の設定だけが可能 */
/* 月を横切る設定の場合は日の値がcurrent時より小さな値になる */
int set_alarm_clock(int pc_byou){   /* Alarmは将来の日,時,分の設定だけ */
   int cal[6];
   int rtc_byou;

      rtc_byou=read_rtc_clock();
      rtc_write(1, set_value(0x02));      /* Alarm interrupt enable */
      sec2date(pc_byou,cal);
//      week = get_value(rtc_read(6) & 0x07);
      rtc_write(12, 0x80);                /*  Alarm週(donot care) */
      rtc_write(11, set_value(cal[2]));   /*  Alarm日 */
      rtc_write(10, set_value(cal[3]));   /*  Alarm時 */
      rtc_write(9, set_value(cal[4]));    /*  Alarm分 */
      return(0);
}
/* set_alarm_clockで日,時,分の設定だけを行った */
int read_alarm_clock(void){
   int
      pc_byou,
      current_year,current_month,current_day,
      alarm_year,alarm_month,alarm_day;

      current_year=alarm_year=get_value(rtc_read(8))+2000;     /* current year 00,01... */
      current_month=alarm_month=get_value(rtc_read(7) & 0x1f); /* current month */
      current_day= get_value(rtc_read(5) & 0x3f);              /* current day */
      week=get_value(rtc_read(6) & 0x07);
      alarm_day=get_value(rtc_read(11) & 0x3f);                /* Alarm day */
      if(current_day > alarm_day){
         if(current_month < 12){
            alarm_month=current_month+1;                  
         }
         else{ /* current_monthが12月である場合 */
            alarm_year=current_year+1;
            alarm_month= 1;
         }
      }
      date1[0]=alarm_year;date1[1]=alarm_month;date1[2]=alarm_day;
      time1[0] = get_value(rtc_read(10) & 0x3f);   /* Alarm hour */
      time1[1] = get_value(rtc_read(9) & 0x7f);    /* Alarm minute */
      time1[2] = 0;
      pc_byou=date2sec(time1,date1);      /* 2000.1.1からの経過秒 */
      return(pc_byou);
}

void alarm_reset(void){
   rtc_write(1,0x00);
}

#define NAK SDA
#define ACK 0
#define SDA 0x08
#define SCL 0x04
#define DREG    PBDR /* PB2=SCL0,PB3=SDA0 (T-SH2MBでは26pinコネクタの8,9) */
#define CREG    PBIOR

static void _wait() {
    int i;
    for(i=0;i<1000;i++) ;
   }

static void start_i2c() {
    CREG = SCL + SDA+0x0200;
    DREG &= ~SCL;
    DREG |= SDA;
    _wait();
    DREG |= SCL;
    DREG &= ~SDA;
    _wait();
    DREG &= ~SCL;
    CREG = SCL+0x0200;
   }

static void stop_i2c() {
    CREG = SCL + SDA+0x0200;
    DREG &= ~SCL;
    DREG &= ~SDA;
    _wait();
    DREG |= SCL;
    DREG |= SDA;
    _wait();
    DREG &= ~SCL;
    CREG = SCL+0x0200;
   }

static int writebyte(int data) {
    int i, mask;

    CREG = SCL + SDA+0x0200;
    mask = 0x80;
    for(i = 0;i < 8;i++) {
        if(data & mask) DREG |= SDA;
        else DREG &= ~SDA;
        DREG |= SCL;
        _wait();
        DREG &= ~SCL;
        mask >>= 1;
        _wait();
    }
    CREG = SCL+0x0200;
    DREG |= SCL;
    _wait();
    i = DREG & SDA;
    DREG &= ~SCL;
    return i;
   }

static int readbyte() {
    int i, data;

    data = 0;
    for(i = 0;i < 8;i++) {
        data <<= 1;
        DREG |= SCL;
        if(DREG & SDA) data++;
        _wait();
        DREG &= ~SCL;
        _wait();
    }
    return data;
   }

static void nak() {
    CREG = SCL + SDA+0x0200;
    DREG |= SDA;
    DREG |= SCL;
    _wait();
    DREG &= ~SCL;
    DREG &= ~SDA;
    CREG = SCL+0x0200;
   }


static int send_ctrl(int ctrl) {
    int ret, count;

    count = 0;
    do {
        start_i2c();
        ret = writebyte(ctrl);
        if(count++ > 1000) return -1;
    } while(ret == NAK);
    return 0;
   }


#define RTC_WRITE   0xa2
#define RTC_READ    0xa3

int rtc_read(int addr) {
    int data, ret;

    ret = send_ctrl(RTC_WRITE);
    if(ret == -1) return -1;
    writebyte(addr);
    ret = send_ctrl(RTC_READ);
    if(ret == -1) return -1;
    data = readbyte();
    nak();
    stop_i2c();
    return data;
   }

int rtc_write(int addr, int data) {
    int ret;

    ret = send_ctrl(RTC_WRITE);
    if(ret == -1) return -1;
    ret = writebyte(addr);
    ret = writebyte(data);
    stop_i2c();
    return 0;
   }

char bin2bcd(int ch){
    char        ch1,c0,c1;
      /* BINARY値をBCD値に変換 */
      c0=(0xff&ch)/10;
      c1=(0xff&ch)%10;
      ch1=c0*16+c1;
       return(0xff&ch1);
        }

unsigned int get_value(int reg){   /* BCD値をBINARY値に変換 */
    char ch1,c0,c1;

      c0=(0xff & reg)/16;
      c1=(0xff & reg)%16;
      ch1=c0*10+c1;
       return((unsigned int)ch1);
        }

unsigned int set_value(int value){  /* BINARY値をBCD値に変換 */
    unsigned int reg, data;

       reg = (value / 10) << 4;
       data = value % 10;
       reg += data;
       return ((unsigned int)reg);
}
int num_check(char *fin){
   int   i,j;
   char  k;
      j=strlen(fin);  /* NULL文字を除いた文字数 */
      for(i=0;i<j;++i){
         k=fin[i];
         if((k==0x20) || (k==0x2e) || (k=='-')) continue;    /* space or period? */
         if((k< 0x30) || (k>0x39)) return(-1);
      }
      return(0);
}
