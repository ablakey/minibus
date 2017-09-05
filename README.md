# Minibus

A Modbus client on an ESP8266.

![Minibus Hardware](./minibus_hardware.png?raw=true)

Minibus has two capabilities:

1. Red LED if COIL 100 is high (an alert)
2. Push button to signal a job to be dispatched. Green LED flashes while job is pending. Green LED is solid when job is in progress. Green LED goes off when job is done.

Blue LED will flash as system starts up and connects. Goes solid when system is running and successfully connected.
