////////////////////////////////////////////////////////////////////////////////
//                     Kijelzési effektek subrutinjai                         //
//                                                                            //
// Szükséges memo területek:                                                  //
// Disp_mem[]      - Ebben tárolódik a kijelzés alatt álló adat               //
// Eff_del_const   - effekt késleltetési állandó 16bit széles                 //
// Disp_Flag       - Léptetési effekt engedélyezõ bit                         //
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
void scr_rotate_left(){ //A teljes kijelzõt srollozza önmagával
//csak voidon belül használt változók
byte  temp_0,                                                                   //utolsó byte átmeneti tároló
      cikl_0;                                                                   //ciklus counter
int16 Effect_Delay;                                                             //Effekt késleltetés számlálója

   IF((Disp_Flag==1) & (Effect_Delay==0)){                                          //Léptetés 1-el balra, ha min. 1x volt teljes kijelzés és ha letelt a késleltetési idõ
      temp_0 = Disp_mem[0];                                                     //Elso bytot kimásol egy ideiglenes reg.-be
      For(cikl_0=0; cikl_0<47; cikl_0++){Disp_mem[cikl_0] = Disp_mem[cikl_0+1];}//For    0-46.byte-ig átléptet minden adatot az 1el elõtte levõ reg.-be
      Disp_mem[47] = temp_0;                                                    //Utolsó bytot feltölt az elsõ byte elõzõleg kimásolt értékével
      Effect_Delay = Eff_Del_Const;                                             //Effect_Delay feltöltése a konstans értékkel
      Disp_Flag=false;                                                          //Disp_Flag törlése
      }//IF
    else
      {IF(Effect_Delay>0){Effect_Delay--;}}                                     //Ha nem léptethetõ még a kijelzendõ érték, akkor a késleltetés csökken
}//scr_rotate_left
//******************************************************************************





//******************************************************************************
void one_bar_scr_rotate_left(byte last_byte){
//csak voidon belül használt változók
byte  cikl_0;                                                                   //ciklus counter

wait_for_flag:
   If(Disp_Flag==1){
      For(cikl_0=0; cikl_0<47; cikl_0++){Disp_mem[cikl_0] = Disp_mem[cikl_0+1];}//For    0-46.byte-ig átléptet minden adatot az 1el elõtte levõ reg.-be
      Disp_mem[47] = last_byte;                                                 //Utolsó bytot feltölti az áthozott last_byte értékével
      Disp_Flag=false;                                                          //Disp_Flag törlése
      Delay_ms(60);
   }//if
      else
      goto wait_for_flag;
}//one_bar_scr_rotate_left
//******************************************************************************





//******************************************************************************
void string_scr_rotate_left(){
byte str_ptr,                                                                   //string pointer
     chr_counter,
     i,
     chr_offset;


   chr_counter = 0;
   do {chr_counter++;} while (Disp_eff_array[chr_counter] != 126);               //ez a ciklus megnézi, hogy hanyadik elem a lezáró karakter a tömbben

   for(str_ptr=0; str_ptr<chr_counter; str_ptr++){                              //ez a ciklus olvassa végig a temp_string tömböt
      if(Disp_eff_array[str_ptr] != 126){
         if((Disp_eff_array[str_ptr] > 47)  &  (Disp_eff_array[str_ptr] < 58)){ //ha szám lesz a kijelzendõ karakter
            chr_offset = (disp_eff_array[str_ptr]-48)*5;
            for(i=0; i<5; i++){
               one_bar_scr_rotate_left(read_eeprom(chr_offset+i));
            }//for
         }//if szám

         if((disp_eff_array[str_ptr] > 64)  &  (disp_eff_array[str_ptr] < 91)){ //ha betû lesz a kijelzendõ karakter
            chr_offset = ((disp_eff_array[str_ptr]-65)*5)+50;
            for(i=0; i<5; i++){
               one_bar_scr_rotate_left(read_eeprom(chr_offset+i));
            }//for
         }//if betû
         
         if(disp_eff_array[str_ptr] == ' '){for(i=0; i<5; i++){one_bar_scr_rotate_left(0x00);}}
            else {one_bar_scr_rotate_left(0x00);}   //2 karakter közti hely
      }//if
      
   }//for   
   
   for(i=0; i<48; i++){Disp_eff_array[i]=0;}
   
}//one_chr_scr_rotate_left
//******************************************************************************




//******************************************************************************
void scr_disp_mem_clr(){   //display törlése
byte i;
   for(i=0; i<48; i++){Disp_mem[i]=0; spin_mask[i]=0;}
   
}//scr_mem_clr
//******************************************************************************





//******************************************************************************
void spin_up(){   //Felfelé forgatja a karaktert
byte i;

   for(spin_count=0; spin_count<8; spin_count++){
   
      for(i=0; i<48; i++){
         if(spin_mask[i]==0){
            shift_left(&Disp_mem[i], 1, bit_test(Disp_eff_array[i],7));
            rotate_left(&Disp_eff_array[i], 1);
         }//if
      }//for2

            Delay_ms(35);   

   }//for
}//spin_up
//******************************************************************************





//******************************************************************************
void ChrToDisp(byte chr, disp_offset, sel){  //Karaktert csinál a kijelzõre(karakter, kezdõ oszlop, ha sel=1 akkor közvetlen kijelzõre,
byte i;                                                                         //ha 0 akkor az effekt memóba kerül a karakter

   For(i=0; i<char_wide; i++){
      if(sel==1){Disp_mem[disp_offset+i] = read_eeprom(((chr)*char_wide)+i);}
      else {Disp_eff_array[disp_offset+i] = read_eeprom(((chr)*char_wide)+i);}
   }//For
}//ChrToDisp
//******************************************************************************





//******************************************************************************
void TimeToDisp(boolean msk_ovr_wrt){   //Ez kiírja az idõt a kijelzõre a Time_array tömbbõl
byte i;


   Time_array[5] = swap(Time_array[2] & 0b00110000);  //Hours
   Time_array[4] = Time_array[2]    &   0b00001111;   
   Time_array[3] = swap(Time_array[1] & 0b01110000);  //Minutes
   Time_array[2] = Time_array[1]    &   0b00001111;
   Time_array[1] = swap(Time_array[0] & 0b01110000);  //Seconds
   Time_array[0] = Time_array[0]    &   0b00001111;

   ChrToDisp(Time_array[5], 2, msk_ovr_wrt);
   ChrToDisp(Time_array[4], 8, msk_ovr_wrt);
   ChrToDisp(Time_array[3], 18, msk_ovr_wrt);
   ChrToDisp(Time_array[2], 24, msk_ovr_wrt);
   ChrToDisp(Time_array[1], 34, msk_ovr_wrt);
   ChrToDisp(Time_array[0], 40, msk_ovr_wrt);

   if(!msk_ovr_wrt){
   if(Last_time_array[4] == Time_array[5]){for(i=0; i<5; i++){spin_mask[i+2]=1;}}   else  {for(i=0; i<5; i++){spin_mask[i+2]=0;}}
   if(Last_time_array[3] == Time_array[4]){for(i=0; i<5; i++){spin_mask[i+8]=1;}}   else  {for(i=0; i<5; i++){spin_mask[i+8]=0;}}
   if(Last_time_array[2] == Time_array[3]){for(i=0; i<5; i++){spin_mask[i+18]=1;}}  else  {for(i=0; i<5; i++){spin_mask[i+18]=0;}}
   if(Last_time_array[1] == Time_array[2]){for(i=0; i<5; i++){spin_mask[i+24]=1;}}  else  {for(i=0; i<5; i++){spin_mask[i+24]=0;}}
   if(Last_time_array[0] == Time_array[1]){for(i=0; i<5; i++){spin_mask[i+34]=1;}}  else  {for(i=0; i<5; i++){spin_mask[i+34]=0;}}
   }

   if(!msk_ovr_wrt){
   spin_up();
   }//if

   //Kettõspontok visszaírása
   Delay_ms(150);
   Disp_mem[15]=0b00010010; Disp_mem[31]=0b00010010;
   
}//TimeToDisp
//******************************************************************************





//******************************************************************************
void DateToDisp(){
byte i, i1;

   disable_interrupts(INT_EXT);
   
   Time_array[5] = swap(Time_array[3] & 0b11110000);                            //Year's tens -> Time_array[5]
   Time_array[4] = Time_array[3]    &   0b00001111;                             //Year's ones -> Time_array[4]
   Time_array[3] = (Time_array[2] & 0b00001111);
   if(bit_test(Time_array[2],4)){Time_array[3] = Time_array[3] + 0b00001010;}   //Months in binary -> Time_array[3]
   Time_array[2] = swap(Time_array[1] & 0b00110000);                            //Date's tens -> Time_array[2]
   Time_array[1] = Time_array[1] & 0b00001111;                                  //Date's ones -> Time_array[1]
   
   scr_disp_mem_clr();

   Disp_eff_array[0] = '2';
   Disp_eff_array[1] = '0';
   Disp_eff_array[2] = Time_array[5]+48;
   Disp_eff_array[3] = Time_array[4]+48;
   Disp_eff_array[4] = ' ';
   
   switch (Time_array[3]){
      case 1:  for(i=0; i<januar[0];     i++){Disp_eff_array[i+5] = januar[i+1];}       break;
      case 2:  for(i=0; i<februar[0];    i++){Disp_eff_array[i+5] = februar[i+1];}      break;
      case 3:  for(i=0; i<marcius[0];    i++){Disp_eff_array[i+5] = marcius[i+1];}      break;
      case 4:  for(i=0; i<aprilis[0];    i++){Disp_eff_array[i+5] = aprilis[i+1];}      break;
      case 5:  for(i=0; i<majus[0];      i++){Disp_eff_array[i+5] = majus[i+1];}        break;
      case 6:  for(i=0; i<junius[0];     i++){Disp_eff_array[i+5] = junius[i+1];}       break;
      case 7:  for(i=0; i<julius[0];     i++){Disp_eff_array[i+5] = julius[i+1];}       break;
      case 8:  for(i=0; i<augusztus[0];  i++){Disp_eff_array[i+5] = augusztus[i+1];}    break;
      case 9:  for(i=0; i<szeptember[0]; i++){Disp_eff_array[i+5] = szeptember[i+1];}   break;
      case 10: for(i=0; i<oktober[0];    i++){Disp_eff_array[i+5] = oktober[i+1];}      break;
      case 11: for(i=0; i<november[0];   i++){Disp_eff_array[i+5] = november[i+1];}     break;
      case 12: for(i=0; i<december[0];   i++){Disp_eff_array[i+5] = december[i+1];}     break;
      default: {Disp_eff_array[5] = 'E';}                                               break;
   }//switch
   
   Disp_eff_array[5+i] = ' ';
   Disp_eff_array[6+i] = Time_array[2]+48;
   Disp_eff_array[7+i] = Time_array[1]+48;
   Disp_eff_array[8+i] = ' ';
   i1=i+9;
   
   
   switch (Time_array[0]){
      case 1:  for(i=0; i<vas[0]; i++){Disp_eff_array[i1+i] = vas[i+1];} break;
      case 2:  for(i=0; i<het[0]; i++){Disp_eff_array[i1+i] = het[i+1];} break;
      case 3:  for(i=0; i<ked[0]; i++){Disp_eff_array[i1+i] = ked[i+1];} break;
      case 4:  for(i=0; i<sze[0]; i++){Disp_eff_array[i1+i] = sze[i+1];} break;
      case 5:  for(i=0; i<csu[0]; i++){Disp_eff_array[i1+i] = csu[i+1];} break;
      case 6:  for(i=0; i<pen[0]; i++){Disp_eff_array[i1+i] = pen[i+1];} break;
      case 7:  for(i=0; i<szo[0]; i++){Disp_eff_array[i1+i] = szo[i+1];} break;
      default: {Disp_eff_array[i1] = 'E';}                               break;
   }//switch   



   i1=i1+i;
   for(i=0; i<10; i++){Disp_eff_array[i1+i]=' ';}
   Disp_eff_array[i1+i] = '~';

   string_scr_rotate_left();
   for(i=0; i<48; i++){spin_mask[i]=0;}
   Last_Time_array=10,10,10,10,10,10;
   enable_interrupts(INT_EXT);
}
//******************************************************************************





//******************************************************************************
void Time_setup(){
byte setup_pointer,
     disp_offset,
     i;
   
   Read_clk();    //idõ kiolvas
   TimeToDisp(0);  //idõ kiír a kijelzõre
   
   do{
      TimeToDisp(1); //Spin nélkül írja vissza az idõt
      Delay_ms(200);
      
      disp_offset = 2;
      For(i=0; i<char_wide; i++){Disp_mem[disp_offset+i] = 0;} //kipontozza a helyét az állítandó karakternek
      Delay_ms(200);
      scr_disp_mem_clr();
      
   } while (input(ok_sw));
}//Time_setup
//******************************************************************************
