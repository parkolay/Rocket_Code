/*
Teensy_Simple_AT-CMD_HTML_Server
For Teensy 3.x and ESP8266
Using a Teensy 3.1 to Connect an ESP8266 to Android Phone *******
Compiled with Arduino 1.6.5 and Teensyduino 1.24b3
ESP8266 Firmware: AT21SDK95-2015-01-24.bin

Used code from http://randomnerdtutorials.com/esp8266-web-server/
Used code from https://forum.pjrc.com/threads/27850-A-Guide-To-Using-ESP8266-With-TEENSY-3/page2
Based on ESP8266 Demo code by Ray Wang
Original sketch by Ray Wang @ Rayshobby LLC - http://raysfiles.com/arduino/ESP8266a_arduino.ino
Teensy ACD library by Pedvide     http://forum.pjrc.com/threads/25532-ADC-library-update-now-with-support-for-Teensy-3-1
And a bunch of other places...

DAP - 07/31/2015 - v-1 - initial compile of code
-Removed java code(from WOOZY), not using internet access to ESP8266 via AP so it does not work without it.
         it could be placed back in and use visabilty:hidden/visable based on Wifisetup()configuration...
-Rewrote the WiFi connection to ESP8266, previous code did not function for me
-Added buttons and modified how analog value was displayed

DAP - 08/04/2015 - v-2 - Added LED PIN13 state change based on user input
-Typos found and fixed, created more
-added toggle example
-Removed some debug code
-added more comments

DAP - 08/04/2015 - v-3 Began GPS handeling merge
-seperated the code into multiple files
*/

#include <ADC.h>
#include <TinyGPS.h>

#define BUFFER_SIZE 4096
char buffer[BUFFER_SIZE];

const int ledPin = 13;          //LED on PIN13
int ledState = HIGH;            // ledState used to set the LED
bool toggle = false;            // toggle used for HTML button example

const int readPin = A9;         // ADC_0
ADC *adc = new ADC();           // adc object
int n = 0;
int value;
float BatteryLevel;
float falt, flat, flon;

TinyGPS gps;

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

String readString;

char OKrn[] = "OK\r\n";         // By default we are looking for OK\r\n

byte wait_for_esp_response(int timeout, char* term = OKrn)
{
  unsigned long t = millis();
  bool found = false;
  int i = 0;
  int len = strlen(term);

  while (millis() < t + timeout) // wait for at most timeout milliseconds
  { // or if OK\r\n is found
    if (Serial1.available())
    {
      buffer[i++] = Serial1.read();
      if (i >= len)
      {
        if (strncmp(buffer + i - len, term, len) == 0)
        {
          found = true;
          break;
        }
      }
    }
  }//end of while(millis()<t+timeout)
  buffer[i] = 0;
  Serial.print(buffer);
  return found;
}//end of byte wait_for_esp_response(int timeout, char* term=OKrn)


void setup()
{
  pinMode(readPin, INPUT); //pin 23 single ended
  adc->setAveraging(50); // set number of averages
  adc->setResolution(12); // set bits of resolution
  adc->setConversionSpeed(ADC_LOW_SPEED); // change the conversion speed
  // setConversionSpeed() can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_LOW_SPEED); // change the sampling speed
  // setSamplingSpeed() can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED

  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  //GPS Module operates @ 9600 baud rate
  Serial2.begin(9600);
  // assume esp8266 operates at 115200 baud rate
  // change if necessary to match your modules' baud rate
  Serial1.begin(115200);  // Teensy Hardware Serial port 1   (pins 0 and 1)
  Serial.begin(115200);   // Teensy USB Serial Port

  delay(5000);
  Serial.println("begin.");
  setupWiFi();            //jump to WebService 
  
  delay(100);
  Serial.print("TinyGPS library v. "); Serial.println(TinyGPS::library_version());

}//end of void setup()

void loop()
{
  value = adc->analogRead(readPin, ADC_0);
  //BatteryLevel = (((value*3.3/adc->getMaxValue(ADC_0))*254.5455)); // 16.8V to 14.8V 4S Li-Ion Battery Pack to Voltage Divider gives 3.3v to 2.91v then Convert to 0 to 100%
  BatteryLevel = ((value) * (3.3 / 4096)); //-->ADC_0 wired to read 3.3v supply pin

  delay(20);
  ParseWeb();

//  delay(100);
//  GetGPS(); //ran too offten for webservice 

}//end of void loop()




