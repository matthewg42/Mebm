#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Mebm.h>

// configure ethernet for this shield's mac address and a statically allocated IP address
byte         myMac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x15, 0xAD };
IPAddress    myIP(192,168,1,100);

// we're going to talk to the control node, whose IP address we're hard coding.
IPAddress    controlNode(192,168,1,1);

// This is going to be used so we only send a message
int sendCounter = 0;

void setup() 
{
    Serial.begin(9600);
    Ethernet.begin(myMac, myIP);
    Mebm.begin("FrontDoor", 1);
    Mebm.addResponder("ControlNode", "OpenDoor", openDoorCallback);
    Serial.println("setup() complete");
}

void loop()
{
    if (sendCounter++ % 50 == 0) {
        Mebm.sendToIP(controlNode, "Heartbeat", "poink");
    }
    delay(50);
    Mebm.listen();  
}

void openDoorCallback(MebmClass& node, const t_mebmMessage* mes)
{
    Serial.println("BUZZZZZZZZZZZZZZZZZZZ");
    Mebm.sendToIP(controlNode, "Ack", "Door opened");
}

