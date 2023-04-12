/* 2013 1.30 sampling freq(Hz) richtig_freq1000 is 1000 times value    */
#define  SEND_DATA_SIZE 1024
#include <string.h>
#include <mes2.h>

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int  save_raw_data(int ping_byou);
void make_file_name(int ping_byou);

/* **** External function declaration ************* */
extern void runlog(int code);
extern int sec2date(int byou,int cal[]);

/* **** external(global) variable declaration ***** */
extern char version[],my_station_name[],code_name[];
extern char file_name[],record_media[];
extern int
   my_station,code_repeat,mod_num,gps_operation,
   richtig_freq1000,
   superimpose_count,
   ad_start_count,
   save_pair_no;

extern short *p_data;

int save_raw_data(int ping_byou){
   int cal[6];
   int fd,ret,send_bytes;
   double d0;
   char path2file[40];
   char *p_d,*p_d_end;

      make_file_name(ping_byou);
      sec2date(ping_byou,cal);
      memset(path2file,0,sizeof(path2file));
      strcpy(path2file,record_media);
      strcat(path2file,"data/");
      strcat(path2file,file_name);
      strcat(path2file,".raw");

      fd=open(path2file,OptAppend);
      if(fd==-1){
         printf("%s cannot open\x0d\x0a",path2file);
         runlog(703);
         return(-1);
      }
      printf("Storing raw data ....\r");
      fprintf(fd,"%s\x0d\x0a",version);
      fprintf(fd,"My station= %s\x0d\x0a",my_station_name);
      fprintf(fd,"Mod number= %d code_name=%s\x0d\x0a",mod_num,code_name);
      fprintf(fd,"repeat number= %d %d\x0d\x0a",code_repeat,superimpose_count);
      switch(gps_operation){
         case 0:fprintf(fd,"CSAC operation\x0d\x0a");break;
         case 1:fprintf(fd,"GPS operation\x0d\x0a");break;
      }
      fprintf(fd,"Ping time= %5d %2d %2d %2d %2d\x0d\x0a",cal[0],cal[1],cal[2],
              cal[3],cal[4]);
      fprintf(fd,"Sampling freq*1000= %10d\x0d\x0a",richtig_freq1000);
      d0=((double)ad_start_count*1000000.0)/(double)(richtig_freq1000/1000.0);
      fprintf(fd,"data start time(usec)=%10d\x0d\x0a",(int)d0);
      fprintf(fd,"pair data no= %6d\x0d\x0a",save_pair_no);
      send_bytes=SEND_DATA_SIZE;
      p_d=(char *)p_data;p_d_end=p_d+save_pair_no*4;
      while(p_d<p_d_end){
         ret=write(fd,p_d,send_bytes);
         if(ret==-1){
            runlog(704);
            printf("Cannot write raw.data\x0d\x0a");
            close(fd);
            return(-1);
         }
         p_d+=SEND_DATA_SIZE;
         if(p_d+SEND_DATA_SIZE< p_d_end) continue;
         send_bytes=p_d_end-p_d;
      }
      close(fd);     
      return(2);
   }
/***** make_file_name *********************************************************/
/* 出力ファイルの名前をping_byou時間を利用して決める                          */
/* filemane=1A141235はStation1,10月14日12時35分のデータであることを示す       */ 
/******************************************************************************/
extern int code_no[];
void make_file_name(int ping_byou){
   char  station[2],month1[2],day1[4],hour1[4],minute1[4];
   int   cal[6];

      sec2date(ping_byou,cal);
      /* cal[0]=year;cal[1]=month;cal[2]=day;cal[3]=hour;cal[4]=min; */
      /* day,hour,minを10進数文字に変換　 */
      sprintf(station,"%1x",code_no[my_station]);
      sprintf(month1,"%1x",cal[1]);
      sprintf(day1,"%02d",cal[2]);
      sprintf(hour1,"%02d",cal[3]);
      sprintf(minute1,"%02d",cal[4]);
      strcpy(file_name,station);strcat(file_name,month1);
      strcat(file_name,day1);strcat(file_name,hour1);
      strcat(file_name,minute1);
      printf("file name=%s\x0d\x0a",file_name);
}
