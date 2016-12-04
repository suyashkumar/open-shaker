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

#include <Arduino.h>
#include <PubSubClient.h>
#include <map>

typedef int (*handler)();
void removeSpace(char* s);


class HomeAuto {
private:
  PubSubClient* _client; // The mqtt client
  const char* _mqtt_server;
  const char* _name;
public:
  HomeAuto(const char* name, const char* server);
  HomeAuto& setClient(PubSubClient& client);
  void addHandler(const char* name, handler f);
  void callHandler(const char* name);
  void handle();
  void reconnect();
  void msgCallback(char* topic, byte* payload, unsigned int length);
  void publishMessage(const char* message);
  void publishData(const char* message, const char* dataStream);
};
