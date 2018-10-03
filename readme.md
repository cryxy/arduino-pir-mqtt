# Arduino-Pir-Mqtt
Arduino sketch for Arduino Mega with Ethernet Shield and PIR sensor. Motion detected by the PIR is published via Serial and MQTT.

## Configuration
The configuration is done within the `config.h` (PIN,MAC) and the `settings.txt` (MQTT-Server, credentials and topics). The `settings.txt` needs the be placed in the root folder of a SD-card within the Ethernet Shield.