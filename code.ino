/*
   -------------------------------------------------------------------------------------
   HX711_ADC
   Arduino library for HX711 24-Bit Analog-to-Digital Converter for Weight Scales
   Olav Kallhovd sept2017
   -------------------------------------------------------------------------------------
*/

/*
   Settling time (number of samples) and data filtering can be adjusted in the config.h file
   For calibration and storing the calibration value in eeprom, see example file "Calibration.ino"

   The update() function checks for new data and starts the next conversion. In order to acheive maximum effective
   sample rate, update() should be called at least as often as the HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS.
   If you have other time consuming code running (i.e. a graphical LCD), consider calling update() from an interrupt routine,
   see example file "Read_1x_load_cell_interrupt_driven.ino".

   This is an example sketch on how to use this library
*/

#include <HX711_ADC.h>
#if defined(ESP8266) || defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 12;  //mcu > HX711 dout pin
const int HX711_sck = 13;   //mcu > HX711 sck pin

const int motorPin1 = 10;
const int motorPin2 = 11;

const int buzzer = 2;  //Buzzer

//Define pins for seven segments
const int a = 3;
const int b = 4;
const int c = 5;
const int d = 6;
const int e = 7;
const int f = 8;
const int g = 9;

int weight = 0;

boolean isOpen = false;

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  //Seven Segment Display
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println("Starting...");

  LoadCell.begin();   
  //LoadCell.setReverseOutput(); //uncomment to turn a negative output value to positive
  float calibrationValue;    // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0;  // uncomment this if you want to set the calibration value in the sketch
#if defined(ESP8266) || defined(ESP32)
  //EEPROM.begin(512); // uncomment this if you use ESP8266/ESP32 and want to fetch the calibration value from eeprom
#endif
  //EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000;  // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true;                  //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1)
      ;
  } else {
    LoadCell.setCalFactor(calibrationValue);  // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;  //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      weight = int(i);
      Serial.print("Load_cell output val: ");
      Serial.println(weight);
      newDataReady = 0;
      t = millis();
    }
  }
  //displaying weight
  displayWeight(weight);

  //Buzzer conditions
  if (weight >= 50) {
    digitalWrite(buzzer, HIGH);
  } else if (weight <= 50) {
    digitalWrite(buzzer, LOW);
  }

  //MOTOR DOOR Program
  if (weight <= 0) {  // Making Door Open
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    delay(1000);
    // digitalWrite(motorPin1,LOW);
    // digitalWrite(motorPin2,LOW);

  }else if((weight<50)&&(weight>=40)){
    delay(2000);
    doorClose(weight);
  }else{
      digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  }

  // else if (weight >= 50) {
  //   isOpen == false;
  //   digitalWrite(motorPin1, LOW);
  //   digitalWrite(motorPin2, LOW);
  // }
  

 

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }
}
void displayWeight(int weight) {
  switch (weight) {
    case 0:
    case -1:
    case -2:
    case -3:
    case -4:
    case -5:
    case -6:
    case -7:
    case -8:
    case -9:
    case -10:
    case -11:
      {
        digitalWrite(a, HIGH);
        digitalWrite(b, HIGH);
        digitalWrite(c, HIGH);
        digitalWrite(d, HIGH);
        digitalWrite(e, HIGH);
        digitalWrite(d, HIGH);
        digitalWrite(e, HIGH);
        digitalWrite(f, HIGH);
        digitalWrite(g, LOW);
        break;
      }
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 1:

      {
        digitalWrite(a, 0);
        digitalWrite(b, 1);
        digitalWrite(c, 1);
        digitalWrite(d, 0);
        digitalWrite(e, 0);
        digitalWrite(f, 0);
        digitalWrite(g, 0);
        break;
      }
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 2:
    case 21:
    case 22:
    case 23:
    case 24:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 1);
        digitalWrite(c, 0);
        digitalWrite(d, 1);
        digitalWrite(e, 1);
        digitalWrite(f, 0);
        digitalWrite(g, 1);
        break;
      }
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 3:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 1);
        digitalWrite(c, 1);
        digitalWrite(d, 1);
        digitalWrite(e, 0);
        digitalWrite(f, 0);
        digitalWrite(g, 1);
        break;
      }
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 4:
    case 41:
    case 42:
    case 43:
    case 44:
      {
        digitalWrite(a, 0);
        digitalWrite(b, 1);
        digitalWrite(c, 1);
        digitalWrite(d, 0);
        digitalWrite(e, 0);
        digitalWrite(f, 1);
        digitalWrite(g, 1);
        break;
      }
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 5:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 0);
        digitalWrite(c, 1);
        digitalWrite(d, 1);
        digitalWrite(e, 0);
        digitalWrite(f, 1);
        digitalWrite(g, 1);
        break;
      }
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 6:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 0);
        digitalWrite(c, 1);
        digitalWrite(d, 1);
        digitalWrite(e, 1);
        digitalWrite(f, 1);
        digitalWrite(g, 1);
        break;
      }
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 7:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 1);
        digitalWrite(c, 1);
        digitalWrite(d, 0);
        digitalWrite(e, 0);
        digitalWrite(f, 0);
        digitalWrite(g, 0);
        break;
      }
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 8:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 1);
        digitalWrite(c, 1);
        digitalWrite(d, 1);
        digitalWrite(e, 1);
        digitalWrite(f, 1);
        digitalWrite(g, 1);
        break;
      }
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 9:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
      {
        digitalWrite(a, 1);
        digitalWrite(b, 1);
        digitalWrite(c, 1);
        digitalWrite(d, 1);
        digitalWrite(e, 0);
        digitalWrite(f, 1);
        digitalWrite(g, 1);
        break;
      }
    default:
      {
        digitalWrite(a, 0);
        digitalWrite(b, 0);
        digitalWrite(c, 0);
        digitalWrite(d, 0);
        digitalWrite(e, 0);
        digitalWrite(f, 0);
        digitalWrite(g, 1);
        break;
      }
  }
}

void doorClose(int weight) {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}
