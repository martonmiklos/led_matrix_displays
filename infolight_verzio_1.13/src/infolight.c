#include <16F873.h>
#fuses HS, NOWDT, PUT, NOPROTECT, NOBROWNOUT
#use delay(clock=12000000)
#use rs232 (baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#use i2c(master, sda=PIN_C4, scl=PIN_C3)


#define     SCLK     PIN_A2
#define     SDATA    PIN_A0
#define     STRBE    PIN_A1
#define     OE       PIN_A3
byte        buffer[48];
byte        k, l;





//*************************INTERRUPT*******************************************
#int_timer0
void timer0interrupt(){
byte i, j;

      if(k==7){k=0; l=128;} else {k++; rotate_right(&l,1);}
         output_low(STRBE);
         for(i=0; i<=5; i++){
            for(j=0; j<=7; j++){
               output_low(SCLK);
               if(bit_test(buffer[8*i+k],j)){output_high(SDATA);} else {output_low(SDATA);}
               output_high(SCLK);
            }//for_j
         }//for_i
         output_high(STRBE);
         output_b(l);

}//timer0

//*****************************************************************************





//*************************Main prog********************************************
void main(){
   byte i, chksum;

   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_4);
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_TIMER0);
   
   output_high(OE);
   output_low(SCLK);
   for(i=0; i<=47; i++){buffer[i] = 0;}
   l=128;
   k=1;
   


   while(1){
      chksum = 0;
      for(i=0; i<=47; i++){
         buffer[i] = getc();
         chksum = chksum + buffer[i];
      }//for
      putc(chksum);

   }//whl


}//main
//*****************************************************************************
