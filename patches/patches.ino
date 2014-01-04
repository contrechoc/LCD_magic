/**************************************
 *
 * script called PATCHES, for a Liquid Crystal of 4 lines of 16 characters
 *
 * several experiments to get more out of the Liquid Crystal Display
 *
 * contrechoc 2014
 *
 * feel free to copy, hack, modify
 *
 * the idea of patches is that there are regions with a certain kind of logic (here several text and format of text)
 * between these regions is "chaos"
 *
 * used is a buffer to make more "special effects" possible
 * problem: to combine special characters with text for the buffer
 *
 * used is another buffer with code for special effects to get bigger characters
 * problem: only 8 special characters possible limiting the smoothness in the "big" font.
 *
 * this code can be made more dense!
 *
 ***************************************/

#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#include <avr/pgmspace.h>

// sine lookup table pre-calculated
// from: http://interface.khm.de/index.php/lab/experiments/arduino-dds-sinewave-generator/
//

prog_uchar PROGMEM sinetable[256] = {
  128,131,134,137,140,143,146,149,152,156,159,162,165,168,171,174,
  176,179,182,185,188,191,193,196,199,201,204,206,209,211,213,216,
  218,220,222,224,226,228,230,232,234,236,237,239,240,242,243,245,
  246,247,248,249,250,251,252,252,253,254,254,255,255,255,255,255,
  255,255,255,255,255,255,254,254,253,252,252,251,250,249,248,247,
  246,245,243,242,240,239,237,236,234,232,230,228,226,224,222,220,
  218,216,213,211,209,206,204,201,199,196,193,191,188,185,182,179,
  176,174,171,168,165,162,159,156,152,149,146,143,140,137,134,131,
  128,124,121,118,115,112,109,106,103,99, 96, 93, 90, 87, 84, 81, 
  79, 76, 73, 70, 67, 64, 62, 59, 56, 54, 51, 49, 46, 44, 42, 39, 
  37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 18, 16, 15, 13, 12, 10, 
  9,  8,  7,  6,  5,  4,  3,  3,  2,  1,  1,  0,  0,  0,  0,  0,  
  0,  0,  0,  0,  0,  0,  1,  1,  2,  3,  3,  4,  5,  6,  7,  8,  
  9,  10, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 35, 
  37, 39, 42, 44, 46, 49, 51, 54, 56, 59, 62, 64, 67, 70, 73, 76, 
  79, 81, 84, 87, 90, 93, 96, 99, 103,106,109,112,115,118,121,124
};


// lookup table for output waveform
unsigned char wavetable[256];

byte t6[8]; // array for the special characters
unsigned char  g = 1;//used for alternating "random" special characters
int flip = 0;//used for alternating "random" special characters
int counter = 0;//used for alternating "random" special characters

char text[] = "fabrics for ever "; //to be changed of course :-)
char text1[] = "happy happyhap"; //to be changed of course :-)
char text2[] = "textile textil"; //to be changed of course :-)

//variables used to get alternating motives
int dir = 0; 
int textLength = 0; 
int whichText = 0;
int chaosNumber = 64;

//alternating the patches
int recycleNum = 11;

//array for generating a random sequence of numbers from 0 to 3
int posRandom[4] = {
  0,1,2,3};

//buffer, length is 17 because of the terminator of strings
char screen1[4][17] = { 
  "                ", "                ", "                ", "                " };

/**************************************
 *
 * function  to fill the wavetable from EEPROM
 *
 * 
 *
 ***************************************/

void sineWave()
{
  for (int i = 0; i < 256; ++i) {
    wavetable[i] = pgm_read_byte_near(sinetable + i);
  }
}

void setup() {

  sineWave();

  //
  // 32 x 2 seems the best way to initialize a 4 x 16 LCD see
  // http://web.alfredstate.edu/weimandn/lcd/lcd_addressing/lcd_addressing_index.html
  // 

  lcd.begin(32, 2);

  lcd.setCursor(0, 0); 
  lcd.clear(); 
  lcd.home(); 

  snow();snow();
  patches();

}

void loop() {

  /**************************************
   *
   * making random characters for the regions "in between" the words
   * because there are only 8 special characters possible, the characters "change" on the fly for the next loop
   *
   ***************************************/

  for ( int q = 0; q < 8; q++)
    if( g%2 == 0 )
      t6[q] = random(8)<<2;
    else
      t6[q] = random(64);

  lcd.createChar(g%17 + 1, t6);
  g++;

  /**************************************
   *
   *  flip - for alternating pieces of text, with random characters, textLength is varying
   * this code is not fundamental
   *
   * the source of the text is alternated in the function "reflip"
   ***************************************/

  if ( flip == 0){
    for ( int i = 0; i < 13; i++ ){
      lcd.setCursor(random (32), random(2));  
      doPrint(textLength);
      counter++;
      delay(10);
    }
  }
  else
  {
    for ( int i = 0; i < 13; i++ ){
      lcd.setCursor(random (32), random(2)); 
      lcd.write(counter%16 + 1);
      counter++;
    }
  }

  //
  //changing the textlength
  //

  if (dir == 0)
    textLength++;
  else
    textLength--;

  //
  //direction is changed as the textlength is either 0 or 12
  //  

  if (( textLength < 0)||(textLength > 12 )) flipDir();

  //  
  //starting the patches of special text, the transistion is using the backscreen to make it less abrupt
  //

  if ( counter >= chaosNumber ){
    reflip();
    counter = 0;
    if ( (recycleNum)%30 == 0 ) writeRecycle();
    if ( (recycleNum)%30 == 10 )writeMessage();
    if ( (recycleNum++)%30 == 20 )circle();
  }
  delay(100);

}

/**************************************
 *
 * function  reflip: alternaing the source for the text in one of the patches
 *
 * 
 *
 ***************************************/

void reflip(){
  if ( flip == 0 ){
    chaosNumber = 255;
    flip = 1;
    if ( (whichText++) % 2 == 0 ){
      for ( int i = 0; i < 13; i++ )
        text[i] = text1[i];
    }
    else
    {
      for ( int i = 0; i < 13; i++ )
        text[i] = text2[i];
    }
  }
  else{
    flip = 0;
    chaosNumber = 64;
  }
}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void flipDir(){
  if ( dir == 0 )
    dir = 1;
  else
    dir = 0;
}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void doPrint(int l){
  for ( int i = 0; i < l; i++ ){
    lcd.print(text[i]);
  }
}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void writeRecycle(){

  emptyBuffer();
  unsigned char nextAddress = 0;

  nextAddress = myStrCopy( 0,"Please          " , screen1[0], 16); //to be changed of course :-)
  nextAddress = myStrCopy( 0,"Recycle         " , screen1[1], 16); //to be changed of course :-)
  nextAddress = myStrCopy( 0,"me!!!!!!        " , screen1[2], 16); //to be changed of course :-)
  nextAddress = myStrCopy( 0,"   :-) ^_^ (-:  " , screen1[3], 16); //to be changed of course :-)

  bufferToScreen();

  delay(1000);

}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void writeMessage(){

  emptyBuffer();

  char a[4][14] = { 
    "Laugh ", "Love  ", "Live   " , ":-)   "                                          }; //to be changed of course :-)
  char b[4][14] = { 
    "well ", "often  ", "much  " , "^_^   "                                          }; //to be changed of course :-)
  int con = 2;
  while ( con-- > 0 ){
    getRandomFour();

    //
    //copy to the buffer
    //

    unsigned char nextAddress = 0;
    nextAddress = myStrCopy( 0, a[posRandom[0]], screen1[0], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[0]], screen1[0], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[1]], screen1[0], 6 );

    nextAddress = myStrCopy( 0, a[posRandom[1]], screen1[1], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[1]], screen1[1], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[1]], screen1[1], 6 );

    nextAddress = myStrCopy( 0, a[posRandom[2]], screen1[2], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[2]], screen1[2], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[2]], screen1[2], 6 );

    nextAddress = myStrCopy( 0, a[posRandom[3]], screen1[3], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[3]], screen1[3], 6 );
    nextAddress = myStrCopy(nextAddress , b[posRandom[3]], screen1[3], 6 );

    bufferToScreen();
    delay(1000);
  }
}

/**************************************
 *
 * function to generate an array of random numbers 
 * (can be improved!)
 * 
 *
 ***************************************/

void getRandomFour(){

  int pos[4] = { 
    random(4),random(4),random(4),random(4)                                        };
  int h = random(4);
  while ( h == pos[0] ) h = random(4);
  pos[1] = h;
  h = random(4);
  while ( (h == pos[0]) || (h == pos[1]) ) h = random(4);
  pos[2] = h;
  while ( (h == pos[0]) || (h == pos[1])|| (h == pos[2]) ) h = random(4);
  pos[3] = h;

  posRandom[0] = pos[0];
  posRandom[1] = pos[1];
  posRandom[2] = pos[2];
  posRandom[3] = pos[3];

}

/**************************************
 *
 * function to copy a char string into another char string at a certain position
 * 
 * (strCopy only copies one string into another)
 *
 ***************************************/

unsigned char myStrCopy( unsigned char startAddress,  char *arraySource,  char *arrayDest, unsigned char arrayLength){
  unsigned char counter = 0;
  while ( counter < arrayLength){
    arrayDest[ startAddress + counter ] = arraySource [ counter ];
    counter++;
  }
  return startAddress + counter;
}

/**************************************
 *
 * function  bufferToScreen to transfer text from backscreen to the lcd. This is done not at once, but in a random way.
 *
 * characters are randomly put from the buffer to the screen and after a while all is copied to the screen to make sure.
 *
 ***************************************/

void bufferToScreen(){
  int cc = 0;

  //255 times random writing of the characters of the buffer
  while ( cc< 255 ){
    int hx = random(4);
    int hy = random (16);

    int hpx = 0;//there is a function for this later on
    int hpy = 0;
    if ( hx == 1 || hx == 3)
      hpx = 1; 
    if ( hx > 1 )
      hpy = 16;
    lcd.setCursor(hy+hpy, hpx);

    lcd.print( screen1[hx][hy] );
    delay(10);
    cc++;
  }
  delay(100);

  //copy all to be sure all is shown
  lcd.setCursor(0, 0); 
  lcd.print( screen1[0] );
  lcd.setCursor(0, 1); 
  lcd.print( screen1[1] );
  lcd.setCursor(16, 0); 
  lcd.print( screen1[2] );
  lcd.setCursor(16, 1); 
  lcd.print( screen1[3] );

}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void emptyBuffer(){
  int c2 = 0;

  //this should "clean" the buffer, but creates a problem (Why???)

  /*
  while ( c2 < 17 ){
   screen1[0][c2] = ' ';
   screen1[1][c2] = ' ';
   screen1[2][c2] = ' ';
   screen1[3][c2] = ' ';
   c2++;
   }
   */

}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void circle(){
  emptyBuffer();

  for ( int i2 = 0; i2 < 255 ; i2+=5){
    int xp = 16*wavetable[  i2]/256;
    int yp = 4*wavetable[ (i2 + 64)%255]/256;
    screen1[yp][xp] = '*';
  }
  bufferToScreen();
  delay(500);

  unsigned char nextAddress = 0;

  nextAddress = myStrCopy( 2, "Threading", screen1[1], 9 );
  nextAddress = myStrCopy( 2, "the Universe", screen1[2], 12 );

  bufferToScreen();
  delay(1000);

}

/**************************************
 *
 * function  
 *
 * 
 *
 ***************************************/

void patches(){

  lcd.setCursor(0,0);

  //
  // creating the parts of the "big" letters
  //

  byte b1[8] = { //all black
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  byte b0[8] = {//all white
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };
  byte b3[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00000,
    0b00000,
    0b00000,
    0b00000
  };

  byte b2[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };

  //
  // this is all we need for the "PATCHES" text
  //

  lcd.createChar(1, b1);
  lcd.createChar(2, b0);
  lcd.createChar(3, b3);
  lcd.createChar(4, b2); 

  //
  // for the BEAM text we need some more special characters
  //

  byte b4[8] = {
    0b11000,
    0b11000,
    0b11000,
    0b11000,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  byte b5[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11100,
    0b11100,
    0b11100,
    0b11100
  };
  byte b6[8] = {
    0b00011,
    0b00011,
    0b00011,
    0b00011,
    0b11111,
    0b11111,
    0b11111,
    0b11111
  };
  byte b7[8] = {
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b00111,
    0b00111,
    0b00111,
    0b00111
  };


  lcd.createChar(5, b4);
  lcd.createChar(6, b5);
  lcd.createChar(7, b6);
  lcd.createChar(8, b7);

  //
  // coding of the first title
  //

  char beam[4][17] = { // word "BEAM" which should be changed: first screen shown once (fits on one screen)
    "1140111011101461", 
    "1210120010101751", 
    "1310130011101001", 
    "1150111010101001"                };

  //
  //write word "beam"
  //

  for ( int j = 0; j < 4; j++ )
  {
    for ( int i = 0; i < 17; i++ )
    {
      setCursorFor16_4(j,i);
      writeCharForBigWords(i, beam[j] );
    }
  }
  delay(2000);
  //the word patches in code
  unsigned char nextAddress = 0;
  nextAddress = myStrCopy( 2, "proudly        ", screen1[1], 14 );
  nextAddress = myStrCopy( 2, "presents       ", screen1[2], 14 );

  bufferToScreen();
  delay(2000);

  //
  // coding of the second title
  //

  char patches[4][28] = {  // word "PATCHES" which should be changed: in first screen shown once - in two halves
    "111011101110111010101110111", 
    "101010100100100012101200120", 
    "111011100100100013101300031", 
    "100010100100111010101110111"                      };
  //
  //the word "patcees" does not fit on this screen, so:
  //

  //write first half of word "patches"
  int cc = 0;
  while ( cc++ <  4 ){
    for ( int j = 0; j < 4; j++ )
    {
      for ( int i = 0; i < 16; i++ )
      {

        setCursorFor16_4(j,i);
        writeCharForBigWords(i, patches[j] );

      }
    }
    delay(2000 -cc*400 );
    //write second half of word "patches"

    for ( int j = 0; j < 4; j++ )
    {
      for ( int i = 0; i < 16; i++ )
      {
        setCursorFor16_4(j,i);
        writeCharForBigWords(i+11, patches[j] );

      }
    }
    delay(2000 -cc*400);

  }
}

/**************************************
 *
 * function  writeCharForBigWords, from the code the special character is printed
 *
 * 
 *
 ***************************************/

void writeCharForBigWords(int i, char *textBit){
  if ( textBit[i] == '1' ) { 
    lcd.write(1);
  }
  else if ( textBit[i] == '0' )  {  
    lcd.write(2);
  }
  else if ( textBit[i] == '2' )  {  
    lcd.write(4);
  }
  else if ( textBit[i] == '3' )  {  
    lcd.write(3);
  }
  else if ( textBit[i] == '4' )  {  
    lcd.write(5);
  }
  else if ( textBit[i] == '5' )  {  
    lcd.write(6);
  }
  else if ( textBit[i] == '6' )  {  
    lcd.write(7);
  }
  else if ( textBit[i] == '7' )  {  
    lcd.write(8);
  }
}

/**************************************
 *
 * function to set the cursor from horizontal, vertical
 *
 * specific for LCD 16 * 4
 *
 ***************************************/
void setCursorFor16_4(int j, int i){
  if ( (j>-1)&&(j<4)&&(i>-1)&&(i<16)){
  int hpx = 0;
  int hpy = 0;
  if ( j == 1 || j == 3)
    hpx = 1; 
  if ( j > 1 )
    hpy = 16;
  lcd.setCursor(i+hpy, hpx);
  }
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






























