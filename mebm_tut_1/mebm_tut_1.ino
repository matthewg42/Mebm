#include <SPI.h>
#include <Ethernet.h>
#include <Mebm.h> 
#include <strings.h>

byte      mac[]        = { 0x90, 0xA2, 0xDA, 0x0D, 0x59, 0x20 };
IPAddress thisNode(192, 168, 1, 101);
IPAddress receiverNode(192, 168, 1, 1); 
char      tempString[10];

void setup() {
    Ethernet.begin(mac, thisNode);
    Mebm.begin( "kitchen" );
}

void loop() {
    delay(1000);
    Mebm.sendToIP(receiverNode, "temp", getTemperature());   
}

const char* getTemperature()
{
    sprintf(tempString, "%d C", random(40));
    return tempString;
}



