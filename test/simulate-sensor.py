import paho.mqtt.client as mqtt
from random import gauss
from time import time
import json
import os

def mqtt_setup(mqtt_config,topics_dict):

    # Setting up mqtt connection
    try:
        global client
        client = mqtt.Client("simulate-sensor")
        client.username_pw_set(mqtt_config["user"], mqtt_config["password"])
        client.connect(mqtt_config["broker"], mqtt_config["port"])
        print("I'm connected to the mqtt broker")
    except Exception:
        print("Sorry, I could not connect to the mqtt broker")

    # Setting up the callback function to the client  
    client.on_message = on_message

    # Starting to listen from the broker
    client.loop_start()

# Defining callback function
def on_message(client, userdata, message):
    pass

def simulate_values():
    #Simulate CO2 random value between 400 and 1000 ppm
    co2_ppm = gauss(600,100)
    #Simulate temperature random values between 20 and 30 ºC
    temperature = gauss(25,2)
    return co2_ppm, temperature

if __name__ == '__main__':

    # Storing topics into a dictionary
    topics_dict = {
        "co2" : "co2sensor/co2",
        "temperature" : "co2sensor/temperature"
    }

    # Reading mqtt config from config file
    directory = os.path.dirname(os.path.abspath(__file__))
    with open(f"{directory}/config.json") as file:
        mqtt_config = json.loads(file.read())
    
    # Setting up connection with the broker
    mqtt_setup(mqtt_config,topics_dict)

    # Simulate CO2 and temperature acquisition values with a gaussian distribution
    timer = 0
    try:
        while True:
            if(time()- timer >= 2):
                co2_ppm, temperature = simulate_values()
                print(f'CO2(ppm): {co2_ppm:.2f}, Tª(ºC): {temperature:.2f}')
                # Publishing data to mqtt broker
                client.publish(topics_dict["co2"], co2_ppm)
                client.publish(topics_dict["temperature"], temperature)
                # Updating timer value
                timer = time()
    except KeyboardInterrupt:
        print("\nSimulation aborted")
