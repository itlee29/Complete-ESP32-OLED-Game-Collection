/*This is my full collection of my games!!! Everything is defined through namespaces, and in each namespace, the main functions (functioning as 
void loop and void setup are void update and void init)*/
/*TO ADD A GAME: Make a namespace, then update it on the indextogame, then update the maxnumberofgames variable, then update the void loop and void 
setup switch case blocks to incorperate the new game! Remember that #define does not work in namespaces, so games must use static const datatype*/
/*Note that in platformer, and battle cats, you can leave the game by moving the xjoy in any direction and pressing the joystick button.
In the other games, press the joybutton in the death page (chrome dino) or in the main game (in moon lander) or in the homepage (in music editor)*/
/*PINS:
USES AN ARDUINO NANO ESP32
Battery:
VIN->Bat+
GND->Bat-

OLED:
3.3V->VCC
GND->GND
SDA->A4
SCL->A5

Joystick:
Note - certain pins and controls may be reversed for different joysticks. Please calibrate to however you feel like it!
D3->switch
THESE MAY BE FLIPPED:
A1->Y
A2->X
3.3V->VCC
GND->GND

Button - uses keystudio button:
D2->S
3.3V->+
GND->-

Speaker - use any passive buzzer
D4->S
3.3V->+
GND->-
Note that the speaker is completely optional but I wrote (and copied) a few songs for it T_T
GPS - this can probably be removed if I want
TX - 5
RX - 6
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>
#include <Preferences.h>
#include "USB.h"//specialized keyboard library
#include "USBHIDKeyboard.h"
#include <TinyGPS++.h>//GPSSSS
const int TX_PIN = 5;
const int RX_PIN = 6;
USBHIDKeyboard Keyboard;
Preferences prefs;//this is the game saving mechanism
#define TICKSPEED 2//tick speed of the game - SET TO 40 FOR SAFETY
#define CLOCKSPEED 40//the clock speed of the game - basically I used deltatime to interpolate, however I need to set how many actual non-interpolated in-game ticks happen 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
//FOR THE JOYSTICK note that the joystick is meant to be played vertically (pins up) so xjoy and yjoy are flipped (A1 goes to y, A2 goes to x)
#define XJOY A1//the pin numbers of the x joystick and the y joystick
#define YJOY A2
#define BUTTONJOY 3//the joystick button
#define BUTTON 2//the pin number of the button
#define SPEAKER 4//the speaker pin
#define XTHRESHOLD 1024//the x threshold to go left and right for joystick
#define YTHRESHOLD 1024
#define CHEATPIN 12//Purely for fun - this is a cheat pin that unlocks some decently overpowered stuff in some games
#define OLED_CS   10
#define OLED_DC    9
#define OLED_RST   8
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC,OLED_RST,OLED_CS);//initialize screen
int currenttime = 0;
float deltatime = 0;//DELTATIME shows how many fractions of a tick happened in between frames.
int prevtime = 0;
#define DELTATIMECAP 2//if deltatime exceeds this, ignore deltatime and set it to deltatimecap
//SETTINGS----
bool invert = 0;//if the screen is being inverted
bool playsound = 1;//if the sound should be played
//initialize control functions - EDIT IF THE CONTROLS ARE REVERSED
/*if(analogRead(XJOY)-2048>=XTHRESHOLD&&left){//left
      playerx+=2048*XSPEED*deltatime;
    }
    if(analogRead(XJOY)-2048<=-XTHRESHOLD&&right){
      playerx-=2048*XSPEED*deltatime;
    }
    if(analogRead(YJOY)-2048<=-YTHRESHOLD&&up){//up
      playery+=2048*YSPEED*deltatime;
    }
    if(analogRead(YJOY)-2048>=YTHRESHOLD&&down){//down
      playery-=2048*YSPEED*deltatime;
    }*/
bool leftpressed(){
  return analogRead(XJOY)-2048>=XTHRESHOLD;
}
bool rightpressed(){
  return analogRead(XJOY)-2048<=-XTHRESHOLD;
}
bool uppressed(){
  return analogRead(YJOY)-2048<=-YTHRESHOLD;
}
bool downpressed(){
  return analogRead(YJOY)-2048>=YTHRESHOLD;
}
//FORWARD INITIALIZE MAIN PAGE
void MAIN_PAGE();//---------------------------------------------------------------------------------
//loading screen, just for the sake of it
//note to refreence this you have to do ::loading_screen();inside a namespace
//THEME SONG STUFF
//I extracted the notes and had Gemini impliment rythems! Note that I did not make this song - it was made by the PONOS corperation. I did not make this song because I am a coder, not a transcriber (although I do play piano quite well!)
int battlecatstheme[] = // The Battle Cats - Main Theme (Staccato Intro) - YEA THIS IS THE ACTUAL THEME!!!
// Format: { Frequency_Hz, Duration_ms, ... }
// Scaled uniformly at 130 BPM. 0 represents a rest.
{
    // ==========================================
    // --- INTRO SECTION (With 50ms gaps between F#'s) ---
    // ==========================================
    
    // Pattern 1: b, f#, f#, f#
    494, 231,  // B4
    370, 181,  // F#4 
    0,   50,   // Tiny Space
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 231,  // F#4 (Last one doesn't need a gap before the next B4)
    
    // Pattern 2: b, f#, f#, f#
    494, 231,  // B4
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 231,  // F#4
    
    // Pattern 3: b, f#, f#, f#
    494, 231,  // B4
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 231,  // F#4
    
    // Chromatic Walk Up 1: e, a, a#
    330, 231,  // E4
    440, 461,  // A4 (Held double time -> Quarter Note)
    466, 231,  // A#4
    
    // Pattern 4: b, f#, f#, f# 
    494, 231,  // B4
    370, 181,  // F#4 
    0,   50,   // Tiny Space
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 231,  // F#4
    
    // Pattern 5: b, f#, f#, f#
    494, 231,  // B4
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 231,  // F#4
    
    // Pattern 6: b, f#, f#, f# (Fast transition variant)
    494, 231,  // B4
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 181,  // F#4
    0,   50,   // Tiny Space
    370, 231,  // F#4

    // Chromatic Walk Up 2: e, a, a#
    330, 231,  // E4
    440, 461,  // A4 (Held double time -> Quarter Note)
    466, 231,  // A#4

    // ==========================================
    // --- CHORUS SECTION ---
    // ==========================================
    
    // Phrase 1: b, b, d, b, f, f#
    494, 461,  // 1st B4
    494, 231,  // 2nd B4
    587, 231,  // D5
    494, 231,  // 3rd B4
    698, 231,  // F5
    740, 461,  // F#5
    
    // Phrase 2: b, d, e, f#, f#, e, e, d, d, b
    494, 231,  // B4
    587, 231,  // D5
    659, 231,  // E5
    740, 231,  // 1st F#5
    740, 231,  // 2nd F#5
    659, 231,  // 1st E5
    659, 231,  // 2nd E5
    587, 231,  // 1st D5
    587, 231,  // 2nd D5
    494, 231,  // B4
    
    // Phrase 3: b, d, b, a, c#, a#
    494, 231,  // B4
    587, 231,  // D5
    494, 461,  // B4
    440, 231,  // A4
    554, 461,  // C#5
    466, 231,  // A#4
    
    // Phrase 4: b, b, d, b, f, f#
    494, 461,  // 1st B4
    494, 231,  // 2nd B4
    587, 231,  // D5
    494, 231,  // 3rd B4
    698, 231,  // F5
    740, 461,  // F#5
    
    // Phrase 5: a, f#, e, b, a, f#, a, f#, e
    440, 231,  // A4
    370, 231,  // F#4
    330, 231,  // E4  
    494, 231,  // B4
    440, 231,  // A4
    370, 231,  // F#4 
    440, 231,  // A4
    370, 231,  // F#4
    330, 231,  // E4  
    
    // Outro Cascade 1: f#, e, d, c#
    370, 231,  // F#4
    330, 231,  // E4
    294, 231,  // D4
    277, 231,  // C#4
    
    // Outro Cascade 2: f#, e, d, c#, b, a
    370, 115,  // F#4
    330, 115,  // E4
    294, 115,  // D4
    277, 115,  // C#4
    247, 115,  // B3
    220, 115,   // A3
    185, 115,  // F#3
    220, 115   // A3 Final
};

int battlecatssonglength = sizeof(battlecatstheme)/(2*sizeof(battlecatstheme[0]));//computes the size of the song - remember the 2* sizeof is because each note requires 2 elements, a value and a delay
int homepagetheme[] = {0,1000};
int homepagesonglength = sizeof(homepagetheme)/(2*sizeof(homepagetheme[0]));//computes the size of the song - remember the 2* sizeof is because each note requires 2 elements, a value and a delay
long prevnotetime = millis();
int currentnote = 0;
int currenttone = 0;
int timeincrement = 50;
void playsong(int song[],int songlength){
  if(playsound){//everything runs inside playsound - playsound determines if playsong even runs at all
    //always plays a tone! it just increments later.
    if(song[currentnote*2]>0&&currenttone!=song[currentnote*2])//the *2 gives you the note itself, and a 0 means that the note is not played
    {//in addition, this only plays notes if the song is actively changing notes!
      currenttone = song[currentnote*2];
      tone(SPEAKER,song[currentnote*2]);
    }
    else if(song[currentnote*2]==0){
      currenttone = 0;
      noTone(SPEAKER);
    }
    if(millis()-prevnotetime>=song[currentnote*2+1]){//currentnote*2+1 gives you the delay till the note
      prevnotetime = millis();
      //increment the note if it is not the end of the song, otherwise dont
      if(currentnote>=songlength-1){
        currentnote=0;
      }
      else{
        currentnote++;
      }
    }
  }
  else{
    noTone(SPEAKER);
  }
}
void loading_screen(){
  //loading screen, just for fun and just to waste people's time :) - also has some use to test the speed of the arduino, but mostly to waste people's time
  for(int i = 0;i<32;i++){
    display.clearDisplay();
    display.setCursor(32,12);
    display.print("Loading...");
    display.drawRect(32,24,64,16,WHITE);
    display.fillRect(32,24,i*2,16,WHITE);//filled in rectangle
    display.display();
    delay(20);
  }//waste 1 second of people's time
}
static const unsigned char PROGMEM arduino_logo[] =
  { 
  B00011000,B00011000,
  B00100100,B00100100,
  B01000010,B01011010,
  B10111101,B10111101,
  B10111101,B10111101,
  B01000010,B01011010,
  B00100100,B00100100,
  B00011000,B00011000};
void start_screen(){
  //also just for fun :) includes a few logos and stuff
  display.clearDisplay();
  if(digitalRead(CHEATPIN)){//yea cheating affects the homescreen too
    display.setCursor(4,12);
    display.print("OLED GAMING CONSOLE!");
    display.setCursor(26,24);
    display.print("By Isaac Lee");
    display.display();
    delay(2000);
  }
  else{
    for(int i = 0;i<100;i++){//the classic random text thing for cheats!
      display.setCursor(4,12);
      display.print("OLED GAMING CONSOLE!");
      display.setCursor(26,24);
      display.print("By Isaac Lee");
      display.setCursor(4,52);
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print("CHEATS ENABLED");
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.display();
      delay(20);
      display.clearDisplay();
    }
  }
  display.clearDisplay();
  display.setCursor(36,4);
  display.print("Made With:");
  display.drawBitmap(56,20,arduino_logo,16,8,WHITE);
  display.setCursor(44,32);
  display.print("Arduino");
  display.display();
  delay(2000);
  display.clearDisplay();
  loading_screen();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
namespace MUSIC_MAKER{
  const int songlength = 1024;
  int song[songlength];//this will be set later
  int convertedsong[songlength] = {0};
  int currentnote = 0;
  bool editingpitch = 1;//if you are editing pitch or duration
  String noteindex(int note){
    switch(note%12){
      case 0:
        return "A"+String(note/13);
      case 1:
        return "A#"+String(note/13);
      case 2:
        return "B"+String(note/13);
      case 3:
        return "C"+String(note/13);
      case 4:
        return "C#"+String(note/13);
      case 5:
        return "D"+String(note/13);
      case 6:
        return "D#"+String(note/13);
      case 7:
        return "E"+String(note/13);
      case 8:
        return "F"+String(note/13);
      case 9:
        return "F#"+String(note/13);
      case 10:
        return "G"+String(note/13);
      case 11:
        return "G#"+String(note/13);
    }
  }
  void editstaff(){
    noTone(SPEAKER);
    while(digitalRead(BUTTON)||digitalRead(BUTTONJOY))//while either the button or joybutton is pressed
    {
      display.setCursor(0,0);
      display.print("Edit Music!");
      display.setCursor(70,0);
      display.print("#");//print number
      display.print(currentnote+1);//play the index+1 because index starts at 0
      display.setCursor(0,20);
      display.print("Note: ");
      display.print(noteindex(song[currentnote*2]));//the *2 access the note, *2+1 access the duration
      display.setCursor(0,30);
      display.print("Duration: ");
      display.print(song[currentnote*2+1]);//access the duration of the note
      if(!digitalRead(BUTTONJOY)&&currentnote>0){
        currentnote--;
        delay(200);
      }
      if(!digitalRead(BUTTON)&&currentnote<19){
        currentnote++;
        delay(200);
      }
      else if(!digitalRead(BUTTON)&&currentnote>=19){
        break;//breaks the loop if the final enemy is selected
      }
      if(downpressed()||uppressed()){
        //switch from editing time or enemy
        editingpitch = !editingpitch;
        delay(200);
      }
      if(rightpressed()){
        //positive
        if(editingpitch){
          song[currentnote*2]++;
        }
        if(!editingpitch){
          song[currentnote*2+1]+=timeincrement;
        }
        delay(200);
      }
      if(leftpressed()){
        //negative
        if(editingpitch){
          song[currentnote*2]--;
        }
        if(!editingpitch&&song[currentnote*2+1]>0){
          song[currentnote*2+1]-=timeincrement;
        }
        delay(200);
      }
      if(editingpitch){
        display.drawRect(0,19,128,10,WHITE);
      }
      else{
        display.drawRect(0,29,128,10,WHITE);
      }
      //end draw
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    ::loading_screen();
    //convert the entire song
    for(int i = 0;i<songlength/2;i++){
      convertedsong[i*2]=27.5*pow(2,float(song[i*2])/12.0);//convert the song
    }
    for(int i = 0;i<songlength/2;i++){
      convertedsong[i*2+1]=song[i*2+1];
    }
  }
  void playsong(int song[],int songlength){//a namespaced playsong - is not affected by playsound because a music editor is useless if it doesnt play sound
    long prevnotetime = millis();
    int currentnotetoplay = 0;
    int currenttone = 0;
    while(digitalRead(BUTTON)&&currentnotetoplay<=songlength){
      display.setCursor(0,0);
      display.print("Playing...");
      display.setCursor(0,10);
      display.print("Press button to stop!");
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
      //always plays a tone! it just increments later.
      if(song[currentnotetoplay*2]>0&&currenttone!=song[currentnotetoplay*2])//the *2 gives you the note itself, and a 0 means that the note is not played
      {//in addition, this only plays notes if the song is actively changing notes!
        currenttone = song[currentnotetoplay*2];
        tone(SPEAKER,song[currentnotetoplay*2]);
      }
      else if(song[currentnotetoplay*2]==0){
        currenttone = 0;
        noTone(SPEAKER);
      }
      if(millis()-prevnotetime>=song[currentnotetoplay*2+1]){//currentnote*2+1 gives you the delay till the note
        prevnotetime = millis();
        //increment the note if it is not the end of the song, otherwise dont
        currentnotetoplay++;
      }
    }
  }
  void HOMEPAGE(){
    noTone(SPEAKER);
    bool play = 0;
    while(digitalRead(BUTTON)){
      if(!digitalRead(BUTTONJOY)){
        ::MAIN_PAGE();//returns to main page if both buttons are pressed
        return;//exit out of the function
      }
      display.setCursor(0,0);
      display.print("Music Editor!");
      display.setCursor(0,32);
      display.print("Edit");
      display.setCursor(64,32);
      display.print("Play");
      if(leftpressed()||rightpressed()){
        play = !play;
        delay(200);
      }
      if(play){
        display.drawRect(64,32,24,8,WHITE);
      }
      else{
        display.drawRect(0,32,24,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    if(play){
      ::loading_screen();
      playsong(convertedsong,sizeof(convertedsong)/(2*sizeof(convertedsong[0])));
    }
    else{
      ::loading_screen();
      editstaff();
    }
  }
  void init(){
    //set song to all 53, or A4
    for (int i = 0; i < songlength/2; i++) {
      song[i*2] = 60;
    }
  }
  void update(){
    HOMEPAGE();//returns to the homepage
  }
}
namespace BATTLE_CATS{
  //THEME SONG
  /*Note - to add cats: add their stats to the respective class, add and reference bitmaps. If they are part of the battlecats, instead of the enemy,
  you also have to update them in drawuibitmap and catcostindex to get everything!*/
  /*To add enemies, add their bitmap, and add their stats in the respective class. Then, on the editlevel function, add 1 to the scrolling up if statment when selecting enemies for the script.*/
  const int STAGEBORDER = 8;//how far from the edge the battlecats stay
  //BITMAPS
  static const unsigned char PROGMEM bitmapbase[] =
  { 
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000};
  static const unsigned char PROGMEM Catbase[] =
  { 
  B11111111,
  B00011111,
  B11111111,
  B00011000,
  B00100100,
  B01000010,
  B11111111,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B11111111};
  static const unsigned char PROGMEM Enemybase[] =
  { 
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B00100100,
  B01000010,
  B11111111,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B11111111};
  static const unsigned char PROGMEM cat_passive[] =
  { 
  B01111110,
  B10000001,
  B10100101,
  B10000001,
  B01111110,
  B00100100,
  B00100100,
  B00100100};
  static const unsigned char PROGMEM cat_attack[] =
  { 
  B01111110,
  B10000001,
  B10100101,
  B10000001,
  B01111110,
  B00000000,
  B00000000,
  B00000000};
  static const unsigned char PROGMEM wall_cat_passive[] =
  { 
  B01111110,
  B10000001,
  B10100101,
  B10000001,
  B10111101,
  B10000001,
  B01111110,
  B00100100};
  static const unsigned char PROGMEM wall_cat_attack[] =
  { 
  B00000000,
  B00000000,
  B01111110,
  B10000001,
  B10111101,
  B10000001,
  B01111110,
  B00100100};
  static const unsigned char PROGMEM brave_cat_passive[] =
  { 
  B00000000,
  B00000000,
  B00100000,
  B00100100,
  B00111010,
  B00001010,
  B00000100,
  B00001010};
  static const unsigned char PROGMEM brave_cat_attack[] =
  { 
  B00000000,
  B00000000,
  B10000000,
  B01000100,
  B00111010,
  B00001010,
  B00000100,
  B00001010};
  static const unsigned char PROGMEM assassin_bear_passive[] =
  { 
  B11110001,
  B01100011,
  B11110110,
  B10011100,
  B10011000,
  B11110000,
  B10010000,
  B10010000};
  static const unsigned char PROGMEM assassin_bear_attack[] =
  { 
  B11110000,
  B01100000,
  B11110000,
  B10011000,
  B10011100,
  B11110110,
  B10010011,
  B10010000};
  static const unsigned char PROGMEM ufo_cat_passive[] =
  { 
  B10000001,
  B01111110,
  B10000001,
  B01111110,
  B00101000,
  B01000100,
  B10000010,
  B00000000};
  static const unsigned char PROGMEM ufo_cat_attack[] =
  { 
  B10000001,
  B01111110,
  B10000001,
  B01111110,
  B00011000,
  B00110000,
  B01100000,
  B11000000};
  static const unsigned char PROGMEM kangeroo_passive[] =
  { 
  B0111000,
  B01100000,
  B01110000,
  B10001101,
  B10001010,
  B01110000,
  B01010000,
  B01010000};
  static const unsigned char PROGMEM kangeroo_attack[] =
  { 
  B0111000,
  B01100001,
  B01110010,
  B10001100,
  B10001000,
  B01110000,
  B01010000,
  B01010000};
  static const unsigned char PROGMEM bullet_train_cat_passive[] =
  { 
  B00000000,
  B00000000,
  B00000000,
  B00011111,
  B00101001,
  B01001001,
  B11111111,
  B01010101};
  static const unsigned char PROGMEM bullet_train_cat_attack[] =
  { 
  B00000000,
  B00110000,
  B01100000,
  B11011111,
  B10101001,
  B11001001,
  B01111111,
  B00110101};
  static const unsigned char PROGMEM doge_passive[] =
  { 
  B00000000,
  B11111100,
  B10000100,
  B10010100,
  B10000111,
  B10000111,
  B11111100,
  B01001000};
  static const unsigned char PROGMEM doge_attack[] =
  { 
  B00000000,
  B11111100,
  B10000111,
  B10010100,
  B10000100,
  B10000111,
  B11111100,
  B01001000};
  static const unsigned char PROGMEM drill_cat_passive[] =
  { 
  B00010000,
  B00101100,
  B01010010,
  B10111001,
  B10100001,
  B01010010,
  B00101100,
  B00010000};
  static const unsigned char PROGMEM drill_cat_attack[] =
  { 
  B01010000,
  B10101100,
  B01010010,
  B10111001,
  B10100001,
  B01010010,
  B10101100,
  B01010000};
  static const unsigned char PROGMEM hippoe_passive[] =
  { 
  B00000110,
  B11111100,
  B10000100,
  B10011111,
  B10010000,
  B11111111,
  B01001000,
  B01001000};
  static const unsigned char PROGMEM hippoe_attack[] =
  { 
  B00000000,
  B11111111,
  B10010000,
  B10011010,
  B10010101,
  B11111111,
  B01001000,
  B01001000};
  static const unsigned char PROGMEM gory_passive[] =
  { 
  B00000110,
  B00111111,
  B01000100,
  B10000100,
  B10000100,
  B11011000,
  B00100100,
  B01000010};
  static const unsigned char PROGMEM gory_attack[] =
  { 
  B00000110,
  B00111111,
  B01000100,
  B10000101,
  B10000110,
  B11011100,
  B00100000,
  B01000000};
  static const unsigned char PROGMEM teacher_bun_bun_passive[] =
  {
  B00000001,B00000000,
  B00000001,B01111000,
  B00000001,B10101000,
  B00111111,B11110000,
  B00111111,B11111000,
  B01111111,B11111100,
  B11110111,B11111100,
  B11101111,B11111110,
  B11101111,B11011110,
  B01110111,B10001110,
  B01111011,B10001110,
  B00111100,B00011100,
  B00011000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000
  };
  static const unsigned char PROGMEM teacher_bun_bun_attack[] =
  {
  B00000010,B00000000,
  B00000001,B00000000,
  B00000001,B01111000,
  B00000001,B10101001,
  B00111111,B11110011,
  B00111111,B11110111,
  B01111111,B11111111,
  B11110111,B11111100,
  B11101111,B11110000,
  B11101111,B11000000,
  B01110111,B10000000,
  B01111011,B10000000,
  B00111100,B00000000,
  B00011000,B00000000,
  B00000000,B00000000,
  B00000000,B00000000
  };
  static const unsigned char PROGMEM bahamut_cat_passive[] =
  { 
  B00000000,B00000000,
  B00000000,B00100000,
  B00010000,B00110000,
  B00110000,B00111000,
  B00111111,B00111100,
  B00111111,B01111110,
  B00111111,B11111110,
  B00111000,B11000110,
  B00100001,B10000010,
  B00100111,B11000000,
  B00011101,B11000000,
  B00001100,B11100000,
  B00001000,B10110000,
  B00000000,B10011000,
  B00000000,B00000000,
  B00000000,B00000000
  };
  static const unsigned char PROGMEM bahamut_cat_attack[] =
  { 
  B00000000,B00000000,
  B00000000,B00100000,
  B00010000,B00110000,
  B00110000,B00111000,
  B00111111,B00111100,
  B00111111,B01111110,
  B00111111,B11111110,
  B00111000,B11000110,
  B00100001,B10000010,
  B00100001,B11000000,
  B00011111,B11100000,
  B0011100,B10110000,
  B01110000,B10110000,
  B11100000,B11100000,
  B1100000,B00000000,
  B10000000,B00000000
  };
  static const unsigned char PROGMEM valkryie_cat_passive[] =
  { 
  B00000000,B10000000,
  B00000001,B00000000,
  B00000010,B00000000,
  B00000101,B11100000,
  B00101001,B11100000,
  B01010110,B01100000,
  B10101001,B11110000,
  B01010000,B11111000,
  B10100000,B11111111,
  B00000000,B11111111,
  B00000000,B11000111,
  B00000001,B10000001,
  B00000000,B11000000,
  B00000000,B01100000,
  B00000000,B00110000,
  B00000000,B00010000
  };
  static const unsigned char PROGMEM valkryie_cat_attack[] =
  { 
  B00000000,B00000000,
  B00000000,B00000000,
  B00000000,B00010000,
  B00000001,B11100000,
  B00001000,B11100000,
  B00001000,B01100000,
  B00001111,B11110000,
  B00001100,B11111000,
  B00010000,B11111111,
  B00010000,B11111111,
  B00010000,B11000111,
  B00010001,B10000001,
  B00100000,B11000000,
  B01110000,B01100000,
  B10101000,B00110000,
  B10101000,B00010000
  };
  static const unsigned char PROGMEM super_cat_passive[] =
  { 
  B00000000,B00000000,
  B00001000,B00010000,
  B00001100,B00110000,
  B00001111,B11110000,
  B00001000,B00010000,
  B00010010,B01001100,
  B00010000,B00001011,
  B00010010,B01001001,
  B00010001,B10001010,
  B00010000,B00001001,
  B00001000,B00010010,
  B00000111,B11100001,
  B00000010,B00100000,
  B00000010,B00100000,
  B00000001,B00010000,
  B00000001,B00010000};
  static const unsigned char PROGMEM super_cat_attack[] =
  { 
  B00000000,B00000000,
  B00001000,B00010000,
  B00001100,B00110000,
  B00001111,B11110000,
  B00001000,B00010000,
  B00010110,B11001100,
  B00001101,B10001011,
  B00011011,B00001001,
  B01110110,B10001010,
  B11001100,B01001001,
  B10011000,B00010010,
  B00110111,B11100001,
  B01100010,B00100000,
  B11000010,B00100000,
  B00000001,B00010000,
  B00000001,B00010000};
  //UI/MONEY-----------------------------------------------------------------------------------------------------------
  int maxmoney = 300;
  float moneyperframe = 0.16f;
  float money = 0;
  //------------------------------------------------------------------------------------------------------------------
  void drawuibitmap(int catindexnumber,int cost){
    if(catindexnumber == 0){
      if(digitalRead(CHEATPIN)){
        display.drawBitmap(0,48,cat_passive,8,8,WHITE);
        display.setCursor(32,44);
        display.print("Cost: ");
        display.print(cost);
        display.setCursor(32,56);
        display.print("Money: ");
        display.print(int(money));
      }
      else{
        display.drawBitmap(0,48,super_cat_attack,16,16,WHITE);
        display.setCursor(32,44);
        display.print("Cost: ");
        display.print(cost);
        display.setCursor(32,56);
        display.print("Money: ");
        display.print(int(money));
      }
    }
    else if(catindexnumber == 1){
      display.drawBitmap(0,48,wall_cat_passive,8,8,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
    else if(catindexnumber == 2){
      display.drawBitmap(0,48,brave_cat_passive,8,8,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
    else if(catindexnumber == 3){
      display.drawBitmap(0,48,ufo_cat_passive,8,8,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
      else if(catindexnumber == 4){
      display.drawBitmap(0,48,bullet_train_cat_passive,8,8,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
    else if(catindexnumber == 5){
      display.drawBitmap(0,48,drill_cat_passive,8,8,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
    else if(catindexnumber == 6){
      display.drawBitmap(0,40,valkryie_cat_passive,16,16,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
    else if(catindexnumber == 7){
      display.drawBitmap(0,40,bahamut_cat_passive,16,16,WHITE);
      display.setCursor(32,44);
      display.print("Cost: ");
      display.print(cost);
      display.setCursor(32,56);
      display.print("Money: ");
      display.print(int(money));

    }
  }
  int catcostindex(int index){//this is a function where if you input the index of the cat, it returns the cost
    switch(index){
      case 0:
        if(digitalRead(CHEATPIN)){
          return 15;
        }
        else{
          return -2147483647;//yea - if you cheat you instantly get money equal to the integer bit limit!
        }
      case 1:
        return 20;
      case 2:
        return 40;
      case 3:
        return 30;
      case 4:
        return 30;
      case 5:
        return 75;
      case 6:
        return 60;
      case 7:
        return 125;
    }
  }
  //CLASSES
  class enemy{
    public:
      //these are the cat's stats
      bool alive = 0;
      float hp = 0;
      int damage = 0;
      int damagepoint = 0;
      float xpos = 0;
      float ypos = 0;
      float speed = 0;
      //phases - how many frames the cat is passive and how many frames it attacks for
      int attackphase = 0;
      int passivephase = 0;
      float phasecounter = 0;//this counts how many frames have passed
      bool attacking = 0;//this tells if the player is attacking - takes up memory, but is helpfull for collision physics
      bool hit = 0;
      int standingdist = 0;
      int moneyondeath = 0;//how much money you gain on the death of this enemy
      const unsigned char* bitmap;
      const unsigned char* passivebitmap;//this is a pointer that gets assigned to a bitmap in init
      /*basically the way void attackphase is made up is it has a passive phase and an attack phase. In the passive phase, it just waits a certain
      number of frames, and during the attack phase, it goes through its attack animation. The passive phase always runs first, then the attack phase*/
      const unsigned char* attackbitmap;
      bool checkallcollisions();
      void checkhealthcollisions();
      void phase(){
        if(phasecounter>attackphase+passivephase){//basically cycles the phase counter around. The passive phase occurs first, then the attack phase
          phasecounter=0;
        }
        if(phasecounter<=passivephase){
          bitmap = passivebitmap;
          attacking = 0;
        }
        else if(phasecounter>=passivephase){
          bitmap = attackbitmap;
          attacking = 1;
        }
        //This only moves the cat when it has space to move
        if(!checkallcollisions()&&xpos+standingdist<SCREEN_WIDTH-8-STAGEBORDER){//the -8 is to account for the enemy being 8 pixils long
            xpos+= speed*deltatime;
            phasecounter = 0;
        }
        else{
          phasecounter+=deltatime;
        }
      }
      void frame(){
        phase();//run the phase check
        checkhealthcollisions();//run the health check
        if(passivebitmap != teacher_bun_bun_passive){
          display.drawBitmap(xpos,SCREEN_HEIGHT-ypos,bitmap,8,8,WHITE);
        }
        else{//this is special - it draws a 16x16 bitmap, which is TEACHER BUN BUN or other bosses
          display.drawBitmap(xpos-8,SCREEN_HEIGHT-ypos-8,bitmap,16,16,WHITE);
        }
        //kill cat
        if(hp <= 0){
          //basically if the cat just died, then you gain the money upon its death
          if(alive == 1){
            money+=moneyondeath;
          }
          alive = 0;
        }
      }
      void init(int catindexnumber){//this is a bool so that I can use the return function
        phasecounter = 0;//resets to the beginning of the passive phase
        if(catindexnumber == 0){
          passivebitmap = doge_passive;
          attackbitmap = doge_attack;
          hp = 20;
          damage = 5;
          damagepoint = 6;
          xpos = 0;
          ypos = 40;
          speed = 0.5;
          attackphase = 2;
          passivephase = 25;
          standingdist = 2;
          moneyondeath = 5;//how much money you gain on the death of this enemy
        }
        else if(catindexnumber == 1){
          passivebitmap = wall_cat_passive;
          attackbitmap = wall_cat_attack;
          hp = 40;
          damage = 1;
          damagepoint = 6;
          xpos = 0;
          ypos = 40;
          speed = 0.2;
          attackphase = 5;
          passivephase = 25;
          standingdist = 2;
          moneyondeath = 7;//how much money you gain on the death of this enemy
        }
        else if(catindexnumber == 2){
          passivebitmap = assassin_bear_passive;
          attackbitmap = assassin_bear_attack;
          hp = 1;
          damage = 5;
          damagepoint = 5;
          xpos = 0;
          ypos = 40;
          speed = 2.0;
          attackphase = 1;
          passivephase = 1;
          standingdist = 1;
          moneyondeath = 15;//how much money you gain on the death of this enemy
        }
        else if(catindexnumber == 3){
          passivebitmap = kangeroo_passive;
          attackbitmap = kangeroo_attack;
          hp = 5;
          damage = 5;
          damagepoint = 5;
          xpos = 0;
          ypos = 40;
          speed = 1.0;
          attackphase = 1;
          passivephase = 2;
          standingdist = 1;
          moneyondeath = 15;//how much money you gain on the death of this enemy
        }
        else if(catindexnumber == 4){
          passivebitmap = hippoe_passive;
          attackbitmap = hippoe_attack;
          hp = 50;
          damage = 1;
          damagepoint = 6;
          xpos = 0;
          ypos = 40;
          speed = 0.5;
          attackphase = 10;
          passivephase = 30;
          standingdist = 2;
          moneyondeath = 15;//how much money you gain on the death of this enemy
        }
        else if(catindexnumber == 5){
          passivebitmap = gory_passive;
          attackbitmap = gory_attack;
          hp = 20;
          damage = 5;
          damagepoint = 6;
          xpos = 0;
          ypos = 40;
          speed = 1.25;
          attackphase = 2;
          passivephase = 10;
          standingdist = 2;
          moneyondeath = 10;//how much money you gain on the death of this enemy
        }
        else if(catindexnumber == 6){
          passivebitmap = teacher_bun_bun_passive;
          attackbitmap = teacher_bun_bun_attack;
          hp = 200;
          damage = 10;
          damagepoint = 6;
          xpos = 0;
          ypos = 42;//floating
          speed = 0.1;
          attackphase = 2;
          passivephase = 10;
          standingdist = 2;
          moneyondeath = 50;//how much money you gain on the death of this enemy
        }
        alive = 1;
      }
  };
  class battlecat{
    public:
      //these are the cat's stats
      bool alive = 0;
      float hp = 0;
      int damage = 0;
      int damagepoint = 0;
      float xpos = 0;
      float ypos = 0;
      float speed = 0;
      //phases - how many frames the cat is passive and how many frames it attacks for
      int attackphase = 0;
      int passivephase = 0;
      float phasecounter = 0;//this counts how many frames have passed
      bool attacking = 0;//this tells if the player is attacking - takes up memory, but is helpfull for collision physics
      bool hit = 0;
      int standingdist = 0;
      bool piledrive = 0;//the piledrive ability
      const unsigned char* bitmap;
      const unsigned char* passivebitmap;//this is a pointer that gets assigned to a bitmap in init
      int catcost = 0;//how much this cat costs
      /*basically the way void attackphase is made up is it has a passive phase and an attack phase. In the passive phase, it just waits a certain
      number of frames, and during the attack phase, it goes through its attack animation. The passive phase always runs first, then the attack phase*/
      const unsigned char* attackbitmap;
      bool checkallcollisions();
      void checkhealthcollisions();
      void phase(){
        if(phasecounter>attackphase+passivephase){//basically cycles the phase counter around. The passive phase occurs first, then the attack phase
          phasecounter=0;
        }
        if(phasecounter<=passivephase){
          bitmap = passivebitmap;
          attacking = 0;
        }
        else if(phasecounter>=passivephase){
          bitmap = attackbitmap;
          attacking = 1;
        }
        //This only moves the cat when it has space to move
        if(!checkallcollisions()&&xpos-standingdist>STAGEBORDER){//the -8 is to account for the enemy being 8 pixils long - THIS ORDER OF CHECKS IS INCREDIBLY IMPORTANT
        //if xpos>stageboarder runs first, checkallcollisions doesnt run at all!
            xpos-= speed*deltatime;
            if(!piledrive){
              phasecounter = 0;//drill cat and bullet train cat run on different mechanics - they automatically attacks first, then waits passively. No other cat can do this
              //this also gives it a questionable piledrive maneuver where it can piledrive through multiple enemies at once, as long as it 1 taps.
              //It's kinda satisfying to watch, so I'll let it have that ability, however with a massive cooldown
            }
        }
        else{
          phasecounter+=deltatime;
        }
      }
      void frame(){
        phase();//run the phase check
        checkhealthcollisions();//run the health check
        if(passivebitmap != valkryie_cat_passive&&passivebitmap != bahamut_cat_passive&& passivebitmap!= super_cat_passive){
          display.drawBitmap(xpos,SCREEN_HEIGHT-ypos,bitmap,8,8,WHITE);
        }
        else{//this is special - it draws a 16x16 bitmap, which is TEACHER BUN BUN or other bosses or bahamut cat
          display.drawBitmap(xpos-8,SCREEN_HEIGHT-ypos-8,bitmap,16,16,WHITE);
        }
        //kill cat
        if(hp <= 0){
          alive = 0;
        }
      }
      bool init(int catindexnumber){
        phasecounter = 0;//resets to the beginning of the passive phase
        if(catindexnumber == 0){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = cat_passive;
          attackbitmap = cat_attack;
          hp = 20;
          damage = 5;
          damagepoint = -6;
          xpos = SCREEN_WIDTH-8;
          ypos = 40;
          speed = 0.5;
          attackphase = 2;
          passivephase = 25;
          standingdist = 2;
          piledrive = 0;
          if(!digitalRead(CHEATPIN)){
            //ok this is really funny, as it changes the cat's sprite into supercat (complete with a cape, and um yea its op)
            passivebitmap = super_cat_passive;
            attackbitmap = super_cat_attack;
            hp = 2147483647;//INTEGER BIT LIMIT... yes - it is um a cheat...
            damage = 2147483647;
            damagepoint = -6;
            xpos = SCREEN_WIDTH-8;
            ypos = 44;
            speed = 3;
            attackphase = 10;
            passivephase = 10;
            standingdist = 2;
            piledrive = 1;
          }
        }
        else if(catindexnumber == 1){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = wall_cat_passive;
          attackbitmap = wall_cat_attack;
          hp = 50;
          damage = 1;
          damagepoint = -6;
          xpos = SCREEN_WIDTH-8;
          ypos = 40;
          speed = 0.2;
          attackphase = 6;
          passivephase = 24;
          standingdist = 2;
          piledrive = 0;
        }
        else if(catindexnumber == 2){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = brave_cat_passive;
          attackbitmap = brave_cat_attack;
          hp = 15;
          damage = 6;
          damagepoint = -6;
          xpos = SCREEN_WIDTH-8;
          ypos = 40;
          speed = 0.4;
          attackphase = 5;
          passivephase = 20;
          standingdist = 2;
          piledrive = 0;
        }
        else if(catindexnumber == 3){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = ufo_cat_passive;
          attackbitmap = ufo_cat_attack;
          hp = 20;
          damage = 20;
          damagepoint = -6;
          xpos = SCREEN_WIDTH-8;
          ypos = 48;//as ufo cat floats, its ypos stat is higher
          speed = 0.4;
          attackphase = 2;
          passivephase = 50;
          standingdist = 4;
          piledrive = 0;
        }
        if(catindexnumber == 4){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = bullet_train_cat_passive;
          attackbitmap = bullet_train_cat_attack;
          hp = 15;
          damage = 10;
          damagepoint = -6;
          xpos = SCREEN_WIDTH-8;
          ypos = 40;
          speed = 2;
          attackphase = 2;
          passivephase = 100;
          standingdist = 4;
          phasecounter = passivephase;//THIS IS ONLY FOR BULLET TRAIN CAT AND DRILL CAT. REMOVE FOR ALL OTHER CATS. IT ENSURES HE ATTACKS INSTANTLY ONCE HE HITS OPPONENT
          piledrive = 1;//piledrive ability initiated
        }
        else if(catindexnumber == 5){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = drill_cat_passive;
          attackbitmap = drill_cat_attack;
          hp = 20;
          damage = 10;
          damagepoint = -6;//long range attacks
          xpos = SCREEN_WIDTH-8;
          ypos = 40;
          speed = 1.5;
          attackphase = 5;
          passivephase = 500;
          standingdist = 4;
          phasecounter = passivephase;//THIS IS ONLY FOR BULLET TRAIN CAT AND DRILL CAT. REMOVE FOR ALL OTHER CATS. IT ENSURES HE ATTACKS INSTANTLY ONCE HE HITS OPPONENT
          piledrive = 1;//piledrive ability initiated
        }
        else if(catindexnumber == 6){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = valkryie_cat_passive;
          attackbitmap = valkryie_cat_attack;
          hp = 50;
          damage = 10;
          damagepoint = -6;//long range attacks
          xpos = SCREEN_WIDTH-8;
          ypos = 44;//hovers, but does not fully fly
          speed = 1.5;
          attackphase = 20;
          passivephase = 50;
          standingdist = 4;
          piledrive = 0;
        }
        else if(catindexnumber == 7){
          catcost = catcostindex(catindexnumber);
          if(catcost<=money){
            money-=catcost;
          }
          else{
            return 0;
          }
          passivebitmap = bahamut_cat_passive;
          attackbitmap = bahamut_cat_attack;
          hp = 100;
          damage = 15;
          damagepoint = -6;//long range attacks
          xpos = SCREEN_WIDTH-8;
          ypos = 42;//slightly levitating
          speed = 2;
          attackphase = 5;
          passivephase = 30;
          standingdist = 4;
          piledrive = 0;
        }
        alive = 1;
        return 1;
      }
  };

  //define the list of battlecats and enemies--------------------------------------
  const int maxcats = 20;//the maximum cats allowed on the field at once
  const int maxenemies = 20;
  battlecat catlist[maxcats];
  enemy enemylist[maxenemies];

  bool battlecat::checkallcollisions(){
    for(enemy &currentenemy:enemylist){
      if(currentenemy.xpos<this->xpos-standingdist&&currentenemy.xpos+8>this->xpos-standingdist&&currentenemy.alive){
        return 1;
      }
    }
    return 0;//if absolutely no collisions are detected
  }
  void battlecat::checkhealthcollisions(){
    for(enemy &currentenemy:enemylist){
      if(currentenemy.attacking&&currentenemy.damagepoint+currentenemy.xpos+8>this->xpos&&currentenemy.damagepoint+currentenemy.xpos+8<this->xpos+8&&currentenemy.alive){
        this->hp-=currentenemy.damage*deltatime;//deltatime because it measures damage per frame
      }
    }
  }
  bool enemy::checkallcollisions(){
    for(battlecat &currentcat:catlist){
      if(currentcat.xpos<this->xpos+8+this->standingdist&&currentcat.xpos+8>this->xpos+8+this->standingdist&&currentcat.alive){
        return 1;
      }
    }
    return 0;//if absolutely no collisions are detected
  }
  void enemy::checkhealthcollisions(){
    for(battlecat &currentcat:catlist){
      if(currentcat.attacking&&currentcat.damagepoint+currentcat.xpos>this->xpos&&currentcat.damagepoint+currentcat.xpos<this->xpos+8&&currentcat.alive){
        this->hp-=currentcat.damage*deltatime;;//deltatime because it measures damage per frame
      }
    }
  }
  //VARIABLE DEFINITIONS
  const int hotbarlength = 8;
  int usablehotbarlength = 2;//this is how long the usable hot bar is - basically you unlock more of your hotbar as you progress
  //<<<<<<<<<<<<<<<<<<<<<<<This is very important (above) to allow the player to only use certain cats to begin with>>>>>>>>>>>>>>>>>>>>
  int hotbar[hotbarlength] = {0,1,2,3,4,5,6,7};
  int currenthotbarslot = 0;
  //button click mechanism - it makes it so the button has to be clicked and released
  bool buttonclick = 0;
  bool joymove = 0;//same as buttonclick but for the joystick
  //BASE HEALTH-------------------------------------------------------------------------------------------------------
  int catbasehealth = 500;
  int enemybasehealth = 500;
  //enemy randomness mechanism
  int randomenemy = 0;
  //LEVELING------------------------------------------------------------------------------------------------------------
  /*These dictate the enemy roster. They can be adjusted, for instance you can do time {1000,2000,1000}, and enemy {1,2,3}, and it will spawn 
  a wall cat after 1 second, then an assassin bear after 2 seconds, and then after another second, a kangeroo.*/
  const int numberofenemies = 20;//the number of enemies in a cycle
  int spawntracktime[numberofenemies] = {0};
  int spawntrackenemy[numberofenemies] = {0};
  //This is the custom level mechanism. It has nothing to apply to other than level 0 (custom)
  int customspawntracktime[numberofenemies] = {0};
  int customspawntrackenemy[numberofenemies] = {0};
  //custom stuff
  int customcatbasehealth = 0;
  int customenemybasehealth = 0;
  int custommaxmoney = 0;
  int customusablehotbarlength = 0;
  int customstartingmoney = 0;
  //------------
  const int maxlevel = 6;
  int level = 1;
  int lastcatsummontime = 0;//the last time a cat was summoned
  int currentcattosummon = 0;//the current index of the cat the enemy is trying to summon
  void resetcats(int catbase, int enemybase){
    //resets the health of the cat base and resets all cats
    catbasehealth = catbase;
    enemybasehealth = enemybase;
    money = 0;
    //set all cats to dead
    for(battlecat &cat:catlist){
      cat.alive = 0;
    }
    for(enemy &cat:enemylist){
      cat.alive = 0;
    }
    //sets hotbar slot to 1
    currenthotbarslot = 0;
  }
  //increments the level when called, kills all cats
  void beginlevel(){
    display.clearDisplay();
    //levels are defined here!!!-------------------------------------------------------------------------------------------------
    /*These dictate the enemy roster. They can be adjusted, for instance you can do time {1000,2000,1000}, and enemy {1,2,3}, and it will spawn 
    a wall cat after 1 second, then an assassin bear after 2 seconds, and then after another second, a kangeroo. Remember to also edit
    maxlevel! Note that the 19th cat will continuously spawn over and over and over again once the level ends!*/
    /*As a reminder, enemies are as follows:
    0:doge
    1:wall_cat
    2:assassin_bear
    3:kangeroo
    4:hippoe
    5:gory
    6:teacher_bun_bun*/
    //CUSTOM LEVEL!!!
    if(level == 0){
      resetcats(customcatbasehealth,customenemybasehealth);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      //sets spawntrackenemy and spawntracktime to the custom level
      usablehotbarlength = customusablehotbarlength;
      for(int i = 0;i<numberofenemies;i++){
        spawntracktime[i] = customspawntracktime[i];
        spawntrackenemy[i] = customspawntrackenemy[i];
      }
      maxmoney = custommaxmoney;
      money = customstartingmoney;
    }
    if(level == 1){
      usablehotbarlength = 2;//IMPORTANT - THIS ALLOWS THE PERSON TO GAIN MORE CATS!
      resetcats(500,500);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      spawntracktime[0] = 3000; spawntrackenemy[0] = 0;
      spawntracktime[1] = 1000; spawntrackenemy[1] = 0;
      spawntracktime[2] = 1000; spawntrackenemy[2] = 0;
      spawntracktime[3] = 3000; spawntrackenemy[3] = 0;
      spawntracktime[4] = 2000; spawntrackenemy[4] = 1;
      spawntracktime[5] = 2000; spawntrackenemy[5] = 1;
      spawntracktime[6] = 2000; spawntrackenemy[6] = 1;
      spawntracktime[7] = 2000; spawntrackenemy[7] = 0;
      spawntracktime[8] = 3000; spawntrackenemy[8] = 0;
      spawntracktime[9] = 3000; spawntrackenemy[9] = 0;
      spawntracktime[10] = 3000; spawntrackenemy[10] = 1;
      spawntracktime[11] = 4000; spawntrackenemy[11] = 1;
      spawntracktime[12] = 4000; spawntrackenemy[12] = 1;
      spawntracktime[13] = 5000; spawntrackenemy[13] = 0;
      spawntracktime[14] = 5000; spawntrackenemy[14] = 1;
      spawntracktime[15] = 6000; spawntrackenemy[15] = 1;
      spawntracktime[16] = 7000; spawntrackenemy[16] = 0;
      spawntracktime[17] = 8000; spawntrackenemy[17] = 0;
      spawntracktime[18] = 9000; spawntrackenemy[18] = 0;
      spawntracktime[19] = 10000; spawntrackenemy[19] = 0;
      //sets money
      maxmoney = 300;
      money = 0;
    }
    else if(level == 2){
      usablehotbarlength=3;//IMPORTANT - THIS ALLOWS THE PERSON TO GAIN MORE CATS!
      resetcats(500,500);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      spawntracktime[0] = 10000; spawntrackenemy[0] = 0;
      spawntracktime[1] = 1000; spawntrackenemy[1] = 0;
      spawntracktime[2] = 1000; spawntrackenemy[2] = 0;
      spawntracktime[3] = 1000; spawntrackenemy[3] = 1;
      spawntracktime[4] = 1000; spawntrackenemy[4] = 1;
      spawntracktime[5] = 100; spawntrackenemy[5] = 0;
      spawntracktime[6] = 100; spawntrackenemy[6] = 0;
      spawntracktime[7] = 3000; spawntrackenemy[7] = 1;
      spawntracktime[8] = 2000; spawntrackenemy[8] = 5;
      spawntracktime[9] = 2000; spawntrackenemy[9] = 1;
      spawntracktime[10] = 5000; spawntrackenemy[10] = 4;
      spawntracktime[11] = 10000; spawntrackenemy[11] = 5;
      spawntracktime[12] = 100; spawntrackenemy[12] = 0;
      spawntracktime[13] = 100; spawntrackenemy[13] = 0;
      spawntracktime[14] = 100; spawntrackenemy[14] = 0;
      spawntracktime[15] = 10000; spawntrackenemy[15] = 0;
      spawntracktime[16] = 3000; spawntrackenemy[16] = 0;
      spawntracktime[17] = 3000; spawntrackenemy[17] = 0;
      spawntracktime[18] = 3000; spawntrackenemy[18] = 0;
      spawntracktime[19] = 3000; spawntrackenemy[19] = 0;
      //sets money
      maxmoney = 300;
      money = 0;
    }
    else if(level == 3){
      usablehotbarlength=5;//IMPORTANT - THIS ALLOWS THE PERSON TO GAIN MORE CATS!
      resetcats(500,500);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      spawntracktime[0] = 5000; spawntrackenemy[0] = 0;
      spawntracktime[1] = 0; spawntrackenemy[1] = 0;
      spawntracktime[2] = 0; spawntrackenemy[2] = 0;
      spawntracktime[3] = 5000; spawntrackenemy[3] = 2;
      spawntracktime[4] = 1000; spawntrackenemy[4] = 1;
      spawntracktime[5] = 2000; spawntrackenemy[5] = 1;
      spawntracktime[6] = 3000; spawntrackenemy[6] = 1;
      spawntracktime[7] = 5000; spawntrackenemy[7] = 3;
      spawntracktime[8] = 5000; spawntrackenemy[8] = 3;
      spawntracktime[9] = 3000; spawntrackenemy[9] = 0;
      spawntracktime[10] = 3000; spawntrackenemy[10] = 0;
      spawntracktime[11] = 3000; spawntrackenemy[11] = 0;
      spawntracktime[12] = 3000; spawntrackenemy[12] = 4;
      spawntracktime[13] = 500; spawntrackenemy[13] = 0;
      spawntracktime[14] = 500; spawntrackenemy[14] = 0;
      spawntracktime[15] = 5000; spawntrackenemy[15] = 5;
      spawntracktime[16] = 2000; spawntrackenemy[16] = 5;
      spawntracktime[17] = 2000; spawntrackenemy[17] = 0;
      spawntracktime[18] = 2000; spawntrackenemy[18] = 0;
      spawntracktime[19] = 10000; spawntrackenemy[19] = 2;
      //sets money
      maxmoney = 500;
      money = 0;
    }
    else if(level == 4){
      usablehotbarlength=6;//IMPORTANT - THIS ALLOWS THE PERSON TO GAIN MORE CATS!
      resetcats(500,500);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      spawntracktime[0] = 5000; spawntrackenemy[0] = 0;
      spawntracktime[1] = 0; spawntrackenemy[1] = 0;
      spawntracktime[2] = 0; spawntrackenemy[2] = 0;
      spawntracktime[3] = 10000; spawntrackenemy[3] = 1;
      spawntracktime[4] = 1000; spawntrackenemy[4] = 1;
      spawntracktime[5] = 1000; spawntrackenemy[5] = 1;
      spawntracktime[6] = 1000; spawntrackenemy[6] = 1;
      spawntracktime[7] = 1000; spawntrackenemy[7] = 1;
      spawntracktime[8] = 1000; spawntrackenemy[8] = 1;
      spawntracktime[9] = 5000; spawntrackenemy[9] = 0;
      spawntracktime[10] = 0; spawntrackenemy[10] = 0;
      spawntracktime[11] = 0; spawntrackenemy[11] = 0;
      spawntracktime[12] = 10000; spawntrackenemy[12] = 2;
      spawntracktime[13] = 1000; spawntrackenemy[13] = 2;
      spawntracktime[14] = 1000; spawntrackenemy[14] = 2;
      spawntracktime[15] = 10000; spawntrackenemy[15] = 5;
      spawntracktime[16] = 2000; spawntrackenemy[16] = 5;
      spawntracktime[17] = 2000; spawntrackenemy[17] = 0;
      spawntracktime[18] = 2000; spawntrackenemy[18] = 0;
      spawntracktime[19] = 10000; spawntrackenemy[19] = 2;
      //sets money
      maxmoney = 500;
      money = 0;
    }
    else if(level == 5){
      usablehotbarlength=7;//IMPORTANT - THIS ALLOWS THE PERSON TO GAIN MORE CATS!
      resetcats(500,500);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      spawntracktime[0] = 10000; spawntrackenemy[0] = 2;
      spawntracktime[1] = 0; spawntrackenemy[1] = 1;
      spawntracktime[2] = 0; spawntrackenemy[2] = 1;
      spawntracktime[3] = 0; spawntrackenemy[3] = 1;
      spawntracktime[4] = 0; spawntrackenemy[4] = 1;
      spawntracktime[5] = 0; spawntrackenemy[5] = 1;
      spawntracktime[6] = 0; spawntrackenemy[6] = 1;
      spawntracktime[7] = 5000; spawntrackenemy[7] = 3;
      spawntracktime[8] = 1000; spawntrackenemy[8] = 3;
      spawntracktime[9] = 3000; spawntrackenemy[9] = 4;
      spawntracktime[10] = 0; spawntrackenemy[10] = 0;//WAVE ATTCACK
      spawntracktime[11] = 0; spawntrackenemy[11] = 0;
      spawntracktime[12] = 0; spawntrackenemy[12] = 0;
      spawntracktime[13] = 0; spawntrackenemy[13] = 0;
      spawntracktime[14] = 0; spawntrackenemy[14] = 0;
      spawntracktime[15] = 0; spawntrackenemy[15] = 0;
      spawntracktime[16] = 10000; spawntrackenemy[16] = 5;
      spawntracktime[17] = 2000; spawntrackenemy[17] = 5;
      spawntracktime[18] = 2000; spawntrackenemy[18] = 0;
      spawntracktime[19] = 5000; spawntrackenemy[19] = 3;
      //sets money
      maxmoney = 750;
      money = 0;
    }
    else if(level == 6){
      usablehotbarlength=8;//IMPORTANT - THIS ALLOWS THE PERSON TO GAIN MORE CATS!
      resetcats(500,500);//THIS IS WRITTEN CAT BASE HEALTH, ENEMY HEALTH. You can edit the healths of everything here!
      spawntracktime[0] = 5000; spawntrackenemy[0] = 0;
      spawntracktime[1] = 0; spawntrackenemy[1] = 0;
      spawntracktime[2] = 0; spawntrackenemy[2] = 0;
      spawntracktime[3] = 0; spawntrackenemy[3] = 2;
      spawntracktime[4] = 5000; spawntrackenemy[4] = 3;
      spawntracktime[5] = 1000; spawntrackenemy[5] = 3;
      spawntracktime[6] = 1000; spawntrackenemy[6] = 3;
      spawntracktime[7] = 5000; spawntrackenemy[7] = 4;
      spawntracktime[8] = 1000; spawntrackenemy[8] = 5;
      spawntracktime[9] = 1000; spawntrackenemy[9] = 5;
      spawntracktime[10] = 10000; spawntrackenemy[10] = 6;//HAVE FUN WITH THIS - TEACHER BUNBUN
      spawntracktime[11] = 1000; spawntrackenemy[11] = 2;
      spawntracktime[12] = 0; spawntrackenemy[12] = 2;
      spawntracktime[13] = 0; spawntrackenemy[13] = 2;
      spawntracktime[14] = 0; spawntrackenemy[14] = 2;
      spawntracktime[15] = 0; spawntrackenemy[15] = 2;
      spawntracktime[16] = 0; spawntrackenemy[16] = 2;
      spawntracktime[17] = 10000; spawntrackenemy[17] = 5;
      spawntracktime[18] = 3000; spawntrackenemy[18] = 5;
      spawntracktime[19] = 3000; spawntrackenemy[19] = 0;
      //sets money
      maxmoney = 1000;
      money = 0;
    }
    //rest summoning time
    lastcatsummontime = millis();//the number of milliseconds since the last cat was summoned
    currentcattosummon = 0;
    //--------------------------------------------------------------------------------------------------------------------------
  }
  void summonenemy(){
    if(millis()-lastcatsummontime>=spawntracktime[currentcattosummon]){
      //basically if the difference between the currrent time and the time of the last summoned cat is greater than the delay, summon cats
      for(enemy &enemypointer:enemylist){
        if(enemypointer.alive == 0){
          enemypointer.init(spawntrackenemy[currentcattosummon]);
          if(currentcattosummon<numberofenemies-1){//makes sure the game repeats the last enemy summoned
            currentcattosummon++;//this is in enemypointer.alive, because it ensures that the cat eventually gets summoned, even if the machine hits the enemy cap
          }
          lastcatsummontime = millis();
          break;
        }
      }
    }
  }
  //LEVEL EDITOR<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  void editlevel(int timespawn[],int enemyspawn[]){
    display.clearDisplay();
    display.setCursor(16,28);
    display.print("Editing Settings");
    display.display();
    delay(1000);
    display.clearDisplay();
    //this is the custom settings part of the level editor
    int currentelementtoedit = 0;//0 is catbasehealth, 1 is enemybasehealth, 2 is maxmoney, 3 is usablehotbarlength
    int basehealthincrement = 50;//increment base health in increments of this
    int moneyincrement = 50;//increment max money using this
    /*customcatbasehealth
    customenemybasehealth
    custommaxmoney
    customusablehotbarlength*/
    while(digitalRead(BUTTON)){
      //display the Level Editor (this goes first because drawing the box over it later requires the text to be written first)
      display.setCursor(0,0);
      display.print("Cat Base HP: ");
      display.print(customcatbasehealth);
      display.setCursor(0,10);
      display.print("Enemy Base HP: ");
      display.print(customenemybasehealth);
      display.setCursor(0,20);
      display.print("Maximum Money: ");
      display.print(custommaxmoney);
      display.setCursor(0,30);
      display.print("Hot Bar Length: ");
      display.print(customusablehotbarlength);
      display.setCursor(0,40);
      display.print("Starting Money: ");
      display.print(customstartingmoney);
      display.setCursor(1,50);
      display.print("Press Button To Cont.");
      //begin setting custom settings
      //remember the y axis is flipped
      if(downpressed()&&currentelementtoedit<4){//edit elements up to the final one
        currentelementtoedit++;
        delay(200);
      }
      if(uppressed()&&currentelementtoedit>0){
        currentelementtoedit--;
        delay(200);
      }
      switch(currentelementtoedit){
        case 0:
          if(leftpressed()&&customcatbasehealth>0){
            customcatbasehealth-=basehealthincrement;
            delay(200);
          }
          if(rightpressed()){
            customcatbasehealth+=basehealthincrement;
            delay(200);
          }
          display.drawRect(0,0,128,8,WHITE);
        break;
        case 1:
          if(leftpressed()&&customenemybasehealth>0){
            customenemybasehealth-=basehealthincrement;
            delay(200);
          }
          if(rightpressed()){
            customenemybasehealth+=basehealthincrement;
            delay(200);
          }
          display.drawRect(0,10,128,8,WHITE);
        break;
        case 2:
          if(leftpressed()&&custommaxmoney>0){
            custommaxmoney-=moneyincrement;
            delay(200);
          }
          if(rightpressed()){
            custommaxmoney+=moneyincrement;
            delay(200);
          }
          display.drawRect(0,20,128,8,WHITE);
        break;
        case 3:
          //fyi this is constrained between 0 and the usablehotbarlength inclusive
          if(leftpressed()&&customusablehotbarlength>0){
            customusablehotbarlength--;
            delay(200);
          }
          if(rightpressed()&&customusablehotbarlength<hotbarlength){//yea you can actually spawn with no cats allowed, for some reason
            customusablehotbarlength++;
            delay(200);
          }
          display.drawRect(0,30,128,8,WHITE);
        break;
        case 4:
          //fyi this is constrained between 0 and the usablehotbarlength inclusive
          if(leftpressed()){//yea money can actually become negative!!! You start with negative money so it gives a delay time before you can summon cats T_T
            customstartingmoney-=moneyincrement;
            delay(200);
          }
          if(rightpressed()){//yea you can actually spawn with no cats allowed, for some reason
            customstartingmoney+=moneyincrement;
            delay(200);
          }
          display.drawRect(0,40,128,8,WHITE);
        break;
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //run loading screen
    ::loading_screen();
    display.setCursor(16,0);
    display.print("Editing Sequence");
    display.display();
    delay(1000);
    display.clearDisplay();
    int currentlevelsection = 0;//this is the current section of the level
    int timeincrement = 500;//when clicking up on the joystick, this is the increment of time.
    bool editingtime = 0;//this tells if the player is editing the enemy (0) or editing the time (1)
    //This is the level editing part of the level editor
    while(currentlevelsection<numberofenemies-1||digitalRead(BUTTON)){
      //this basically repeats until someone presses the button after editing the level
      if(!digitalRead(BUTTONJOY)&&currentlevelsection>0){
        currentlevelsection--;
        delay(200);
      }
      if(!digitalRead(BUTTON)&&currentlevelsection<19){
        currentlevelsection++;
        delay(200);
      }
      else if(!digitalRead(BUTTON)&&currentlevelsection>=19){
        break;//breaks the loop if the final enemy is selected
      }
      if(leftpressed()||rightpressed()){
        //switch from editing time or enemy
        editingtime = !editingtime;
        delay(200);
      }
      if(uppressed()){
        //positive
        if(editingtime){
          timespawn[currentlevelsection]+=timeincrement;
        }
        if(!editingtime&&enemyspawn[currentlevelsection]<6){//NOTE - IF I CHANGE THE ENEMIES, EDIT THIS
          enemyspawn[currentlevelsection]++;
        }
        delay(200);
      }
      if(downpressed()){
        //negative
        if(editingtime&&timespawn[currentlevelsection]>0){
          timespawn[currentlevelsection]-=timeincrement;
        }
        if(!editingtime&&enemyspawn[currentlevelsection]>0){
          enemyspawn[currentlevelsection]--;
        }
        delay(200);
      }
      //render the level editor
      //uses a little trick - summons a dummy enemy, sets it's y and x positions, and then accesses it's bitmap without moving it using frame
      enemy dummy;//quite literally create an initiate a dummy enemy but dont trigger its frame
      dummy.init(enemyspawn[currentlevelsection]);
      display.setCursor(1,28);
      display.print("Enemy:");
      //renders the dummy enemy!
      if(dummy.passivebitmap != teacher_bun_bun_passive){
        display.drawBitmap(40,28,dummy.passivebitmap,8,8,WHITE);
      }
      else{//this is special - it draws a 16x16 bitmap, which is TEACHER BUN BUN or other bosses
        display.drawBitmap(40,24,dummy.passivebitmap,16,16,WHITE);
      }
      display.setCursor(60,28);
      display.print("Delay:");
      display.print(timespawn[currentlevelsection]);
      if(editingtime){
        display.drawRect(59,27,30,10,WHITE);
      }
      else{
        display.drawRect(0,27,30,10,WHITE);
      }
      display.setCursor(0,0);
      display.print("Element: ");
      display.print(currentlevelsection+1);//remember +1 because currentlevelselection includes 0 to 19, and +1 means 1 to 20
      if(currentlevelsection == numberofenemies-1){//shows the last level section to be edited
        display.setCursor(42,56);
        display.print("FINISH!");
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //display confirmation message
    display.clearDisplay();
    display.setCursor(32,32);
    display.print("Level Edited!");
    display.display();
    delay(2000);
    display.clearDisplay();
  }
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  void HOMEPAGE(){
    //the navigation homepage
    //joystick up and down to navigate up and down, button to select.
    /*NOTES - To do this, use setcursor and print, use MAXLEVEL to set the max scrolling, print the current level, the level before it above it, 
    and the next level under it. Change the variable level as you do this, and use a while loop until */
    noTone(SPEAKER);//NO MUSIC DURING HOMEPAGE
    while(digitalRead(BUTTON)!=0){
      if(!digitalRead(BUTTONJOY)&&(leftpressed()||rightpressed())){
        ::MAIN_PAGE();//returns to main page if both buttons are pressed
        return;//completely exits out of the homepage function
      }
      //clears the display
      display.clearDisplay();
      //while the button is not pressed
      //add computational logic here
      if(uppressed()&&level<maxlevel){
        //joystick up - cannot go over MAXLEVEL
        level++;
      }
      if(downpressed()&&level>0){
        //joystick down - cannot go under 1
        level--;
      }
      //add printing commands - print out the screen
      //prints out the current level
      display.setCursor(32,32);
      //level 0 is the custom level
      if(level!=0){
        display.print("Level: ");
        display.print(level);
      }
      else{
        //custom level!
        display.print("Level: ");
        display.print("Custom");
        display.setCursor(0,44);
        display.print("Both buttons: edit");
        display.setCursor(0,56);
        display.print("Main button: play");
      }
      //prints out previous level
      if(level>=1){
        //level>1+1 because if level is one, there is no previous level - prints below current level
        display.setCursor(72,48);
        if(level-1!=0){
          display.print(level-1);
        }
        else{
          //custom level!
          display.print("Custom");
        }
      }
      //prints out next level
      if(level<=maxlevel-1){
        //level>1+1 because if level is one, there is no previous level - prints below current level
        display.setCursor(72,16);
        display.print(level+1);
      }
      //prints out words "Homepage!"
      display.setCursor(0,8);
      display.print("THE BATTLE");
      display.setCursor(0,20);
      display.print("CATS LITE!");
      //displays
      display.display();
      delay(200);
    }
    //THIS IS IMPORTANT - IT MAKES IT SO IF YOU HAVE THE JOYSTICK BUTTON PRESSED TOO, YOU CAN EDIT THE LEVEL INSTEAD OF STARTING
    //REMEMBER JOYBUTTON IS INPUT_PULLUP
    if(level==0&&!digitalRead(BUTTONJOY)){
      display.clearDisplay();
      display.setCursor(32,32);
      display.print("Editing Level");
      display.display();
      delay(1000);//give time to enter the level editor
      display.clearDisplay();
      ::loading_screen();//to waste people's time
      editlevel(customspawntracktime,customspawntrackenemy);
      //after editing level, call HOMEPAGE again
      HOMEPAGE();
    }
    else{
      ::loading_screen();//to waste people's time
      beginlevel();
    }
    //
  }
  void nextlevel(){
    if(level<maxlevel){
      level++;
    }
    HOMEPAGE();
  }
  void init() {
    HOMEPAGE();//begins the level!
    ::prevtime = millis();
  }
  void update() {
    //main loop
    //MONEY
    if(money<maxmoney){
      money+=moneyperframe*deltatime;
    }
    //UHHH CHEATING AFFECTS MONEY TOO FYI
    if(!digitalRead(CHEATPIN)){
      money = 2147483647;//INTEGER BIT... 
    }
    //reset button and joy click
    if(digitalRead(BUTTON)){
      buttonclick = 0;
    }
    if(abs(analogRead(XJOY)-2048)<XTHRESHOLD){
      joymove = 0;
    }
    if(leftpressed()&&currenthotbarslot<hotbarlength-1&&currenthotbarslot<usablehotbarlength-1&&joymove==0){
      //note the usable hotbar slot - early in the game you only have access to like 1 or 2 enemies
      currenthotbarslot++;
      joymove = 1;
    }
    if(rightpressed()&&currenthotbarslot>0&&joymove==0){
      currenthotbarslot--;
      joymove = 1;
    }
    //use the UI, move the joystick
    if(!digitalRead(BUTTON)&&buttonclick == 0){
      buttonclick = 1;//it only registers a button press if the button has been released and then pressed
      for(battlecat &catpointer:catlist){
        if(catpointer.alive == 0){
          catpointer.init(hotbar[currenthotbarslot]);
          break;
        }
      }
    }
    for(battlecat &catpointer:catlist){
      if(catpointer.alive){
        catpointer.frame();
      }
    }
    for(enemy &enemypointer:enemylist){
      if(enemypointer.alive){
        enemypointer.frame();
      }
    }
    summonenemy();//SUMMON THE ENEMY CATSSS<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    //CAT AND ENEMY BASE HEALTH MODIFIERS
    //cat
    for(enemy &currentenemy:enemylist){
      if(currentenemy.attacking&&currentenemy.damagepoint+currentenemy.xpos+8>SCREEN_WIDTH-8&&currentenemy.damagepoint+currentenemy.xpos+8<SCREEN_WIDTH&&currentenemy.alive){
        catbasehealth-=currentenemy.damage*deltatime;
      }
    }
    if(catbasehealth<=0){
      display.clearDisplay();
      display.setCursor(SCREEN_WIDTH/2-25,SCREEN_HEIGHT/2);
      display.print("GAME OVER");
      display.display();
      noTone(SPEAKER);//turn off the music
      currentnote = 0;
      delay(3000);
      HOMEPAGE();
    }
    //enemy
    for(battlecat &currentcat:catlist){
      if(currentcat.attacking&&currentcat.damagepoint+currentcat.xpos>0&&currentcat.damagepoint+currentcat.xpos<0+8&&currentcat.alive){
        enemybasehealth-=currentcat.damage*deltatime;
      }
    }
    if(enemybasehealth<=0){
      display.clearDisplay();
      display.setCursor(SCREEN_WIDTH/2-25,SCREEN_HEIGHT/2);
      display.print("SUCCESS!");
      display.display();
      delay(2000);
      display.clearDisplay();
      nextlevel();
    }
    //draw cat and enemy bases
    display.drawBitmap(SCREEN_WIDTH-8,16,Catbase,8,16,WHITE);
    display.drawBitmap(0,16,Enemybase,8,16,WHITE);
    //draw cat and enemy base healths
    //draw enemy base health
    display.setCursor(0,0);
    display.print(enemybasehealth);
    display.setCursor(SCREEN_WIDTH-24,0);
    display.print(catbasehealth);
    //draw floor
    display.drawLine(0, 32, SCREEN_WIDTH, 32, WHITE);
    //draw current cat
    drawuibitmap(hotbar[currenthotbarslot],catcostindex(hotbar[currenthotbarslot]));//THIS DRAWS THE MONEY AS WELL AS THE CAT IMAGE AND THE COST!!!
    display.display();
    delay(2);
    display.clearDisplay();
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
namespace PLATFORMER{
  static const int  LEVELMAXUP = 16;//max height of level
  static const int  LEVELMAXRIGHT = 64;//max width towards right of level
  static const int  LEVELMAX = 9;//the number of levels
  static const int  GRAVITY = 1;//the gravity coeficent
  static const float  JUMP = 7.1;//how much the yvelocity increases by when jump is pressed -- EDIT -- I THINK THIS WORKS BETTER AS 7.2
  static const int  FLIGHTJUMP = 3;//when you're flying, you move pretty slow but um well fly
  static const float XSPEED = -1.5/1024;//the speed that the character goes left and right
  static const float YSPEED = 1.5/256;
  static const int ONSCREENX = 64;//the position on the screen of the character
  static const int ONSCREENY = 32;//from the bottom - note that with 32, the player is slightly up, which helps when falling- 24 is dead center
  static const int BCEXPANDX = 1;//the amound that the box collider should be expanded by to account for gothrough
  static const int BCEXPANDY = 1;//set to 1 because the frames are shorter
  static const int DOUBLEJUMPS = 1;//the number of times you can double jump - 1 means you only jump once T_T
  static const float PLAYERSTARTX = 8.0;//where the player starts
  static const float PLAYERSTARTY = 8.0;
  static const int FALLDEATH = -64;//if you fall 64 blocks below the stage, you die.
  static const int RISE = 1;//you can now rise from the ground if you get embedded by falling - also looks like a spring
  static const int ENEMYX = 2;//how far the enemy moves back and forth IN BLOCKS, NOT PIXILS
  static const int ENEMYY = 2;//how high the enemy moves up and down IN BLOCKS, NOT PIXILS
  static const int PLATFORMX = 8;//how far a platform moves horizontally IN BLOCKS, NOT PIXILS
  static const int PLATFORMY = 4;//how high a platform moves vertically IN BLOCKS, NOT PIXILS
  static const float PLATFORMSPEED = 1;//how fast a platform moves
  static const float ENEMYSPEED = 1;//every frame, the enemy moves a certain distance
  static const float BOUNCEADD = -0.1;//every time a bounce collider is hit, this is added to the bounce velocity to prevent infinite bounces
  static const float TERMINALVELOCITY = 10;//how much terminal velocity is decreased by from max terminal velocity
  static const float LAUNCHVELOCITY = 10.1;//the speed at which the launcher launches you
  static const float WATERBOYANCY = 0.3;//how much the water brings you up
  static const int LASERDIST = 6;//how far the laser travels
  static const float LASERSPEED = 2;
  static const int CONVEYORSPEED = 1;//how fast the conveyor moves you
  static const int POWERJUMP = 10;//how high the player jumps using the jump powerup
  static const int ORBJUMP = 8;//how high the player flies using a jump orb
  static const int POWERUPTIME = 500;//how long the powerup lasts
  static const int MAXSAVESLOTS = 10;//the max number of save slots allowed
  static const int GLOBALSWITCHFRAMES = 100;//how many frames the switch is allowed to be on for before shutting off. At 100, it's roughly 5 seconds, so it's roughly 20 fps
  static const int WARPCOOLDOWN = 0;//the cooldown from using the warp - at 20 it's roughly 0.25 seconds - There's no cooldown now because warps are one way now, with a warp to a warp platform. If there were a cooldown, then stuff like player cannons would stop functioning
  char saveslot = 0;/*THIS ALLOWS THE PLAYER TO USE CUSTOM LEVELS THEY MADE FROM FLASH. IT TELLS WHICH SAVE LEVEL THE PLAYER IS SAVING OR READING FROM
  //saveslot is a char fyi so it can save in flash easier, but it has to be converted to an int to be legible
  IF A LEVEL IS SAVED IN FLASH AND THE PLAYER CHOOSES LEVEL0, THEN THEY CAN CHANGE THIS VARIABLE. 
  saveslot = 0 means playing the level they made and have saved on sram, and other numbers mean playing more 
  custom levels saved in flash*/
  // - expands the number of platform types you can have to 17 instead of just 0-9
  enum TileMarkers : byte {
    A = 10,
    B = 11,
    C = 12,
    D = 13,
    E = 14,
    F = 15,
    G = 16,
    H = 17,
    I = 18,
    J = 19,
    K = 20,
    L = 21,
    M = 22,
    N = 23,
    O = 24,
    P = 25
  };
  /*static const unsigned char blank[] PROGMEM = {
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000
  };*/
  static const unsigned char leveleditsprite[] PROGMEM = {
    B11111111,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B11111111
};
  static const unsigned char Powerup[] PROGMEM = {
      B00111100,
      B01000010,
      B10011001,
      B10100101,
      B10100101,
      B10011001,
      B01000010,
      B00111100
  };
  static const unsigned char blasterright[] PROGMEM = {
      B00000000,
      B01110000,
      B10011111,
      B01110000,
      B11000000,
      B10110000,
      B10001100,
      B11111111
  };
  static const unsigned char blasterleft[] PROGMEM = {
      B00000000,
      B00001110,
      B11111001,
      B00001110,
      B00000011,
      B00001101,
      B00110001,
      B11111111
  };
  static const unsigned char laser[] PROGMEM = {
      B00000000,
      B01011010,
      B10100101,
      B01011010,
      B00000000,
      B00000000,
      B00000000,
      B00000000
  };
  static const unsigned char water[] PROGMEM = {
      B01000100,
      B10101010,
      B00010001,
      B00000000,
      B01000100,
      B10101010,
      B00010001,
      B00000000
  };
  static const unsigned char launcher[] PROGMEM = {
      B00111100,
      B11111111,
      B01000010,
      B00100100,
      B00011000,
      B00100100,
      B01000010,
      B11111111
  };
  static const unsigned char bounce[] PROGMEM = {
      B00000000,
      B00000000,
      B00111100,
      B11111111,
      B11100111,
      B10011001,
      B11100111,
      B11111111
  };
  static const unsigned char jumpthrough[] PROGMEM = {
      B11111111,
      B10100101,
      B11111111,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000
  };
  static const unsigned char enemy[] PROGMEM = {
      B00100100,
      B01111110,
      B11111111,
      B01111110,
      B01111110,
      B11111111,
      B01111110,
      B00100100
  };
  static const unsigned char end[] PROGMEM = {
      B11110000,
      B11011100,
      B11000111,
      B11011100,
      B11110000,
      B11000000,
      B11000000,
      B11000000
  };
  static const unsigned char player[] PROGMEM = {
      B11111111,
      B10000001,
      B10111101,
      B10100101,
      B10100101,
      B10111101,
      B10000001,
      B11111111
  };
  static const unsigned char wall[] PROGMEM = {
      B11111111,
      B11000011,
      B10100101,
      B10011001,
      B10011001,
      B10100101,
      B11000011,
      B11111111,
  };
  static const unsigned char spike[] PROGMEM = {
      B00011000,
      B00011000,
      B00111100,
      B00111100,
      B01111110,
      B01111110,
      B11111111,
      B11111111
  };
  static const unsigned char switch_off[] PROGMEM = {
      B00000000,
      B00000000,
      B11000000,
      B01100000,
      B00110000,
      B00011000,
      B01111110,
      B11111111
  };
  static const unsigned char switch_on[] PROGMEM = {
      B00000000,
      B00000000,
      B00000011,
      B00000110,
      B00001100,
      B00011000,
      B01111110,
      B11111111
  };
  static const unsigned char door_closed[] PROGMEM = {
      B11111111,
      B10000001,
      B10011001,
      B10111101,
      B10111101,
      B10011001,
      B10000001,
      B11111111
  };
  static const unsigned char door_open[] PROGMEM = {
      B10000001,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B10000001
  };
  static const unsigned char conveyor_right[] PROGMEM = {
      B01111110,
      B10100101,
      B01111110,
      B11111111,
      B10001001,
      B10111101,
      B10001001,
      B11111111
  };
  static const unsigned char conveyor_left[] PROGMEM = {
      B01111110,
      B10100101,
      B01111110,
      B11111111,
      B10010001,
      B10111101,
      B10010001,
      B11111111
  };
  static const unsigned char jump_orb[] PROGMEM = {
      B10000001,
      B01000010,
      B00011000,
      B00111100,
      B00111100,
      B00011000,
      B01000010,
      B10000001
  };
  static const unsigned char warpleft[] PROGMEM = {
      B00101100,
      B01010010,
      B01010010,
      B10100001,
      B10100001,
      B01010010,
      B01010010,
      B00101100
  };
  static const unsigned char warpright[] PROGMEM = {
    B00110100,
    B01001010,
    B01001010,
    B10000101,
    B10000101,
    B01001010,
    B01001010,
    B00110100
  };
  static const unsigned char warpup[] PROGMEM = {
    B00011000,
    B01100110,
    B10011001,
    B01100110,
    B10000001,
    B10000001,
    B01100110,
    B00011000
  };
  static const unsigned char warpdown[] PROGMEM = {
    B00011000,
    B01100110,
    B10000001,
    B10000001,
    B01100110,
    B10011001,
    B01100110,
    B00011000
  };
  static const unsigned char warpstop[] PROGMEM = {//this is a stop platform for warps
    B00000000,
    B01000010,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B01000010,
    B00000000
  };
  static const unsigned char warpparticle[] PROGMEM = {//this is a stop platform for warps
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B00100100,
    B00011000,
    B00000000,
    B00000000
  };
  String tileindex(int tile){ 
    switch(tile){
      //the x and y refer to sideways and up respectively
      case 0: return "Air";
      case 1: return "Block";
      case 2: return "Spike";
      case 3: return "X enemy";
      case 4: return "Y enemy";
      case 5: return "Level End";
      case 6: return "X platform";
      case 7: return "Y platform";
      case 8: return "OneWay Platform";
      case 9: return "Bounce Platform";
      case A: return "Launch Platform";
      case B: return "Water";
      case C: return "Right Blaster";
      case D: return "Left Blaster";
      case E: return "Powerup";
      case F: return "Switch";
      case G: return "Closed door";
      case H: return "Open door";
      case I: return "Right conveyor";
      case J: return "Left conveyor";
      case K: return "Jump orb";
      case L: return "Warp right";
      case M: return "Warp left";
      case N: return "Warp up";
      case O: return "Warp down";
      case P: return "Warp end";
      /*  WARPS - The way each warp works is that once you enter it, it uses a for loop to search for a corisponding warp stop in its direction (if it is a right warp, it searches right)
  Once it finds that warp stop, it will teleport the player. Warps are only 1 way - they only warp someone to the warp stop, not back*/
      return "n/a";
    }
  }
  //note that for the horizontal enemy, it starts on the left, vertical starts on bottom
  //note that byte is used instead of int because byte takes up 4x less memory
  //byte ranges numbers from 0 to 255 - I believe you can have around 20 or so levels
  /*static const byte Blanklevel[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
  };*/
  /*EXTRA LEVELS!!!:
  Basic Level - Very Easy to complete
  {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,A,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,4,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,5,},
  {0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,B,B,1,1,1,1,1,1,1,1,1,1,},
  {0,A,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,8,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,1,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,7,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,A,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,A,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {A,1,0,0,1,1,1,1,1,A,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,}

  */
  byte blankflashreadlevel[LEVELMAXUP][LEVELMAXRIGHT] = {//THIS IS THE LEVEL THAT IS READ FROM FLASH MEMORY. DO NOT EDIT EVER
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
  };
  byte level0[LEVELMAXUP][LEVELMAXRIGHT] = {//THIS LEVEL IS BLANK BECUSE IT IS STORED IN SRAM! In addition, it also does not have the const modifier so it's editable!
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,}
  };
  static const byte level1[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,2,0,0,0,2,1,1,1,2,2,1,1,0,0,1,0,0,0,2,1,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
  {1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,1,1,2,0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,1,1,1,0,0,2,0,0,1,1,2,2,2,1,1,1,1,0,0,0},
  {1,0,0,2,0,0,1,0,0,0,0,1,0,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,2,0,0,0,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,0,2,1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,2,0,0,2,0,0,0,0,0,0,0,0,1,1,0},
  {1,0,5,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
  {1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
  {1,0,0,0,1,1,1,1,0,0,0,0,1,2,2,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0},
  {1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,0,2,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},
  {1,1,1,1,1,1,1,1,2,2,2,2,1,0,0,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,2,2,1,2,1,2,1,1,1,1,2,2,2,1,1,1,1,1,1,1},
  };
  static const byte level2[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,C,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,0,2,0,0,0,0,0,2,B,B,B,B,B,B,B,B,B,B,B,B,2,0,},
  {0,1,6,1,1,1,1,1,1,1,1,1,C,0,0,2,0,0,0,2,C,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,2,2,2,0,0,0,2,0,0,0,0,2,B,B,B,B,B,B,2,2,B,B,B,B,B,B,2,},
  {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,A,0,0,0,0,9,9,2,0,0,0,0,0,2,0,0,0,2,B,B,B,B,B,B,2,0,0,2,B,B,B,B,B,B,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,2,B,B,B,B,B,B,2,0,0,0,2,B,B,B,B,B,B,},
  {A,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,2,B,B,B,B,B,B,2,0,0,0,0,2,B,B,B,B,B,B,},
  {0,0,6,0,0,0,0,0,0,1,6,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,2,0,2,B,B,B,B,B,B,2,0,0,0,0,2,B,B,B,B,B,B,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,D,C,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,2,2,0,2,B,B,B,B,B,B,2,0,0,0,0,2,B,B,B,B,B,B,},
  {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,2,2,2,D,C,A,A,A,A,A,A,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,2,B,B,B,B,B,B,2,0,0,0,2,B,B,B,B,B,B,2,},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,2,B,B,B,B,B,B,2,0,0,2,B,B,B,B,B,B,2,0,},
  {0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,2,B,B,B,B,B,B,2,0,2,B,B,B,B,B,B,2,0,0,},
  {0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,7,0,0,0,0,0,0,2,2,0,0,0,0,2,0,2,B,B,B,B,B,B,2,2,B,B,B,B,B,B,2,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,2,2,0,0,0,1,1,1,B,B,B,B,B,B,2,B,B,B,B,B,B,2,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,B,B,B,B,B,B,B,B,B,2,2,B,B,B,B,B,B,1,1,1,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,B,B,B,B,B,B,B,B,B,2,0,2,B,B,B,B,B,B,0,0,0,},
  {0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,B,B,B,B,B,B,2,0,0,2,B,B,B,B,B,B,0,5,},
  {1,1,1,1,0,2,0,9,0,2,0,9,0,2,0,9,0,2,0,9,0,0,0,9,0,2,0,9,0,2,0,1,1,1,A,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,2,1,1,1,1,1,1,1,}
  };
  static const byte level3[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,9,0,0,0,9,0,0,0,A,0,0,0,2,0,0,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,A,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,A,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,1,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,C,B,B,B,B,B,B,B,B,B,B,B,B,B,1,0,0,0,0,0,A,9,9,9,0,0,0,0,0,0,0,0,3,1,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,B,B,B,B,B,B,B,B,B,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,A,1,B,B,B,B,B,B,B,B,B,B,B,B,B,D,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,A,0,0,0,1,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,B,B,B,B,B,B,B,B,B,B,B,1,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,1,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,C,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,B,B,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,0,0,A,A,A,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,4,1,1,1,A,0,0,0,0,0,1,5,0,0,1,0,},
  {1,1,1,1,1,A,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,}
  };
  static const byte level4[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2,0,0,0,1,0,0,4,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,0,0,0,1,0,0,A,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,A,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,9,9,9,0,0,0,9,9,9,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,7,0,0,0,0,0,1,0,0,0,0,2,1,A,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2,2,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,},
  {0,0,0,0,6,0,0,0,0,0,1,1,1,A,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,A,1,0,0,0,9,9,9,0,0,0,0,0,0,0,A,9,9,9,0,0,0,0,0,A,1,2,1,1,1,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,5,},
  {1,1,1,1,1,A,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,A,2,1,9,9,9,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,1,A,1,1,1,1,}
  };
  static const byte level5[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,2,2,2,2,2,B,B,B,B,B,1,2,0,0,0,0,1,B,B,B,1,1,1,1,A,B,B,B,B,B,B,B,A,A,A,B,B,B,B,B,A,B,B,B,1,2,2,},
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,1,2,B,B,B,B,B,B,1,2,2,0,0,0,1,B,B,B,1,B,B,B,B,2,2,2,2,2,2,2,B,B,B,2,2,2,2,2,1,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,1,B,B,B,B,B,B,B,1,2,2,2,0,0,1,2,B,B,1,B,B,B,B,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,2,2,2,2,2,2,B,B,2,2,2,2,1,B,B,B,B,B,B,B,1,0,0,0,0,0,1,B,B,B,1,B,B,B,B,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,1,B,B,B,B,B,B,B,1,0,0,0,0,0,1,B,B,B,1,B,B,B,B,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,1,B,B,B,B,2,2,2,1,0,0,0,0,0,1,B,B,B,1,B,B,B,B,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,1,B,B,B,B,B,2,2,1,0,0,0,0,0,1,B,B,2,1,B,B,B,B,B,B,1,0,0,1,1,A,1,1,1,1,A,B,B,1,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,1,B,B,B,B,B,B,2,1,0,0,0,0,0,1,B,B,B,1,B,B,B,B,B,B,1,B,B,2,2,B,5,1,B,B,1,B,B,B,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,2,2,2,2,B,B,B,2,2,2,2,2,1,B,B,B,B,B,B,B,1,0,0,0,0,2,1,B,B,B,1,B,B,B,B,B,B,1,B,B,B,B,B,B,1,B,B,1,B,B,B,B,B,B,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,2,B,B,B,2,B,B,B,B,1,B,B,B,B,B,B,B,1,0,0,0,2,2,1,B,B,B,1,B,B,B,B,B,B,1,B,B,B,B,B,B,1,B,B,1,1,1,1,1,1,1,1,B,B,},
  {B,B,B,B,B,B,1,B,B,B,2,B,B,B,2,B,B,B,B,1,B,B,B,B,B,B,B,1,0,0,2,2,2,1,2,B,B,1,B,B,B,B,B,B,1,2,B,B,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,},
  {B,B,B,B,B,B,1,B,B,B,2,B,B,B,B,2,B,B,B,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,B,B,B,1,B,B,B,B,B,B,1,2,2,B,B,B,2,1,B,B,B,B,B,B,B,B,B,B,B,B,},
  {B,B,B,B,B,B,1,B,B,B,2,2,B,B,B,B,2,1,1,1,1,1,0,0,0,0,0,1,0,0,0,0,0,1,B,B,B,1,B,B,B,B,B,B,1,1,1,1,1,1,1,1,B,B,B,B,B,B,B,B,B,B,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,2,2,B,B,B,B,B,B,B,0,0,0,0,0,0,0,1,0,0,0,0,0,0,B,B,B,1,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,},
  {B,B,B,B,B,B,1,B,B,B,B,B,2,2,B,B,B,B,B,B,0,0,0,0,0,0,0,1,0,0,0,0,0,0,B,B,2,1,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,},
  {1,1,1,1,1,A,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,1,1,1,1,2,2,1,1,1,1,1,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,B,}
  };
  static const byte level6[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,2,2,0,1,0,0,0,0,0,0,0,0,0,0,0,0,E,0,0,},
  {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,1,1,1,8,},
  {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,0,0,0,A,A,A,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,D,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {2,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,},
  {C,0,0,0,0,1,0,0,0,0,0,4,0,0,4,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {2,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,5,1,3,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {2,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,A,A,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,},
  {C,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {2,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {2,0,0,0,0,0,0,7,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,},
  {2,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,A,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,A,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {1,1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,9,0,0,0,9,0,0,0,A,0,0,0,0,0,0,0,0,0,0,A,0,0,0,0,A,0,0,0,0,0,0,0,}
  };
  static const byte level7[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2,2,0,E,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,1,1,0,0,0,2,2,1,1,0,0,0,0,1,A,2,1,1,1,2,2,1,1,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,2,2,2,2,2,A,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,C,2,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,B,B,2,2,2,0,0,0,0,0,2,0,0,0,A,2,2,2,2,2,1,0,0,0,2,5,0,2,2,2,2,2,2,2,2,2,A,0,0,0,},
  {0,0,0,0,2,A,0,0,2,1,0,0,2,0,0,0,0,0,0,0,2,0,0,0,B,B,B,2,2,2,0,0,0,0,2,0,0,0,0,2,2,2,0,0,0,0,0,0,2,D,1,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,1,1,0,0,2,0,0,0,B,B,B,2,2,2,0,0,0,0,2,0,0,0,0,0,2,2,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,2,0,0,0,2,1,0,0,0,0,D,A,0,0,0,1,B,B,B,2,2,2,0,0,0,0,2,A,0,0,0,0,0,2,0,0,0,0,9,9,2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,A,},
  {0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,2,B,B,2,2,0,0,0,0,2,2,2,0,0,0,0,0,2,2,0,0,0,0,0,2,0,0,0,0,4,0,0,2,0,0,A,0,0,1,0,},
  {0,0,0,A,2,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,B,B,B,2,0,0,0,0,2,2,2,2,2,0,0,0,0,2,2,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,C,2,0,0,0,2,1,1,2,2,1,1,1,1,1,0,2,B,B,B,2,0,0,0,0,2,2,2,2,1,A,0,0,0,2,2,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,0,2,0,0,1,2,2,2,2,2,2,2,2,2,2,2,2,B,B,B,2,0,0,0,2,2,2,2,2,0,0,0,0,0,2,2,2,1,1,0,0,D,0,0,0,0,0,0,0,2,A,0,0,0,A,0,0,},
  {0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,B,B,2,2,0,0,0,1,1,1,1,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,2,A,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,B,B,B,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,3,},
  {0,0,0,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,B,B,B,2,2,0,0,0,0,0,0,0,0,0,A,A,A,2,2,0,0,0,0,2,2,0,0,A,0,2,0,0,0,0,0,0,0,0,0,A,},
  {0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,2,2,0,0,0,0,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,3,0,0,2,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,C,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,2,2,2,0,0,0,0,1,1,A,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,3,0,0,0,0,0,},
  {1,1,0,A,2,0,0,0,2,0,0,1,1,1,1,A,1,1,A,1,1,A,1,1,2,B,B,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,A,0,0,2,9,9,2,9,9,9,9,9,9,9,}
  };
  static const byte level8[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,0,0,0,3,B,B,1,1,1,1,0,0,0,2,2,2,2,2,2,2,2,2,2,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,B,B,0,0,0,C,0,0,0,1,1,1,1,1,1,0,0,0,2,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,A,A,2,0,0,0,3,0,0,0,3,B,B,0,0,0,C,0,0,0,0,0,0,0,0,0,0,0,0,2,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,B,B,0,0,0,C,0,0,0,0,0,0,0,0,0,0,0,0,2,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,0,B,B,0,0,C,0,0,0,0,0,0,0,0,0,0,0,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,0,0,B,B,0,C,0,0,0,0,0,0,0,9,9,0,0,2,0,},
  {0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,A,A,0,0,2,0,0,0,3,0,0,0,3,0,0,B,B,0,1,0,0,0,0,0,0,0,0,1,0,0,2,0,},
  {0,0,0,0,0,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,0,B,B,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,},
  {0,0,0,0,B,B,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,B,B,0,0,0,C,0,0,0,0,0,A,0,0,1,0,0,0,0,},
  {0,0,0,B,B,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,3,0,0,0,3,B,B,0,0,0,C,0,0,0,0,0,0,0,0,1,0,0,0,0,},
  {0,0,0,B,B,0,0,0,0,0,0,B,0,0,E,0,0,0,0,0,0,0,1,B,B,1,2,2,2,2,2,2,2,2,A,A,2,0,0,0,3,0,0,0,3,0,B,B,0,0,C,0,0,0,0,0,0,0,0,1,0,0,0,0,},
  {0,0,0,B,B,0,0,0,0,0,0,0,0,0,B,0,0,0,1,1,0,0,1,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,3,0,0,B,B,0,C,0,0,0,0,0,0,0,0,1,1,0,0,0,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,C,0,0,0,0,0,0,0,0,1,2,0,0,5,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,2,2,0,1,},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,B,B,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,A,1,2,2,2,2,},
  {1,1,1,A,A,A,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,A,A,1,1,1,1,1,1,1,1,1,1,0,0,0,0,}
  };
  static const byte level9[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,A,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,4,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,5,},
  {0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,B,B,1,1,1,1,1,1,1,1,1,1,},
  {0,A,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,8,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,B,B,1,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,7,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,A,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,},
  {0,A,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,0,0,0,0,0,0,0,0,0,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,},
  {0,0,0,0,1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,},
  {A,1,0,0,1,1,1,1,1,A,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,B,B,B,B,B,B,B,B,B,B,B,B,B,0,0,0,0,0,0,0,0,0,0,0,0,0,}
  };
  //player controls and variables
  float playerx = PLAYERSTARTX;//player position to start
  float playery = PLAYERSTARTY;
  float yvelocity = 0;//y velocity that the player is going at
  int jumpsleft = 0;//how many double jumps a player has left
  bool buttontriggered = 0;//checks if the main button has been triggered - it makes it so you can just hold down the jump button
  bool up = 1;//can the player move in certian directions
  bool down = 1;
  bool right = 1;
  bool left = 1;
  bool shorterdown = 1;//same as down but for a smaller box collider - is used so player can rise out of the ground if gravity function detects that the player is in the ground - that is not affected if the player hits the side of a wall
  //the level you are playing
  int level = 1;
  //the type of thing on each level - 0 is air, 1 is block...
  int levelpart = 0;
  //the collision of objects - sets the player's moveable options
  float enemyx = 0;//as the enemies move side to side
  float enemyy = 0;//as the enemy moves up and down
  float platformx = 0;//same for platforms
  float platformy = 0;
  bool enemyxflip = 0;//when to turn around the enemies and platforms so they move back and forth
  bool enemyyflip = 0;
  bool platformxflip = 0;
  bool platformyflip = 0;
  bool globalswitch = 0;//IF THE GLOBAL SWITCH IS SET TO ON!
  float globalswitchtimer = 0;//the timer for the global switch - uses ticks rather than milliseconds
  bool canwarp = 1;//if the player can warp
  float warptimer = 0;
  float laserpos = 0;//the position where the laser is from the launcher - cordinated for right lasers, left lasers do the opposite
  float poweruptimer = 0;
  void collision(int x,int y,int platform = 1){
    //the platform - if it is 6, it adds horizontal movement to the player. 7 adds vertical movement
    //uses x*8 and y*8 cordinates - not y inversed
    if(playerx<x+8+BCEXPANDX && playerx+8+BCEXPANDX>x && playery-8-BCEXPANDY<y && playery>y-8-BCEXPANDY){
      //if the player has collided, with what?
      //2nd if statement needed so player can slip sideways - checks if the player's hitbox is within the actual hitbox(not expanded) of the terrain
      //this is needed because the box colliders were expanded to prevent falling through, so to prevent the player from not being able to move sideways, when checking the left and right moving abilities, it only checks if the player is deeply embedded in the wall, not its expand hitbox. The player will stop falling if it is in the expanded hitbox, but it will not catch on the next hitboxes when moving sideways because those are shorter than the falling expanded hitbox.
      if(playery-8<y && playery>y-8){
        if(playerx<x+8+BCEXPANDX && playerx>x+4){//player is to right of wall
          left = 0;
        }
        if(playerx+8+BCEXPANDX>x && playerx+4<x){//player is to left of wall
          right = 0;
        }
      }
      if(playerx-8<x && playerx>x-8){
        //2nd if statement needed so player can slip sideways - same as above
        if(playery-8-BCEXPANDY<y && playery-4>y){//player is above wall
          down = 0;
          //this is used so if the player lands on a platform, if it is 6, or horizontal, the player moves sideways with the platform, and if it is 7, or vertical, the player moves vertically with the platform. - 7 is not really needed, but it smooths out the movement
          switch(platform){
            case 6:
              if(!platformxflip){
                playerx+=PLATFORMSPEED*deltatime;
              }
              else{
                playerx-=PLATFORMSPEED*deltatime;
              }
            break;
            case 7:
              if(!platformyflip){
                playery+=PLATFORMSPEED*deltatime;
              }
              else{
                playery-=PLATFORMSPEED*deltatime;
              }
              break;
            case 9:
              //for bounce colliders - negates player velocity and makes them bounce upwards
              yvelocity = abs(yvelocity)+BOUNCEADD;
              jumpsleft = DOUBLEJUMPS;//because the velocity is immediately set positive, I needed to add this to reset the jumps if you hit a bounce collider, otherwise, bounce colliders will not let you jump
            break;
            case 10:
              //for jump colliders - makes them jump upwards
              yvelocity = LAUNCHVELOCITY;
              jumpsleft = DOUBLEJUMPS;//because the velocity is immediately set positive, I needed to add this to reset the jumps if you hit a jump collider, otherwise, bounce colliders will not let you jump
            break;
            case I://right conveyor - if I wanted, I could convert it to momentum for some fun
              playerx+=CONVEYORSPEED*deltatime;
            break;
            case J://left conveyor
              playerx-=CONVEYORSPEED*deltatime;
            break;
          }
        }
        if(playery>y-8-BCEXPANDY && playery<y-4){//player is under wall
          up = 0;
        }
        if(playery-8<y && playery-4>y){//same as player is above wall, but for a lower, unexpanded box collider - for the rising out of the ground function
          shorterdown = 0;
        }
      }
    }
  }
  void jumpthroughcollision(int x,int y){
    //this type of collision only detects landing - nothing else happens if there is no collision other than landing. It is used for jump through platforms that you can jump through and land on
    if(playerx-8<x && playerx>x-8){
      //2nd if statement needed so player can slip sideways - same as above
      if(playery-8-BCEXPANDY<y && playery-4>y){//player is above wall
        down = 0;
      }
    }
  }
  //simply checks if the player collided - used for enemies and spikes
  bool collided(int x,int y){//uses x*8 and y*8 cordinates - not y inversed - BCEXPAND is not needed because this is only for enemies and spikes, which player can pass through and die
    if(playerx<x+8 && playerx+8>x && playery-8<y && playery>y-8){
      return 1;
    }
    else{
      return 0;
    }
  }
  int rendery(int yvalue){
    //the OLED y position when trying to animate on the OLED - note it is reversed
    return 56-yvalue;
  }
  //gameover function
  void LEVELRESET(){
    yvelocity = 1;
    playerx = PLAYERSTARTX;
    playery = PLAYERSTARTY;
    enemyx = 0;
    enemyy = 0;
    platformx = 0;
    platformy = 0;
    enemyxflip = 0;
    enemyyflip = 0;
    platformxflip = 0;
    platformyflip = 0;
    laserpos = 0;
    poweruptimer = 0;
    globalswitchtimer = 0;//reset switch
    globalswitch = 0;
    warptimer = 0;//reset warp
    canwarp = 1;
  }
  //LEVEL EXPORTER - ABSOLUTELY SILLY, BUT IT WORKS:)
  void exportlevel(byte level[LEVELMAXUP][LEVELMAXRIGHT]){
    Keyboard.write(uint8_t('{'));
    Keyboard.write(uint8_t('\n'));// /newline
    for(int i = 0;i<LEVELMAXUP;i++){
      Keyboard.write(uint8_t('{'));//write the start of the row
      for(int j = 0;j<LEVELMAXRIGHT;j++){
        if(level[i][j]<=9){
          Keyboard.print(level[i][j]);
        }
        else{
          switch(level[i][j]){//yea I know you can just do Keyboard.write(uint8_t('A'+leveltoedit[i][j]-10)); - every number over 10 is converted to A+a certain number (A+0 = A,A+1=B...)
            case 10:
            Keyboard.write(uint8_t('A'));
            break;
            case 11:
            Keyboard.write(uint8_t('B'));
            break;
            case 12:
            Keyboard.write(uint8_t('C'));
            break;
            case 13:
            Keyboard.write(uint8_t('D'));
            break;
            case 14:
            Keyboard.write(uint8_t('E'));
            break;
            case 15:
            Keyboard.write(uint8_t('F'));
            break;
            case 16:
            Keyboard.write(uint8_t('G'));
            break;
            case 17:
            Keyboard.write(uint8_t('H'));
            break;
            case 18:
            Keyboard.write(uint8_t('I'));
            break;
            case 19:
            Keyboard.write(uint8_t('J'));
            break;
            case 20:
            Keyboard.write(uint8_t('K'));//added extra characters just in case :)
            break;
            case 21:
            Keyboard.write(uint8_t('L'));
            break;
            case 22:
            Keyboard.write(uint8_t('M'));
            break;
            case 23:
            Keyboard.write(uint8_t('N'));
            break;
            case 24:
            Keyboard.write(uint8_t('O'));
            break;
            case 25:
            Keyboard.write(uint8_t('P'));
            break;
          }
        }
        Keyboard.write(uint8_t(','));
      }
      Keyboard.write(uint8_t('}'));//write the end of the 
      //the last row has no comma
      if(i<LEVELMAXUP-1){
        Keyboard.write(uint8_t(','));
      }
      Keyboard.write(uint8_t('\n'));// /newline
    }
    Keyboard.write(uint8_t('}'));
  }
  //IMPORTANT BIT OF CODE - THIS IS THE CODE THAT LETS YOU ACCESS EACH ARRAY!!! CHANGE WHEN ADDING MORE LEVELS!!!
  int getlevelcomponent(int coorx,int coory,int leveltoread){//given a set of coordinates and a level, you can get the tile at each individual point in the level
    switch(leveltoread){
      //note that getlevelcomponent also handles the math - so playerx = 0, playery = 0 = the bottom left tile!
      case 0:
        if(saveslot==0){
          //sram
          return level0[LEVELMAXUP-coory-1][coorx];//this is stored in flash, as you can edit this level using the level editor
        }
        else{
          return blankflashreadlevel[LEVELMAXUP-coory-1][coorx];//this level is read from flash memory in the custom level selection screen.
        }
      break;
      case 1:
        return pgm_read_byte_near(&level1[LEVELMAXUP-coory-1][coorx]);
      break;
      case 2:
        return pgm_read_byte_near(&level2[LEVELMAXUP-coory-1][coorx]);
      break;
      case 3:
        return pgm_read_byte_near(&level3[LEVELMAXUP-coory-1][coorx]);
      break;
      case 4:
        return pgm_read_byte_near(&level4[LEVELMAXUP-coory-1][coorx]);
      break;
      case 5:
        return pgm_read_byte_near(&level5[LEVELMAXUP-coory-1][coorx]);
      break;
      case 6:
        return pgm_read_byte_near(&level6[LEVELMAXUP-coory-1][coorx]);
      break;
      case 7:
        return pgm_read_byte_near(&level7[LEVELMAXUP-coory-1][coorx]);
      break;
      case 8:
        return pgm_read_byte_near(&level8[LEVELMAXUP-coory-1][coorx]);
      break;
      case 9:
        return pgm_read_byte_near(&level9[LEVELMAXUP-coory-1][coorx]);
      break;
    }
  }
  //LEVEL EDITOR - AN ABSOLUTE MASTERPIECE!!!!!!!!!!!!!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  void editlevel(byte leveltoedit[LEVELMAXUP][LEVELMAXRIGHT]){
    //accepts the level - THIS IS FOR SOME REASON A REFERENCE, SO IT EDITS THE REAL LEVEL

    //FLASH EDIT
    //asks to edit from flash and if so, assigns leveltoedit to the flash level
    bool flash = 0;
    while(digitalRead(BUTTON)){
      display.setCursor(13,0);
      display.print("Edit From Flash?");//automatically saves in sram, however you have to input if you want to save it in flash
      display.setCursor(24,32);
      display.print("Yes");
      display.setCursor(58,32);
      display.print("No");
      if(leftpressed()||rightpressed()){
        //switch from saving to not saving
        flash = !flash;
        delay(200);
      }
      if(flash){
        display.drawRect(23,32,18,8,WHITE);
      }
      else{
        display.drawRect(57,32,18,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //rather than a saveslot stored here, it uses the saveslot variable initiated at the very top
    if(flash){//if using level from flash memory
      ::loading_screen();
      saveslot = 1;
      while(digitalRead(BUTTON)){
        display.setCursor(0,32);
        display.print("Save Slot: ");
        if(leftpressed()&&saveslot>1){
          saveslot--;
          delay(200);
        }
        if(rightpressed()&&saveslot<MAXSAVESLOTS){
          saveslot++;
          delay(200);
        }
        display.print(int(saveslot));
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      display.setCursor(37,32);
      display.print("Editing..");
      display.display();
      prefs.begin("Levels", true);//begin the level reading mechanism - true means read only.
      //flash
      prefs.getBytes(&saveslot, leveltoedit, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
      //it uses sizeof(byte) to figure out the size of an array of bytes
      //note that saveslot's char is actually a weird ASCII conversion - its not 
      //SETS LEVELTOEDIT TO THE FLASH's BYTE
      prefs.end();//it only reads the level, stores it in SRAM, and then puts stops reading it.
      delay(2000);
      display.clearDisplay();
    }
    else{
      //editing SRAM level
      saveslot = 0;//means editing new level draft, not something from flash
      display.setCursor(37,32);
      display.print("Editing..");
      display.display();
      delay(2000);
      display.clearDisplay();
    }
    ::loading_screen();
    //finish flash
    //asks the player if they want to erase the current level

    //ERASER CODE
    bool erase = 0;
    while(digitalRead(BUTTON)){
      display.setCursor(4,0);
      display.print("Erase Current Level?");//automatically saves in sram, however you have to input if you want to save it in flash
      display.setCursor(24,32);
      display.print("Yes");
      display.setCursor(58,32);
      display.print("No");
      if(leftpressed()||rightpressed()){
        //switch from saving to not saving
        erase = !erase;
        delay(200);
      }
      if(erase){
        display.drawRect(23,32,18,8,WHITE);
      }
      else{
        display.drawRect(57,32,18,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    if(erase){
      display.setCursor(34,32);
      display.print("Erasing...");
      display.display();
      delay(2000);
      display.clearDisplay();
      for(int i = 0;i<LEVELMAXUP;i++){
        for(int j=0;j<LEVELMAXRIGHT;j++){
          leveltoedit[i][j] = 0;//reset the entier level to 0
        }
      }
    }
    else{
      display.setCursor(34,32);
      display.print("Editing...");
      display.display();
      delay(2000);
      display.clearDisplay();
    }
    //BEGIN EDITING
    //starts the player, however it does some weird stuff
    playerx = PLAYERSTARTX;
    playery = PLAYERSTARTY;
    while(!(!digitalRead(BUTTONJOY)&&!digitalRead(BUTTON))){
      //RUN DELTATIME
      ::currenttime = millis();
      ::deltatime = float(::currenttime - ::prevtime) / float(CLOCKSPEED);  // normalized to your original 50ms tick
      if(::deltatime>DELTATIMECAP){//cap deltatime
        ::deltatime=DELTATIMECAP;
      }
      ::prevtime=::currenttime;
      //because button is inversed due to INPUT_PULLUP, then they both need to be off to be both pressed (buttonjoy is normal)
      //NOTE: this uses all the same variables as the player does, it's like the player's walking through the level! 
      //In addition, if the joystick is used, there is a delay built in so the joystick does not spam.
      //note the the x controls are a bit flipped - it's possible to edit them later
      if(leftpressed()){//left
        playerx-=8;
        delay(100);
      }
      if(rightpressed()){
        playerx+=8;
        delay(100);
      }
      if(uppressed()){//left
        playery+=8;
        delay(100);
      }
      if(downpressed()){
        playery-=8;
        delay(100);
      }
      //constrain functions----------------------------------------------------------------------------------------------------------------
      playerx = constrain(playerx,0,(LEVELMAXRIGHT-1)*8);
      playery = constrain(playery,0,(LEVELMAXUP-1)*8);
      //Select the sprite used! - remember button is input_pullup
      if(!digitalRead(BUTTON)){
        //quick delay - this is so you dont accedentially place blocks if you are trying to exit
        delay(200);
        if(!digitalRead(BUTTONJOY)&&!digitalRead(BUTTON)){
          break;//leave the level editor if you clicked both
        }
        //remember the player's y position is completely flipped!
        if(leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]<25){
          //increments part
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]++;
        }
        else{
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]=0;//if the leveltoedit part is greater than or equal to 14, the tape resets, and the part is set to 0, or air again.
        }
      }
      if(!digitalRead(BUTTONJOY)){
        //quick delay - this is so you dont accedentially place blocks if you are trying to exit
        delay(200);
        if(!digitalRead(BUTTONJOY)&&!digitalRead(BUTTON)){
          break;//leave the level editor if you clicked both
        }
        //remember the player's y position is completely flipped!
        if(leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]>0){
          //increments part
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]--;
        }
        else{
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]=25;//if the leveltoedit part is greater than or equal to 14, the tape resets, and the part is set to 0, or air again.
        }
      }
      //renders the level
      for(int posy = 0;posy<LEVELMAXUP;posy++){
        for(int posx = 0;posx<LEVELMAXRIGHT;posx++){
          //note - levelmaxup-posy is caused because the for loop is inverted for posy. Also, for some reason, the level ordinates are y,x
          levelpart = leveltoedit[LEVELMAXUP-posy-1][posx];//reads the level, then renders it
          switch(levelpart){
            //note that you have to do pgm_read_byte_near(&level[LEVELMAXUP-posy-1][posx]) to read progmem data - note the -8 at the end - I had to include it because the occlusion culling at 0 cut the rendering short
            case 1://wall
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),wall,8,8,WHITE);
              }
            break;
            case 2://spike
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),spike,8,8,WHITE);
              }
            break;
            case 3://horizontal enemy
              if(posx*8-playerx+ONSCREENX+enemyx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+enemyx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx+enemyx,rendery(posy*8-playery+ONSCREENY),enemy,8,8,WHITE);
              }
            break;
            case 4://vertical enemy
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+enemyy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+enemyy)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY+enemyy),enemy,8,8,WHITE);
              }
            break;
            case 5://end
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),end,8,8,WHITE);
              }
            break;
            case 6://horizontal platform
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX+platformx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+platformx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX+platformx,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
              }
            break;
            case 7://vertical platform
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+platformy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+platformy)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY+platformy),jumpthrough,8,8,WHITE);
              }
            break;
            case 8://jumpthrough platform
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
              }
            break;
            case 9://bounce platform
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),bounce,8,8,WHITE);
              }
            break;
            case A://jump platform
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),launcher,8,8,WHITE);
              }
            break;
            case B://water
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),water,8,8,WHITE);
              }
            break;
            case C://laser right
              //draws laser blaster
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterright,8,8,WHITE);
              }
              //draw laser
              if(posx*8-playerx+ONSCREENX+laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx+laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
              }
            break;
            case D://laser left
              //draws laser blaster
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterleft,8,8,WHITE);
              }
              //draw laser
              if(posx*8-playerx+ONSCREENX-laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX-laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx-laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
              }
            break;
            case E://powerup
              //draw powerup
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),Powerup,8,8,WHITE);
              }
            break;
            case F://switch
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),switch_off,8,8,WHITE);
              }
            break;
            case G://door closed
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_closed,8,8,WHITE);
              }
            break;
            case H://door open
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_open,8,8,WHITE);
              }
            break;
            case I://right conveyor
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),conveyor_right,8,8,WHITE);
              }
            break;
            case J://left conveyor
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),conveyor_left,8,8,WHITE);
              }
            break;
            case K://jump orb
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),jump_orb,8,8,WHITE);
              }
            break;
            case L://WARP RIGHT
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpright,8,8,WHITE);
              }
            break;
            case M://WARP LEFT
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpleft,8,8,WHITE);
              }
            break;
            case N://WARP UP
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpup,8,8,WHITE);
              }
            break;
            case O://WARP DOWN
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpdown,8,8,WHITE);
              }
            break;
            case P://WARP STOP
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpstop,8,8,WHITE);
              }
            break;
          }
        }
      }
      //animate stuff (for the fun of it)----
      //Move and flip the laser
      laserpos+=LASERSPEED*deltatime;
      if(laserpos>=LASERDIST*8){
        //note the *8 because laserspeed is in pixils
        laserpos = 0;
      }
      //Move the enemies
      if(!enemyxflip){
        enemyx+=ENEMYSPEED*deltatime;
      }
      else{
        enemyx-=ENEMYSPEED*deltatime;
      }
      //flip the enemy - make it move left and right
      if(enemyx>=ENEMYX*8||enemyx<=0){
        enemyxflip = !enemyxflip;
      }
      //move the vertical enemy
      if(!enemyyflip){
        enemyy+=ENEMYSPEED*deltatime;
      }
      else{
        enemyy-=ENEMYSPEED*deltatime;
      }
      //flip the enemy - make it move up and down
      if(enemyy>=ENEMYY*8||enemyy<=0){
        enemyyflip = !enemyyflip;
      }
      //move the horizontal platform
      if(!platformxflip){
        platformx+=PLATFORMSPEED*deltatime;
      }
      else{
        platformx-=PLATFORMSPEED*deltatime;
      }
      //flip the platform - make it move up and down
      if(platformx>=PLATFORMX*8||platformx<=0){
        platformxflip = !platformxflip;
      }
      //move the vertical platform
      if(!platformyflip){
        platformy+=PLATFORMSPEED*deltatime;
      }
      else{
        platformy-=PLATFORMSPEED*deltatime;
      }
      //flip the platform - make it move up and down
      if(platformy>=PLATFORMY*8||platformy<=0){
        platformyflip = !platformyflip;
      }
      //draw UI in the corner - coordinates + tile type - like f3
      //coordinates
      display.setCursor(0,0);
      display.print("X: ");
      display.print(playerx);
      display.print(" Y: ");
      display.print(playery);
      //write tile discription
      display.setCursor(0,56);
      display.print("Tile: ");
      //note the odd math inside the brackets - this converts playery and playerx into x and y positions in the array!
      display.print(tileindex(leveltoedit[LEVELMAXUP-int(playery/8)-1][int(playerx/8)]));
      //end of animations
      display.drawBitmap(ONSCREENX,rendery(ONSCREENY),leveleditsprite,8,8,WHITE);//draw the sprite to level select
      //draw edge boarders
      display.drawRect(64-playerx,32-LEVELMAXUP*8+playery,LEVELMAXRIGHT*8,LEVELMAXUP*8,WHITE);//draw the edge boarders around the entire screen
      display.display();//display everything
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //display confirmation message
    display.clearDisplay();
    display.setCursor(32,32);
    display.print("Level Edited!");
    display.display();
    delay(2000);
    display.clearDisplay();
    //SAVING CODE - IF SAVESLOT = 0 - BASICALLY THE PLAYER DIDNT EDIT A PREVIOUS SAVE, THEN ALLOW THE PLAYER TO CHOOSE THE FLASH TO SAVE IN
    //IF THEY DID EDIT A PREVIOUS SAVE, OVERWRITE THAT SAVE.
    if(saveslot==0){
      //IF I CREATED AN ENTIRELY NEW LEVEL
      //SAVE LEVEL
      bool save = 0;
      while(digitalRead(BUTTON)){
        display.setCursor(4,0);
        display.print("Save Level in Flash?");//automatically saves in sram, however you have to input if you want to save it in flash
        display.setCursor(24,32);
        display.print("Yes");
        display.setCursor(58,32);
        display.print("No");
        if(leftpressed()||rightpressed()){
          //switch from saving to not saving
          save = !save;
          delay(200);
        }
        if(save){
          display.drawRect(23,32,18,8,WHITE);
        }
        else{
          display.drawRect(57,32,18,8,WHITE);
        }
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      saveslot = 1;
      bool cancel = 0;
      ::loading_screen();
      if(save){//if saving, begin saving mechanism
        while(digitalRead(BUTTON)){
          display.setCursor(0,32);
          display.print("Save Slot: ");
          if(leftpressed()&&saveslot>1){
            saveslot--;
            delay(200);
          }
          if(rightpressed()&&saveslot<MAXSAVESLOTS){
            saveslot++;
            delay(200);
          }
          display.print(int(saveslot));
          if(!digitalRead(BUTTONJOY)){
            //cancel save
            cancel = 1;
            break;
          }
          display.display();
          delay(TICKSPEED);
          display.clearDisplay();
        }
        if(!cancel){
          //saving
          display.setCursor(37,32);
          display.print("Saving...");
          display.display();
          delay(2000);
          prefs.begin("Levels", false);//begin the level editing mechanism
          prefs.putBytes(&saveslot, leveltoedit, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
          //it uses sizeof(byte) to figure out the size of an array of bytes
          //note that um saveslot's char is actually not saved as an integer - it's saved as some weird ASCII conversion
          prefs.end();
          display.clearDisplay();
        }
        else{
          //canceled
          display.clearDisplay();
          display.setCursor(37,32);
          display.print("Canceled");
          display.display();
          delay(2000);
          display.clearDisplay();
        }
      }
    }
    else{
      display.setCursor(37,32);
      display.print("Saving...");
      display.display();
      delay(2000);
      prefs.begin("Levels", false);//begin the level editing mechanism
      prefs.putBytes(&saveslot, leveltoedit, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
      //it uses sizeof(byte) to figure out the size of an array of bytes
      //note that um saveslot's char is actually not saved as an integer - it's saved as some weird ASCII conversion
      prefs.end();
      display.clearDisplay();
    }
    bool executeexport = 0;
    if(true){//REPLACE WITH if serial detects if a USB cable is plugged in. If there is one, then it gives the option to export. If there isnt, then it doesnt
      while(digitalRead(BUTTON)){
        display.setCursor(13,0);
        display.print("Export level?");//automatically saves in sram, however you have to input if you want to save it in flash
        display.setCursor(24,32);
        display.print("Yes");
        display.setCursor(58,32);
        display.print("No");
        if(leftpressed()||rightpressed()){
          //switch from saving to not saving
          executeexport = !executeexport;
          delay(200);
        }
        if(executeexport){
          display.drawRect(23,32,18,8,WHITE);
        }
        else{
          display.drawRect(57,32,18,8,WHITE);
        }
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      if(executeexport){
        display.setCursor(37,32);
        display.print("Exporting..");
        display.display();
        exportlevel(leveltoedit);
        delay(1000);
        display.clearDisplay();
      }
    }
    ::loading_screen();
  }
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  void CUSTOMHOMEPAGE(){
    //a secondary homepage for selecting the custom level
    //SAVE LEVEL
    bool flash = 0;
    while(digitalRead(BUTTON)){
      display.setCursor(13,0);
      display.print("Levels From Flash?");//automatically saves in sram, however you have to input if you want to save it in flash
      display.setCursor(24,32);
      display.print("Yes");
      display.setCursor(58,32);
      display.print("No");
      if(leftpressed()||rightpressed()){
        //switch from saving to not saving
        flash = !flash;
        delay(200);
      }
      if(flash){
        display.drawRect(23,32,18,8,WHITE);
      }
      else{
        display.drawRect(57,32,18,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //rather than a saveslot stored here, it uses the saveslot variable initiated at the very top
    if(flash){//if using level from flash memory
      ::loading_screen();
      saveslot = 1;
      while(digitalRead(BUTTON)){
        display.setCursor(0,32);
        display.print("Save Slot: ");
        if(leftpressed()&&saveslot>1){
          saveslot--;
          delay(200);
        }
        if(rightpressed()&&saveslot<MAXSAVESLOTS){
          saveslot++;
          delay(200);
        }
        display.print(int(saveslot));
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      display.setCursor(37,32);
      display.print("Playing..");
      display.display();
      prefs.begin("Levels", true);//begin the level reading mechanism - true means read only.
      //flash
      prefs.getBytes(&saveslot, blankflashreadlevel, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
      //it uses sizeof(byte) to figure out the size of an array of bytes
      //note that saveslot's char is actually a weird ASCII conversion - its not 
      prefs.end();//it only reads the level, stores it in SRAM, and then puts stops reading it.
      delay(2000);
      display.clearDisplay();
    }
    else{
      //playing SRAM level
      saveslot = 0;
      display.setCursor(37,32);
      display.print("Playing..");
      display.display();
      delay(2000);
      display.clearDisplay();
    }
    //
  }
  void HOMEPAGE(){
    //the navigation homepage - called by GAMEOVER function
    //joystick up and down to navigate up and down, button to select.
    /*NOTES - To do this, use setcursor and print, use LEVELMAX to set the max scrolling, print the current level, the level before it above it, 
    and the next level under it. Change the variable level as you do this, and use a while loop until */
    while(digitalRead(BUTTON)!=0){
      if(!digitalRead(BUTTONJOY)&&(leftpressed()||rightpressed())){
        ::MAIN_PAGE();//returns to main page if both buttons are pressed
        return;//completely exits out of the homepage function
      }
      //clears the display
      display.clearDisplay();
      //while the button is not pressed
      //add computational logic here
      if(uppressed()&&level<LEVELMAX){
        //joystick up - cannot go over LEVELMAX
        level++;
      }
      if(downpressed()&&level>0){
        //joystick down - cannot go under 1
        level--;
      }
      //add printing commands - print out the screen
      //prints out the current level
      display.setCursor(32,32);
      if(level!=0){
        display.print("Level: ");
        display.print(level);
      }
      else{
        //custom level!
        display.print("Level: ");
        display.print("Custom");
        display.setCursor(0,44);
        display.print("Both buttons: edit");
        display.setCursor(0,56);
        display.print("Main button: play");
      }
      //prints out previous level
      if(level>=0+1){
        //level>1+1 because if level is one, there is no previous level - prints below current level
        display.setCursor(72,48);
        if(level-1!=0){
          display.print(level-1);
        }
        else{
          display.print("Custom");
        }
      }
      //prints out next level
      if(level<=LEVELMAX-1){
        //level>1+1 because if level is one, there is no previous level - prints below current level
        display.setCursor(72,16);
        display.print(level+1);
      }
      //prints out words "Homepage!"
      display.setCursor(0,8);
      display.print("Platformer!");
      //displays
      display.display();
      delay(200);
    }
    //THIS IS IMPORTANT - IT MAKES IT SO IF YOU HAVE THE JOYSTICK BUTTON PRESSED TOO, YOU CAN EDIT THE LEVEL INSTEAD OF STARTING
    //REMEMBER JOYBUTTON IS INPUT_PULLUP
    if(level==0&&!digitalRead(BUTTONJOY)){
      ::loading_screen();//to waste people's time
      display.clearDisplay();
      editlevel(level0);
      //after editing level, call HOMEPAGE again
      HOMEPAGE();
    }
    else{
      if(level==0){
        ::loading_screen();//to waste people's time
        CUSTOMHOMEPAGE();//allows people to select custom levels from sram or flash
      }
      ::loading_screen();//to waste people's time
      LEVELRESET();
    }
    //
  }
  void GAMEOVER(){
    //called whenever you hit an enemy or spike or fall, ends the game perminately
    if(digitalRead(CHEATPIN)){//CHEATING AGAIN???
      display.clearDisplay();
      display.setCursor(32,32);
      display.print("GAME OVER!!!");
      display.display();
      delay(2000);//wait 2 seconds and then reset the game
      HOMEPAGE();
    }
    else{
      display.setCursor(4,52);
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print("CHEATED DEATH!");//uhh yea - CHEATER
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.display();
    }
  }
  void NEXTLEVEL(){
    //called when you reach the end flag - sets up the next level - sends you to the homepage and makes the level go up
    display.clearDisplay();
    display.setCursor(4,32);
    display.print("LEVEL ");
    display.print(level);
    display.print(" COMPLETED!!!");
    display.display();
    delay(2000);//wait 2 seconds and then enter the homepage
    if(level<LEVELMAX){//make sure levels cannot go past LEVELMAX
      level++;
    }
    HOMEPAGE();
  }
  void init() {
    HOMEPAGE();
  }
  void update() {
    //this following code is not optomized
    for(int posy = 0;posy<LEVELMAXUP;posy++){
      for(int posx = 0;posx<LEVELMAXRIGHT;posx++){
        //note - levelmaxup-posy is caused because the for loop is inverted for posy. Also, for some reason, the level ordinates are y,x
        //looks through the levels
        //add levels if needed
        levelpart = getlevelcomponent(posx,posy,level);
        //note that all flash and custom levels are under level 0. Customhomepage lets you choose your flash or custom level and autopopulates level0 for you with your designated level.
        switch(levelpart){
          //note that you have to do pgm_read_byte_near(&level[LEVELMAXUP-posy-1][posx]) to read progmem data - note the -8 at the end - I had to include it because the occlusion culling at 0 cut the rendering short
          case 1://wall
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),wall,8,8,WHITE);
            }
            collision(posx*8,posy*8);
          break;
          case 2://spike
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),spike,8,8,WHITE);
            }
            if(collided(posx*8,posy*8)){
              GAMEOVER();
            }
          break;
          case 3://horizontal enemy
            if(posx*8-playerx+ONSCREENX+enemyx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+enemyx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx+enemyx,rendery(posy*8-playery+ONSCREENY),enemy,8,8,WHITE);
            }
            if(collided(posx*8+enemyx,posy*8)){
              GAMEOVER();
            }
          break;
          case 4://vertical enemy
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+enemyy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+enemyy)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY+enemyy),enemy,8,8,WHITE);
            }
            if(collided(posx*8,posy*8+enemyy)){
              GAMEOVER();
            }
          break;
          case 5://end
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),end,8,8,WHITE);
            }
            if(collided(posx*8,posy*8)){
              NEXTLEVEL();
            }
          break;
          case 6://horizontal platform
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX+platformx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+platformx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX+platformx,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
            }
            collision(posx*8+platformx,posy*8,6);
          break;
          case 7://vertical platform
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+platformy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+platformy)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY+platformy),jumpthrough,8,8,WHITE);
            }
            collision(posx*8,posy*8+platformy,7);
          break;
          case 8://jumpthrough platform
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
            }
            //note for collision - this platform only affects landing on it, nothing else. - therefore, it uses a special type of collision
            jumpthroughcollision(posx*8,posy*8);
          break;
          case 9://bounce platform
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),bounce,8,8,WHITE);
            }
            collision(posx*8,posy*8,9);
          break;
          case A://jump platform
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),launcher,8,8,WHITE);
            }
            collision(posx*8,posy*8,A);
          break;
          case B://water
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),water,8,8,WHITE);
            }
            if(collided(posx*8,posy*8)){
              yvelocity+=WATERBOYANCY*deltatime;
              jumpsleft = DOUBLEJUMPS;//for infinite jumps in water
            }
          break;
          case C://laser right
            //draws laser blaster
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterright,8,8,WHITE);
            }
            collision(posx*8,posy*8);
            //draw laser
            if(posx*8-playerx+ONSCREENX+laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx+laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
            }
            if(collided(posx*8+laserpos,posy*8)){
              GAMEOVER();
            }
          break;
          case D://laser left
            //draws laser blaster
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterleft,8,8,WHITE);
            }
            collision(posx*8,posy*8);
            //draw laser
            if(posx*8-playerx+ONSCREENX-laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX-laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx-laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
            }
            if(collided(posx*8-laserpos,posy*8)){
              GAMEOVER();
            }
          break;
          case E://powerup
            //draw powerup
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),Powerup,8,8,WHITE);
            }
            if(collided(posx*8,posy*8)){
              poweruptimer = POWERUPTIME;//sets the time that the powerup works for
            }
          break;
          case F://switch
            //draw switch
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              //switch on vs switch off - if the global switch is on, all switches render on
              if(globalswitch){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),switch_on,8,8,WHITE);
              }
              else{
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),switch_off,8,8,WHITE);
              }
            }
            if(collided(posx*8,posy*8)){
              globalswitchtimer = GLOBALSWITCHFRAMES;//reset the switch timer
              globalswitch = 1;//turn globalswitch on
            }
          break;
          case G://door closed
            //draw switch
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              //switch on vs switch off - if the global switch is on, all switches render on
              if(globalswitch){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_open,8,8,WHITE);
              }
              else{
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_closed,8,8,WHITE);
                collision(posx*8,posy*8);//if the door is closed run collisions
              }
            }
          break;
          case H://door open - this door stays open and closes when the switch is hit
            //draw switch
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              //switch on vs switch off - if the global switch is on, all switches render on
              if(!globalswitch){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_open,8,8,WHITE);
              }
              else{
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_closed,8,8,WHITE);
                collision(posx*8,posy*8);//if the door is closed run collisions
              }
            }
          break;
          case I://right conveyor
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),conveyor_right,8,8,WHITE);
              collision(posx*8,posy*8,I);//run I collisions (move right)
            }
          break;
          case J://left conveyor
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),conveyor_left,8,8,WHITE);
              collision(posx*8,posy*8,J);//run I collisions (move left)
            }
          break;
          case K://jump orb
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),jump_orb,8,8,WHITE);
              if(collided(posx*8,posy*8)&&!digitalRead(BUTTON)&&!buttontriggered){//if you jump while standing on the jump orb
              buttontriggered = 1;
              //buttontriggered has 2 uses here - it makes sure you dont use up your jump on a jump orb, and it also makes it so you have to press jump on the jump orb to jump
              yvelocity = ORBJUMP;
            }
            }
          break;
          case L://WARP RIGHT
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpright,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp
                for(int xsearch = posx; xsearch<LEVELMAXRIGHT; xsearch++){
                  //search starts at posx
                  if(getlevelcomponent(xsearch,posy,level)==P){
                    //basically if it finds a portal stop, stop
                    playerx = xsearch*8;//warp the player to the searched tile!
                    playery = posy*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case M://WARP LEFT
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpleft,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp
                for(int xsearch = posx; xsearch>=0; xsearch--){
                  //search starts at posx
                  if(getlevelcomponent(xsearch,posy,level)==P){
                    //basically if it finds a portal stop, stop
                    playerx = xsearch*8;//warp the player to the searched tile!
                    playery = posy*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case N://WARP UP
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpup,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp
                for(int ysearch = posy; ysearch<LEVELMAXUP; ysearch++){
                  //search starts at posy
                  if(getlevelcomponent(posx,ysearch,level)==P){
                    //basically if it finds a portal stop, stop
                    playerx = posx*8;//warp the player to the searched tile!
                    playery = ysearch*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case O://WARP DOWN
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpdown,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp - I know it's reversed but the top left corner is the [0,0] so ysearch must increase to go down
                for(int ysearch = posy; ysearch>=0; ysearch--){
                  //search starts at posy
                  if(getlevelcomponent(posx,ysearch,level)==P){
                    //basically if it finds a portal stop, stop
                    playerx = posx*8;//warp the player to the searched tile!
                    playery = ysearch*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case P://WARPSTOP
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpstop,8,8,WHITE);
            }
          break;
        }
      }
    }
    //Move and flip the laser
    laserpos+=LASERSPEED*deltatime;
    if(laserpos>=LASERDIST*8){
      //note the *8 because laserspeed is in pixils
      laserpos = 0;
    }
    //Move the enemies
    if(!enemyxflip){
      enemyx+=ENEMYSPEED*deltatime;
    }
    else{
      enemyx-=ENEMYSPEED*deltatime;
    }
    //flip the enemy - make it move left and right
    if(enemyx>=ENEMYX*8.0||enemyx<=0.0){
      enemyxflip = !enemyxflip;
    }
    //move the vertical enemy
    if(!enemyyflip){
      enemyy+=ENEMYSPEED*deltatime;
    }
    else{
      enemyy-=ENEMYSPEED*deltatime;
    }
    //flip the enemy - make it move up and down
    if(enemyy>=ENEMYY*8.0||enemyy<=0.0){
      enemyyflip = !enemyyflip;
    }
    //move the horizontal platform
    if(!platformxflip){
      platformx+=PLATFORMSPEED*deltatime;
    }
    else{
      platformx-=PLATFORMSPEED*deltatime;
    }
    //flip the platform - make it move up and down
    if(platformx>=PLATFORMX*8||platformx<=0){
      platformxflip = !platformxflip;
    }
    //move the vertical platform
    if(!platformyflip){
      platformy+=PLATFORMSPEED*deltatime;
    }
    else{
      platformy-=PLATFORMSPEED*deltatime;
    }
    //flip the platform - make it move up and down
    if(platformy>=PLATFORMY*8||platformy<=0){
      platformyflip = !platformyflip;
    }
    //power ups
    if(poweruptimer>0){
      //decreases the power up timer
      poweruptimer--;
      //prints the powerup time
      display.setCursor(64,8);
      display.print("Power:");
      display.print(int(poweruptimer));
    }
    //switches and globalswitch------------------------------------------
    if(globalswitchtimer<=0){
      globalswitch = 0;//reset global switch
    }
    else{
      globalswitchtimer-=deltatime;//subtracts constantly if the timer is greater than 0
    }
    //warp timer---------------------------------------------------------
    if(warptimer<=0){
      canwarp = 1;//reset warp
    }
    else{
      warptimer-=deltatime;//subtracts constantly if the timer is greater than 0
    }
    //note yjoy is flipped
    //GRAVITY AND JUMP
    //Gravity
    yvelocity-=float(GRAVITY)*deltatime;
    if(!down && yvelocity<=0){
      //if it is touching the ground and not jumping
      yvelocity = 0;
      jumpsleft = DOUBLEJUMPS;
      if(!shorterdown){
        //allows the player to rise out of the ground if it gets embedded - ENABLE OR DISABLE IF NEEDED - WORKS PERFECTLY
        playery+=RISE*deltatime;
      }
    }
    else{
      if(up||yvelocity<0){
          /*terminal velocity - if the yvelocity exceeds a point(4(the inner box collider area)+BCEXPAND(essentially combined, 
          the entire box collider area)), then the player will continue to move at that velocity - the exact point where the velocity exceeded. 
          This is because if the player moves faster than that, it can fall through the floor as the box collider does not trigger.
          Note that this does not change the velocity, the velocity can exceed that, but it changes how fast the player moves for each velocity tick that is higher than the terminal velocity.
          this allows for infinite height jumping, but also prevents the player from falling through the floor*/
        if(yvelocity>=TERMINALVELOCITY){//yvelocity
          //terminal velocity up - terminalsubtract makes sure that this statement does not run every single time, redunding itself
          playery += (TERMINALVELOCITY)*deltatime;//everything multiplied by deltatime so it moves at a constant speed per tick
        }
        else if(yvelocity<=-TERMINALVELOCITY){
          //terminal velocity down - terminalsubtract makes sure that this statement does not run every single time, redunding itself
          playery -= TERMINALVELOCITY*deltatime;
        }
        else{
          playery+=yvelocity*deltatime;
        }
      }
      else{
        //if the player hits the ceiling, all velocity is immediately stopped, and the player goes downwards
        yvelocity = 0;
      }
    }
    //jump
    if(!digitalRead(BUTTON) && jumpsleft>0 && !buttontriggered){//not button triggered so you cant hold jump
      buttontriggered = 1;//prevents spam jumping
      //if the powerup is activated, you get higher jump
      if(poweruptimer>0){
        yvelocity = POWERJUMP;
      }
      else{
        yvelocity = JUMP;
      }
      playery+=BCEXPANDY;//to escape the box collider - not multiplied by time.deltatime because it's a teleportation
      if(digitalRead(CHEATPIN)){//CHEATER!!! anyways if cheatpin is off, you never lose jumps, and jump um indefinately (remember input pullup)
        jumpsleft--;
      }
      else{
        yvelocity = FLIGHTJUMP;//A special kinda jump - less of a jump, and more of a controlled flight, because as I've found, its very hard
        //to control a player as they're inifinitely jumping
        buttontriggered = 0;//to allow for constant flight vs spam jumping
      }
    }
    if(digitalRead(BUTTON)){
      //if the button is released
      buttontriggered = 0;//reset the buttontriggered mechanism
    }
    if(leftpressed()&&left){//left
      playerx+=2048*XSPEED*deltatime;
    }
    if(rightpressed()&&right){
      playerx-=2048*XSPEED*deltatime;
    }
    display.drawBitmap(ONSCREENX,rendery(ONSCREENY),player,8,8,WHITE);
    //undo movements into walls - sets playerx,y  it to a multiple of 8, round in one direction or another
    up = 1;//reset box collider stops
    down = 1;
    left = 1;
    right = 1;
    shorterdown = 1;
    //FALLDEATH - checks if you fall past the stage
    if(playery<=FALLDEATH){
      GAMEOVER();
    }
    // put your main code here, to run repeatedly:
    //display.drawBitmap(x,y,bitmap,bx,by,WHITE);
    //display.invertDisplay(true);
    //display.setCursor(x,y);
    //display.print(text);
    //display.write(character);
    //literal F3 in my game - shows coords
    if(!digitalRead(BUTTONJOY)){
      display.setCursor(0,0);
      display.print("X: ");
      display.print(playerx);
      display.print(" Y: ");
      display.print(playery);
      if(!digitalRead(CHEATPIN)){
        //F3 CHEAT MODE
        display.setCursor(0,10);
        display.print("Warp: ");
        display.print(canwarp);
        display.print(" T: ");
        display.print(warptimer);
        display.setCursor(0,20);
        display.print("Tile: ");
        if(playery>=0&&playery<(LEVELMAXUP+1)*8&&playerx>=0&&playerx<LEVELMAXRIGHT*8){//makes sure u cant go under!
          display.print(getlevelcomponent(int(playerx/8),int(playery/8)-1,level));//the -1 ensures you get the component under you, not the one you are in
        }
        display.print("Collide:: ");
        display.print(!up ? "U" :
              !down ? "D" :
              !left ? "L" :
              !right ? "R" : "-");
      }
    }
    display.display();
    delay(TICKSPEED);//tune to the board used
    display.clearDisplay();
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
namespace MAZE_GAME{
  static const int  LEVELMAXUP = 32;//max height of level
  static const int  LEVELMAXRIGHT = 32;//max width towards right of level
  static const int  LEVELMAX = 9;//the number of levels
  static const float XSPEED = -1.5/1024;//the speed that the character goes left and right
  static const float YSPEED = 1.5/1024;
  static const int ONSCREENX = 64;//the position on the screen of the character
  static const int ONSCREENY = 32;//from the bottom - note that with 32, the player is slightly up, which helps when falling- 24 is dead center
  static const int BCEXPANDX = 1;//the amound that the box collider should be expanded by to account for gothrough - set to 1 because the frames are shorter
  static const int BCEXPANDY = 1;
  static const float PLAYERSTARTX = 8.0;//where the player starts
  static const float PLAYERSTARTY = 8.0;
  static const int ENEMYX = 2;//how far the enemy moves back and forth IN BLOCKS, NOT PIXILS
  static const int ENEMYY = 2;//how high the enemy moves up and down IN BLOCKS, NOT PIXILS
  static const int PLATFORMX = 8;//how far a platform moves horizontally IN BLOCKS, NOT PIXILS
  static const int PLATFORMY = 4;//how high a platform moves vertically IN BLOCKS, NOT PIXILS
  static const float PLATFORMSPEED = 1;//how fast a platform moves
  static const float ENEMYSPEED = 1;//every frame, the enemy moves a certain distance
  static const int LASERDIST = 6;//how far the laser travels
  static const float LASERSPEED = 2;
  static const int MAXSAVESLOTS = 10;//the max number of save slots allowed
  static const int GLOBALSWITCHFRAMES = 100;//how many frames the switch is allowed to be on for before shutting off. At 100, it's roughly 5 seconds, so it's roughly 20 fps
  static const int WARPCOOLDOWN = 0;//the cooldown from using the warp - at 20 it's roughly 0.25 seconds - There's no cooldown now because warps are one way now, with a warp to a warp platform. If there were a cooldown, then stuff like player cannons would stop functioning
  char saveslot = 0;/*THIS ALLOWS THE PLAYER TO USE CUSTOM LEVELS THEY MADE FROM FLASH. IT TELLS WHICH SAVE LEVEL THE PLAYER IS SAVING OR READING FROM
  //saveslot is a char fyi so it can save in flash easier, but it has to be converted to an int to be legible
  IF A LEVEL IS SAVED IN FLASH AND THE PLAYER CHOOSES LEVEL0, THEN THEY CAN CHANGE THIS VARIABLE. 
  saveslot = 0 means playing the level they made and have saved on sram, and other numbers mean playing more 
  custom levels saved in flash*/
  // - expands the number of platform types you can have to 17 instead of just 0-9
  enum TileMarkers : byte {
    A = 10,
    B = 11,
    C = 12,
    D = 13,
    E = 14,
    F = 15,
    G = 16,
    H = 17,
    I = 18
  };
  /*static const unsigned char blank[] PROGMEM = {
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000
  };*/
  static const unsigned char leveleditsprite[] PROGMEM = {
    B11111111,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B10000001,
    B11111111
};
  static const unsigned char blasterright[] PROGMEM = {
      B00000000,
      B01110000,
      B10011111,
      B01110000,
      B11000000,
      B10110000,
      B10001100,
      B11111111
  };
  static const unsigned char blasterleft[] PROGMEM = {
      B00000000,
      B00001110,
      B11111001,
      B00001110,
      B00000011,
      B00001101,
      B00110001,
      B11111111
  };
  static const unsigned char laser[] PROGMEM = {
      B00000000,
      B01011010,
      B10100101,
      B01011010,
      B00000000,
      B00000000,
      B00000000,
      B00000000
  };
  static const unsigned char jumpthrough[] PROGMEM = {
      B11111111,
      B10100101,
      B11111111,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000
  };
  static const unsigned char enemy[] PROGMEM = {
      B00100100,
      B01111110,
      B11111111,
      B01111110,
      B01111110,
      B11111111,
      B01111110,
      B00100100
  };
  static const unsigned char end[] PROGMEM = {
      B11110000,
      B11011100,
      B11000111,
      B11011100,
      B11110000,
      B11000000,
      B11000000,
      B11000000
  };
  static const unsigned char player[] PROGMEM = {
      B11111111,
      B10000001,
      B10111101,
      B10100101,
      B10100101,
      B10111101,
      B10000001,
      B11111111
  };
  static const unsigned char wall[] PROGMEM = {
      B11111111,
      B11000011,
      B10100101,
      B10011001,
      B10011001,
      B10100101,
      B11000011,
      B11111111,
  };
  static const unsigned char spike[] PROGMEM = {
      B00011000,
      B00111100,
      B01011010,
      B11111111,
      B11111111,
      B01011010,
      B00111100,
      B00011000
  };
  static const unsigned char switch_off[] PROGMEM = {
      B00000000,
      B00000000,
      B11000000,
      B01100000,
      B00110000,
      B00011000,
      B01111110,
      B11111111
  };
  static const unsigned char switch_on[] PROGMEM = {
      B00000000,
      B00000000,
      B00000011,
      B00000110,
      B00001100,
      B00011000,
      B01111110,
      B11111111
  };
  static const unsigned char door_closed[] PROGMEM = {
      B11111111,
      B10000001,
      B10011001,
      B10111101,
      B10111101,
      B10011001,
      B10000001,
      B11111111
  };
  static const unsigned char door_open[] PROGMEM = {
      B10000001,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B00000000,
      B10000001
  };
  static const unsigned char warpleft[] PROGMEM = {
      B00101100,
      B01010010,
      B01010010,
      B10100001,
      B10100001,
      B01010010,
      B01010010,
      B00101100
  };
  static const unsigned char warpright[] PROGMEM = {
    B00110100,
    B01001010,
    B01001010,
    B10000101,
    B10000101,
    B01001010,
    B01001010,
    B00110100
  };
  static const unsigned char warpup[] PROGMEM = {
    B00011000,
    B01100110,
    B10011001,
    B01100110,
    B10000001,
    B10000001,
    B01100110,
    B00011000
  };
  static const unsigned char warpdown[] PROGMEM = {
    B00011000,
    B01100110,
    B10000001,
    B10000001,
    B01100110,
    B10011001,
    B01100110,
    B00011000
  };
  static const unsigned char warpstop[] PROGMEM = {//this is a stop platform for warps
    B00000000,
    B01000010,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B01000010,
    B00000000
  };
  static const unsigned char warpparticle[] PROGMEM = {//this is a stop platform for warps
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B00100100,
    B00011000,
    B00000000,
    B00000000
  };
  String tileindex(int tile){ 
    switch(tile){
      //the x and y refer to sideways and up respectively
      case 0: return "Air";
      case 1: return "Block";
      case 2: return "Spike";
      case 3: return "X enemy";
      case 4: return "Y enemy";
      case 5: return "Level End";
      case 6: return "X platform";
      case 7: return "Y platform";
      case 8: return "OneWay Platform";
      //case -1: //return "Bounce Platform";
      //case -1: //return "Launch Platform";
      //case -1: //return "Water";
      case 9: return "Right Blaster";
      case A: return "Left Blaster";
      //case -1: //return "Powerup";
      case B: return "Switch";
      case C: return "Closed door";
      case D: return "Open door";
      //case -1: //return "Right conveyor";
      //case -1: //return "Left conveyor";
      //case -1: //return "Jump orb";
      case E: return "Warp right";
      case F: return "Warp left";
      case G: return "Warp up";
      case H: return "Warp down";
      case I: return "Warp end";
      /*  WARPS - The way each warp works is that once you enter it, it uses a for loop to search for a corisponding warp stop in its direction (if it is a right warp, it searches right)
  Once it finds that warp stop, it will teleport the player. Warps are only 1 way - they only warp someone to the warp stop, not back*/
      return "n/a";
    }
  }
  //note that for the horizontal enemy, it starts on the left, vertical starts on bottom
  //note that byte is used instead of int because byte takes up 4x less memory
  //byte ranges numbers from 0 to 255 - I believe you can have around 20 or so levels
  /*static const byte Blanklevel[LEVELMAXUP][LEVELMAXRIGHT] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  */
  byte blankflashreadlevel[LEVELMAXUP][LEVELMAXRIGHT] = //THIS IS THE LEVEL THAT IS READ FROM FLASH MEMORY. DO NOT EDIT EVER
  {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level0[LEVELMAXUP][LEVELMAXRIGHT] = //THIS LEVEL IS BLANK BECUSE IT IS STORED IN SRAM! In addition, it also does not have the const modifier so it's editable!
  {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level1[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level2[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level3[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level4[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level5[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level6[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level7[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level8[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  byte level9[LEVELMAXUP][LEVELMAXRIGHT] = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  };
  //player controls and variables
  float playerx = PLAYERSTARTX;//player position to start
  float playery = PLAYERSTARTY;
  bool buttontriggered = 0;//checks if the main button has been triggered - it makes it so you can just hold down the jump button
  bool up = 1;//can the player move in certian directions
  bool down = 1;
  bool right = 1;
  bool left = 1;
  bool shorterdown = 1;//same as down but for a smaller box collider - is used so player can rise out of the ground if gravity function detects that the player is in the ground - that is not affected if the player hits the side of a wall
  //the level you are playing
  int level = 1;
  //the type of thing on each level - 0 is air, 1 is block...
  int levelpart = 0;
  //the collision of objects - sets the player's moveable options
  float enemyx = 0;//as the enemies move side to side
  float enemyy = 0;//as the enemy moves up and down
  float platformx = 0;//same for platforms
  float platformy = 0;
  bool enemyxflip = 0;//when to turn around the enemies and platforms so they move back and forth
  bool enemyyflip = 0;
  bool platformxflip = 0;
  bool platformyflip = 0;
  bool globalswitch = 0;//IF THE GLOBAL SWITCH IS SET TO ON!
  float globalswitchtimer = 0;//the timer for the global switch - uses ticks rather than milliseconds
  bool canwarp = 1;//if the player can warp
  float warptimer = 0;
  float laserpos = 0;//the position where the laser is from the launcher - cordinated for right lasers, left lasers do the opposite
  void collision(int x,int y){
    //the platform - if it is 6, it adds horizontal movement to the player. 7 adds vertical movement
    //uses x*8 and y*8 cordinates - not y inversed
    if(playerx<x+8+BCEXPANDX && playerx+8+BCEXPANDX>x && playery-8-BCEXPANDY<y && playery>y-8-BCEXPANDY){
      //if the player has collided, with what?
      //2nd if statement needed so player can slip sideways - checks if the player's hitbox is within the actual hitbox(not expanded) of the terrain
      //this is needed because the box colliders were expanded to prevent falling through, so to prevent the player from not being able to move sideways, when checking the left and right moving abilities, it only checks if the player is deeply embedded in the wall, not its expand hitbox. The player will stop falling if it is in the expanded hitbox, but it will not catch on the next hitboxes when moving sideways because those are shorter than the falling expanded hitbox.
      if(playery-8<y && playery>y-8){
        if(playerx<x+8+BCEXPANDX && playerx>x+4){//player is to right of wall
          left = 0;
        }
        if(playerx+8+BCEXPANDX>x && playerx+4<x){//player is to left of wall
          right = 0;
        }
      }
      if(playerx-8<x && playerx>x-8){
        //2nd if statement needed so player can slip sideways - same as above
        if(playery-8-BCEXPANDY<y && playery-4>y){//player is above wall
          down = 0;
        }
        if(playery>y-8-BCEXPANDY && playery<y-4){//player is under wall
          up = 0;
        }
      }
    }
  }
  void jumpthroughcollision(int x,int y){
    //this type of collision only detects landing - nothing else happens if there is no collision other than landing. It is used for jump through platforms that you can jump through and land on
    if(playerx-8<x && playerx>x-8){
      //2nd if statement needed so player can slip sideways - same as above
      if(playery-8-BCEXPANDY<y && playery-4>y){//player is above wall
        down = 0;
      }
    }
  }
  //simply checks if the player collided - used for enemies and spikes
  bool collided(int x,int y){//uses x*8 and y*8 cordinates - not y inversed - BCEXPAND is not needed because this is only for enemies and spikes, which player can pass through and die
    if(playerx<x+8 && playerx+8>x && playery-8<y && playery>y-8){
      return 1;
    }
    else{
      return 0;
    }
  }
  int rendery(int yvalue){
    //the OLED y position when trying to animate on the OLED - note it is reversed
    return 56-yvalue;
  }
  //gameover function
  void LEVELRESET(){
    playerx = PLAYERSTARTX;
    playery = PLAYERSTARTY;
    enemyx = 0;
    enemyy = 0;
    platformx = 0;
    platformy = 0;
    enemyxflip = 0;
    enemyyflip = 0;
    platformxflip = 0;
    platformyflip = 0;
    laserpos = 0;
    globalswitchtimer = 0;//reset switch
    globalswitch = 0;
    warptimer = 0;//reset warp
    canwarp = 1;
  }
  //LEVEL EXPORTER - ABSOLUTELY SILLY, BUT IT WORKS:)
  void exportlevel(byte level[LEVELMAXUP][LEVELMAXRIGHT]){
    Keyboard.write(uint8_t('{'));
    Keyboard.write(uint8_t('\n'));// /newline
    for(int i = 0;i<LEVELMAXUP;i++){
      Keyboard.write(uint8_t('{'));//write the start of the row
      for(int j = 0;j<LEVELMAXRIGHT;j++){
        if(level[i][j]<=9){
          Keyboard.print(level[i][j]);
        }
        else{
          switch(level[i][j]){//yea I know you can just do Keyboard.write(uint8_t('A'+leveltoedit[i][j]-10)); - every number over 10 is converted to A+a certain number (A+0 = A,A+1=B...)
            case 10:
            Keyboard.write(uint8_t('A'));
            break;
            case 11:
            Keyboard.write(uint8_t('B'));
            break;
            case 12:
            Keyboard.write(uint8_t('C'));
            break;
            case 13:
            Keyboard.write(uint8_t('D'));
            break;
            case 14:
            Keyboard.write(uint8_t('E'));
            break;
            case 15:
            Keyboard.write(uint8_t('F'));
            break;
            case 16:
            Keyboard.write(uint8_t('G'));
            break;
            case 17:
            Keyboard.write(uint8_t('H'));
            break;
            case 18:
            Keyboard.write(uint8_t('I'));
            break;
          }
        }
        Keyboard.write(uint8_t(','));
      }
      Keyboard.write(uint8_t('}'));//write the end of the 
      //the last row has no comma
      if(i<LEVELMAXUP-1){
        Keyboard.write(uint8_t(','));
      }
      Keyboard.write(uint8_t('\n'));// /newline
    }
    Keyboard.write(uint8_t('}'));
  }
  //IMPORTANT BIT OF CODE - THIS IS THE CODE THAT LETS YOU ACCESS EACH ARRAY!!! CHANGE WHEN ADDING MORE LEVELS!!!
  int getlevelcomponent(int coorx,int coory,int leveltoread){//given a set of coordinates and a level, you can get the tile at each individual point in the level
    switch(leveltoread){
      //note that getlevelcomponent also handles the math - so playerx = 0, playery = 0 = the bottom left tile!
      case 0:
        if(saveslot==0){
          //sram
          return level0[LEVELMAXUP-coory-1][coorx];//this is stored in flash, as you can edit this level using the level editor
        }
        else{
          return blankflashreadlevel[LEVELMAXUP-coory-1][coorx];//this level is read from flash memory in the custom level selection screen.
        }
      break;
      case 1:
        return pgm_read_byte_near(&level1[LEVELMAXUP-coory-1][coorx]);
      break;
      case 2:
        return pgm_read_byte_near(&level2[LEVELMAXUP-coory-1][coorx]);
      break;
      case 3:
        return pgm_read_byte_near(&level3[LEVELMAXUP-coory-1][coorx]);
      break;
      case 4:
        return pgm_read_byte_near(&level4[LEVELMAXUP-coory-1][coorx]);
      break;
      case 5:
        return pgm_read_byte_near(&level5[LEVELMAXUP-coory-1][coorx]);
      break;
      case 6:
        return pgm_read_byte_near(&level6[LEVELMAXUP-coory-1][coorx]);
      break;
      case 7:
        return pgm_read_byte_near(&level7[LEVELMAXUP-coory-1][coorx]);
      break;
      case 8:
        return pgm_read_byte_near(&level8[LEVELMAXUP-coory-1][coorx]);
      break;
      case 9:
        return pgm_read_byte_near(&level9[LEVELMAXUP-coory-1][coorx]);
      break;
    }
  }
  //LEVEL EDITOR - AN ABSOLUTE MASTERPIECE!!!!!!!!!!!!!!<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  void editlevel(byte leveltoedit[LEVELMAXUP][LEVELMAXRIGHT]){
    //accepts the level - THIS IS FOR SOME REASON A REFERENCE, SO IT EDITS THE REAL LEVEL

    //FLASH EDIT
    //asks to edit from flash and if so, assigns leveltoedit to the flash level
    bool flash = 0;
    while(digitalRead(BUTTON)){
      display.setCursor(13,0);
      display.print("Edit From Flash?");//automatically saves in sram, however you have to input if you want to save it in flash
      display.setCursor(24,32);
      display.print("Yes");
      display.setCursor(58,32);
      display.print("No");
      if(leftpressed()||rightpressed()){
        //switch from saving to not saving
        flash = !flash;
        delay(200);
      }
      if(flash){
        display.drawRect(23,32,18,8,WHITE);
      }
      else{
        display.drawRect(57,32,18,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //rather than a saveslot stored here, it uses the saveslot variable initiated at the very top
    if(flash){//if using level from flash memory
      ::loading_screen();
      saveslot = 1;
      while(digitalRead(BUTTON)){
        display.setCursor(0,32);
        display.print("Save Slot: ");
        if(leftpressed()&&saveslot>1){
          saveslot--;
          delay(200);
        }
        if(rightpressed()&&saveslot<MAXSAVESLOTS){
          saveslot++;
          delay(200);
        }
        display.print(int(saveslot));
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      display.setCursor(37,32);
      display.print("Editing..");
      display.display();
      prefs.begin("Maze_Levels", true);//begin the level reading mechanism - true means read only.
      //flash
      prefs.getBytes(&saveslot, leveltoedit, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
      //it uses sizeof(byte) to figure out the size of an array of bytes
      //note that saveslot's char is actually a weird ASCII conversion - its not 
      //SETS LEVELTOEDIT TO THE FLASH's BYTE
      prefs.end();//it only reads the level, stores it in SRAM, and then puts stops reading it.
      delay(2000);
      display.clearDisplay();
    }
    else{
      //editing SRAM level
      saveslot = 0;//means editing new level draft, not something from flash
      display.setCursor(37,32);
      display.print("Editing..");
      display.display();
      delay(2000);
      display.clearDisplay();
    }
    ::loading_screen();
    //finish flash
    //asks the player if they want to erase the current level

    //ERASER CODE
    bool erase = 0;
    while(digitalRead(BUTTON)){
      display.setCursor(4,0);
      display.print("Erase Current Level?");//automatically saves in sram, however you have to input if you want to save it in flash
      display.setCursor(24,32);
      display.print("Yes");
      display.setCursor(58,32);
      display.print("No");
      if(leftpressed()||rightpressed()){
        //switch from saving to not saving
        erase = !erase;
        delay(200);
      }
      if(erase){
        display.drawRect(23,32,18,8,WHITE);
      }
      else{
        display.drawRect(57,32,18,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    if(erase){
      display.setCursor(34,32);
      display.print("Erasing...");
      display.display();
      delay(2000);
      display.clearDisplay();
      for(int i = 0;i<LEVELMAXUP;i++){
        for(int j=0;j<LEVELMAXRIGHT;j++){
          leveltoedit[i][j] = 0;//reset the entier level to 0
        }
      }
    }
    else{
      display.setCursor(34,32);
      display.print("Editing...");
      display.display();
      delay(2000);
      display.clearDisplay();
    }
    //BEGIN EDITING
    //starts the player, however it does some weird stuff
    playerx = PLAYERSTARTX;
    playery = PLAYERSTARTY;
    while(!(!digitalRead(BUTTONJOY)&&!digitalRead(BUTTON))){
      //RUN DELTATIME
      ::currenttime = millis();
      ::deltatime = float(::currenttime - ::prevtime) / float(CLOCKSPEED);  // normalized to your original 50ms tick
      if(::deltatime>DELTATIMECAP){//cap deltatime
        ::deltatime=DELTATIMECAP;
      }
      ::prevtime = ::currenttime;
      //because button is inversed due to INPUT_PULLUP, then they both need to be off to be both pressed (buttonjoy is normal)
      //NOTE: this uses all the same variables as the player does, it's like the player's walking through the level! 
      //In addition, if the joystick is used, there is a delay built in so the joystick does not spam.
      //note the the x controls are a bit flipped - it's possible to edit them later
      if(leftpressed()){//left
        playerx-=8;
        delay(100);
      }
      if(rightpressed()){
        playerx+=8;
        delay(100);
      }
      if(uppressed()){//left
        playery+=8;
        delay(100);
      }
      if(downpressed()){
        playery-=8;
        delay(100);
      }
      //constrain functions----------------------------------------------------------------------------------------------------------------
      playerx = constrain(playerx,0,(LEVELMAXRIGHT-1)*8);
      playery = constrain(playery,0,(LEVELMAXUP-1)*8);
      //Select the sprite used! - remember button is input_pullup
      if(!digitalRead(BUTTON)){
        //quick delay - this is so you dont accedentially place blocks if you are trying to exit
        delay(200);
        if(!digitalRead(BUTTONJOY)&&!digitalRead(BUTTON)){
          break;//leave the level editor if you clicked both
        }
        //remember the player's y position is completely flipped!
        if(leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]<18){
          //increments part
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]++;
        }
        else{
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]=0;//if the leveltoedit part is greater than or equal to 14, the tape resets, and the part is set to 0, or air again.
        }
      }
      if(!digitalRead(BUTTONJOY)){
        //quick delay - this is so you dont accedentially place blocks if you are trying to exit
        delay(200);
        if(!digitalRead(BUTTONJOY)&&!digitalRead(BUTTON)){
          break;//leave the level editor if you clicked both
        }
        //remember the player's y position is completely flipped!
        if(leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]>0){
          //increments part
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]--;
        }
        else{
          leveltoedit[LEVELMAXUP-1-int(playery/8.0)][int(playerx/8.0)]=18;//if the leveltoedit part is greater than or equal to 14, the tape resets, and the part is set to 0, or air again.
        }
      }
      //renders the level
      for(int posy = 0;posy<LEVELMAXUP;posy++){
        for(int posx = 0;posx<LEVELMAXRIGHT;posx++){
          //note - levelmaxup-posy is caused because the for loop is inverted for posy. Also, for some reason, the level ordinates are y,x
          levelpart = leveltoedit[LEVELMAXUP-posy-1][posx];//reads the level, then renders it
          switch(levelpart){
            //note that you have to do pgm_read_byte_near(&level[LEVELMAXUP-posy-1][posx]) to read progmem data - note the -8 at the end - I had to include it because the occlusion culling at 0 cut the rendering short
            case 1://wall
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),wall,8,8,WHITE);
              }
            break;
            case 2://spike
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),spike,8,8,WHITE);
              }
            break;
            case 3://horizontal enemy
              if(posx*8-playerx+ONSCREENX+enemyx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+enemyx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx+enemyx,rendery(posy*8-playery+ONSCREENY),enemy,8,8,WHITE);
              }
            break;
            case 4://vertical enemy
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+enemyy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+enemyy)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY+enemyy),enemy,8,8,WHITE);
              }
            break;
            case 5://end
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),end,8,8,WHITE);
              }
            break;
            case 6://horizontal platform
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX+platformx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+platformx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX+platformx,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
              }
            break;
            case 7://vertical platform
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+platformy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+platformy)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY+platformy),jumpthrough,8,8,WHITE);
              }
            break;
            case 8://jumpthrough platform
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
              }
            break;
            case 9://laser right
              //draws laser blaster
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterright,8,8,WHITE);
              }
              //draw laser
              if(posx*8-playerx+ONSCREENX+laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx+laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
              }
            break;
            case A://laser left
              //draws laser blaster
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterleft,8,8,WHITE);
              }
              //draw laser
              if(posx*8-playerx+ONSCREENX-laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX-laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx-laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
              }
            break;
            case B://switch
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),switch_off,8,8,WHITE);
              }
            break;
            case C://door closed
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_closed,8,8,WHITE);
              }
            break;
            case D://door open
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_open,8,8,WHITE);
              }
            break;
            case E://WARP RIGHT
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpright,8,8,WHITE);
              }
            break;
            case F://WARP LEFT
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpleft,8,8,WHITE);
              }
            break;
            case G://WARP UP
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpup,8,8,WHITE);
              }
            break;
            case H://WARP DOWN
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpdown,8,8,WHITE);
              }
            break;
            case I://WARP STOP
              //draws bitmap if in screen - allows for occlusion culling
              if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
                display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpstop,8,8,WHITE);
              }
            break;
          }
        }
      }
      //animations
      //Move and flip the laser
      laserpos+=LASERSPEED*deltatime;
      if(laserpos>=LASERDIST*8){
        //note the *8 because laserspeed is in pixils
        laserpos = 0;
      }
      //Move the enemies
      if(!enemyxflip){
        enemyx+=ENEMYSPEED*deltatime;
      }
      else{
        enemyx-=ENEMYSPEED*deltatime;
      }
      //flip the enemy - make it move left and right
      if(enemyx>=ENEMYX*8||enemyx<=0){
        enemyxflip = !enemyxflip;
      }
      //move the vertical enemy
      if(!enemyyflip){
        enemyy+=ENEMYSPEED*deltatime;
      }
      else{
        enemyy-=ENEMYSPEED*deltatime;
      }
      //flip the enemy - make it move up and down
      if(enemyy>=ENEMYY*8||enemyy<=0){
        enemyyflip = !enemyyflip;
      }
      //move the horizontal platform
      if(!platformxflip){
        platformx+=PLATFORMSPEED*deltatime;
      }
      else{
        platformx-=PLATFORMSPEED*deltatime;
      }
      //flip the platform - make it move up and down
      if(platformx>=PLATFORMX*8||platformx<=0){
        platformxflip = !platformxflip;
      }
      //move the vertical platform
      if(!platformyflip){
        platformy+=PLATFORMSPEED*deltatime;
      }
      else{
        platformy-=PLATFORMSPEED*deltatime;
      }
      //flip the platform - make it move up and down
      if(platformy>=PLATFORMY*8||platformy<=0){
        platformyflip = !platformyflip;
      }
      //draw UI in the corner - coordinates + tile type - like f3
      //coordinates
      display.setCursor(0,0);
      display.print("X: ");
      display.print(playerx);
      display.print(" Y: ");
      display.print(playery);
      //write tile discription
      display.setCursor(0,56);
      display.print("Tile: ");
      //note the odd math inside the brackets - this converts playery and playerx into x and y positions in the array!
      display.print(tileindex(leveltoedit[LEVELMAXUP-int(playery/8)-1][int(playerx/8)]));
      //end of animations
      display.drawBitmap(ONSCREENX,rendery(ONSCREENY),leveleditsprite,8,8,WHITE);//draw the sprite to level select
      //draw edge boarders
      display.drawRect(64-playerx,32-LEVELMAXUP*8+playery,LEVELMAXRIGHT*8,LEVELMAXUP*8,WHITE);//draw the edge boarders around the entire screen
      display.display();//display everything
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //display confirmation message
    display.clearDisplay();
    display.setCursor(32,32);
    display.print("Level Edited!");
    display.display();
    delay(2000);
    display.clearDisplay();
    //SAVING CODE - IF SAVESLOT = 0 - BASICALLY THE PLAYER DIDNT EDIT A PREVIOUS SAVE, THEN ALLOW THE PLAYER TO CHOOSE THE FLASH TO SAVE IN
    //IF THEY DID EDIT A PREVIOUS SAVE, OVERWRITE THAT SAVE.
    if(saveslot==0){
      //IF I CREATED AN ENTIRELY NEW LEVEL
      //SAVE LEVEL
      bool save = 0;
      while(digitalRead(BUTTON)){
        display.setCursor(4,0);
        display.print("Save Level in Flash?");//automatically saves in sram, however you have to input if you want to save it in flash
        display.setCursor(24,32);
        display.print("Yes");
        display.setCursor(58,32);
        display.print("No");
        if(leftpressed()||rightpressed()){
          //switch from saving to not saving
          save = !save;
          delay(200);
        }
        if(save){
          display.drawRect(23,32,18,8,WHITE);
        }
        else{
          display.drawRect(57,32,18,8,WHITE);
        }
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      saveslot = 1;
      bool cancel = 0;
      ::loading_screen();
      if(save){//if saving, begin saving mechanism
        while(digitalRead(BUTTON)){
          display.setCursor(0,32);
          display.print("Save Slot: ");
          if(leftpressed()&&saveslot>1){
            saveslot--;
            delay(200);
          }
          if(rightpressed()&&saveslot<MAXSAVESLOTS){
            saveslot++;
            delay(200);
          }
          display.print(int(saveslot));
          if(!digitalRead(BUTTONJOY)){
            //cancel save
            cancel = 1;
            break;
          }
          display.display();
          delay(TICKSPEED);
          display.clearDisplay();
        }
        if(!cancel){
          //saving
          display.setCursor(37,32);
          display.print("Saving...");
          display.display();
          delay(2000);
          prefs.begin("Maze_Levels", false);//begin the level editing mechanism
          prefs.putBytes(&saveslot, leveltoedit, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
          //it uses sizeof(byte) to figure out the size of an array of bytes
          //note that um saveslot's char is actually not saved as an integer - it's saved as some weird ASCII conversion
          prefs.end();
          display.clearDisplay();
        }
        else{
          //canceled
          display.clearDisplay();
          display.setCursor(37,32);
          display.print("Canceled");
          display.display();
          delay(2000);
          display.clearDisplay();
        }
      }
    }
    else{
      display.setCursor(37,32);
      display.print("Saving...");
      display.display();
      delay(2000);
      prefs.begin("Maze_Levels", false);//begin the level editing mechanism
      prefs.putBytes(&saveslot, leveltoedit, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
      //it uses sizeof(byte) to figure out the size of an array of bytes
      //note that um saveslot's char is actually not saved as an integer - it's saved as some weird ASCII conversion
      prefs.end();
      display.clearDisplay();
    }
    bool executeexport = 0;
    if(true){//REPLACE WITH if serial detects if a USB cable is plugged in. If there is one, then it gives the option to export. If there isnt, then it doesnt
      while(digitalRead(BUTTON)){
        display.setCursor(13,0);
        display.print("Export level?");//automatically saves in sram, however you have to input if you want to save it in flash
        display.setCursor(24,32);
        display.print("Yes");
        display.setCursor(58,32);
        display.print("No");
        if(leftpressed()||rightpressed()){
          //switch from saving to not saving
          executeexport = !executeexport;
          delay(200);
        }
        if(executeexport){
          display.drawRect(23,32,18,8,WHITE);
        }
        else{
          display.drawRect(57,32,18,8,WHITE);
        }
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      if(executeexport){
        display.setCursor(37,32);
        display.print("Exporting..");
        display.display();
        exportlevel(leveltoedit);
        delay(1000);
        display.clearDisplay();
      }
    }
    ::loading_screen();
  }
  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  void CUSTOMHOMEPAGE(){
    //a secondary homepage for selecting the custom level
    //SAVE LEVEL
    bool flash = 0;
    while(digitalRead(BUTTON)){
      display.setCursor(13,0);
      display.print("Levels From Flash?");//automatically saves in sram, however you have to input if you want to save it in flash
      display.setCursor(24,32);
      display.print("Yes");
      display.setCursor(58,32);
      display.print("No");
      if(leftpressed()||rightpressed()){
        //switch from saving to not saving
        flash = !flash;
        delay(200);
      }
      if(flash){
        display.drawRect(23,32,18,8,WHITE);
      }
      else{
        display.drawRect(57,32,18,8,WHITE);
      }
      display.display();
      delay(TICKSPEED);
      display.clearDisplay();
    }
    //rather than a saveslot stored here, it uses the saveslot variable initiated at the very top
    if(flash){//if using level from flash memory
      ::loading_screen();
      saveslot = 1;
      while(digitalRead(BUTTON)){
        display.setCursor(0,32);
        display.print("Save Slot: ");
        if(leftpressed()&&saveslot>1){
          saveslot--;
          delay(200);
        }
        if(rightpressed()&&saveslot<MAXSAVESLOTS){
          saveslot++;
          delay(200);
        }
        display.print(int(saveslot));
        display.display();
        delay(TICKSPEED);
        display.clearDisplay();
      }
      display.setCursor(37,32);
      display.print("Playing..");
      display.display();
      prefs.begin("Maze_Levels", true);//begin the level reading mechanism - true means read only.
      //flash
      prefs.getBytes(&saveslot, blankflashreadlevel, LEVELMAXUP*LEVELMAXRIGHT*sizeof(byte));//remember that saveslot must be a char
      //it uses sizeof(byte) to figure out the size of an array of bytes
      //note that saveslot's char is actually a weird ASCII conversion - its not 
      prefs.end();//it only reads the level, stores it in SRAM, and then puts stops reading it.
      delay(2000);
      display.clearDisplay();
    }
    else{
      //playing SRAM level
      saveslot = 0;
      display.setCursor(37,32);
      display.print("Playing..");
      display.display();
      delay(2000);
      display.clearDisplay();
    }
    //
  }
  void HOMEPAGE(){
    //the navigation homepage - called by GAMEOVER function
    //joystick up and down to navigate up and down, button to select.
    /*NOTES - To do this, use setcursor and print, use LEVELMAX to set the max scrolling, print the current level, the level before it above it, 
    and the next level under it. Change the variable level as you do this, and use a while loop until */
    while(digitalRead(BUTTON)!=0){
      if(!digitalRead(BUTTONJOY)&&(leftpressed()||rightpressed())){
        ::MAIN_PAGE();//returns to main page if both buttons are pressed
        return;//completely exits out of the homepage function
      }
      //clears the display
      display.clearDisplay();
      //while the button is not pressed
      //add computational logic here
      if(uppressed()&&level<LEVELMAX){
        //joystick up - cannot go over LEVELMAX
        level++;
      }
      if(downpressed()&&level>0){
        //joystick down - cannot go under 1
        level--;
      }
      //add printing commands - print out the screen
      //prints out the current level
      display.setCursor(32,32);
      if(level!=0){
        display.print("Level: ");
        display.print(level);
      }
      else{
        //custom level!
        display.print("Level: ");
        display.print("Custom");
        display.setCursor(0,44);
        display.print("Both buttons: edit");
        display.setCursor(0,56);
        display.print("Main button: play");
      }
      //prints out previous level
      if(level>=0+1){
        //level>1+1 because if level is one, there is no previous level - prints below current level
        display.setCursor(72,48);
        if(level-1!=0){
          display.print(level-1);
        }
        else{
          display.print("Custom");
        }
      }
      //prints out next level
      if(level<=LEVELMAX-1){
        //level>1+1 because if level is one, there is no previous level - prints below current level
        display.setCursor(72,16);
        display.print(level+1);
      }
      //prints out words "Homepage!"
      display.setCursor(0,8);
      display.print("Maze Game!");
      //displays
      display.display();
      delay(200);
    }
    //THIS IS IMPORTANT - IT MAKES IT SO IF YOU HAVE THE JOYSTICK BUTTON PRESSED TOO, YOU CAN EDIT THE LEVEL INSTEAD OF STARTING
    //REMEMBER JOYBUTTON IS INPUT_PULLUP
    if(level==0&&!digitalRead(BUTTONJOY)){
      ::loading_screen();//to waste people's time
      display.clearDisplay();
      editlevel(level0);
      //after editing level, call HOMEPAGE again
      HOMEPAGE();
    }
    else{
      if(level==0){
        ::loading_screen();//to waste people's time
        CUSTOMHOMEPAGE();//allows people to select custom levels from sram or flash
      }
      ::loading_screen();//to waste people's time
      LEVELRESET();
    }
    //
  }
  void GAMEOVER(){
    //called whenever you hit an enemy or spike or fall, ends the game perminately
    if(digitalRead(CHEATPIN)){//CHEATING AGAIN???
      display.clearDisplay();
      display.setCursor(32,32);
      display.print("GAME OVER!!!");
      display.display();
      delay(2000);//wait 2 seconds and then reset the game
      HOMEPAGE();
    }
    else{
      display.setCursor(4,52);
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print("CHEATED DEATH!");//uhh yea - CHEATER
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.print(char(random(0,255)));
      display.display();
    }
  }
  void NEXTLEVEL(){
    //called when you reach the end flag - sets up the next level - sends you to the homepage and makes the level go up
    display.clearDisplay();
    display.setCursor(4,32);
    display.print("LEVEL ");
    display.print(level);
    display.print(" COMPLETED!!!");
    display.display();
    delay(2000);//wait 2 seconds and then enter the homepage
    if(level<LEVELMAX){//make sure levels cannot go past LEVELMAX
      level++;
    }
    HOMEPAGE();
  }
  void init() {
    HOMEPAGE();
  }
  void update() {
    //this following code is not optomized
    for(int posy = 0;posy<LEVELMAXUP;posy++){
      for(int posx = 0;posx<LEVELMAXRIGHT;posx++){
        //note - levelmaxup-posy is caused because the for loop is inverted for posy. Also, for some reason, the level ordinates are y,x
        //looks through the levels
        //add levels if needed
        levelpart = getlevelcomponent(posx,posy,level);
        //note that all flash and custom levels are under level 0. Customhomepage lets you choose your flash or custom level and autopopulates level0 for you with your designated level.
        switch(levelpart){
          //note that you have to do pgm_read_byte_near(&level[LEVELMAXUP-posy-1][posx]) to read progmem data - note the -8 at the end - I had to include it because the occlusion culling at 0 cut the rendering short
          case 1://wall
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),wall,8,8,WHITE);
            }
            collision(posx*8,posy*8);
          break;
          case 2://spike
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),spike,8,8,WHITE);
            }
            if(collided(posx*8,posy*8)){
              GAMEOVER();
            }
          break;
          case 3://horizontal enemy
            if(posx*8-playerx+ONSCREENX+enemyx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+enemyx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx+enemyx,rendery(posy*8-playery+ONSCREENY),enemy,8,8,WHITE);
            }
            if(collided(posx*8+enemyx,posy*8)){
              GAMEOVER();
            }
          break;
          case 4://vertical enemy
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+enemyy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+enemyy)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY+enemyy),enemy,8,8,WHITE);
            }
            if(collided(posx*8,posy*8+enemyy)){
              GAMEOVER();
            }
          break;
          case 5://end
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),end,8,8,WHITE);
            }
            if(collided(posx*8,posy*8)){
              NEXTLEVEL();
            }
          break;
          case 6://horizontal platform
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX+platformx<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+platformx>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX+platformx,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
            }
            collision(posx*8+platformx,posy*8);
          break;
          case 7://vertical platform
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY+platformy)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY+platformy)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY+platformy),jumpthrough,8,8,WHITE);
            }
            collision(posx*8,posy*8+platformy);
          break;
          case 8://jumpthrough platform
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),jumpthrough,8,8,WHITE);
            }
            //note for collision - this platform only affects landing on it, nothing else. - therefore, it uses a special type of collision
            jumpthroughcollision(posx*8,posy*8);
          break;
          case 9://laser right
            //draws laser blaster
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterright,8,8,WHITE);
            }
            collision(posx*8,posy*8);
            //draw laser
            if(posx*8-playerx+ONSCREENX+laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX+laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx+laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
            }
            if(collided(posx*8+laserpos,posy*8)){
              GAMEOVER();
            }
          break;
          case A://laser left
            //draws laser blaster
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),blasterleft,8,8,WHITE);
            }
            collision(posx*8,posy*8);
            //draw laser
            if(posx*8-playerx+ONSCREENX-laserpos<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX-laserpos>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8+ONSCREENX-playerx-laserpos,rendery(posy*8-playery+ONSCREENY),laser,8,8,WHITE);
            }
            if(collided(posx*8-laserpos,posy*8)){
              GAMEOVER();
            }
          break;
          case B://switch
            //draw switch
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              //switch on vs switch off - if the global switch is on, all switches render on
              if(globalswitch){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),switch_on,8,8,WHITE);
              }
              else{
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),switch_off,8,8,WHITE);
              }
            }
            if(collided(posx*8,posy*8)){
              globalswitchtimer = GLOBALSWITCHFRAMES;//reset the switch timer
              globalswitch = 1;//turn globalswitch on
            }
          break;
          case C://door closed
            //draw switch
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              //switch on vs switch off - if the global switch is on, all switches render on
              if(globalswitch){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_open,8,8,WHITE);
              }
              else{
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_closed,8,8,WHITE);
                collision(posx*8,posy*8);//if the door is closed run collisions
              }
            }
          break;
          case D://door open - this door stays open and closes when the switch is hit
            //draw switch
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              //switch on vs switch off - if the global switch is on, all switches render on
              if(!globalswitch){
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_open,8,8,WHITE);
              }
              else{
                display.drawBitmap(posx*8+ONSCREENX-playerx,rendery(posy*8-playery+ONSCREENY),door_closed,8,8,WHITE);
                collision(posx*8,posy*8);//if the door is closed run collisions
              }
            }
          break;
          case E://WARP RIGHT
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpright,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp
                for(int xsearch = posx; xsearch<LEVELMAXRIGHT; xsearch++){
                  //search starts at posx
                  if(getlevelcomponent(xsearch,posy,level)==I){
                    //basically if it finds a portal stop, stop
                    playerx = xsearch*8;//warp the player to the searched tile!
                    playery = posy*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case F://WARP LEFT
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpleft,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp
                for(int xsearch = posx; xsearch>=0; xsearch--){
                  //search starts at posx
                  if(getlevelcomponent(xsearch,posy,level)==I){
                    //basically if it finds a portal stop, stop
                    playerx = xsearch*8;//warp the player to the searched tile!
                    playery = posy*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case G://WARP UP
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpup,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp
                for(int ysearch = posy; ysearch<LEVELMAXUP; ysearch++){
                  //search starts at posy
                  if(getlevelcomponent(posx,ysearch,level)==I){
                    //basically if it finds a portal stop, stop
                    playerx = posx*8;//warp the player to the searched tile!
                    playery = ysearch*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case H://WARP DOWN
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpdown,8,8,WHITE);
              if(collided(posx*8,posy*8)&&canwarp){
                //start cooldown
                warptimer = WARPCOOLDOWN;
                canwarp = 0;
                //search for a warp stop in the direction of the warp - I know it's reversed but the top left corner is the [0,0] so ysearch must increase to go down
                for(int ysearch = posy; ysearch>=0; ysearch--){
                  //search starts at posy
                  if(getlevelcomponent(posx,ysearch,level)==I){
                    //basically if it finds a portal stop, stop
                    playerx = posx*8;//warp the player to the searched tile!
                    playery = ysearch*8;
                    break;//break the loop
                  }
                }
              }
            }
          break;
          case I://WARPSTOP
            //draws bitmap if in screen - allows for occlusion culling
            if(posx*8-playerx+ONSCREENX<=SCREEN_WIDTH && posx*8-playerx+ONSCREENX>=-8 && rendery(posy*8-playery+ONSCREENY)<=SCREEN_HEIGHT && rendery(posy*8-playery+ONSCREENY)>=-8){
              display.drawBitmap(posx*8-playerx+ONSCREENX,rendery(posy*8-playery+ONSCREENY),warpstop,8,8,WHITE);
            }
          break;
        }
      }
    }
    //Move and flip the laser
    laserpos+=LASERSPEED*deltatime;
    if(laserpos>=LASERDIST*8){
      //note the *8 because laserspeed is in pixils
      laserpos = 0;
    }
    //Move the enemies
    if(!enemyxflip){
      enemyx+=ENEMYSPEED*deltatime;
    }
    else{
      enemyx-=ENEMYSPEED*deltatime;
    }
    //flip the enemy - make it move left and right
    if(enemyx>=ENEMYX*8||enemyx<=0){
      enemyxflip = !enemyxflip;
    }
    //move the vertical enemy
    if(!enemyyflip){
      enemyy+=ENEMYSPEED*deltatime;
    }
    else{
      enemyy-=ENEMYSPEED*deltatime;
    }
    //flip the enemy - make it move up and down
    if(enemyy>=ENEMYY*8||enemyy<=0){
      enemyyflip = !enemyyflip;
    }
    //move the horizontal platform
    if(!platformxflip){
      platformx+=PLATFORMSPEED*deltatime;
    }
    else{
      platformx-=PLATFORMSPEED*deltatime;
    }
    //flip the platform - make it move up and down
    if(platformx>=PLATFORMX*8||platformx<=0){
      platformxflip = !platformxflip;
    }
    //move the vertical platform
    if(!platformyflip){
      platformy+=PLATFORMSPEED*deltatime;
    }
    else{
      platformy-=PLATFORMSPEED*deltatime;
    }
    //flip the platform - make it move up and down
    if(platformy>=PLATFORMY*8||platformy<=0){
      platformyflip = !platformyflip;
    }
    //switches and globalswitch------------------------------------------
    if(globalswitchtimer<=0){
      globalswitch = 0;//reset global switch
    }
    else{
      globalswitchtimer-=deltatime;//subtracts constantly if the timer is greater than 0
    }
    //warp timer---------------------------------------------------------
    if(warptimer<=0){
      canwarp = 1;//reset warp
    }
    else{
      warptimer-=deltatime;//subtracts constantly if the timer is greater than 0
    }
    //movements
    if(digitalRead(BUTTON)){
      //if the button is released
      buttontriggered = 0;//reset the buttontriggered mechanism
    }
    if(leftpressed()&&left){//left
      playerx+=2048*XSPEED*deltatime;
    }
    if(rightpressed()&&right){
      playerx-=2048*XSPEED*deltatime;
    }
    if(uppressed()&&up){//up
      playery+=2048*YSPEED*deltatime;
    }
    if(downpressed()&&down){//down
      playery-=2048*YSPEED*deltatime;
    }
    display.drawBitmap(ONSCREENX,rendery(ONSCREENY),player,8,8,WHITE);
    //undo movements into walls - sets playerx,y  it to a multiple of 8, round in one direction or another
    up = 1;//reset box collider stops
    down = 1;
    left = 1;
    right = 1;
    shorterdown = 1;
    // put your main code here, to run repeatedly:
    //display.drawBitmap(x,y,bitmap,bx,by,WHITE);
    //display.invertDisplay(true);
    //display.setCursor(x,y);
    //display.print(text);
    //display.write(character);
    //literal F3 in my game - shows coords
    if(!digitalRead(BUTTONJOY)){
      display.setCursor(0,0);
      display.print("X: ");
      display.print(playerx);
      display.print(" Y: ");
      display.print(playery);
      if(!digitalRead(CHEATPIN)){
        //F3 CHEAT MODE
        display.setCursor(0,10);
        display.print("Warp: ");
        display.print(canwarp);
        display.print(" T: ");
        display.print(warptimer);
        display.setCursor(0,20);
        display.print("Tile: ");
        if(playery>=0&&playery<(LEVELMAXUP+1)*8&&playerx>=0&&playerx<LEVELMAXRIGHT*8){//makes sure u cant go under!
          display.print(getlevelcomponent(int(playerx/8),int(playery/8)-1,level));//the -1 ensures you get the component under you, not the one you are in
        }
        display.print("Collide:: ");
        display.print(!up ? "U" :
              !down ? "D" :
              !left ? "L" :
              !right ? "R" : "-");
      }
    }
    display.display();
    delay(TICKSPEED);//tune to the board used
    display.clearDisplay();
  }
}
//------------------------------------------------------------------------------------------------------------------
namespace MOON_LANDER{
  float playery = 64;
  /*IMPORTANT NOTE: the top left corner of the OLED is (0,0), however in all drawBitmap commands, 
  I include SCREEN_HEIGHT-playery. This is so the y axis is normalized - 
  basically it makes the bottom left corner (0,0), which makes physics actually make sense!
  I'm ignoring everyone who claims that south is up, and I'm acknowleging the innate computing bias.*/
  float playerx = 64;
  float playervelocityx = 0;
  float playervelocityy = 0;
  const float playeraccelerationx = 0.2;
  const float playeraccelerationgravity = -0.1;
  const float playerengineacceleration = 0.2;
  //drag (I know there's little to no drag on the moon, but like I dont want the player to accelerate indefinately)
  float dragaccelforce = 0;
  const float dragscalar = 0.025;
  //the way drag is calculated is by using the actual drag formula in real life!
  //I'm using dragscalar as the rest of the drag formula, and using playervelocity^2 as the other part
  //fuel
  float fuel = 128;//current fuel
  const float maxfuel = 128;//maximum fuel
  const float fuelbarwidth = 4;//the width of the fuel bar - 4 pixils
  //target
  const float fuelbarmaxheight = 64;//the max height of the fuel bar
  float targetx = 0;
  float targety = 6;//5 + 1, because the entire lower row of pixils is the ground
  const float maxlandingvelocityx = 1;
  const float maxlandingvelocityy = 1;
  //BITMAPS
  static const unsigned char PROGMEM LanderIdle[] =
  { 
  B11111111,
  B10011001,
  B10011001,
  B11111111,
  B00100100,
  B01000010,
  B10000001,
  B10000001 };
  static const unsigned char PROGMEM LanderEngine[] =
  { 
  B11111111,
  B10011001,
  B10011001,
  B11111111,
  B00100100,
  B01011010,
  B10111101,
  B10011001 };
  static const unsigned char PROGMEM target[] =
  { 
  B11000000, B00000011,
  B11100000, B00000111,
  B10000000, B00000001,
  B11111111, B11111111,
  B11000000, B00000011,
  };
  void init() {
    // put your setup code here, to run once:
    display.begin(SSD1306_SWITCHCAPVCC);
    display.cp437(true);
    display.setTextColor(WHITE, BLACK);
    display.clearDisplay();
    Serial.begin(9600);
    pinMode(2,INPUT_PULLUP);
    pinMode(3,INPUT_PULLUP);
    display.setTextWrap(false);
    targetx = random(0,SCREEN_WIDTH-16);
  }
  void update() {
    //exit out of the game if joybutton is pressed
    if(!digitalRead(BUTTONJOY)){
      MAIN_PAGE();//returns to main page if both buttons are pressed
      return;//completely exits out of the homepage function
    }
    //AFFECT ENGINE FORCES
    //Affect engine thrust sideways
    if(leftpressed()&&fuel>0){//left
      playervelocityx-=playeraccelerationx*deltatime;
      fuel-=deltatime;
    }
    if(rightpressed()&&fuel>0){//right
      playervelocityx+=playeraccelerationx*deltatime;
      fuel-=deltatime;
    }
    //Affect Engine Thrust Upwards
    if(!digitalRead(BUTTON)&&fuel>0){
      playervelocityy+=playerengineacceleration*deltatime;
      fuel-=deltatime;
    }
    if(!digitalRead(CHEATPIN)){
      //remember its input pullup - this cheat gives infinite fuel... definately not op
      fuel = maxfuel;
    }
    //Affect environmental forces
    //affect lateral drag
    if(playervelocityx>0){
      playervelocityx-=pow(playervelocityx,2)*dragscalar*deltatime;
    }
    if(playervelocityx<0){
      playervelocityx+=pow(playervelocityx,2)*dragscalar*deltatime;
    }
    //affect gravity
    playervelocityy+=playeraccelerationgravity*deltatime;
    //calculus - integrate the velocity to get position
    playerx+=playervelocityx*deltatime;
    playery+=playervelocityy*deltatime;
    //EDGE LIMITS
    if(playerx>SCREEN_WIDTH){
      playerx = 0;
    }
    if(playerx<0){
      playerx = SCREEN_WIDTH;
    }
    if(playery>=SCREEN_HEIGHT){
      playery = SCREEN_HEIGHT;
      playervelocityy = 0;
    }
    //TARGET
    if(playery<=8){//note <=8 because the player is 8 pixils tall
      if(playerx>=targetx&&playerx<=targetx+8&&abs(playervelocityy)<maxlandingvelocityy&&abs(playervelocityx)<maxlandingvelocityx){
        /*everything is calculated using the left corner, so its +8 because when the player's left top corner
        is over 8 pixils away from the target's left top corner, then the player's right leg is on the right flag*/
        display.setCursor(SCREEN_WIDTH/2-25,SCREEN_HEIGHT/2);
        display.print("SUCCESS!");
        display.display();
        delay(1000);
        ::loading_screen();//to waste people's time
        targetx = random(0,SCREEN_WIDTH-16);
        playery = SCREEN_HEIGHT;
        playerx = SCREEN_WIDTH/2;
        playervelocityy = 0;
        playervelocityx = 0;
        fuel = maxfuel;
      }
      else{
        display.setCursor(SCREEN_WIDTH/2-25,SCREEN_HEIGHT/2);
        display.print("GAME OVER");
        display.display();
        delay(1000);
        ::loading_screen();//to waste people's time
        targetx = random(0,SCREEN_WIDTH-16);
        playery = SCREEN_HEIGHT;
        playerx = SCREEN_WIDTH/2;
        playervelocityy = 0;
        playervelocityx = 0;
        fuel = maxfuel;
      }
    }
    //draw player
    if(!digitalRead(BUTTON)&&fuel>0){
      display.drawBitmap(playerx,SCREEN_HEIGHT-playery,LanderEngine,8,8,WHITE);
    }
    else{
      display.drawBitmap(playerx,SCREEN_HEIGHT-playery,LanderIdle,8,8,WHITE);
    }
    //draw target
    display.drawBitmap(targetx,SCREEN_HEIGHT-targety,target,16,5,WHITE);
    //draw fuel bar
    display.fillRect(SCREEN_WIDTH-fuelbarwidth, 0, fuelbarwidth, fuelbarmaxheight*fuel/maxfuel, WHITE);
    //draw empty fuel bar background
    display.drawRect(SCREEN_WIDTH-fuelbarwidth, 0, fuelbarwidth, fuelbarmaxheight, WHITE);
    //draw ground
    display.fillRect(0,SCREEN_HEIGHT-1, 128, 1, WHITE);//draws a very big rectangle as the floor
    display.display();
    delay(TICKSPEED);
    display.clearDisplay();
  }
}
namespace CHROME_DINO{
  static const int MAXHEIGHT = 20;
  static const float SCORE_SPEED_RATIO = 0.0005;//The ratio between the speed and the score
  static const int SCOREBOOTS = 200;//the amount of points gained in a coinrush per coin
  static const int POWERDURATION = 200;//the time a powerup lasts
  static const int COINWAIT = 1320;//the distance the coin starts from
  static const unsigned char dino[] PROGMEM = {
      B00001111,
      B00001101,
      B00001111,
      B10000100,
      B11111111,
      B11111000,
      B00101000,
      B00101000
  };
  static const unsigned char cactusbig[] PROGMEM = {
      B00011011,
      B00011111,
      B11011111,
      B11111000,
      B11111011,
      B00011111,
      B00011111,
      B00011000
  };
  static const unsigned char cactussmall[] PROGMEM = {
      B00000000,
      B00000000,
      B00000000,
      B00011000,
      B11011000,
      B11111011,
      B00011111,
      B00011000
  };
  static const unsigned char bird[] PROGMEM = {
      B00000000,
      B00000000,
      B00000000,
      B00000010,
      B00000110,
      B01001110,
      B11111111,
      B00111110
  };
  static const unsigned char coin[] PROGMEM = {
      B00111100,
      B01000010,
      B10011001,
      B10111101,
      B10111101,
      B10011001,
      B01000010,
      B00111100
  };
  int height = 0;
  unsigned int score = 0;
  bool hit = 0;
  float speed = 1.0;
  int coins = 0;
  class coinclass{
    public:
      float position = 0;
      bool destroyed = 0;
      int up = 0;
      void randup(){
        this->up = random(0,2)*16;//position 0 or position 1*maxcoinheight
      }
      void render(){
        display.drawBitmap(position, 56-up, coin, 8, 8, WHITE);
      }
      void check(bool coinrush){
        if(!coinrush){
          if(up == 0){
            if(height<=1 && this->position<=22 && this->position>=8){
                coins++;
                destroyed = 1;
            }
          }
          else{
            if(height>=8 && height<=24 && this->position<=22 && this->position>=8){//change height variables in relation to maxheight
                coins++;
                destroyed = 1;
            }
          }
        }
        else{//if there is a coinrush
          if(up == 0){
            if(height<=1 && this->position<=22 && this->position>=8){
              score+=100;
              destroyed = 1;
            }
          }
          else{
            if(height>=8 && height<=24 && this->position<=22 && this->position>=8){//change height variables in relation to maxheight
              score+=100;
              destroyed = 1;
            }
          }
        }
      }
      coinclass(int pos){
        position = pos;
        this->randup();
      }
      void doduringframe(bool coinrush = 0){//coinrush is for the left powerup, where each coin hit is 100 points
        if(!destroyed){
          this->check(coinrush);
          this->render();
        }
        position-=speed;
          if(position<-16 && (random(0,500) == 0||coinrush)){//random tells if the coin will spawn again. If coinrush is on, coins play indefinately.
            position = 128;
            this->randup();
            destroyed = 0;
          }
      }
  };
  class enemy{
    public:
      int image = 0;//1-3 are 1-3 cacti, 4-6 are small cacti, 7 is bird low, 8 is bird middle, 9 is bird high
      float position = 0;//the position of the sprite from 128(right) to 0() 
      bool destroyed = 0;
      void setimage(){
          this->image = random(1,10);
      }
      void render(){
        switch(image){
          case 1:
            display.drawBitmap(position, 56, cactusbig, 8, 8, WHITE);
          break;
          case 2:
            display.drawBitmap(position, 56, cactusbig, 8, 8, WHITE);
            display.drawBitmap(position+8, 56, cactusbig, 8, 8, WHITE);
          break;
          case 3:
            display.drawBitmap(position, 56, cactusbig, 8, 8, WHITE);
            display.drawBitmap(position+8, 56, cactusbig, 8, 8, WHITE);
            display.drawBitmap(position+16, 56, cactusbig, 8, 8, WHITE);
          break;
          case 4:
            display.drawBitmap(position, 56, cactussmall, 8, 8, WHITE);
          break;
          case 5:
            display.drawBitmap(position, 56, cactussmall, 8, 8, WHITE);
            display.drawBitmap(position+4, 56, cactussmall, 8, 8, WHITE);
          break;
          case 6:
            display.drawBitmap(position, 56, cactussmall, 8, 8, WHITE);
            display.drawBitmap(position+4, 56, cactussmall, 8, 8, WHITE);
            display.drawBitmap(position+8, 56, cactussmall, 8, 8, WHITE);
          break;
          case 7:
            display.drawBitmap(position, 56, bird, 8, 8, WHITE);
          break;
          case 8:
            display.drawBitmap(position, 52, bird, 8, 8, WHITE);
          break;
          case 9:
            display.drawBitmap(position, 48, bird, 8, 8, WHITE);
          break;
        }
      }
      void check(){
        switch(image){
          case 1:
            if(height<=8 && this->position<=22 && this->position>=8){
              hit = 1;
            }
          break;
          case 2:
            if(height<=8 && this->position<22 && this->position>0){
              hit = 1;
            }
          break;
          case 3:
            if(height<=8 && this->position<20 && this->position>-8){
              hit = 1;
            }
          break;
          case 4:
            if(height<=4 && this->position<=24 && this->position>=16){
              hit = 1;
            }
          break;
          case 5:
            if(height<=4 && this->position<=24 && this->position>=12){
              hit = 1;
            }
          break;
          case 6:
            if(height<=4 && this->position<=24 && this->position>=8){
              hit = 1;
            }
          break;
          case 7:
            if(height<=4 && this->position<=24 && this->position>=8){
              hit = 1;
            }
          break;
          case 8:
            if(height<=8 && height>=-3 && this->position<=24 && this->position>=8){
              hit = 1;
            }
          break;
          case 9:
            if(height<=9 && height>=1 && this->position<24 && this->position>8){
              hit = 1;
            }
          break;
        }
      }
      enemy(int pos){
        position = pos;
        this->setimage();
      }
      void doduringframe(){
        if(!destroyed){
          this->check();
          this->render();
        }
        position-=speed;
          if(position<-16){
            position = 128;
            this->setimage();
            destroyed = 0;
          }
      }
  };
  void init() {

  }
  enemy one(128);
  enemy two(216);//declaring the enemies
  coinclass powerup(COINWAIT);//declaring coins, when it will enter the stage
  void update() {
    // put your main code here, to run repeatedly:
    //night and day
    score+=speed;
    if(score%2000 <= 1000){
      display.invertDisplay(true);
    }
    else{
      display.invertDisplay(false);
    }
    powerup.doduringframe();
    one.doduringframe();
    if(score<1000){
          two.doduringframe();
    }
    if(!digitalRead(BUTTONJOY)){//jump and duck
      height=-4;
    }
    else if(digitalRead(BUTTON) == LOW){
      for(int i = 0;i<sqrt(MAXHEIGHT)*4;i++){
        score+=speed;
        display.clearDisplay();
        powerup.doduringframe();
        one.doduringframe();
        if(score<1000){
          two.doduringframe();
        }
        height=-pow(float(i)/2.0-float(sqrt(MAXHEIGHT)),2.0)+float(MAXHEIGHT);
        display.drawBitmap(16, 56-height, dino, 8, 8, WHITE);
        display.setCursor(88,8);
        display.print(score);
        display.setCursor(8,8);
        display.print(coins);
        display.display();
        if(hit == 1&&digitalRead(CHEATPIN)){//um cheats... if you hotwire cheatpin to ground you gain immortaility
          display.setCursor(32,32);
          display.print("GAME OVER!!!");
          display.display();
          delay(1000);
          if(!digitalRead(BUTTONJOY)){
            display.invertDisplay(false);
            MAIN_PAGE();//returns to main page if both buttons are pressed
            return;//completely exits out of the homepage function
          }
          ::loading_screen();//just to waste people's time
          height = 0;
          score = 0;
          hit = 0;
          speed = 1.0;
          coins = 0;
          one.position = 128;//reset the enemies
          two.position = 216;
          one.destroyed = 1;
          two.destroyed = 1;
        }
        delay(40);
      }
      height = 0;
    }
    else{
      height=0;
    }
    speed = float(float(score)*float(SCORE_SPEED_RATIO)+float(2.0));//set speed
    display.drawBitmap(16, 56-height, dino, 8, 8, WHITE);
    display.setCursor(88,8);
    display.print(score);
    display.setCursor(8,8);
    display.print(coins);
    if(coins >= 1){//POWERUPS!!!
      if(uppressed()){//POWERUP UP
        coins--;
      }
      else if(downpressed()){//POWERUP DOWN
        coins--;
      }
      else if(leftpressed()){//POWERUP LEFT COINRUSH
        coins--;
        for(int i = 0;i<POWERDURATION;i++){
            display.clearDisplay();
            powerup.doduringframe(1);
            if(!digitalRead(BUTTONJOY)){//jump and duck
              height=-4;
            }
            else if(digitalRead(BUTTON) == LOW){
              for(int i = 0;i<sqrt(MAXHEIGHT)*4;i++){
                score+=speed;
                display.clearDisplay();
                powerup.doduringframe(1);
                height=-pow(float(i)/2.0-float(sqrt(MAXHEIGHT)),2.0)+float(MAXHEIGHT);
                display.drawBitmap(16, 56-height, dino, 8, 8, WHITE);
                display.setCursor(88,8);
                display.print(score);
                display.setCursor(8,8);
                display.print(coins);
                display.display();
                delay(40);
              }
              height = 0;
            }
            else{
              height=0;
            }
            if(score%2000 <= 1000){//daynight
              display.invertDisplay(true);
            }
            else{
              display.invertDisplay(false);
            }
            score+=speed;
            speed = float(float(score)*float(SCORE_SPEED_RATIO)+float(2.0));//set speed
            display.drawBitmap(16, 56-height, dino, 8, 8, WHITE);
            display.setCursor(88,8);
            display.print(score);
            display.setCursor(8,8);
            display.print(coins);
            display.display();
            delay(40);
        }
        one.position = 128;//resetting the positions of the coins and enemies
        two.position = 216;
        powerup.position = COINWAIT;
      }
      else if(rightpressed()){//POWERUP RIGHT
        coins--;
      }
    }
    display.display();
    if(hit == 1&&digitalRead(CHEATPIN)){//cheatpin is input pullup so um yea time to gain immortality! yippee
      display.setCursor(32,32);
      display.print("GAME OVER!!!");
      display.display();
      delay(1000);
      if(!digitalRead(BUTTONJOY)){
        display.invertDisplay(false);//reset invert display
        MAIN_PAGE();//returns to main page if both buttons are pressed
        return;//completely exits out of the homepage function
      }
      ::loading_screen();//just to waste people's time
      height = 0;
      score = 0;
      hit = 0;
      speed = 1.0;
      coins = 0;
      one.position = 128;//reset the enemies
      two.position = 216;
      one.destroyed = 1;
      two.destroyed = 1;
    }
    delay(40);
    display.clearDisplay();
  }
}
namespace RENDER3D{
  //const int
  static const int renderscalar = 1;//this is multiplied by the resultx and resultz
  static const int FRAME_HEIGHT = 64;//these variables dictate the viewer's frame of view dimensions
  static const int FRAME_WIDTH = 128; 
  float FOV = 140;//this is how many degrees you can see in total!
  float FOCAL = FRAME_WIDTH/2*1/tan(PI/180*FOV/2.0);//the focal is the distance from the viewing frame where pixils are drawn at 1:1 - basically with a focal of 5, at z=5, pixils would be 1:1
  //calculated using triganometry - draw a triangle with a vertical cut in it (not the altitude), and then you can calculate the smaller right triangles using tan
  //remember that tan accepts radians, not degrees
  //UHH QUATERNIONSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
  class quaternion{
    public:
      float w;
      float x;
      float y;
      float z;
      quaternion(float wval,float xval,float yval,float zval){
        w = wval;
        x = xval;
        y = yval;
        z = zval;
      }
      //the static part allows functions to be used without the class
      static quaternion multiplyquaternions(quaternion a,quaternion b){//multiplying quaternion a by b (a rotates b)
        quaternion result(1,0,0,0);
        result.w = a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z;
        result.x = a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y;
        result.y = a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x;
        result.z = a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w;
        return result;
      }
      static quaternion conjugate(quaternion a){
        quaternion result(1,0,0,0);
        result.w = a.w;
        result.x = -a.x;
        result.y = -a.y;
        result.z = -a.z;
        return result;
      }
      static quaternion rotatequaternion(quaternion a, quaternion b){//a rotates b fyi
        quaternion result(1,0,0,0);
        quaternion firstmultipulcation(1,0,0,0);
        firstmultipulcation = multiplyquaternions(a,b);
        result = multiplyquaternions(firstmultipulcation,conjugate(a));
        return result;
      }
      static quaternion normalizequaternion(quaternion a){
        quaternion result(1,0,0,0);
        result.x = a.x;
        result.y = a.y;
        result.z = a.z;
        result.w = sqrt(1-pow(a.x,2)-pow(a.y,2)-pow(a.z,2));
        return result;
      }
  };
  class point2d{
    public:
      int x;
      int y;
      point2d(int xval,int yval){
        x = xval;
        y = yval;
      }
  };
  //functions
  //these draw functions turn a set of coordinates given that the center of the screen is the origin and convert it into the way a OLED screen thinks (origen is top left corner)
  //You might notice a bug - I used frame_width for everything. This is actually not a bug, and it ensures that things keep rendering even if the points go beyond the screen, allowing for a square, especially important for lines and faces.
  void drawPoint(point2d point){
    //draws a point (x,y)
    if(point.x<=FRAME_WIDTH/2&&point.x>=-FRAME_WIDTH/2&&point.y<=FRAME_HEIGHT/2&&point.y>=-FRAME_HEIGHT/2){
      display.fillRect(point.x+SCREEN_WIDTH/2,SCREEN_HEIGHT/2-point.y,1,1,WHITE);//because of the really annoying way the oled screen works the origin has to be moved to the middle
    }
  }
  void drawLine(point2d one,point2d two){
    //draws a line from (x,y) to (a,b) ONLY if it is within the render distance
    if((one.x<=FRAME_WIDTH/2&&one.x>=-FRAME_WIDTH/2&&one.y<=FRAME_WIDTH/2&&one.y>=-FRAME_WIDTH/2)||(two.x<=FRAME_WIDTH/2&&two.x>=-FRAME_WIDTH/2&&two.y<=FRAME_WIDTH/2&&two.y>=-FRAME_WIDTH/2)){
      //note the or function - it ensures that the line draws if either one of the points of the line are in render distance
      display.drawLine(one.x+SCREEN_WIDTH/2,SCREEN_HEIGHT/2-one.y,two.x+SCREEN_WIDTH/2,SCREEN_HEIGHT/2-two.y,WHITE);
    }
  }
  //this is the 3d render function!
  //note that essentially, 0,0 is in the CENTER of the oled screen, as represented by drawPoint<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
  point2d render(int x,int y,int z,int playerx,int playery, int playerz, quaternion playerrot){
    /*ok this is a lot of variables so lets walk through them. x,y,z are the point's coordinates, playerx,playery,playerz are the player's coordinates
    the rot variables make up a quaternion, basically x,y,z represent a vector, while rotw represents the rotation around that vector in degrees.*/
    //the rest of the stuff
    int deltax = x-playerx;//the delta variables
    int deltay = y-playery;
    int deltaz = z-playerz;
    //TIME FOR QUATERNIONS
    quaternion point(0,deltax,deltay,deltaz);//NOT ACTUALLY A QUATERNION - this is a vector hiding as a quaternion T_T but I dont feel like making a whole seperate datatype
    //conjugate (reverse) the direction of the player to make points rotate oppositely
    quaternion shiftedpoint = quaternion::rotatequaternion(quaternion::conjugate(playerrot), point);
    deltax = shiftedpoint.x;//the new quaternion coords based on the player!
    deltay = shiftedpoint.y;
    deltaz = shiftedpoint.z;
    if(deltaz>0){//prevents the code from throwing a division by 0 error and only renders stuff in front.
      int resultx = float(FOCAL*deltax*1.0/deltaz);
      int resulty = float(FOCAL*deltay*1.0/deltaz);
      point2d returnpoint(resultx,resulty);
      return returnpoint;
    }
    else{//this is what happens if it's behind the screen -it just keeps the same ratio, keeping deltaz at its smallest nonzero point
      int resultx = float(FOCAL*deltax*1.0/1.0);//uhh deltaz goes to infinity - I'm just gonna set it to 1 because that's the smallest integer datapoint we can do
      int resulty = float(FOCAL*deltay*1.0/1.0);
      point2d returnpoint(resultx,resulty);
      return returnpoint;
    }
  }
  //PLAYER VARIABLES
  float playerx = 0;
  float playery = 0;
  float playerz = 0;
  float xrot = 0;
  quaternion player(1,0,0,0);
  void init() {
  }

  void update() {
    // put your main code here, to run repeatedly:
    //run quaternions
    if(rightpressed()){
      xrot+=deltatime;
      player.w = cos(PI/180*xrot/2);
      player.y = sin(PI/180*xrot/2);
    }
    if(leftpressed()){
      xrot-=deltatime;
      player.w = cos(PI/180*xrot/2);
      player.y = sin(PI/180*xrot/2);
    }
    if(analogRead(YJOY)-2048<=YTHRESHOLD){
      playerz+=cos(PI/180*xrot)*deltatime;
      playerx+=sin(PI/180*xrot)*deltatime;
    }
    if(analogRead(YJOY)-2048>=-YTHRESHOLD){
      playerz-=cos(PI/180*xrot)*deltatime;
      playerx-=sin(PI/180*xrot)*deltatime;
    }
    if(!digitalRead(BUTTON)){
      playerx+=cos(PI/180*xrot)*deltatime;
      playerz-=sin(PI/180*xrot)*deltatime;
    }
    if(!digitalRead(BUTTONJOY)){
      playerx-=cos(PI/180*xrot)*deltatime;
      playerz+=sin(PI/180*xrot)*deltatime;
    }
    drawLine(render(-5, 5, 15, playerx, playery, playerz, player),render(5, 5, 15, playerx, playery, playerz, player));
    drawLine(render(5, 5, 15, playerx, playery, playerz, player),render(5, -5, 15, playerx, playery, playerz, player));
    drawLine(render(5, -5, 15, playerx, playery, playerz, player),render(-5, -5, 15, playerx, playery, playerz, player));
    drawLine(render(-5, -5, 15, playerx, playery, playerz, player),render(-5, 5, 15, playerx, playery, playerz, player));

    drawLine(render(-5, 5, 15, playerx, playery, playerz, player),render(-5, 5, 25, playerx, playery, playerz, player));
    drawLine(render(5, 5, 15, playerx, playery, playerz, player),render(5, 5, 25, playerx, playery, playerz, player));
    drawLine(render(5, -5, 15, playerx, playery, playerz, player),render(5, -5, 25, playerx, playery, playerz, player));
    drawLine(render(-5, -5, 15, playerx, playery, playerz, player),render(-5, -5, 25, playerx, playery, playerz, player));

    drawLine(render(-5, 5, 25, playerx, playery, playerz, player),render(5, 5, 25, playerx, playery, playerz, player));
    drawLine(render(5, 5, 25, playerx, playery, playerz, player),render(5, -5, 25, playerx, playery, playerz, player));
    drawLine(render(5, -5, 25, playerx, playery, playerz, player),render(-5, -5, 25, playerx, playery, playerz, player));
    drawLine(render(-5, -5, 25, playerx, playery, playerz, player),render(-5, 5, 25, playerx, playery, playerz, player));
    display.display();
    delay(TICKSPEED);
    display.clearDisplay();
  }
}
namespace GPS_TRACKER{
  /*
 */
 //tx 5, rx 6 - defined at top
  const double distancefromlastread = 0.1;//the distance from the last read 
  TinyGPSPlus gps;
  HardwareSerial gpsSerial(2);
  const int maxcoordinates = 128;//max coordinates this can store
  double coordinates[maxcoordinates*2] = {0};//25 miles worth of data
  int currentcoordinate = 0;//*2 is latitude, *2+1 is longitude
  double leftbound = -1;//centered around (0,0), or coordinates [0]
  double rightbound = 1;
  double upbound = 1;
  double downbound = -1;
  double lattomi(double lat){
    return lat*69.05;
  }
  double lontomi(double lat,double lon){
    return cos(PI/180*lat)*69.17*lon;
  }
  void drawpoint(int coordinatetodraw){
    double xdifference = lattomi(coordinates[coordinatetodraw*2]-coordinates[0]);
    double ydifference = lontomi((coordinates[coordinatetodraw*2]+coordinates[0])/2,coordinates[coordinatetodraw*2+1]-coordinates[1]);
    //note the use of an average here.
    //expand the borders
    if(xdifference>rightbound){
      rightbound = xdifference;
    }
    if(xdifference<leftbound){
      leftbound = xdifference;
    }
    if(ydifference>upbound){
      upbound = ydifference;
    }
    if(ydifference<downbound){
      downbound=ydifference;
    }
    display.drawPixel(128*(xdifference-leftbound)/(rightbound-leftbound),64*(1-(ydifference-downbound)/(upbound-downbound)),WHITE);
    /*note the odd logic here - so you take the ratio of the distance from one side to another, and you multiply it by 128, or the screen width, to get the total distance across the screen. Also note the 1- - this is caused because the entire screen is inverted and needs to be righted to display propertly.*/
  }
  void init() {
    gpsSerial.begin(9600,SERIAL_8N1,TX_PIN,RX_PIN);
    display.setCursor(0,0);
    display.print("Waiting...");
    display.display();
    while (!(gps.location.isValid() && gps.location.isUpdated())) {
      if (gpsSerial.available()) gps.encode(gpsSerial.read());
    if(!digitalRead(BUTTONJOY)){//return to main page
      MAIN_PAGE();
      return;
    }
    }
    display.clearDisplay();
    coordinates[0] = gps.location.lat();//set initial coordinates
    coordinates[1] = gps.location.lng();
    currentcoordinate++;
  }

  void update() {
    //recieve coords
    if(gpsSerial.available()){
      gps.encode(gpsSerial.read());
      if(!digitalRead(BUTTONJOY)){
        display.setCursor(0,0);
        display.print("Lat: ");
        display.print(gps.location.lat(),8);//the ,8 specifies 8 decimal places
        display.setCursor(0,10);
        display.print("Lon: ");
        display.print(gps.location.lng(),8);
        display.setCursor(0,20);
        display.print("Angle: ");
        display.print(gps.course.deg());
      }
      if(sqrt(pow(lattomi(gps.location.lat())-lattomi(coordinates[(currentcoordinate-1)*2]),2)+pow(lontomi(gps.location.lat(),gps.location.lng())-lontomi(coordinates[(currentcoordinate-1)*2],coordinates[(currentcoordinate-1)*2+1]),2))>=distancefromlastread){
        //THIS IS JUST THE DISTANCE FORMULA FYI
        //run update code here
        coordinates[currentcoordinate*2]=gps.location.lat();
        coordinates[currentcoordinate*2+1]=gps.location.lng();
        currentcoordinate++;
        currentcoordinate = constrain(currentcoordinate,0,maxcoordinates-1);
      }
    }
    //draw the map
    for(int i = 0;i<maxcoordinates;i++){
      if(coordinates[i*2]==0&&coordinates[i*2+1]==0){
        break;
      }
      drawpoint(i);
    }
    display.display();
    delay(TICKSPEED);
    display.clearDisplay();
  }

}
//-----------------------------------------------------------------------------------------------------------------------------------------------
//game choosings
int gamechosen = 0;
const int maxnumberofgames = 8;//the number of games
/*the levels are as follows:
BATTLE_CATS:0
PLATFORMER:1
CHROME_DINO:2
MOON_LANDER:3
3D RENDERER:4
MUSIC_MAKER:5
MAZE_GAME:6
GPS_TRACKER:7
*/
String indextogame(int index){
  switch(index){
    case -2:
    if(playsound){
      return "Sound: On!!";//sound setting
    }
    else{
      return "Sound: Off!";
    }
    case -1:
    return "Invert Disp";//if you scroll all the way down you get an invert display option for fun
    case 0:
    return "Battle Cats";
    case 1:
    return "Platformer ";
    case 2:
    return "Chrome Dino";
    case 3:
    return "Moon Lander";
    case 4:
    return "3D Renderer";
    case 5:
    return "Music Maker";
    case 6:
    return "Maze Game!!";
    case 7:
    return "GPS Tracker";
  }
}
void MAIN_PAGE(){
  //the navigation homepage - called by GAMEOVER function
  //joystick up and down to navigate up and down, button to select.
  /*NOTES - this allows the player to select a game - it does not run a function immediately after it, however, as once the game is selected
  the console runs simularly to if you were only playing that game. It just chooses a namespace for you. */
  while(digitalRead(BUTTON)!=0){
    //plays music
    playsong(homepagetheme,homepagesonglength);
    //clears the display
    display.clearDisplay();
    //while the button is not pressed
    //add computational logic here
    if(uppressed()&&gamechosen<maxnumberofgames-1){
      //joystick up - cannot go over LEVELMAX
      gamechosen++;
    }
    if(downpressed()&&gamechosen>-2){//greater than -2 because -1 and -2 are settings
      //joystick down - cannot go under 1
      gamechosen--;
    }
    //add printing commands - print out the screen
    //prints out the current level
    display.setCursor(48,32);
    display.print(">");
    display.print(indextogame(gamechosen));
    display.print("<");
    //prints out previous level
    if(gamechosen>=-1){//greater than -1 because -1 and -2 are settings
      //level>1+1 because if level is one, there is no previous level - prints below current level
      display.setCursor(56,48);
      display.print(indextogame(gamechosen-1));
    }
    //prints out next level
    if(gamechosen<=maxnumberofgames-2){
      //level>1+1 because if level is one, there is no previous level - prints below current level
      display.setCursor(56,16);
      display.print(indextogame(gamechosen+1));
    }
    //prints out words "Homepage!"
    display.setCursor(0,8);
    display.print("Choose");
    display.setCursor(0,20);
    display.print("A Game!");
    //displays
    display.display();
    delay(200);
  }
  ::loading_screen();//to waste people's time
  //begin game
  switch(gamechosen){
    case -2:
    playsound = !playsound;
    MAIN_PAGE();
    break;
    case -1:
    invert = !invert;
    display.invertDisplay(invert);
    MAIN_PAGE();
    break;
    case 0:
    BATTLE_CATS::init();
    break;
    case 1:
    PLATFORMER::init();
    break;
    case 2:
    CHROME_DINO::init();
    break;
    case 3:
    MOON_LANDER::init();
    break;
    case 4:
    RENDER3D::init();
    break;
    case 5:
    MUSIC_MAKER::init();
    break;
    case 6:
    MAZE_GAME::init();
    break;
    case 7:
    GPS_TRACKER::init();
    break;
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  display.begin(SSD1306_SWITCHCAPVCC);
  display.cp437(true);
  display.setTextColor(WHITE, BLACK);
  //flips OLED - REPLACE IF NEEDED
  display.setRotation(2);
  display.clearDisplay();
  Serial.begin(9600);
  SPI.begin();
  Keyboard.begin();//begin keyboard and USB
  USB.begin();
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(CHEATPIN,INPUT_PULLUP);
  pinMode(SPEAKER,OUTPUT);
  display.invertDisplay(false);
  display.setTextWrap(false);
  randomSeed(analogRead(A7));
  display.clearDisplay();
  start_screen();//credits!!!
  MAIN_PAGE();
  //note that all the init functions have been moved to the bottom of MAIN_PAGE
}
void loop(){
  //run deltatime operations
  currenttime = millis();
  deltatime = float(currenttime - prevtime) / float(CLOCKSPEED);  // normalized to your original 50ms tick
  if(deltatime>DELTATIMECAP){//cap deltatime
    deltatime=DELTATIMECAP;
  }
  prevtime = currenttime;
  switch(gamechosen){
    case 0:
    BATTLE_CATS::update();
    playsong(battlecatstheme,battlecatssonglength);
    break;
    case 1:
    PLATFORMER::update();
    break;
    case 2:
    CHROME_DINO::update();
    break;
    case 3:
    MOON_LANDER::update();
    break;
    case 4:
    RENDER3D::update();
    break;
    case 5:
    MUSIC_MAKER::update();
    break;
    case 6:
    MAZE_GAME::update();
    break;
    case 7:
    GPS_TRACKER::update();
    break;
  }
}
