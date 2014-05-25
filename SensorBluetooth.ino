#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(2, 3);

//  VARIABLES
int pulsePin = 0;                 // Pulse Sensor connected to analog pin 0
int tempPin = A1;                 // Temperature Sensor connected to analog pin 1
int blinkPin = 13;                // pin to blink led at each beat
const int buzzPin = 10;
const int pulseMax = 100;
const float baselineTemp = 20.0;

int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, must be seeded! 
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduino finds a beat.


void setup(){
  pinMode(buzzPin, OUTPUT);        // pin that will buzz if the heartbeat exceeds the threshold
  Serial.begin(57600);

  mySerial.begin(115200);           // baud rate must match data rate on bluetooth device
  
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS  
 
}



void loop(){
  int tempSensorVal = analogRead(tempPin); 
  float voltage = (tempSensorVal/1024.0) * 5.0;  // convert the ADC reading to voltage
  int temperature = (voltage -.5) * 100;         // convert the voltage to Celsium temperature
  
  sendDataViaBluetooth('I', 11072);              // send the ID number to verify the incoming data
  sendDataViaBluetooth('S', Signal);             // send the raw incoming signal data
  sendDataViaBluetooth('T', temperature);        // Send temperature data
  if (QS == true){                               // QF flag is true if Arduino board finds a heartbeat
        fadeRate = 255;                          // set 'fadeRate' Variable to 255 to fade LED with pulse
        sendDataViaBluetooth('B',BPM);           // send heart rate with a 'B' prefix
        sendDataViaBluetooth('Q',IBI);           // send time between beats with a 'Q' prefix
        QS = false;                              // reset the Quantified Self flag for next time    
     }
  
  ledFadeToBeat();
  
  if (BPM > pulseMax) {
    digitalWrite(buzzPin, HIGH);
  } else {
      digitalWrite(buzzPin, LOW);
    }
  delay(20);                             //  take a break
}


void ledFadeToBeat(){
    fadeRate -= 15;                         //  set LED fade value
    fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
    analogWrite(fadePin,fadeRate);          //  fade LED
  }


void sendDataViaBluetooth(char symbol, int data ){
   // mySerial.print("{");
    mySerial.print(symbol);    // symbol prefix tells Processing what type of data is coming
  //  mySerial.print(": ");
    mySerial.println(data);    // the data to send culminating in a carriage return
   // mySerial.println("}");  
}







