

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
#define CAN_SELECT 21
#define BACKLIGHT 43
#define PIN CAN_SELECT 

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(PIN, OUTPUT);     
  pinMode(13, OUTPUT);
}
#define DELAY 1

void loop() { 
  digitalWrite(13, HIGH);    // set the LED on 
  digitalWrite(PIN, HIGH);   // set the LED on  
  delay(DELAY);                // wait for a second
  digitalWrite(13, LOW);     // set the LED off
  digitalWrite(PIN, LOW);    // set the LED off  
  delay(DELAY);                // wait for a second
}


