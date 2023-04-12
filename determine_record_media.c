/* 20210810 revised */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mes2.h>
/* **** Function prototype declaration ************ */
int determine_record_media(void){

/* **** External function declaration ************* */
extern char record_media[];

int determine_record_media(void){
   int   ret;

      serPuts(1,"\x0d\x0a");
      serPuts(1,"****** determin record media ******** \x0d\x0a");
      memset(work,0,sizeof(record_media));
      eject("spc0");ret=mount("spc0");sleep(15000);/* USB memory */
      if(ret==0){
         strcpy(record_media,"/spc0/");
         return(0);
      }
      eject("mmc0");ret=mount("mmc0");sleep(500);/* SD memory */
      if(ret==0){
         strcpy(record_media,"/mmc0/");
         return(0);
      }
      return(-1);
}
}
