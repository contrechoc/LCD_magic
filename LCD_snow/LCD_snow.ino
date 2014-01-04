#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {


  lcd.begin(32, 2);

  lcd.setCursor(0, 0); 
  lcd.clear(); 
  lcd.home(); 




}

void loop() {
  snow();
}


void snow(){
  byte snow[32];
  byte flake[4] = {
    0b01010,
    0b11111,
    0b11111,
    0b01010
  };

  for ( int falling = 0 ; falling < 32; falling++){ //insert the flake, make it start at -4 

    for (int i = -4 ; i<36; i++) //not 0 because of the  lcd.createChar(0, ) issue
      snow[i] = 0b00000;

    for (int j = 0 ; j<4 ; j++)
      if( ((falling + j) > -1) && ((falling + j)< 32))  
        snow[falling+j] = flake[j];

    //making four special characters
    for (int i = 1 ; i<5 ; i++) //not 0 because of the  lcd.createChar(0, ) issue
    {
      byte s1[8];
      for (int j = 0 ; j<8 ; j++)
        s1[j] = snow[(i-1)*8+j];
      lcd.createChar(i, s1);
      //make snowflakes appear on the screen
      
      //using the trick of rotating the chars to get another position
      setCursorFor16_4(i-1, 0) ; 
      lcd.write(i);
      setCursorFor16_4((i-1 +4 -3 )%4, 3) ; 
      lcd.write(i);
      setCursorFor16_4((i-1 +4 +2 )%4, 5) ; 
      lcd.write(i);
      setCursorFor16_4((i-1 +4 +1 )%4, 7) ; 
      lcd.write(i);
      setCursorFor16_4((i-1 +4 -1 )%4, 8) ; 
      lcd.write(i);
      setCursorFor16_4((i-1 +4 -4 )%4, 12) ; 
      lcd.write(i);
    }
    delay(100);
  }
}

/**************************************
 *
 * function to set the cursor from horizontal, vertical
 *
 * specific for LCD 16 * 4
 *
 ***************************************/
unsigned char setCursorFor16_4(int j, int i){
  if ( (j>-1)&&(j<4)&&(i>-1)&&(i<16)){
    int hpx = 0;
    int hpy = 0;
    if ( j == 1 || j == 3)
      hpx = 1; 
    if ( j > 1 )
      hpy = 16;
    lcd.setCursor(i+hpy, hpx);
    return 1;
  } 
  else return 0;
}


