/*
Open CO2 Monitoring System
Air Quality Monitoring System for rooms with limited ventilation
ESP8266 MCU and MH-Z19C sensor
MQTT CO2 ppm data publishing

Author: David Adrián Rodríguez García
*/

#include <ESP8266WiFi.h>    //Library to connect WiFi
#include <WiFiManager.h>    //Library to make an AP to introduce WiFi credentials
#include <MHZ19.h>          //Library to manage CO2 sensor
#include <SoftwareSerial.h> //Library to use another pins than RX and TX to UART communication
#include <PubSubClient.h>   //Library to manage MQTT connection to the broker

#define RX_PIN 7
#define TX_PIN 6
#define brokerIP "X.X.X.X" //MQTT Broker IP
#define brokerPort 1883    //MQTT Broker Port
#define clientId "Smart-CO2-Sensor"
#define mqtt_topic "mqtt"                        //This is the [root topic]
#define co2Topic "co2sensor/co2"                 //MQTT topic to publish CO2 ppm values
#define temperatureTopic "co2sensor/temperature" //MQTT topic to publish temperature values

MHZ19 myMHZ19;                             // Constructor for MHZ19 library
SoftwareSerial softSerial(RX_PIN, TX_PIN); // Create a software serial instead of using hardware serial
WiFiClient espClient;                      //Intializing WiFi Client
PubSubClient client(espClient);            //Intializing MQTT client

//Counter to take into account the time delay between different CO2 acquisition values
unsigned long timer = 0;

//Function to setup sensor
void setupSensor()
{
    //Start serial communication
    softSerial.begin(9600);
    //Start the comunication with the sensor
    myMHZ19.begin(softSerial);
    // Turn auto calibration ON (OFF autoCalibration(false))
    myMHZ19.autoCalibration();
}

void setupWiFi()
{
    //Start Wifi communication setup
    WiFi.mode(WIFI_STA);

    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;

    //Reset settings - wipe credentials for testing
    //wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ("Smart-CO2-Sensor"),
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
    res = wm.autoConnect("Smart-CO2-Sensor"); // AP without password
    //res = wm.autoConnect("AutoConnectAP", "password"); // Password protected AP

    if (!res)
    {
        Serial.println("Failed to connect");
        ESP.restart(); //Restarting the esp8266 in order to try again WiFi matching
    }
    else
    {
        //WiFi Connected to the microcontroller
        Serial.println("Connected to the WiFi");
    }
}

void reconnect()
{
    // Loop until device is connected to the broker
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(clientId))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(mqtt_topic, "connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.print(" wifi=");
            Serial.print(WiFi.status());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void callback(char *topic, byte *payload, unsigned int length)
{
    //MQTT Callback, pending to implement
}

void setup()
{
    Serial.begin(115200);

    //Setting up the sensor communication
    setupSensor();

    //Setting up WiFi communication
    setupWiFi();

    //Setting up MQTT connection with the broker
    client.setServer(brokerIP, 1883);
    client.setCallback(callback);
}

void loop()
{
    //Confirm if is still connected to mqtt broker
    if (!client.connected())
    {
        reconnect();
    }
    // Take a new CO2 value each 2 seconds
    if (millis() - timer >= 2000)
    {

        // Get the CO2 ppm values from the sensor
        int ppmCO2 = myMHZ19.getCO2();

        // Print the CO2 level on serial monitor
        Serial.print("CO2 (ppm): ");
        Serial.println(ppmCO2);

        // Take current temperature value
        int temperature = myMHZ19.getTemperature();

        // Print temperature on serial monitor
        Serial.print("Temperature (C): ");
        Serial.println(temperature);

        //Publishing CO2 and Temperature values to the broker
        client.publish(co2Topic, (char *)ppmCO2);
        client.publish(temperatureTopic, (char *)temperature);

        // Updating time value to control acquisition time loop
        timer = millis();
    }
}
