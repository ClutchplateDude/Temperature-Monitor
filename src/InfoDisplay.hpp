#pragma once
#include <Arduino.h>

#define ARDUINO_ARCH_ESP32 1

#include "SSD1306_128x64_Display.hpp" 

// Base class to implement a
class InfoDisplay : public SDD1306OLED128x64
{
private:
  float temperature = 0.0;

public:
  InfoDisplay() : SDD1306OLED128x64(0x3c, SDA, SCL) {};

  void setTemperature(float temperature)
  {
    this->temperature = temperature;
  };
  
  String getTemperature() {
    return String(this->temperature, 1) + "F";
  }

  virtual void drawContent()
  {
    // Draw the content
    display->setFont(Bitmap7x15);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0, yMaxStatus /2,  String(temperature, 1) + char(40));
 };
};
