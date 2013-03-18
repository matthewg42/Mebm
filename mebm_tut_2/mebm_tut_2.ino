#include <SPI.h>
#include <Ethernet.h>
#include <Mebm.h> 
#include <stdlib.h>

byte      mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x59, 0x20 };
IPAddress thisNode(192, 168, 1, 101);
int       ovenTemperature = 200;

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac, thisNode);
    Mebm.begin( "kitchen", 3 );
    Mebm.addResponder("*",  "turnOn",      turnOvenOn);
    Mebm.addResponder("*",  "turnOff",     turnOvenOff);
    Mebm.addResponder("*",  "setOvenTemp", setOvenTemperature);
    Serial.println("setup() done");
}

void loop() {
    delay(100);
    Mebm.listen();
}

void turnOvenOn(MebmClass &node, const t_mebmMessage *message)
{
    Serial.print("Click! The oven is now on at ");   
    Serial.print(ovenTemperature);
    Serial.println(" C");
    
}

void turnOvenOff(MebmClass &node, const t_mebmMessage *message)
{
    Serial.println("Click! The oven is now off");   
}

void setOvenTemperature(MebmClass &node, const t_mebmMessage *message)
{
    ovenTemperature = atoi(message->msgData);
    Serial.print("The oven temperature is now ");   
    Serial.print(ovenTemperature);
    Serial.println(" C");
}


