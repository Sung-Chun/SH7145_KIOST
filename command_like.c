/* 20210730 KIOST */
#include <string.h>
#include <stdlib.h>
#include "SH7145.h"
#include <MES2.h>

/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
int  fgets(char *fin,int k,int fp);
int  gets(char *bf);
int  num_check(char *fin);
int  copy(char* fin, char* fout);
int  ismatch(const char *pat, const char *fn);
int  fnsplit(const char *path, char *dir, char *fn);
void delete_with_wildcard_expand(const char *pat);
int  dir_execute(char *dir_path,int attr,int display);
void warikomi(int warikomi_flag);
extern int  serSend(int port,int ch);
extern int  serRecv(int port);

int fgets(char *fin,int k,int fp){
   int j,f;
   char ch,cf[4];

      for(j=0;j<k;j++){
         f=read(fp,cf,1);
         if(f != 1){
            fin[j]=0;
            break;  
         }
         ch=(char)(cf[0] & 0xff);
         fin[j]=ch;
         if(ch==0x0a){
            fin[j+1]='\0'; break;
         }
      }
      return(j);
}

int gets(char *bf){  /* terminal(SCI2からの文字列入力 */
   int   i,k;
   char  ch=0;
   k=0;
      while(1){
         i=serRecv(1);
         if(i!=-1){
            ch=(unsigned char)i;
            bf[k]=ch;
            serSend(1,ch);
            ++k;
         }
         if(ch==0x0d){
            bf[k]=0;
            printf("%s",bf);
            return(k);
         }/* Carrage return code catch */
      }
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

#define  COPYBLKSIZ  512

int copy(char *source, char *destination) {
   int   fdin, fdout, n, c;
   char  *buf;

   buf = malloc(COPYBLKSIZ);
   if(buf == 0) return(-2);
   fdin = open(source, OptRead);
   if(fdin == -1) {
      free(buf);
      return(-3);
   }
   fdout = open(destination, OptWrite);
   if(fdout == -1) {
      close(fdin);
      free(buf);
      return(-4);
   }
   c = 0;
   do {
      n = read(fdin, buf, COPYBLKSIZ);
      write(fdout, buf, n);
      if(++c > 16) {
         c = 0;
      }
   } while(n == COPYBLKSIZ);
   close(fdin);
   close(fdout);
   free(buf);
   return(0);
}

/* wild card characterを使用してdelete fileを行う */
void delete_with_wildcard_expand(const char *pat){
   char dir[100],name[13],tmp[100];
   int fd;
   DirEnt ent;
   fnsplit(pat,dir,name);
   fd = SetDirent(dir,&ent);
   if(fd == -1) return;
   while( NextRecord(fd,&ent)==0 ){
      if(!(ent.attr & 0x10)){
         if(ismatch(name,ent.name)){
            sprintf(tmp,"%s%s",dir,ent.name);
            DeleteFile(tmp);
         }
      }
   }
   close(fd);
}

int fnsplit(const char *path, char *dir, char *fn){
   char *p="dummy";
   char *save;
   char dummy;
        
      for(p=&dummy; *p ; p++);
      for(p--; *p!='/' && p>path; p--);
      if(*p=='/'){
         save = ++p;
         for(; *p; p++,fn++) *fn = *p;
         *fn = 0;
         for(p=&dummy; p<save; p++,dir++) *dir = *p;
         *dir = 0;
         return 1;
      }else{
         *dir = 0;
         for(p=&dummy; *p; p++,fn++) *fn = *p;
         *fn = 0;
         return 0;
      }
}

#include <ctype.h>
int ismatch(const char *pat, const char *fn){
   char *p,*q;
   char dummy0,dummy1;
   
      dummy0=*pat;dummy1=*fn;
      p=&dummy0;q=&dummy1;

      while(*p){
         //printf("%d,%d\n",*p,*q);
         if(!*q) return 0;
         if(*p=='?'){
            p++; q++;
         }else if(*p=='*'){
            p++;
            if(!*p){
               return 1;
            }else{
               for(; *q && toupper(*q)!=toupper(*p); q++);
               if(!*q) return 0;
            }
         }else if(toupper(*p++) != toupper(*q++)){
            return 0;
         }
      }
      if(*q)
         return 0;
      else
         return 1;
}

static char f_name[1000][12];
int dir_execute(char *dir_path,int attr,int display){
   char  bf[12],c1,cf[10];
   int   fd,i=0,k,fmax,date_value_max,i1=0,i2=0,i3=0,i4=0;
   DirEnt  ent;
   char   *d_value,*order;

      k=0;
      fd=SetDirent(dir_path,&ent);
      if(fd == -1) return(-1);
      /* はじめの２個はcurrentと親directoryを示すので２個省く */
      if(NextRecord(fd, &ent)!=0){close(fd);return(-2);}
      if(NextRecord(fd, &ent)!=0){close(fd);return(-2);}

      while(NextRecord(fd, &ent) == 0) {
         switch(attr){
            case 0:
               if(ent.name[10]=='d'){
                  if(display==1){printf("i=%3d %s\r",i,ent.name);}
                  strcpy(f_name[k],ent.name);
                  ++k;
                  read(0,bf,10) ;
                  if(bf[0]==0x02){close(fd);return(-3);}/* Control+B catch */
               }
               break;
            case 1:
               if(ent.name[10]=='r'){          
                  if(display==1){printf("i=%3d %s\r",i,ent.name);}
                  strcpy(f_name[k],ent.name);
                  ++k;
                  read(0,bf,10) ;
                  if(bf[0]==0x02){close(fd);return(-4);}/* Control+B catch */
               }
               break;
         }  /* switch end */
      }     /* while end */
      close(fd);
      fmax=k;
      if(display==1) return(0);

      /* fileを時間の新しい順に並べ替えをする */
      fd=open("dir.dat",OptWrite);
      if(fd==0){
         printf("Cannot create dir.dat\r");
         return(-5);
      }      
      d_value = malloc(4*1000);
      if(d_value == 0){close(fd);return(-6);}
      order = malloc(4*1000);
      if(order == 0){close(fd);return(-7);}
   
      if(fmax>=1000) fmax=1000;
      for(i=0;i<fmax;++i){
         strcpy(bf,f_name[i]);
         c1=bf[1];
         if((c1< 0x39) && (c1 >0x30)) i1=c1 & 0x0f;
         else if (c1=='a') i1=10;                 
         else if (c1=='b') i1=10;                 
         else if (c1=='c') i1=10;                 

         cf[0]=bf[2];cf[1]=bf[3];cf[2]=0;
         i2=atoi(cf);
         cf[0]=bf[4];cf[1]=bf[5];cf[2]=0;
         i3=atoi(cf);
         cf[0]=bf[6];cf[1]=bf[7];cf[2]=0;
         i4=atoi(cf);
         d_value[i]=i1*1000000+i2*10000+i3*100+i4;
      }
      for(i=1;i<fmax;++i){  /* iは順位 */
         date_value_max=100000000;
         for(k=1;k<fmax;++k){
            i1=d_value[k];
            if(i1>date_value_max){
               i2=k;
               date_value_max=i1;
            }
         }
         d_value[i2]=0;  /* 今見つけたMaxmunを除外する */
         order[i]=i2;       /* 新しく順位iになったのはj番目のファイル */
      }

      for(i=0;i<fmax;++i){
         k=order[i];
         write(fd,f_name[k],8);
      }
      free(d_value);free(order);
      close(fd);
      return(0);     
}

void warikomi(int warikomi_flag){
   switch(warikomi_flag){
      case 0:  /*全部禁止*/
         asm("stc sr,r0");
         asm("or #0xf0,r0");
         asm("ldc  r0,sr");
         break;
      case 1:  /*全部許可 */
         asm("stc sr,r0");
         asm("and #0x0f,r0");
         asm("ldc r0,sr");
         break;
      case 2:  /*優先度15（AD変換）のみ可*/
         asm("stc sr,r0");
         asm("and #0x0f,r0");
         asm("or #0xe0,r0");
         asm("ldc  r0,sr");
         break;   
      }
   }
