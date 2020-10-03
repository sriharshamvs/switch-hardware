#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define GPIO_LENGTH     8
#define ANALOG_LENGTH   6
#define TOTAL_PINS_LENGTH 14

//byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };      // Ethernet shield (W5100) MAC address
//IPAddress ip(192, 168, 0, 111);                           // Ethernet shield (W5100) IP address
byte mac[] = { 0x00, 0xAB, 0xBC, 0xCD, 0xDF, 0x03 };      // Ethernet shield (W5100) MAC address
IPAddress ip(192, 168, 0, 112);                           // Ethernet shield (W5100) IP address

IPAddress mqttServer(192, 168, 0, 123);                   // MTTQ server IP address

const int digitalGPIO[] = {2, 3, 4, 5, 6, 7, 8, 9};
const int analogPins[] = {A0, A1, A2, A3, A4, A5};

const char pins[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

EthernetClient ethClient;
PubSubClient client(ethClient);

int checkForPin(char device) {
  for (int i = 0; i < TOTAL_PINS_LENGTH; i++) {
    if (pins[i] == device) {
      return 1;
    }
  }
  return 0;
}

int setPin(char device) {
  int pin;
  switch(device) {
    case '2':
      pin = 2;
      break;
    case '3':
      pin = 3;
      break;
    case '4':
      pin = 4;
      break;
    case '5':
      pin = 5;
      break;
    case '6':
      pin = 6;
      break;
    case '7':
      pin = 7;
      break;
    case '8':
      pin = 8;
      break;
    case '9':
      pin = 9;
      break;
    case 'a':
      pin = A0;
      break;
    case 'b':
      pin = A1;
      break;
    case 'c':
      pin = A2;
      break;
    case 'd':
      pin = A3;
      break;
    case 'e':
      pin = A4;
      break;
    case 'f':
      pin = A5;
      break;   
  }
  return pin;
}

void performAction( int pin, String action) {
  if (action == "1") {
    digitalWrite(pin, HIGH);
    Serial.print("Pin: ");
    Serial.print(pin);
    Serial.println(" OFF");
  }
  if (action == "0") {
    digitalWrite(pin, LOW);
    Serial.print("Pin: ");
    Serial.print(pin);
    Serial.println(" ON");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String receivedChar = "";
  char i = 0;
  for (i = 0; i < length; i++) {
    receivedChar = (char)payload[i];
    Serial.print(receivedChar);
    Serial.println();
  }
  String TOPIC = topic;
  TOPIC.remove(0, 6);
  char device[2];
  TOPIC.toCharArray(device, 2);
  int pinStatus = checkForPin(device[0]);
  if (pinStatus) {
    int pin = setPin(device[0]);
    performAction(pin, receivedChar);
  } else {
    Serial.println("Pin Not Found");
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
//    if (client.connect("SmartHome_1")) {
    if (client.connect("SmartHome_2")) {
      Serial.println("connected");
      for (int i = 0; i < 14; i++) {
//        String topic = "room1/" + String(pins[i]);
        String topic = "room2/" + String(pins[i]);
        char topic_buff[10];
        topic.toCharArray(topic_buff, 10);
        client.subscribe(topic_buff);
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_ethernet() {
  delay(10);
  // We start by connecting to a Network
  Ethernet.begin(mac, ip);
}

void setup() {
  for (int i = 0; i <= GPIO_LENGTH; i++) {
    pinMode(digitalGPIO[i], OUTPUT);
    digitalWrite(digitalGPIO[i], HIGH);
  }
  for (int i = 0; i <= ANALOG_LENGTH; i++) {
    pinMode(analogPins[i], OUTPUT);
    digitalWrite(analogPins[i], HIGH);
  }

  Serial.begin(115200);
  setup_ethernet();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
