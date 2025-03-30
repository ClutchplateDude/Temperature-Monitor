#include <Arduino.h>
#include <WString.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include <NonBlockingDallas.h>                  //Include the NonBlockingDallas library

#ifdef ESP32
#include <WiFi.h>
#include <WiFiSTA.h>
#include "esp_task_wdt.h"

#endif
// #include <ESPAsyncTCP.h>
#include "indexhtml.hpp"
#include <ESPAsyncWebServer.h>

char *ssid = "Clutchplate";
const char *password = "36FA8A7748";

AsyncWebServer server(80);


#define ONE_WIRE_BUS 19                          //PIN of the Maxim DS18B20 temperature sensor
#define TIME_INTERVAL 1500                      //Time interval among sensor readings [milliseconds]

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasTemp(&oneWire);
NonBlockingDallas temperatureSensors(&dallasTemp);    //Create a new instance of the NonBlockingDallas class

#include "InfoDisplay.hpp"

#define SERIAL_BAUDRATE 19200

InfoDisplay* display = new InfoDisplay();

//------------- Core0 -------------//

void handleIntervalElapsed(int deviceIndex, int32_t temperatureRAW)
{
}

// Replace some parts of the webpage with the status of the speaker box.
String processor( const String& var)
{
    if (var =="TEMP"){
        return display->getTemperature();
    }
    return String();
}


//Invoked ONLY when the temperature changes between two VALID sensor readings. "valid" parameter will be removed in a feature version
void handleTemperatureChange(int deviceIndex, int32_t temperatureRAW)
{
  Serial.print(F("[NonBlockingDallas] handleTemperatureChange ==> deviceIndex="));
  Serial.print(deviceIndex);
  Serial.print(F(" | RAW="));
  Serial.print(temperatureRAW);
  Serial.print(F(" | "));
  Serial.print(temperatureSensors.rawToCelsius(temperatureRAW));
  Serial.print(F("°C | "));
  Serial.print(temperatureSensors.rawToFahrenheit(temperatureRAW));
  Serial.println(F("°F"));
  display->setTemperature(temperatureSensors.rawToFahrenheit(temperatureRAW));
  /*
   *  DO SOME AMAZING STUFF WITH THE TEMPERATURE
   */
}


void handleRootPath(AsyncWebServerRequest *request)
{
    request->send_P(200, "text/html", index_html, processor);
}

 void handleTemperature(AsyncWebServerRequest *request)
 {
     request->send(200, "text/plain", display->getTemperature());
 }

// void handleUpdates(AsyncWebServerRequest *request)
// {
//     String state;
//     if (request->hasParam("sound"))
//     {
//         state = request->getParam("sound")->value();
//         if (state == "on")
//         {
//             pulpit->turnSoundOn();
//         }
//         else if (state == "off")
//         {
//             pulpit->turnSoundOff();
//         }
//     } else     if (request->hasParam("light"))
//     {
//         state = request->getParam("light")->value();
//         int level = state.toInt();
          
//         pulpit->setLightBrightness(level);
//     }

//     request->send(200, "text/plain", "OK");
// }


//Invoked when the sensor reading fails
void handleDeviceDisconnected(int deviceIndex)
{
  Serial.print(F("[NonBlockingDallas] handleDeviceDisconnected ==> deviceIndex="));
  Serial.print(deviceIndex);
  Serial.println(F(" disconnected."));
}

void setup()
{
    Serial.begin(SERIAL_BAUDRATE);
    Serial.print("Connecting to Wifi. ");
    WiFi.begin(ssid, password);
    Serial.print("Waiting.");

    unsigned long startTime = millis();

	// Attempt to connect to the Wifi for 7.5s
    while ((WiFi.status() != WL_CONNECTED) && ((millis() - startTime) < 7500))
    {
        //ESP.wdtFeed();
        esp_task_wdt_reset();
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
		// We are connected to the Wifi, so configure the web server and start it
        Serial.print(" success at ");
        Serial.println(WiFi.localIP());

        server.on("/", HTTP_GET, handleRootPath);
        server.on("/temp", HTTP_GET, handleTemperature);
        // server.on("/update", handleUpdates);
        server.begin();
    }
    else
    {
        Serial.print(" failed to connect to SSID '");
        Serial.print(ssid);
        Serial.println("'.");
    }

    display->init(); //Initialize the display
    temperatureSensors.begin(NonBlockingDallas::resolution_12, TIME_INTERVAL);

    //Callbacks
    //temperatureSensors.onIntervalElapsed(handleIntervalElapsed);
    temperatureSensors.onTemperatureChange(handleTemperatureChange);
    //temperatureSensors.onDeviceDisconnected(handleDeviceDisconnected);
    
    //Call the following function whenever you want to request a new temperature reading without waiting for TIME_INTERVAL to elapse
    //temperatureSensors.requestTemperature();

    Serial.println("Display initialized.");
}

long lastRender = 0;

void loop()
{
    temperatureSensors.update();
    // Run the display
    if (lastRender + 20 < millis())
    {
        display->render();
        lastRender = millis();
    }
}
