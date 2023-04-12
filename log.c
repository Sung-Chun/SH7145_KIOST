/* 20220906 */
#include <string.h>
#include <macro.h>
#include <MES2.h>

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void openning_mes(int rtc_time,int batter_low);
void runlog(int code);
void time_information_log(int value);

/* **** External function declaration ************* */
extern void WDT(int wdt_flag);
extern int  read_rtc_clock(void);
extern int  sec2date(int byou,int cal[]);

/* **** external(global) variable declaration ***** */
extern char version[];
extern char record_media[];

static char   fout[160];

void openning_mes(int rtc_byou,int battery_low){
   int   cal[6],fp1;
   char  pc_utc[20],bf[40];
   char openning[160];

      INT_ENABLE() ;     /* すべての割り込みを許可 */
      WDT(1);
      memset(bf,'\0',128);
      strcpy(bf,record_media);
      strcat(bf,"runlog.txt");
      fp1=open(bf,OptAppend);    /* 追加書き込みモード */
      if((int)fp1==-1)return;

      memset(openning,'\0',160);
      memset(fout,'\0',160);

      strcpy(openning,version);
      strcat(openning," program start ");
      sec2date(rtc_byou,cal);
      sprintf(pc_utc,"%4d %2d %2d %2d:%02d %02d\0x0d\x0a",
         cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);

      /* battery voltage check result */
      if(battery_low==0) strcpy(fout,"battery good ");
      else strcpy(fout,"battery voltage low ");
      strcat(openning,fout);
      strcat(openning,pc_utc);
      printf("%s\x0d\x0a",openning);
      fprintf(fp1,"%s\x0d\x0a",openning);
      close(fp1);
}

void runlog(int code){
   int pc_byou;
   int   cal[6];
   int   fp1;
   char  pc_utc[20],work[20],bf[80];

      INT_ENABLE() ;     /* すべての割り込みを許可 */
      WDT(1);
      strcpy(bf,record_media);
      strcat(bf,"runlog.txt");
      fp1=open(bf,OptAppend);    /* 追加書き込みモード */
      if((int)fp1==-1)return;

      pc_byou = read_rtc_clock();
      sec2date(pc_byou,cal);
      sprintf(pc_utc,"%4d %2d %2d %2d:%02d %02d\0",
              cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);      
     
      memset(fout,'\0',160);
      switch(code){
      /* main program */
         case  1:strcpy(fout,"Program ternimated by terminal\t");break;
         case  2:strcpy(fout,"Program ternimated because bad cfg\t");break;
         case  3:strcpy(fout,"Program ternimated by terminal\t");break;
         case  4:strcpy(fout,"CSAC clock is not adjusted/CSAC.txt cannot open\t");break;
         case  5:strcpy(fout,"too shor time before ping\t");break;
         case  6:strcpy(fout,"not yet wakeup time\t");break;
         case  9:strcpy(fout,"shutdown because battery voltage is too low\t");break;

         /* read_cfg.c */
         case  201:strcpy(fout,"Tomo.cfg file can not open\t");break;
         case  202:  case  203:  case  204:  case  205:  case  206:  case  207:
         case  208:  case  209:  case  210:  case  211:  case  212:  case  213:
         case  215:  case  216:  case  217:  case  218:  case  219:
                    strcpy(fout,"number check (Tomo.cfg) error\t");break;
                   
         case  221:strcpy(fout,"round_interval*total_st is greater than ping_interval\t");
                     break;
         case  222:strcpy(fout,"burst parameter error\t");break;
         case  223:strcpy(fout,"test_time format error\t");break;
         case  224:strcpy(fout,"start_time format error\t");break;
         case  225:strcpy(fout,"end_time format error\t");break;
         case  226:strcpy(fout,"right brace character error\t");break;
         case  227:strcpy(fout,"Nothing station\t");break;
         case  228:strcpy(fout,"total station is different with cfg station no\t");
                  break;
         case  229:strcpy(fout,"my_station can not found\t");break;
         
         /* calc_distance.c */         
         case  251:strcpy(fout,"too_far_distance(145km) error\t");break;
         
         /* modification_code_set.c */
         case  302:strcpy(fout,"code_file can not open\t");break;

         /* concurrent.c */
         case  401:  case 402:
               strcpy(fout,"data Memory alloc failed\t");break;
         case  403:
               strcpy(fout,"corr Memory alloc failed\t");break;

         /* round_robin.c */
         case  411:  case 412:
               strcpy(fout,"data Memory alloc failed\t");break;
         case  413:
               strcpy(fout,"corr Memory alloc failed\t");break;
         case  414:strcpy(fout,"countdown failed\t");break;

         /* wait_time.c */
         case  601:strcpy(fout,"CSAC has been reset\t\t");break;
         case  602:strcpy(fout,"time catch mis(CSAC)\t\t");break;
         case  603:strcpy(fout,"time catch mis(GPS)\t\t");break;

         /* save_corr.c & save_raw.c */
         case  701:strcpy(fout,"can not create corr file\t");break;
         case  702:strcpy(fout,"can not write corr file\t");break;
         case  703:strcpy(fout,"can not create raw file\t");break;
         case  704:strcpy(fout,"can not write raw file\t");break;

         /* user_mode */
         case  801:strcpy(fout,"CSAC.txt deleted manually\t");break;
         case  802:strcpy(fout,"runlog.txt deleted manually\t");break;
         default:strcpy(fout,"");
      }     /* switch end */
      sprintf(work," code=%4d ",code);

      strcat(fout,work);
      strcat(fout,pc_utc);
      printf("%s\x0d\x0a",fout);
      fprintf(fp1,"%s\x0d\x0a",fout);
      close(fp1);
}
void time_information_log(int value){
   int   pc_byou,cal[6];
   char  pc_utc[20],bf[80];   /* 2000.1.1からの経過秒 */
   int   fp1;

      memset(bf,'\0',80);
      strcpy(bf,record_media);
      strcat(bf,"clocklog.txt");
      fp1=open(bf,OptAppend);    /* 追加書き込みモード */
      if((int)fp1==-1)return;

      memset(fout,'\0',160);
      sprintf(fout,"clock error(usec)=%d\t",value);
      pc_byou=read_rtc_clock();
      (void)sec2date(pc_byou,cal);
      sprintf(pc_utc,"%4d %2d %2d %2d:%02d %02d\0",
              cal[0],cal[1],cal[2],cal[3],cal[4],cal[5]);      
     
      strcat(fout,pc_utc);
      fprintf(fp1,"%s\x0d\x0a",fout);
      close(fp1);
}
