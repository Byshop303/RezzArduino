// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <vector>
#include "LSM6DS3.h"
#include "Wire.h"
#include "SPI.h"
#include <ArduinoBLE.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        5 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 88 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
LSM6DS3Core myIMU(I2C_MODE, 0x6A);
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

#define DELAYVAL 40 // Time (in milliseconds) to pause between pixels
uint8_t i;
uint8_t ion;
int32_t outerring1index = 0;
uint8_t outerring1shapelength = 5;
uint8_t outerring1shapeend;
uint8_t midring1index = 40;
uint8_t midring1shapelength = 3;
uint8_t midring1shapeend;
uint8_t centerring1shapelength = 2;
uint8_t centerring1index = 41;
uint8_t centerring1shapeend;
int32_t outerring1speed = 50;
int32_t midring1speed = 100;
int32_t centerring1speed = 200;
int32_t outerring1color;
int32_t midring1color;
int32_t centerring1color;
int32_t outerring1direction = -1;
int32_t midring1direction = -1;
int32_t centerring1direction = -1;

uint8_t outerring2index = 44;
uint8_t outerring2shapeend;
uint8_t outerring2shapelength = 8;
uint8_t midring2index = 68;
uint8_t midring2shapelength = 4;
uint8_t midring2shapeend;
uint8_t centerring2shapelength = 2;
uint8_t centerring2shapeend;
uint8_t centerring2index = 84;
int32_t outerring2speed = 50;
int32_t midring2speed = 100;
int32_t centerring2speed = 200;
int32_t outerring2color;
int32_t midring2color;
int32_t centerring2color;
int32_t outerring2direction = 1;
int32_t midring2direction = 1;
int32_t centerring2direction = 1;

std::vector<int> mirror;



// uint8_t brightness = 10;
uint8_t brightness = 6;
int32_t currentcolor; // Current color of the LEDs

static unsigned long OR1lastEventTime = 0;
static unsigned long MR1lastEventTime = 0;
static unsigned long CR1lastEventTime = 0;
static unsigned long OR1LastColorTime = 0;
static unsigned long MR1LastColorTime = 0;
static unsigned long CR1LastColorTime = 0;
static unsigned long OR2lastEventTime = 0;
static unsigned long MR2lastEventTime = 0;
static unsigned long CR2lastEventTime = 0;
static unsigned long OR2LastColorTime = 0;
static unsigned long MR2LastColorTime = 0;
static unsigned long CR2LastColorTime = 0;
static unsigned long LastXGyroTime = 0;
static unsigned long LastYGyroTime = 0;
static unsigned long LastZGyroTime = 0;

uint16_t errorsAndWarnings = 0;

struct RainbowColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

RainbowColor rainbowColors[] = {
  {255, 0, 0},      // Red
//  {255, 127, 0},    // Orange
//  {255, 255, 0},    // Yellow
  {0, 255, 0},      // Green
  {0, 0, 255},      // Blue
  {75, 0, 130},     // Indigo
  {148, 0, 211}     // Violet
};
bool boolArray[NUMPIXELS] = {false};
bool dorando = true;


void startpixels(){
  pixels.clear();
  pixels.setBrightness(brightness);
  pixels.show();   // Send the updated pixel colors to the hardware.
}

void lit(int index, int color){
  pixels.setPixelColor(index, color);
}

int32_t rando(){
  //i=random(0,6);
  i=random(0,4);
  RainbowColor color = rainbowColors[i];
  int32_t coloroutput = pixels.Color(color.r,color.g,color.b);
  return coloroutput;
}

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

i = 0;

do {
  if (i < 24) {
    mirror.push_back((i + 12) % 24);
  } else if (i >= 24 && i <= 39) {
    if (i + 8 <= 39) {
      mirror.push_back(i + 8);
    } else {
      mirror.push_back(i - 8);
    }
  } else if (i >= 40 && i <= 43) {
    if (i + 2 <= 43) {
      mirror.push_back(i + 2);
    } else {
      mirror.push_back(i - 2);
    }
  } else if (i >= 44 && i <= 67) {
    if (i + 12 <= 67) {
      mirror.push_back(i + 12);
    } else {
      mirror.push_back(i - 12);
    }
  } else if (i >= 68 && i <= 83) {
    if (i + 8 <= 83) {
      mirror.push_back(i + 8);
    } else {
      mirror.push_back(i - 8);
    }
  } else if (i >= 84 && i <= 87) {
    if (i + 2 <= 87) {
      mirror.push_back(i + 2);
    } else {
      mirror.push_back(i - 2);
    }
  }
  i++;
} while (i != 88);

  Serial.begin(115200);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  centerring1color = rando();
  midring1color = rando();
  outerring1color = rando();
  centerring2color = rando();
  midring2color = rando();
  outerring2color = rando();
  startpixels();

      //Call .beginCore() to configure the IMU
    if (myIMU.beginCore() != 0) {
        Serial.print("\nDevice Error.\n");
    } else {
        Serial.print("\nDevice OK.\n");
    }


    uint8_t dataToWrite = 0;  //Temporary variable

    //Setup the accelerometer******************************
    dataToWrite = 0; //Start Fresh!
    dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
    dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_8g;
    dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;
        //Now, write the patched together data
    errorsAndWarnings += myIMU.writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

    //Set the ODR bit
    errorsAndWarnings += myIMU.readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
    dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);




  

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("LED");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");

}



void loop() {
  
    int16_t xreading;
    int16_t yreading;
    int16_t zreading;
    //Get all parameters
    Serial.print("\nAccelerometer Counts:\n");

    //Acelerometer axis X
    if (myIMU.readRegisterInt16(&xreading, LSM6DS3_ACC_GYRO_OUTX_L_XL) != 0) {
        errorsAndWarnings++;
    }
    Serial.print(" X = ");
    Serial.print(xreading);

    //Acelerometer axis Y
    if (myIMU.readRegisterInt16(&yreading, LSM6DS3_ACC_GYRO_OUTY_L_XL) != 0) {
        errorsAndWarnings++;
    }
    Serial.print(" Y = ");
    Serial.print(yreading);

    //Acelerometer axis Z
    if (myIMU.readRegisterInt16(&zreading, LSM6DS3_ACC_GYRO_OUTZ_L_XL) != 0) {
        errorsAndWarnings++;
    }
    Serial.print(" Z = ");
    Serial.println(zreading);

    Serial.println();
    Serial.print("Total reported Errors and Warnings: ");
    Serial.println(errorsAndWarnings);


  unsigned long currentTime = millis();
/*
  if (currentTime - LastXGyroTime >= 1000){
    if (xreading < -2000) {
      outerring1speed -= 20;
      outerring2speed -= 20;
      midring1speed -= 20;
      midring2speed -= 20;
      centerring1speed -= 20;
      centerring2speed -= 20;
      LastXGyroTime = currentTime;
    } else if (xreading > 2000) {
      outerring1speed += 20;
      outerring2speed += 20;
      midring1speed += 20;
      midring2speed += 20;
      centerring1speed += 20;
      centerring2speed += 20;
      LastXGyroTime = currentTime;
    }
  }
  */

    if (currentTime - LastZGyroTime >= 100){
    if (zreading < -2500) {
      centerring1color = rando();
      midring1color = rando();
      outerring1color = rando();
      centerring2color = rando();
      midring2color = rando();
      outerring2color = rando();
      LastZGyroTime = currentTime;
      dorando=true;
    } else if (zreading > 2500) {
      // set color of all rings to red
      centerring1color = pixels.Color(0,255,0);
      midring1color = pixels.Color(0,255,0);
      outerring1color = pixels.Color(0,255,0);
      centerring2color = pixels.Color(0,255,0);
      midring2color = pixels.Color(0,255,0);
      outerring2color = pixels.Color(0,255,0);
      LastZGyroTime = currentTime;
      dorando=false;
    }
  }

  if (currentTime - CR1LastColorTime >= 10000) {
    // 5 seconds have passed since the last event
    CR1LastColorTime = currentTime;
    MR1LastColorTime = currentTime;
    if(dorando){
      centerring1color = rando();
    }
  }

  if(currentTime - MR1LastColorTime >= 500){
    MR1LastColorTime = currentTime;
    OR1LastColorTime = currentTime;
    midring1color = centerring1color;
  }

  if(currentTime - OR1LastColorTime >= 400){
    OR1LastColorTime = currentTime;
    outerring1color = midring1color;
  }
  if (currentTime - CR2LastColorTime >= 12000) {
    // 5 seconds have passed since the last event
    CR2LastColorTime = currentTime;
    MR2LastColorTime = currentTime;
        if(dorando){
      centerring2color = rando();
    }
  }

  if(currentTime - MR2LastColorTime >= 500){
    MR2LastColorTime = currentTime;
    OR2LastColorTime = currentTime;
    midring2color = centerring2color;
  }

  if(currentTime - OR2LastColorTime > 400){
    OR2LastColorTime = currentTime;
    outerring2color = midring2color;
  }

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  
if(currentTime - OR1lastEventTime >= outerring1speed){
  outerring1index = (outerring1index + outerring1direction) % 24;
  if (outerring1index < 0) {
    outerring1index += 24;
  }
  outerring1shapeend = (outerring1index + outerring1shapelength) % 24;
  if(outerring1shapeend < 0){
    outerring1shapeend += 24;
  }

  pixels.setPixelColor(outerring1index, pixels.Color(0,0,0));
  pixels.setPixelColor((outerring1shapeend), outerring1color);
  pixels.setPixelColor(mirror[outerring1index], pixels.Color(0,0,0));
  pixels.setPixelColor((mirror[outerring1shapeend]), outerring1color);
  OR1lastEventTime = currentTime;
  /*
  Serial.print(" ORIndex1: ");
  Serial.print(outerring1index);
  Serial.print(" OR1shapeend1: ");
  Serial.print(outerring1shapeend);
  Serial.print(" mirror: ");
  Serial.print(mirror[outerring1index]);
  Serial.print(" mirrorshapeend: ");
  Serial.println(mirror[outerring1shapeend]);
  */
}


if(currentTime - OR2lastEventTime >= outerring2speed){
  outerring2index = (outerring2index + outerring2direction) % 68;
  if (outerring2index < 44 && outerring2index > 19) {
    outerring2index += 24;
  } else if(outerring2index < 19){
    outerring2index += 44;
  }
  outerring2shapeend = (outerring2index + outerring2shapelength) % 68;
  if (outerring2shapeend < 44 && outerring2shapeend > 19) {
    outerring2shapeend += 24;
  }else if(outerring2shapeend < 19){
    outerring2shapeend += 44;
  }


pixels.setPixelColor(outerring2index, pixels.Color(0,0,0));
pixels.setPixelColor(outerring2shapeend, outerring2color);
pixels.setPixelColor(mirror[outerring2index], pixels.Color(0,0,0));
pixels.setPixelColor(mirror[outerring2shapeend], outerring2color);
OR2lastEventTime = currentTime;

  Serial.print("ORIndex2: ");
  Serial.print(outerring2index);
  Serial.print(" ORShapeEnd2: ");
  Serial.print(outerring2shapeend);
  Serial.print(" ORMirrorIndex2: ");
  Serial.print(mirror[outerring2index]);
  Serial.print(" ORMirrorShapeEnd2: ");
  Serial.println(mirror[outerring2shapeend]);

}


if(currentTime - MR1lastEventTime >= midring1speed){
  midring1index = (midring1index + midring1direction) % 40;
  if (midring1index < 24 && midring1index > 15) {
    midring1index += 16;
  } else if (midring1index < 24) {
    midring1index += 24;
  }
  midring1shapeend = (midring1index + midring1shapelength) % 40;
  if (midring1shapeend < 24 && midring1shapeend > 15) {
    midring1shapeend += 16;
  } else if (midring1shapeend < 24) {
    midring1shapeend += 24;
  }
  pixels.setPixelColor(midring1index, pixels.Color(0,0,0));
  pixels.setPixelColor(midring1shapeend, midring1color);
  pixels.setPixelColor(mirror[midring1index], pixels.Color(0,0,0));
  pixels.setPixelColor(mirror[midring1shapeend], midring1color);
  MR1lastEventTime = currentTime;
  /*
  Serial.print(" midring1index: ");
  Serial.print(midring1index);
  Serial.print(" midring1shapeend: ");
  Serial.print(midring1index);
  Serial.print(" mirror: ");
  Serial.print(mirror[midring1index]);
  Serial.print(" midring1shapeend: ");
  Serial.println(mirror[midring1shapeend]);
  */
}



if(currentTime -MR2lastEventTime >= midring2speed){
  midring2index = (midring2index + midring2direction) % 84;
  if (midring2index < 68 && midring2index > 59) {
    midring2index += 16;
    }else if (midring2index < 60) {
      midring2index += 68;
    }
  
  midring2shapeend = (midring2index + midring2shapelength) % 84;
  if (midring2shapeend < 68) {
    midring2shapeend += 68;
  }
  pixels.setPixelColor(midring2index, pixels.Color(0,0,0));
  pixels.setPixelColor(midring2shapeend, midring2color);
  pixels.setPixelColor(mirror[midring2index], pixels.Color(0,0,0));
  pixels.setPixelColor(mirror[midring2shapeend], midring2color);
  MR2lastEventTime = currentTime;
}


if(currentTime - CR1lastEventTime >= centerring1speed){
  centerring1index = (centerring1index + centerring1direction) % 44;
  if (centerring1index < 40 && centerring1index > 36) {
    centerring1index += 4;
  } else if (centerring1index < 36) {
    centerring1index += 40;
  }
  centerring1shapeend = (centerring1index + centerring1shapelength) % 44;
  if (centerring1shapeend < 40 && centerring1shapeend > 36) {
    centerring1shapeend += 4;
  } else if (centerring1shapeend < 36) {
    centerring1shapeend += 40;
  } 

  pixels.setPixelColor(centerring1index, pixels.Color(0,0,0));
  pixels.setPixelColor(centerring1shapeend, centerring1color);
  pixels.setPixelColor(mirror[centerring1index], pixels.Color(0,0,0));
  pixels.setPixelColor(mirror[centerring1shapeend], centerring1color);
  CR1lastEventTime = currentTime;
  /*
  Serial.print(" centerring1index: ");
  Serial.print(centerring1index);
  Serial.print(" centerring1shapeend: ");
  Serial.print(centerring1shapeend);
  Serial.print(" mirrorcenterring1index: ");
  Serial.print(mirror[centerring1index]);
  Serial.print(" mirrorcenterring1shapeend: ");
  Serial.println(mirror[centerring1shapeend]);
  */
}

if(currentTime - CR2lastEventTime >= centerring2speed){
  centerring2index = (centerring2index + centerring2direction) % 88;
  if (centerring2index < 84 && centerring2index > 79) {
    centerring2index += 4;
    } else if(centerring2index < 80){
      centerring2index += 84;
    }
  
  centerring2shapeend = (centerring2index + centerring2shapelength) % 88;
  if (centerring2shapeend < 84 && centerring2shapeend > 79) {
    centerring2shapeend += 4;
  } else if (centerring2shapeend < 80) {
    centerring2shapeend += 84;
  }
  pixels.setPixelColor(centerring2index, pixels.Color(0,0,0));
  pixels.setPixelColor(centerring2shapeend, centerring2color);
  pixels.setPixelColor(mirror[centerring2index], pixels.Color(0,0,0));
  pixels.setPixelColor(mirror[centerring2shapeend], centerring2color);
  CR2lastEventTime = currentTime;
  //Serial.print("CRIndex2:");
  //Serial.print(centerring2index);
}
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
  while (central.connected()) {
        if (switchCharacteristic.written()) {
          if (switchCharacteristic.value()) {   
            Serial.println("LED on");

          } else {                              
            Serial.println(F("LED off"));

          }
        }
      }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
  pixels.show();   // Send the updated pixel colors to the hardware.
}
