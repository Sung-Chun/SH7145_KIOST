/* 20180810 Cat3 KIOST_CSAC—p‘•’u */
#include <MES2.h>
#include "SH7145.h"
#include "SH2MB.h"   /* SH2MB */
/* ****************************************************
 *  declaration of prototype function
 **************************************************** */
void Amp_power(int on_off);
void receive_relay_switch(int on_off);
void gps_power(int on_off);

void Amp_power(int on_off){
   switch(on_off){
      case 0:
         PBDR &= ~0x0200;     /* Analg Board Power OFF */
         pio16_1 &= ~0x0010;  /* receive relay OFF */
         break;
      case 1:           /* Analg Board Power ON */
         PBDR |= 0x0200;break;         
   }
}
void receive_relay_switch(int on_off){
   switch(on_off){
      case 0:           /* receive relay switch(Amp Board) OFF */
         pio16_1 &= ~0x0010;break;
      case 1:
         pio16_1 |= 0x0010;break;         
   }
}
void gps_power(int on_off){
   switch(on_off){
      case 0:           /* RasPi powerOnOff */
         pio16_1 &= ~0x0020;break;
      case 1:
         pio16_1 |= 0x0020;break;         
   }
}
