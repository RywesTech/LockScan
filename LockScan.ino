/*
 |----------- FPS Wiring -----------|
 |Pin 1: TX (black wire)            |
 |Pin 2: RX (first white wire)      |
 |Pin 3: Ground (second white wire) |
 |Pin 4: 5V (third white wire)      |
 |----------------------------------|

 Version 1.0: Created by Ryan Westcott, January 12th 2015
 Version 2.0: Created by Ryan Westcott, September 26th 2016

 Licensed for non-commercial use, must include this license message.
 Basically, Feel free to hack away at it, but just give me credit for my work.

 Hookup:
    arduino tx - 560ohm resistor fps rx - 1000ohm resistor - ground

    2: Left Button
    3: Right Button
    4: FPS
    5: FPS
    6: Servo
    7: Lights
    8: Buzzer
 */

#include "FPS_GT511C3.h" //The FPS library
#include "SoftwareSerial.h" //Another needed library
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

//For the NeoPixel:
#define LEDPin     7
#define NUMPIXELS  24

FPS_GT511C3  fps(4, 5);// The FPS object and it's pins 4 = FPS tx, 5 = FPS rx
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPin, NEO_GRB + NEO_KHZ800);
Servo lockServo, doorServo;

const int btn01        = 2;  // Left (01) button pin
const int btn02        = 3;  // Right (02) button pin
const int lockServoPin = 6;  // The servo to lock/unlock the door
const int doorServoPin = 7;  // The servo to push the door open
const int buzzer       = 8;  // The pin for a buzzer to provide audible feedback
const int debugLED     = 13; // Debug LED used for providing diagnostic feedback

// Lock Servo: used for locking/unlocking the door.
const int unlocked = 180; // The lock servo's position for unlocked
const int locked   = 0;   // The lock servo's position for locked

// Door Servo: used for magicly pushing the door open
const int doorPosClosed = 0;   // The door servo position for closed
const int doorPosOpen   = 180; // The door servo position for closed

void setup() {

  // Initiate pin inputs/outputs:
  pinMode(btn01, INPUT);
  pinMode(btn02, INPUT);
  pinMode(debugLED, OUTPUT);

  digitalWrite(debugLED, LOW); // debugLED is used to show information when the device is in use

  fps.UseSerialDebug = false; // no serial, so not needed
  fps.Open(); // Connect the FPS
  fps.SetLED(true); // Turn on the FPS's LED

  // NeoPixel Library code startup code
  pixels.begin();

  for (int i = 0; i < NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 0, 3));
    pixels.setPixelColor(NUMPIXELS - i, pixels.Color(0, 0, 3));

    pixels.show();
    delay(30);
  }

  // lockServo is a servo to open/close the lock:
  lockServo.attach(lockServoPin);
  lockServo.write(locked);

  doorServo.attach(doorServoPin);
  doorServo.write(doorPosClosed);
}

void loop() {

  if (fps.IsPressFinger()) {
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    if (id < 200) {

      if (id == 0) {
        success();
      }

      // add the option for other fingers to be scanned here
    } else {
      fail();
    }

  } else {
    
    lockServo.write(locked);

    for (int i = 0; i < NUMPIXELS; i++) {

      pixels.setPixelColor(i, pixels.Color(0, 0, 3));
      pixels.setPixelColor(NUMPIXELS - i, pixels.Color(0, 0, 3));

      pixels.show();
      delay(30);
    }
  }
}

void success() { // What to do to open the lock:
  lockServo.write(unlocked);
  doorServo.write(doorPosOpen);
  tone(buzzer,800,500);
  for (int i = 0; i < NUMPIXELS/2+1; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 4, 0));
    pixels.setPixelColor(NUMPIXELS - i, pixels.Color(0, 4, 0));

    pixels.show();
    delay(30);
  }
  digitalWrite(debugLED, HIGH);
  delay(1000);
  for (int i = 0; i < NUMPIXELS; i++) {

    pixels.setPixelColor(i, pixels.Color(0, 0, 3));
    pixels.setPixelColor(NUMPIXELS - i, pixels.Color(0, 0, 3));

    pixels.show();
    delay(30);
  }
}

void fail() {
  lockServo.write(unlocked);
  doorServo.write(doorPosClosed);
  tone(buzzer, 100,700);
  for (int i = 0; i < NUMPIXELS/2+1; i++) {

    pixels.setPixelColor(i, pixels.Color(4, 0, 0));
    pixels.setPixelColor(NUMPIXELS - i, pixels.Color(4, 0, 0));

    pixels.show();
    delay(30);
  }
  delay(500);
}

