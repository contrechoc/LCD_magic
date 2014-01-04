// looking at the characters of an LCD with a potentiometer

#include <LiquidCrystal.h>

int c = 0;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
byte t6[8];

void setup(){
  
   lcd.begin(32, 2);

  lcd.setCursor(0, 0); 
  lcd.clear(); 
  lcd.home(); 
  
  pinMode(14,OUTPUT);
    pinMode(15,OUTPUT);
    
    digitalWrite(14, LOW);
      digitalWrite(15, HIGH);
      
        for ( int q = 0; q < 8; q++)
 {
      t6[q] = 0b01010;
  lcd.createChar(q, t6);
}


  
}

 
void loop ()
{
  /*
  c+=4;
  
    lcd.setCursor (0, 0);
  for (int i = c*16; i< (c+1)*16; i++)
   lcd.print (char(i));
   
  lcd.setCursor (0, 1);
  for (int i = (c+1)*16; i< (c+2)*16; i++)
   lcd.print (char(i));
   
    lcd.setCursor (16, 0);
  for (int i =  (c+2)*16; i<  (c+3)*16; i++)
   lcd.print (char(i));
   
    lcd.setCursor (16, 1);
  for (int i = (c+3)*16; i<  (c+4)*16; i++)
   lcd.print (char(i));
   
   delay(10000);
   
   if ( c*16 > 255 ) c=0;
   */
   
   
  lcd.setCursor(0, 0); 
  lcd.clear(); 
  lcd.home(); 
   
   int c = 0;
   int hh = analogRead(A5)/4;
   
   if ( hh < 10 ) hh = 0;
   if ( hh > 245 ) hh = 245 - 48;
   
     lcd.setCursor (0, 0);
   lcd.print(char(hh));
     lcd.setCursor (10, 0);
   lcd.print(hh);
   
   
    lcd.setCursor (0, 1);
  for (int i = hh; i< hh + 16; i++)
   lcd.print (char(i));
   
    lcd.setCursor (16, 0);
  for (int i =  hh + 16; i<  hh + 32; i++)
   lcd.print (char(i));
   
     lcd.setCursor (16, 1);
  for (int i = hh + 32; i<  hh + 48; i++)
   lcd.print (char(i));
   
   delay(500);
   
}


