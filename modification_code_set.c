/* KIOST tomo */
//#define  DEBUG
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mes2.h>

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int modification_code_set(int total_st,char *code_name);

/* **** External function declaration ************* */
extern void runlog(int code);

/* **** external(global) variable declaration ****** */
extern char record_media[];
extern int
      code_length,
      code_no[],
      m_seq[16][128];

/* If m_tabl[] is an auto variable, the stack will overflow */
static int m_table[4096];

int modification_code_set(int total_st,char *code_name){
   char code_file_name[20],df[80];
   int  i,j,k,n,mask,st,mseq_order;
   int  val,i1,atai;
            
   char  af,cf;
   int   fp;

      if((strcmp(code_name,"M6")==0) || (strcmp(code_name,"m6")==0)){
         strcpy(code_file_name,"M6.bin");mseq_order=6;
      }
      else if((strcmp(code_name,"M7")==0) || (strcmp(code_name,"m7")==0)){
         strcpy(code_file_name,"M7.bin");mseq_order=7;
      }
      else if((strcmp(code_name,"M8")==0) || (strcmp(code_name,"m8")==0)){
         strcpy(code_file_name,"M8.bin");mseq_order=8;
      }
      else if((strcmp(code_name,"M9")==0) || (strcmp(code_name,"m9")==0)){
         strcpy(code_file_name,"M9.bin");mseq_order=9;
      }
      else if((strcmp(code_name,"M10")==0) || (strcmp(code_name,"m10")==0)){
         strcpy(code_file_name,"m10.bin");mseq_order=10;
      }
      else if((strcmp(code_name,"M11")==0) || (strcmp(code_name,"m11")==0)){
         strcpy(code_file_name,"m11.bin");mseq_order=11;
      }
      else if((strcmp(code_name,"M12")==0) || (strcmp(code_name,"m12")==0)){
         strcpy(code_file_name,"m12.bin");mseq_order=12;
      }
      else if((strcmp(code_name,"G10")==0) || (strcmp(code_name,"g10")==0)){
         strcpy(code_file_name,"g10.bin");mseq_order=10;
      }
      else if((strcmp(code_name,"B12")==0) || (strcmp(code_name,"b12")==0)){
         strcpy(code_file_name,"b12.bin");mseq_order=12;
      }
      else return(-1);

      /* m_table[]の作成 */
      memset(df,0,sizeof(df));
      strcpy(df,record_media);
      strcat(df,code_file_name);
      fp=open(df,OptRead);
      if(fp!=-1) goto next1;
      cd("/rom0");
      strcpy(df,"/rom0/");
      strcat(df,code_file_name);
      fp=open(df,OptRead);
      if(fp==-1){
         printf("modification_code cannot open\x0d\x0a");
         runlog(302);
         return(-1);
      }
   next1:
      code_length=(1<<mseq_order)-1;
      for(i=0;i<code_length;++i) {
         af=(char)fgetc(fp);
         cf=(char)fgetc(fp);
         val=(unsigned char)cf*256+(unsigned char)af;
         m_table[i]=val;
      }
      close(fp);
           
/* 実験に使う局のmseqのコード取り出しそれを配列に作り替える */
/* m_sequence data is packed to 32bit(unsigned int) data */
/* 32bitデータとして再構築されるがLSBから順に格納されていく */
/* 読み出してくるfile(m10.bin)のコードデータはMSBがcode=0、LSBがcode=15 */
      for(st=0;st<total_st;++st){
         mask=1<<(15-code_no[st]);
         for(i=0;i<(int)((code_length+1)/32);i++) {
            atai=0;
            for(j=0;j<32;++j){
               k=32*i+j;
               n=m_table[k];
               i1=n & mask;
               if(i1!=0) i1=1;
               atai=atai+i1*(unsigned int)(1<<j);
            }
            m_seq[st][i]=atai; /* m_seq is global variable */
         }
      }
      printf("code file %s set normally\x0d\x0a",df);
      return(mseq_order);
}
