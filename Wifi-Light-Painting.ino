#include <ESP8266WiFi.h>
#include "FastLED.h"
//#include "Arduino.h"

#define LERP(xa,xb,val) ((1.0-val)*xa + val*xb)
#define LED_DATA_PIN 2
//#define CUL(x,y) (min(x,y))
 
const char* ssid = "JA-RdC";
const char* password = "SuperLapin";
float signalNormal = 0.0;
float rssi_min = 5.3;
float rssi_max = 7.0;
CRGB leds[1];

byte step_count = 6;
//byte red_over_strength[] = {  0,  0,  255,  255,  255,  255};
//byte green_over_strength[] = {0,  0,  0,    0,    255,  255};
//byte blue_over_strength[] = { 0,  255,255,  0,    0,    255};
//byte brightness_over_strength[] = {42,87,129,171,213,255};

byte step_count = 3;
byte red_over_strength[] = {  0,  0,  255};
byte green_over_strength[] = {0,  0,  255};
byte blue_over_strength[] = { 0,  255,0};
byte brightness_over_strength[] = {0,127,255};

int inc = 0;

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


struct Color
{
  float r;
  float g;
  float b;
};

byte min(byte a, byte b)
{
    if (a < b)
      return a;
    else
      return b;
}

byte max(byte a, byte b)
{
    if (a > b)
      return a;
    else
      return b;
}

float min(float a, float b)
{
    if (a < b)
      return a;
    else
      return b;
}

float max(float a, float b)
{
    if (a > b)
      return a;
    else
      return b;
}
 
Color GetColor(float strength);

void setup()
{
  
    FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, 1);
    Serial.begin(115200);
    Serial.println();
    Serial.print("connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
 
void loop()
{
    long rssi = WiFi.RSSI();
    Serial.print(" RSSI:");
    Serial.print(rssi);
    
    float signalNormalized = 1.0/(1.0-rssi * 0.2);
    signalNormalized *= 100.0;
    
    Serial.print(" RSSI / 100 :");
    Serial.print(signalNormalized);


    //float print_signal = signalNormalized;
    signalNormalized = mapFloat(signalNormalized, rssi_min,rssi_max,0.0,1.0);
    //signalNormalized -= 5.0;
    //signalNormalized *= 0.2;
    Serial.print(" normalized :");
    Serial.print(signalNormalized);


    signalNormal = LERP(signalNormal,signalNormalized,0.03);
    
    
    //Color ledCol = GetColor(((float)(inc%100)) * 0.01);
    //inc += 1;
    Color ledCol = GetColor(signalNormal);
    
    Serial.print("    r :");
    Serial.print(ledCol.r);
    Serial.print(" g :");
    Serial.print(ledCol.g);
    Serial.print(" b :");
    Serial.println(ledCol.b);
    //delay(100);

    
    leds[0] = CRGB((byte)ledCol.r,(byte)ledCol.g,(byte)ledCol.b);
    FastLED.show();
    delay(30); 
  }

Color GetColor(float strength)
{
  Color results;

  strength *= step_count;

  byte stepA = strength;
  byte stepB = strength + 1;

  //clamp between 0 and step_count-1
  stepA = min(max(0,stepA),step_count-1);
  stepB = min(max(0,stepB),step_count-1);

  float mixValue = strength - floor(strength);
  
    Serial.print(" stepA:");
    Serial.print(stepA);
    Serial.print(" stepB:");
    Serial.print(stepB);
    Serial.print(" mixValue:");
    Serial.print(mixValue);

  results.r = LERP(red_over_strength[stepA],red_over_strength[stepB],mixValue);
  results.g = LERP(green_over_strength[stepA],green_over_strength[stepB],mixValue);
  results.b = LERP(blue_over_strength[stepA],blue_over_strength[stepB],mixValue);
  
  results.r *= LERP(brightness_over_strength[stepA],brightness_over_strength[stepB],mixValue)/256.0;
  results.g *= LERP(brightness_over_strength[stepA],brightness_over_strength[stepB],mixValue)/256.0;
  results.b *= LERP(brightness_over_strength[stepA],brightness_over_strength[stepB],mixValue)/256.0;

  return results;
}


