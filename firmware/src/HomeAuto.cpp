/*
HomeAuto.cpp
A library that handles ESP8266 communication with a server (even on private
networks). Consumers of this library can simply write functions and have them
be fired whenver the server fires a given event directed at this device. There is
a 1-1 mapping of event to function. For example the "led" event may fire the
ledToggle function on the device. The communication needed to get that event to the
device and decide what funciton to all is abstracted away entirely by this library.

@author: Suyash Kumar <suyashkumar2003@gmail.com>
*/

#include "HomeAuto.h"

typedef struct node {
  handler f;
  const char* name;
  node *next;
} node_t;

node_t* root;
node_t* current;

HomeAuto::HomeAuto(const char* name, const char* server){
  // Set name and server
  this->_name = name;
  this->_mqtt_server = server;

  // Init linked list
  root = (node_t *)malloc(sizeof(node_t));
  root->next=0;
  root->name="ROOT"; // reserved name for root node (for now)
  current = root;
}

void HomeAuto::addHandler(const char* name, handler f){
  node *newNode = (node_t*) malloc(sizeof(node_t));
  newNode->f = f;
  newNode->next=0;
  newNode->name=name;
  current->next=newNode;
  current=newNode;
}

void HomeAuto::callHandler(const char* name){
  node_t* currentInSearch = root;
  while(true){
    if (strcmp(name, currentInSearch->name)==0){
      currentInSearch->f(); // Call function assoc with handler
      break;
    }
    if(currentInSearch->next == 0){
      break;
    }
    currentInSearch = currentInSearch->next;
  }
}

HomeAuto& HomeAuto::setClient(PubSubClient& client){
  this->_client = &client;
  client.setServer(this->_mqtt_server, 1883);
  client.setCallback([&](char* topic, byte* payload, unsigned int length){
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    char payloadStr[length];
    for (int i = 0; i < length; i++) {
      payloadStr[i] = (char)payload[i];
    }
    payloadStr[length] = 0; // Null terminate payloadStr
    removeSpace(payloadStr);
    Serial.println(payloadStr);
    this->callHandler(payloadStr); // Call function assoc with this handler
  });
  return *this;
}

void HomeAuto::handle(){
  if (!this->_client->connected()){
    this->reconnect();
  }
  this->_client->loop();
}

void HomeAuto::reconnect() {
  // Loop until we're reconnected
  while (!this->_client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (this->_client->connect(this->_name)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      this->_client->publish("outTopic", "hello world");
      // Suscribe to topics:
      this->_client->subscribe(this->_name); // suscribe to events meant for this device
    } else {
      Serial.print("failed, rc=");
      Serial.print(this->_client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void HomeAuto::publishMessage(const char* message){
  char str[20];
  strcpy(str, this->_name);
  strcat(str, "/device");
  const char* topicName = str;
  this->_client->publish(topicName, message);
}

void HomeAuto::publishData(const char* message, const char* dataStream) { 
	char topicBuffer[20];
	strcpy(topicBuffer, this->_name);
	strcat(topicBuffer, "/stream/");
	strcat(topicBuffer, dataStream);
	const char* topicName = topicBuffer;
	this->_client->publish(topicName, message);
}

void removeSpace(char* s) {
    for (char* s2 = s; *s2; ++s2) {
        if (*s2 != ' ')
            *s++ = *s2;
    }
    *s = 0;
}
