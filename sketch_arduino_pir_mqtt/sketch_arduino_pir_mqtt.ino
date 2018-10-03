#include <Ethernet.h>
#include <PubSubClient.h>
#include "PropertiesReader.h"
#include "config.h"

int bewegungsstatus = 0; //Das Wort „bewegungsstatus“ steht jetzt zunächst für den Wert 0. Später wird unter dieser Variable gespeichert, ob eine Bewegung erkannt wird oder nicht.

EthernetClient ethClient;
PubSubClient mqttClient;
String mqttTopicStatus;
String mqttTopicMotion;

void (*resetFunc)(void) = 0; //declare reset function at address 0

void setup()
{
  // put your setup code here, to run once:
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_PIR, INPUT); //Der Pin mit dem Bewegungsmelder (Pin 7) ist jetzt ein Eingang.

  Serial.begin(9600);

  Serial.println(F("MQTT Ethernet PIR Sensor"));
  Serial.println();

  // read configs
  readConfigFile("settings.txt"); //FILE NAME
  String mqttClientName = String(requestSetting("mqttClientName"));
  String mqttUser = String(requestSetting("mqttUser"));
  String mqttPassword = String(requestSetting("mqttPassword"));
  String mqttServer = String(requestSetting("mqttServer"));
  String mqttTopic = String(requestSetting("mqttTopic")); 
  mqttTopicStatus = mqttTopic + "status";
  mqttTopicMotion = mqttTopic + "motion";

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
    else if (Ethernet.linkStatus() == LinkOFF)
    {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true)
    {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

  // setup mqtt client
  mqttClient.setClient(ethClient);
  mqttClient.setServer(mqttServer.c_str(), mqttPort);
  mqttClient.setCallback(callback);

  while (!mqttClient.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (mqttClient.connect(mqttClientName.c_str(), mqttUser.c_str(), mqttPassword.c_str()))
    {
      Serial.println("connected");
      mqttClient.publish(mqttTopicStatus.c_str(), "Online");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void loop()
{
  if (digitalRead(PIN_PIR) != bewegungsstatus)
  {
    // Aenderung erkannt
    Serial.println("bewegungstatus changed.");
    bewegungsstatus = digitalRead(PIN_PIR); //ier wird der Pin7 ausgelesen. Das Ergebnis wird unter der Variablen „bewegungsstatus“ mit dem Wert „HIGH“ für 5Volt oder „LOW“ für 0Volt gespeichert.
    if (bewegungsstatus == HIGH)             //Verarbeitung: Wenn eine Bewegung detektiert wird (Das Spannungssignal ist hoch)
    {                                        //Programmabschnitt des IF-Befehls öffnen.
      digitalWrite(LED_BUILTIN, HIGH);       //dann soll der Piezo piepsen.
      Serial.println("Motion ON");
      if (!mqttClient.publish(mqttTopicMotion.c_str(), "ON"))
      {
        Serial.println('Publish failed');
      };
    }                                 //Programmabschnitt des IF-Befehls schließen.
    else                              //ansonsten...
    {                                 //Programmabschnitt des else-Befehls öffnen.
      digitalWrite(LED_BUILTIN, LOW); //...soll der Piezo-Lautsprecher aus sein.
      Serial.println("Motion OFF");
      mqttClient.publish(mqttTopicMotion.c_str(), "OFF");
    }
  }
  if (!mqttClient.loop())
  {
    Serial.println("Connection lost -> reset");
    resetFunc(); //call reset
  };
  delay(25);
}

void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  char digiValue[length + 1];
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    digiValue[i] = (char)payload[i];
    //Serial.print(digiValue[i]);
  }
  digiValue[length] = '\0';
  Serial.println();
  Serial.print("Length " + String(length));
  Serial.println();
  Serial.println("-----------------------");
}
