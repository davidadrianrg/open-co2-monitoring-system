# open-co2-monitoring-system
CO2 monitoring system using ESP8266 as microcontroller and MH-Z19C as CO2 sensor, the data collected will be send to the cloud through MQTT protocol. In the cloud server the data will be managed by Node-Red, stored in an InfluxDB database and plot using Grafana service.

## Installation

### Flashing the microcontroller
Clone the repository using the following command.

```bash
git clone https://github.com/davidadrianrg/open-co2-monitoring-system.git
```
Create your secret `keys.h` file inside the folder `co2-sensor`, add your mqtt address and credentials in it like this.

```c++
//Configuration secret variables for the CO2 sensor
char brokerIP[15] = "ADD YOUR BROKER IP";
int brokerPort = 1883; //Or set another different than the default one
char mqtt_username[20] = "ADD YOUR MQTT USERNAME";
char mqtt_password[16] = "ADD YOUR MQTT PASSWORD";
```

Flash your ESP8266 microcontroller using Arduino IDE with the `co2-sensor.ino` file.

### Cloud services deployment

In your cloud instance, first install docker if you do not have installed it yet.

```bash
sudo apt update && apt install docker
```

Create a `.env` file to add the environment variables to config the containers described in `docker-compose.yaml`. Add the following to the file.

```.env
GF_PATHS_DATA=/var/lib/grafana
GF_PATHS_LOGS=/var/log/grafana
GF_SECURITY_ADMIN_USER=YOUR_GRAFANA_USER
GF_SECURITY_ADMIN_PASSWORD=YOUR_GRAFANA_PASSWORD

INFLUXDB_USERNAME=YOUR_INFLUXDB_USER
INFLUXDB_PASSWORD=YOUR_INFLUXDB_PASSWORD

MOSQUITTO_USERNAME=YOUR_MOSQUITTO_USER
MOSQUITTO_PASSWORD=YOUR_MOSQUITTO_PASSWORD
```

Create and start the containers with the services (Mosquitto, Node-Red, InfluxDB and Grafana). In the main folder of the project type the following.

```bash
sudo docker-compose up -d
```

Enjoy your IoT system!