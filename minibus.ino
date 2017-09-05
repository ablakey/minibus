#include <ESP8266WiFi.h>
#include <Modbus.h>
#include <ModbusIP_ESP8266.h>
#include "config.h"

const unsigned long GREEN_FLASH_RATE = 500;
const unsigned long DISPATCH_DEBOUNCE = 2000;

const int ALERT_COIL = 100;
const int DISPATCH_COIL = 102;  // Set high to signal to a Modbus client that the user has pressed the button.
const int IS_DISPATCHING_COIL = 104;    // Set high by a Modbus client to signal that the dispatch signal was received.

const int RED_LED_PIN = 0;
const int BLUE_LED_PIN = 4;
const int GREEN_LED_PIN = 14;
const int BUTTON_PIN = 5;

bool sendDispatchSignal = false;

unsigned long lastGreenFlash = 0;

ModbusIP mb;

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  WiFi.disconnect();  // ESP8266 autoreconnects on last startup. Disconnect to ensure consistent lifecycle.
  connect();
}

void loop() {
  mb.task();  // The Modbus server is processed here.

  if (!isConnected()) {
    connect();
  }

  // Button pressed, user wants a dispatch to occur.
  if (digitalRead(BUTTON_PIN)) {
    sendDispatchSignal = true;
  }

  // Modbus client reports dispatching is underway.
  if (mb.Coil(IS_DISPATCHING_COIL))
  {
    sendDispatchSignal = false; // Cannot dispatch if already dispatching.
    digitalWrite(GREEN_LED_PIN, HIGH);
    mb.Coil(DISPATCH_COIL, false);
  }
  // No dispatch is happening, and user signaled that a dispatch is wanted.
  else if (sendDispatchSignal)
  {

    // Signal to Modbus client that a dispatch is wanted.
    mb.Coil(DISPATCH_COIL, true);

    // Toggle Green LED so long as a dispatch is wanted, but not acknowledged.
    unsigned long currentTime = millis();
    if (currentTime - lastGreenFlash > GREEN_FLASH_RATE)
    {
      lastGreenFlash = currentTime;
      digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN));
    }
  }

  // No dispatch is underway, no dispatch is being requested, turn LED off.
  if (!mb.Coil(IS_DISPATCHING_COIL) && !mb.Coil(DISPATCH_COIL))
  {
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  // If Modbus client set ALERT state to high, set error state.
  if (mb.Coil(ALERT_COIL))
  {
    digitalWrite(RED_LED_PIN, HIGH);
  }
  else
  {
    digitalWrite(RED_LED_PIN, LOW);
  }
}

bool isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void connect() {
  WiFi.hostname("minibus");
  WiFi.begin(SSID, PASSWORD);
  while (!isConnected()) {
    digitalWrite(BLUE_LED_PIN, !digitalRead(BLUE_LED_PIN));
    delay(1000);
  }
  mb.begin();
  mb.addCoil(ALERT_COIL);
  mb.addCoil(DISPATCH_COIL);
  mb.addCoil(IS_DISPATCHING_COIL);
  digitalWrite(BLUE_LED_PIN, HIGH);

}
