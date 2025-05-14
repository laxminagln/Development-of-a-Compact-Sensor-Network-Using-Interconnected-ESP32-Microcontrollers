# Development-of-a-Compact-Sensor-Network-Using-Interconnected-ESP32-Microcontrollers

The project will provide an IoT-enabled Environmental Monitoring System using ESP32 microcontrollers interconnected with each other. This system solves a big problem in continuous temperature and humidity monitoring using a scalable and energy-efficient architecture. The ambient data shall be collected through sensors, being locally processed and visualized with the Node-Red dashboard. This makes it very versatile and user-friendly since the user can set key parameters such as sampling intervals, communication intervals, and alert thresholds from the dashboard. The system also focuses on energy conservation by implementing deep sleep modes and ESP-Now communication to reduce power consumption efficiently.

The diagram depicts the connections for an IoT-based temperature and humidity monitoring system that employs ESP32 microcontrollers. Two slave nodes, each with an ESP32 and a DHT11 sensor, are powered by rechargeable batteries. The DHT11 sensors are linked to the ESP32’s GPIO pins to measure temperature and humidity, and their data ports are wired appropriately. These slave nodes use the ESP-Now protocol to connect wirelessly with the master node. The master node, which is also an ESP32, is powered by a laptop via USB and functions as the central hub, sending data from the slave nodes to a Node-Red dashboard or MQTT broker. This system exemplifies an efficient, energy-conscious, and scalable architecture for environmental monitoring.

Master ESP32 Connections:
Communication:
• Uses ESP-NOW for wireless communication with Slave 1 and Slave 2
• MAC addresses of Slave 1 and Slave 2 are configured in the code to establish the wireless connection.
Slave 1 ESP32 Connections
Communication:
• Sends temperature, humidity, and node ID (Node ID = 1) to the Master via ESPNOW
• Receives configuration updates (sampling and communication intervals) from the Master.
Slave 2 ESP32 Connections
Communication:
• Sends temperature, humidity, and node ID (Node ID = 2) to the Master via ESPNOW.
• Receives configuration updates (sampling and communication intervals) from the Master.
Software Design

The software architecture is modular, with a focus on segregating roles to ensure optimal functionality. The master node gets data from slave nodes using ESP-Now and sends it to a MQTT broker. It also sends configuration updates, like sample intervals and alert thresholds, to slave nodes. Slave nodes regularly awaken from deep slumber to collect data, transfer it to the master node, and then go back to sleep. This minimizes energy use. On the user side, the Node-Red dashboard displays real-time statistics, offers configurable choices, and sends warnings when thresholds are exceeded.

The system architecture will involve many ESP32 microcontrollers, each with an attached DHT11 sensor, communicating over a Node-RED application. The simplified view of the working is as follows:

### ESP32 Master and Slaves:
The setup includes one master ESP32 and two slave ESP32 devices. Each slave relates to a DHT11 sensor, which measures temperature and humidity. Both slave devices will collect the temperature and humidity data from their respective sensors, which are then sent to the master via ESP-NOW communication. This is a wireless protocol that allows the ESP32 devices to communicate with one another through their MAC addresses.

### Master ESP32:
The data from sensors is fetched from slaves by the master. Collected data publishes information for higher-order processing using MQTT-Message Queuing Telemetry Transport to a cloud or local server. MQTT is one of the lightest weight protocols, ideal in implementing IoT applications.

### Node-RED App:
The Node-RED application subscribes to the MQTT topics to receive the data on temperature and humidity coming from the ESP32 Master. In turn, it processes the data and displays it in a dashboard form. The Dashboard is interactive, thus enabling users to set Temperature and humidity for alert thresholding. Sampling intervals are time intervals that define how often data is gathered and sent. An alert is generated to the user if either temperature or humidity reading crosses the set threshold by the user.

This Node-Red diagram shown in figure 3 is a process for monitoring and controlling data from three temperature nodes and three humidity nodes, with an integrated alert and visualization system. Each pipeline begins with an input node that receives real-time data from a single ESP32-based sensor node. The data is passed to a “Chart” node to depict the temperature or humidity measurements, and then to a comparison function, which compares the results to user-defined thresholds. If a threshold is exceeded, a notification or alert message is generated and delivered to the user via a notification block. These workflows provide data visualization, automated monitoring, and real-time anomaly notifications, resulting in an efficient and user-friendly environmental monitoring system.

The Node-Red workflow collects and handles real-time data from various temperature and humidity sensor nodes, using three pipelines for temperature readings and three for humidity as shown in figure 4. Each pipeline processes data from a single sensor node, beginning with an input node that accepts raw sensor data supplied via ESP32 slave nodes. The data is first routed through “Message Output” nodes before being processed by “Payload” nodes, which clean, format, and transform the data into a useful structure. Finally, the processed data is routed to “Payload Output” nodes, which send it to dashboards, databases, or MQTT topics for real-time monitoring and analysis. This modular design ensures scalability by allowing for the easy addition of new nodes, as well as the freedom to integrate advanced processing or alerting systems.

### User Interaction:
It enables the user to interact with the system through a Node-RED dashboard, where they can view real-time and historical data in graphical form, besides setting parameters such as communication intervals and thresholds for sending alerts. Summary: The design involves an automated multipurpose system developed using interfaced ESP32s and several DHT11 sensors. In controlling or monitoring temperature and humidity displays, the system was used for wireless data from sensors downlinked by slaves onto the main board (master) ESP32 on, finally, a Node RED server. Users can remotely inspect through Node-RED Dashboard condition-based parameters via triggers on predefined situations.

### System Flow Description

The system flow explains how data and communication occur in this architecture between different components. This is explained step by step as follows:

Collecting Data by slave ESP32: Each ESP32 Slave is interfaced with the DHT11 sensor for the measurement of temperature and humidity in real time. Sensors measure data-temperature and humidity values-and send it to their respective ESP32 Slave devices.
Radio Transmission to ESP32 Master: The ESP-NOW wireless communication protocol is used to send sensor data back to the ESP32 Master from the slave devices. As the name goes, ESP-NOW uses unique MAC addresses of the devices and sets up a peer-to-peer communication link, ensuring that the data transfer is efficient and fast.
ESP32 Master processes the data and publishes it: Data is received in both the slave devices from the ESP32 master. It first prepares the data to be sent and then sends it over the MQTT protocol to an MQTT broker. Besides sensor values, the master delivers configuration information about communication settings (such as sampling intervals).
Handling Node-RED Application Data The Node-RED application subscribes to the corresponding MQTT topics that the ESP32 Master sends the temperature and humidity data to. Node-RED then processes the incoming data and performs the following:
I. Displays real-time data on a dashboard.
II. The data is then checked against the user-defined threshold of temperature and humidity.
III. Triggers alerts if the thresholds are exceeded.
User Interaction with Node-RED Dashboard: The user interacts with the system through a Node-RED dashboard where there is a possibility to check real-time and historical data (temperature and humidity charts), threshold values setting (temperature and humidity alerts), set up sampling intervals, which are representative of how frequently data is sent across from the ESP32 Slaves to the Master, and this dashboard will reflect any alert/warning based on defined thresholds.
Alerts and Feedback: Thus, Node-RED will alert users based on higher-ordered temperature or humidity data sets. After such an event takes place, a user might choose to act further or makes some necessary changes in settings through this dashboard.
Testing Process

The system was thoroughly tested to assure reliability and efficiency. Unit tests confirmed the accuracy of DHT11 sensors and the reliability of ESP-Now connectivity. Integration tests ensured that data flowed smoothly between slave nodes, master nodes, MQTT brokers, and the Node-Red dashboard. Performance testing focused on energy usage, assessing the influence of different sampling and communication intervals. For example, testing revealed that using deep sleep modes lowered power consumption by up to 80%. Furthermore, the system’s response time for triggering warnings was consistently less than 2 seconds, matching real-time expectations.

### Instructions for Application and Use

To configure the system, connect the DHT11 sensors to the slave nodes and assign them MAC addresses. Upload the master and slave nodes’ Arduino codes to the ESP32 microcontrollers. The master node communicates to the MQTT broker via Wi-Fi, while the slave nodes communicate via ESP-Now. Once implemented, users can utilize the Node-Red dashboard to view real-time data, define intervals, and set alarm levels. Maintenance consists mostly of refilling the slave nodes’ batteries and verifying sensor calibration.
