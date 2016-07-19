/* Pulse Sensor Amped with NeoPixels

Pulse an arbitrary number of Adafruit NeoPixels based on a heartbeat sensor

The pulsesensor.com code needs to be in module interrupt.ino in the sketch directory
   http://pulsesensor.com/pages/pulse-sensor-amped-arduino-v1dot1
Code also uses the Adafruit NeoPixel library code discussed at
   https://learn.adafruit.com/adafruit-neopixel-uberguide

Version 1.0 by Mike Barela for Adafruit Industries, Fall 2015 
*/
#include <Adafruit_NeoPixel.h>    // Library containing 

// Behavior setting variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // Digital pin to blink led at each beat
int fadePin  = 8;                 // pin to do fancy neopixel effects at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// these variables are volatile because they are used during the interrupt service routine
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Set up use of NeoPixels
const int NUMPIXELS = 16;           // Put the number of NeoPixels you are using here
const int BRIGHTNESS = 60;          // Set brightness of NeoPixels here
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, fadePin, NEO_GRB + NEO_KHZ800);

void setup(){
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  Serial.begin(115200);           // Serial output data for debugging or external use
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  for (int x=0; x < NUMPIXELS; x++) {  // Initialize all pixels to 'off'
     strip.setPixelColor(x, strip.Color(0, 0, 0));
  }
  strip.show();                     // Ensure the pixels are off 
  delay(1000);                      // Wait a second
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 
}

void loop(){
//  sendDataSerial('S', Signal);      // send Processing the raw Pulse Sensor data
  if (QS == true){        // Quantified Self flag is true when arduino finds a heartbeat
     Serial.println("QS true");
     fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
     sendDataSerial('B',BPM);       // send heart rate with a 'B' prefix
 //    sendDataSerial('Q',IBI);       // send time between beats with a 'Q' prefix
     QS = false;                      // reset the Quantified Self flag for next time    
  }
  //ledFadeToBeat();                    // Routine that fades color intensity to the beat
  
  // breathing code
  int TOTAL_LEDS = 16;
  float MaximumBrightness = 15;
  
  Serial.print("SpeeedFactor");
  //Serial.print(SpeedFactor);
  float StepDelay = 5; // ms for a step delay on the lights
  
  
  for (int i = 0; i < 65535; i++) {
    // main NeoPixel execution
    
    // variables that will change
    int SpeedFactorBig;
    int SpeedFactor;
    //float SpeedFactorFloat;
    
    // this won't change
    long interval = 2000; // 1000ms = 1s
    
    // check the time
    unsigned long currentMillis = millis();
    
    // set half to red
//    for (int ledNumber=1; ledNumber<TOTAL_LEDS; ledNumber+=2) {
//      strip.setPixelColor(ledNumber, 155, 0, 0);
//    }
int QSCount;

if (QS == true){        // Quantified Self flag is true when arduino finds a heartbeat
     Serial.println("QS true");
     fadeRate = 255;                  // Set 'fadeRate' Variable to 255 to fade LED with pulse
     sendDataSerial('B',BPM);       // send heart rate with a 'B' prefix
     QSCount++;
     sendDataSerial('Q',QSCount);       // send time between beats with a 'Q' prefix
     QS = false;                      // reset the Quantified Self flag for next time    
  }
    
    
    float intensity;
    uint32_t stripColor;
    
    float SpeedFactorFloat = (SpeedFactor/1000.0);
      
      intensity = MaximumBrightness /2.0 * (1.0 + sin(SpeedFactorFloat * i));

    if(currentMillis % interval == 0){
      
      
           
      if((BPM > 150) || (BPM < 60)){
       BPM = 0; 
       SpeedFactor = 0;
      } else {
         // grab BPM and set SpeedFactor
      // inclusive 
      SpeedFactor = mapf(BPM, 60, 150, 9, 30); //0.030 - max | 0.008 - min  
      //float SpeedFactorMini = SpeedFactor / 1000.0;
      SpeedFactor = constrain(SpeedFactor, 9, 30); // keep within limits
      }
      
      
      
     
      //Serial.println(QSCount);
      Serial.println(BPM);
      Serial.println(SpeedFactor);
      //Serial.println(SpeedFactorFloat);
      //Serial.println("this");
     if( (SpeedFactor >= 9) && (SpeedFactor < 16)  ){
      stripColor = strip.Color(0, 0, 250);
     } else if ( (SpeedFactor >= 16) && (SpeedFactor < 23) ){
       stripColor = strip.Color(125, 0, 125);
     } else if ( (SpeedFactor >= 23) && (SpeedFactor < 30) ){
       stripColor = strip.Color(250, 0, 0);
     } else {
       stripColor = strip.Color(0, 0, 0); 
     }

     
     // Serial.println(SpeedFactorMini);
    }
     
    strip.setBrightness(intensity);

    
    for (int ledNumber=0; ledNumber<TOTAL_LEDS; ledNumber++) {

    
    strip.setPixelColor(ledNumber, stripColor);

    //strip.setPixelColor(ledNumber, 0, 0, 255);
      
    }


    strip.show(); 
  
     
  }
  Serial.println("after loop");
  
  // end breathing code
  delay(1000);                          //  take a break
}

void ledFadeToBeat() {
    fadeRate -= 15;                         // Set LED fade value
    fadeRate = constrain(fadeRate,0,255);   // Keep LED fade value from going into negative numbers
    setStrip(fadeRate);                     // Write the value to the NeoPixels 
//    sendDataSerial('R',fadeRate);
}

void sendDataSerial(char symbol, int data ) {
    Serial.print(symbol);                // symbol prefix tells Processing what type of data is coming
    Serial.println(data);                // the data to send culminating in a carriage return
}

void setStrip(int b) {     // Set the strip to one color intensity (red)
   int g = 0;              // Green is set to zero (for non-red colors, change this)
   //int b = 0;              // Blue is set to zero (for non-red colors, change this)
   int r = 205;
   for (int x=0; x < NUMPIXELS; x++) {
      strip.setPixelColor(x, strip.Color(r, g, b));
   }
   strip.show();
}

float mapf(long x, float in_min, float in_max, float out_min, float out_max)
 {
   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }
