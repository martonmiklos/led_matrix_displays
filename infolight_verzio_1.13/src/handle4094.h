void SFill (boolean bit){  //Fill the 595 data
   output_low(SCLOCK);
   if (bit==0){output_high(SDATA);} else {output_low(SDATA);}
   output_high(SCLOCK);
}//SFill


void SWrite () {output_high(HCLOCK); output_low(HCLOCK);}  //Data going output in 595


void SClear(){
byte i;
   For(i=0; i<47; i++) {SFill(0);} SWrite();
}//Clear 595
