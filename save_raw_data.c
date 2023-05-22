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
extern char version[],my_station_name[],code_name[],str_latitude[],str_longitude[];
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
      strcat(path2file,"data/");
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
         case 1:
            fprintf(fd,"GPS operation\x0d\x0a");
            fprintf(fd,"position= %s %s\x0d\x0a",str_latitude,str_longitude);
            break;
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
/* �o�̓t�@�C���̖��O��ping_byou���Ԃ𗘗p���Č��߂�                          */
/* filemane=1A141235��Station1,10��14��12��35���̃f�[�^�ł��邱�Ƃ�����       */ 
/******************************************************************************/
extern int code_no[];
void make_file_name(int ping_byou){
   char  station[2],month1[2],day1[4],hour1[4],minute1[4];
   int   cal[6];

      sec2date(ping_byou,cal);
      /* cal[0]=year;cal[1]=month;cal[2]=day;cal[3]=hour;cal[4]=min; */
      /* day,hour,min��10�i�������ɕϊ��@ */
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

static int create_folder(int data, char folder_name[])
{
      char  bf[40];
      int   fd;

      memset(bf, '\0', 40);
      strcpy(bf, record_media);
      if (data)
          strcat(bf, "data/");
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

static int get_day_of_week(int y, int m, int d)
{
      static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
      y -= m < 3;
      return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

static void get_log_folder_name(char log_folder_name[])
{
      int   cal[6], present_sec, log_time_sec;
      int   day_of_week, days_to_trace_back;

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
      }
      /* week */
      else {
          if ((rolling_log_day < 0) || (rolling_log_day >= 7))
              rolling_log_day = 1;

          day_of_week = get_day_of_week(cal[0], cal[1], cal[2]);
          if (rolling_log_day != day_of_week) {
              days_to_trace_back = (day_of_week - rolling_log_day + 7) % 7;
              log_time_sec = present_sec - (days_to_trace_back * 86400);
              sec2date(log_time_sec, cal);
          }
      }

      // Folder name is 'YYYYMMDD'.
      sprintf(log_folder_name, "%4d%02d%02d", cal[0],cal[1],cal[2]);
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
          ret = create_folder(0, "data");
          if (ret == -1) {
              printf("[FOLDER ROLLING] Failed to create data folder\x0d\x0a");
              return -1;
          }
          else if (ret == 1)
              printf("[FOLDER ROLLING] data folder already exists(first)\x0d\x0a");
          else
              printf("[FOLDER ROLLING] Succeed to create data (first)\x0d\x0a");

          ret = create_folder(1, log_folder_name_temp);
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

          ret = create_folder(1, log_folder_name_temp);
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

static int mkdir_p(const char *full_path)
{
    int slash_pos[8];
    int i, num_slash, leng;
    int fd;
    char full_path_to_create[80];

    if (full_path[0] != '/')
        return -1;

    leng = strlen(full_path);
    num_slash = 0;
    for (i = 0; i < leng; i++) {
        if (full_path[i] == '/')
            slash_pos[num_slash++] = i;
    }
    // If the full_path has '/' at the end, then ignore it.
    if (slash_pos[num_slash - 1] == leng)
        num_slash--;
    // Full path should be under media folder such as /spc0, /mmc0
    if (num_slash < 2) {
        printf("[ERROR] Number of depths to create directory is less than 2. [%s]\x0d\x0a", full_path);
        return -1;
    }

    strcpy(full_path_to_create, full_path);

    // Create directory in higher position
    for (i = 2; i < num_slash; i++) {
        full_path_to_create[slash_pos[i]] = '\0';
        fd = open(full_path_to_create, OptRead);
        if (fd == -1) {
            if (__mkdir(full_path_to_create) != 0)
                return -1;
        }
        close(fd);

        full_path_to_create[slash_pos[i]] = '/';
    }

    // Create directory specified as the function argument
    fd = open(full_path_to_create, OptRead);
    if (fd == -1) {
        if (__mkdir(full_path_to_create) != 0)
            return -1;
    }
    close(fd);

    return 0;
}

static int move_file(const char *src_dir, const char *src_filename, const char *dst_dir)
{
    int fd_src, fd_dst;
    int ret;
    char src_filepath[80], dst_filepath[80];

    int tt[4];

    tt[0] = get_present_time();

    ret = mkdir_p(dst_dir);
    if (ret == -1) {
        printf("[MOVE RAW FILE] Failed to create data folder, %s\x0d\x0a", dst_dir);
        return -1;
    }

    sprintf(src_filepath, "%s/%s", src_dir, src_filename);
    sprintf(dst_filepath, "%s/%s", dst_dir, src_filename);

    ret = copy(src_filepath, dst_filepath);
    if (ret != 0) {
        printf("[MOVE RAW FILE] Error in copying file (%d), %s\x0d\x0a", ret , src_filepath);
        return ret;
    }

    ret = DeleteFile(src_filepath);
    if (ret != -1)
        printf("[MOVE RAW FILE] %s is deleted!\x0d\x0a", src_filepath);

    tt[1] = get_present_time();
    printf("[MOVE RAW FILE] Elapsed time: %dsec\x0d\x0a", tt[1] - tt[0]);

    return 0;
}

static int move_raw_files(const char *src_dir, const char *dst_dir, int num_to_move, int depth)
{
    int num_moved = 0;
    int n_iter = 0;

    int fd, filename_len;
    char full_name[80], dst_full_name[80];
    char fname[32], *ptr;
    DirEnt ent;

    fd = SetDirent(src_dir, &ent);
    if(fd == -1) {
        printf("Path not found\x0d\x0a");
        return -1;
    }
    num_moved = 0;
    while(NextRecord(fd, &ent) == 0) {
        if ((ent.attr & 0x10) && (ent.name[0] == '.'))
            continue;

        ptr = ent.name;
        if(get_longname(fd, fname, 31) > 0) ptr = fname;

        // length of name of the file (or folder)
        filename_len = strlen(ptr);

        // depth == 1, then get into subfolder
        if (depth == 1) {
            if (ent.attr & 0x10) {
                printf("[MOVE RAW FILE] Checking folder, [%s]\x0d\x0a", ptr);

                if ((num_to_move - num_moved) > 0) {
                    sprintf(full_name, "%s/%s", src_dir, ptr);
                    sprintf(dst_full_name, "%s/%s", dst_dir, ptr);
                    num_moved += move_raw_files(full_name, dst_full_name, num_to_move - num_moved, 0);
                }
            }
        }
        // if depth == 0, then check files only.
        else if ((ent.attr == 0x0) && (filename_len >= 5)) {
#define FILESIZE_LIMIT_MIN	(4096 * 4)
#define FILESIZE_LIMIT_MAX	(1024 * 512)

            if ((strcmp(ptr + filename_len - 4, ".raw") == 0)
                && (ent.length > FILESIZE_LIMIT_MIN)
                && (ent.length < FILESIZE_LIMIT_MAX)) {

                sprintf(full_name, "%s/%s", src_dir, ptr);
                sprintf(dst_full_name, "%s/%s", dst_dir, ptr);

                printf("[MOVE RAW FILE] src: [%s] => dst: [%s]\x0d\x0a", full_name, dst_full_name);
                if (move_file(src_dir, ptr, dst_dir) == 0)
                    num_moved++;
            }
        }

        if (num_moved >= num_to_move)
            break;
        if (++n_iter > 7)
            break;
    }
    close(fd);

    if (depth == 1)
        printf("[MOVE RAW FILE] Num. files moved=%d\x0d\x0a", num_moved);
    return num_moved;
}

int move_raw_files_to_usb(int num_to_move)
{
    char path2file[40];

    strcpy(path2file, record_media);
    strcat(path2file, "data");
    move_raw_files(path2file, "/spc0/data_bak", num_to_move, 1);
}
