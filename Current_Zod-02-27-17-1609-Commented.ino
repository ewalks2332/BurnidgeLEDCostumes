#include <Adafruit_NeoPixel.h>

#define BUTTON_PIN_GO   2    //to go forward in steps
#define BUTTON_PIN_BACK  3   //to go back in steps


#define PIXEL_PIN 6    // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 60

int stripNum= 4;   //change for number of tapes

// Parameter 1 = number of pixels in strip,  neopixel stick has 8
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream, correct for neopixel stick
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip), correct for neopixel stick
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT * stripNum, 6, NEO_GRB + NEO_KHZ800);

volatile int cue = 0;
int timeout = 0;
volatile bool trigger = false;
volatile int triggerCount = 0;
int buttonState = 1;


bool oldStateGo = HIGH;
bool oldStateBack = HIGH;

int showType = 0;   //determines what step we are on

//startPoints for each of the cues so that we can fade in

void Dec(){
  //delay(300);

}

void Inc(){
  //noInterrupts();
  //delay(2000);
  trigger = true;
  triggerCount++;
  //CircuitPlayground.clearPixels();
  Serial.print("TC: "); Serial.print(triggerCount); Serial.println("");
  //Serial.println("Trigger!");
  //delay(300);
}


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_GO, INPUT_PULLUP);
  pinMode(BUTTON_PIN_BACK, INPUT_PULLUP);
  attachInterrupt(0, Inc, CHANGE);
  attachInterrupt(1, Dec, CHANGE);
  interrupts();
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  trigger = false;
}



void loop() {
  //Serial.println(newStateGo);
  startShow();
}

void startShow() {
  switch(cue){
    //i changes and thte effect
    // each case is really the step
    case 1:                                                         //CUE 1
                                                                    //change line below to make a different effect happen for step 0 for example
            colorWipe(strip.Color(0, 0, 0), 0);                     // Black/off
                                                                    //removing break shouldn't have any effect in this case (because all numbers are different), but should probably be left in.        
            break;
   
    case 2:                                                         //CUE 1.5
            for(int i = 0; i<24; i=i+4){                            //goes to 24, with them all being white
                colorWipe(strip.Color(i, i, i), 0);    
            }
            cue++;                                                  //This cue will autofollow to the next
            break;
            
    case 3:                                                         //CUE 2
            whitePulse(30, 15, 1, 1, 10);                           //goes from all at 30 to all at 15, though there is middle ground, every 4 pixels will hit max and low
            break;						    //delay of 10 milliseconds
   
    case 4:                                                         //CUE 3 
            twoPulse(36, 15, 0, .57, 20);                           // goes from green at 36 to 15, blue to green is .57:1
            break;

    case 5:                                                        //CUE 3.5
            for(int i= 26; i>0; i=i-2){                            //slow fade to black
              colorWipe(strip.Color(0, i, (int) (i*.57)), 0);
            }
            cue++;                                                 //This cue will autofollow to the next
            break;

    case 6:                                                        //CUE 4
            rainbowCycle(10);                                      //faster rainbow cycle 10 delay
            break;
    case 7:                                                        //CUE 5
            twoPulse(30, 15, 1, 1, 40);                            //jump to white pulse for the dancers
		  						   //high 30, low 15, delay of 40 milliseconds
            break;
    case 8:                                                        //CUE 6
            for(int i= 30; i>=0; i= i-1){                          //Goes from all at 30, to out
              colorWipe(strip.Color(i, i, i), 5);                  //Very slow fadeout at the end of the piece
            }
            cue++;
            break;
    case 9:
            colorWipe(strip.Color(0, 0, 0), 0);                     // Black/off       
            break;
          
            
  }
  if (trigger == true) {
    if ( triggerCount < 3) {
      if (cue > 9) {
        cue = 1;
      } else {
        cue++;
      }
    }
    triggerCount = 0;
    //interrupts();
    trigger = false;
  }
}


//this method has everyother led be at high or low. High and low refers to the max level of green,
//so this method requires taht you always pulse with a green color mixed with something. The red and
//blue mult refer to the ratio between the red and blue values compared to the green. wait is the delay
void twoPulse(int high, int low, double redMult, double blueMult, uint8_t wait){  
  for(int j=0; j<1; j++){
    int start1 = high;
    int start2 = low;
    while(start1 != low){
      for(int i = 0; i<stripNum*PIXEL_COUNT; i= i+2){
      
        strip.setPixelColor(i, start1*redMult, start1, start1*blueMult);
        strip.setPixelColor(i+1, start2*redMult, start2, start2*blueMult);
      
      }
      
      start1 = start1 - 1;
      start2 = start2 +1;
      strip.show();
      delay(wait);
    }

    while(start1!=high){
      for(int i = 0; i<stripNum*PIXEL_COUNT; i= i+2){
        strip.setPixelColor(i, start1*redMult, start1, start1*blueMult);
        strip.setPixelColor(i+1, start2*redMult, start2, start2*blueMult);
       }
      
      start1 = start1 +1;
      start2 = start2 -1;
      strip.show();
      delay(wait);
    }
  }
}

//this function is hardcoded, but certain values can be adjusted to customize it
void chase(uint8_t wait) {
  int c1r = 75;                             //highest value of red for the first color                                                                                                                                                                                    
  int c2r = 32;                             //highest value of red for the second color
  int c3r = 45;                             //highest value of red for the third color

  double c1[60];
  double c2[60];
  double c3[60];
                                            //this for loop will have each color blend into the next
  for(int i = 0; i<60; i++){
   
    if(i<20){
      c1[i] = c1r * (1-(.05*i));
      c2[i] = c2r * (i*.05);
      c3[i] = 0;
    }
    else if(i<40){
      c1[i] = 0;
      c2[i] = c2r * (1-(.05*(i-20)));
      c3[i] = c3r * ((i-20)*.05);
    }
    else if(i<60){
      c1[i] = c1r * ((i-40)*.05);
      c2[i] = 0;
      c3[i] = c3r * (1-(.05*(i-40)));
    }
  }

  for(int i = 0; i<60; i++){
    for(int j = 0; j<stripNum*PIXEL_COUNT; j++){
                                                                          //this is creating the actual colors based on
                                                                          //the ratio you want, change what the number you are 
                                                                          // multiplying it by to have different blue and green values
                        
      double red = (c1[j%60]) + (c2[j%60]) + (c3[j%60]);
      double blue = (c1[j%60]) + (c2[j%60]*2.25) + (c3[j%60]*.92);
      double green = (c1[j%60]) + (c2[j%60]*.70) + (c3[j%60]*0);

      int r = (int) red;
      int b = (int) blue;
      int g = (int) green;
      
      strip.setPixelColor(j, r, g, b);
      
    }
    delay(wait);
    strip.show();
    double c1first = c1[0];
    double c2first = c2[0];
    double c3first = c3[0];
    for(int k=0; k<59; k++){                                        //this is what is shifting the colors and making them "move"
      c1[k] = c1[k+1];
      c2[k] = c2[k+1];
      c3[k] = c3[k+1];
    }
      c1[59] = c1first;
      c2[59] = c2first;
      c3[59] = c3first;    
        
  }
  
}
//this is from the example code, will use wheel to make red green and blue transition into each other
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 1; j++) { // 5 cycles of all colors on wheel
      for(i=0; i< stripNum*PIXEL_COUNT; i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      delay(wait);
  }
}

//Similar to the two pulse, but with ever two pixels having different values
//will require some green to work correctly
void whitePulse(int high, int low, double redMult, double blueMult, uint8_t wait){
  for(int j=0; j<1; j++){
    int start1 = high;
    int start2 = low;
    while(start1 != low){
      for(int i = 0; i<stripNum*PIXEL_COUNT; i= i+4){
      
        strip.setPixelColor(i, start1*redMult, start1, start1*blueMult);
        strip.setPixelColor(i+1, start1*redMult, start1, start1*blueMult);
        strip.setPixelColor(i+2, start2*redMult, start2, start2*blueMult);
        strip.setPixelColor(i+3, start2*redMult, start2, start2*blueMult);
      }
      
      start1 = start1 - 1;
      start2 = start2 +1;
      strip.show();
      delay(wait);
    }

    while(start1!=high){
      for(int i = 0; i<stripNum*PIXEL_COUNT; i= i+4){
        strip.setPixelColor(i, start1*redMult, start1, start1*blueMult);
        strip.setPixelColor(i+1, start1*redMult, start1, start1*blueMult);
        strip.setPixelColor(i+2, start2*redMult, start2, start2*blueMult);
        strip.setPixelColor(i+3, start2*redMult, start2, start2*blueMult);
       }
      
      start1 = start1 +1;
      start2 = start2 -1;
      strip.show();
      delay(wait);
    }
  }
}



// Fill the dots one after the other with a color
//Edited so that it lights up by strip and not consecutively
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<PIXEL_COUNT; i++) {
    for(int j = 0; j <=stripNum; j ++){
	      strip.setPixelColor(i+(j*30), c);
    }
    
  }
  delay(wait);
  strip.show();
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    
    return strip.Color((int) ((255 - WheelPos * 3)/4), 0,  (int) ((WheelPos * 3)/4));
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, (int) ((WheelPos * 3)/4), (int) ((255 - WheelPos * 3)/4));
  }
  WheelPos -= 170;
  return strip.Color((int)((WheelPos * 3)/4), (int) ((255 - WheelPos * 3)/4), 0);
}




