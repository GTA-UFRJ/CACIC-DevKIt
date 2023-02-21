#include <WiFi.h>
#include <HTTPClient.h>
#include "EmonLib.h"

// Create this file with two macros: SSID ans PASS
#include "wifi_info.h"

const char* ssid = SSID;
const char* password = PASS;

// Access point address (IP address and TCP port)
String ap_address = "http://192.168.1.1:8080/smart-meter";

// Sending interval
unsigned long last_send_time = 0;
unsigned long last_measure_time = 0;
unsigned long sending_interval = 10000;

bool http_enabled = true;

int voltage_calibration = 393;
int current_calibration = 4.2;
long acumulated_energy = 0.0; // in Ws
EnergyMonitor emon;

// Compute riemmen area
long computeEnergy() {
  long measured  = 0;
  emon.serialprint();
  emon.calcVI(20,2000);

  if(emon.Irms > 0.045) 
    measured = emon.apparentPower * (millis() - last_measure_time) / 1000.0; // Ws
    //measured = emon.apparentPower * (millis() - last_measure_time) / (3600.0); // mWh
  last_measure_time = millis();
  return measured;
  
}

void setup() {

  // Serial monitor baud rate for debugging 
  Serial.begin(115200); 
  Serial.println("Welcome to debugg monitor!");

  if(http_enabled) {
    // Initialize WiFi
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
  }

  emon.voltage(35, voltage_calibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, current_calibration);
  for(unsigned time=0; time<30; time++)
    emon.calcVI(20,2000); 
  last_send_time = millis();
  last_measure_time = millis();
}

void loop() {

  // Compute energy using numeric integration with Euler
  acumulated_energy += computeEnergy();
  // acumulated_energy = ((millis() - last_send_time) > sending_interval) ? 0 : acumulated_energy; 

  // Send smart meter data every x seconds
  if (((millis() - last_send_time) > sending_interval) && http_enabled && acumulated_energy) {
    
    last_send_time = millis();

    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      // Send HTTP GET request
      String ap_path = ap_address + "=" + String(acumulated_energy);
      http.begin(ap_path.c_str());
      Serial.println("Sending: " + ap_path);
      int http_response_code = http.GET();
      
      if (http_response_code > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(http_response_code);
        String payload = http.getString();
        Serial.print("Returned payload: ");
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(http_response_code);
      }
      acumulated_energy = 0.0;
      // Free resources
      http.end();
    }
    else 
      Serial.println("WiFi Disconnected");

    //for(unsigned time=0; time<30; time++)
    //  emon.calcVI(20,2000); 
  }
}