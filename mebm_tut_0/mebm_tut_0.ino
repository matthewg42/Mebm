#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x59, 0x20 };
IPAddress poeIP ( 192,168,1,101 ) ; 

void setup() {
    Ethernet.begin(mac, poeIP);
}

void loop() {
}
    
