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

/* Rolling log folder interval (number of days)
 *
 * This global variable is set from the configuration file */
char rolling_log_day_or_week;   /* Rolling log folder day('D') or week('W')  */
int  rolling_log_day;           /* Day of rolling log folder */
static char log_folder_name[12] = {'\0', };

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
      strcat(path2file,log_folder_name);
      strcat(path2file,"/");
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

static int create_folder(char folder_name[])
{
      char  bf[40];
      int   fd;

      memset(bf, '\0', 40);
      strcpy(bf, record_media);
      strcat(bf, folder_name);

      // Check if the folder name already exists.
      // If not, then create it.
      fd=open(bf, OptRead);
      if (fd == -1) {
          if (__mkdir(bf) != 0) {
              return -1;
          }
          // mkdir OK
          return 0;
      }

      close(fd);

      // Folder already exists
      return 1;
}

static void get_log_folder_name(char log_folder_name[])
{
      int   cal[6], present_sec;

      /* Get the present time */
      present_sec = get_present_time();
      sec2date(present_sec, cal);
      printf("[FOLDER ROLLING] present_time=  %4d/%02d/%02d  %2d:%02d:%02d\x0d\x0a",
         cal[0],cal[1],cal[2], cal[3],cal[4],cal[5]);

      /* day */
      if (rolling_log_day_or_week == 'D') {
          if ((rolling_log_day < 0) || (rolling_log_day > 28))
              rolling_log_day = 1;

          if (rolling_log_day != 0) {
              if (cal[2] < rolling_log_day) {
                  // If today is before the rolling_log_day,
                  // then go to the day of the last month.
                  cal[1]--;
                  if (cal[1] == 0) {
                      cal[1] = 12;
                      cal[0]--;
                  }
              }
              cal[2] = rolling_log_day;
          }
          // Folder name is 'YYYYMMDD'.
          sprintf(log_folder_name, "%4d%02d%02d", cal[0],cal[1],cal[2]);
      }
      /* week */
      else {
          // To be implemented (Below log folder name is temporarily set to today.)
          sprintf(log_folder_name, "%4d%02d%02d", cal[0],cal[1],cal[2]);
      }
}

int roll_log_folder_name()
{
      int   elapsed_sec_since_last_rolling;
      char  log_folder_name_temp[12];
      int   ret;

      /* Get the log folder name as of the day */
      get_log_folder_name(log_folder_name_temp);

      /* If no log_folder_name, then it's the first call.
       * Create new log folder */
      if (log_folder_name[0] == '\0') {
          printf("[FOLDER ROLLING] Set the log folder name and create it if no exists.\x0d\x0a");
          ret = create_folder(log_folder_name_temp);
          if (ret == -1) {
              printf("[FOLDER ROLLING] Failed to create new folder, %s\x0d\x0a", log_folder_name_temp);
              return -1;
          }
          else if (ret == 1)
              printf("[FOLDER ROLLING] Log folder already exists(first), %s\x0d\x0a", log_folder_name_temp);
          else
              printf("[FOLDER ROLLING] Succeed to create new folder (first), %s\x0d\x0a", log_folder_name_temp);

          strcpy(log_folder_name, log_folder_name_temp);

          return 0;
      }

      /* If the log folder name as of the day is different from the log_folder_name,
       * then do the log folder rolling. */
      if (strcmp(log_folder_name_temp, log_folder_name) != 0) {
          printf("[FOLDER ROLLING] Rolling log folder start\x0d\x0a");

          ret = create_folder(log_folder_name_temp);
          if (ret == -1) {
              printf("[FOLDER ROLLING] Failed to create new folder, %s\x0d\x0a", log_folder_name_temp);
              return -1;
          }
          else if (ret == 1)
              printf("[FOLDER ROLLING] Log folder already exists, %s\x0d\x0a", log_folder_name_temp);
          else
              printf("[FOLDER ROLLING] Succeed to create new folder, %s\x0d\x0a", log_folder_name_temp);

          strcpy(log_folder_name, log_folder_name_temp);
      }
      else
          printf("[FOLDER ROLLING] No change in the log folder name, %s\x0d\x0a", log_folder_name_temp);

      return 0;
}
