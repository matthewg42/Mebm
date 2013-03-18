#include <Ethernet.h>
#include <SPI.h>
#include <Mebm.h>
#include <TickTimer.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x59, 0x20 };
IPAddress greenhouseIP(192, 168, 1, 101); 
IPAddress houseIP(192, 168, 1, 1);
int ticks = 0;
TickTimer sprinklerTimer(turnOffSprinkler);
bool heaterIsOn = false;
bool sprinklerIsOn = false;
int greenhouseTemperature = 15;
int soilHumidity = 75;

void setup()
{
    Serial.begin(9600);
    Ethernet.begin(mac, greenhouseIP);
    Mebm.begin("greenhouse", 3);     // second parameter = 3 responders

    // Add responders for the various incoming messages
    Mebm.addResponder("house", "Heater", handleHeaterMessage);
    Mebm.addResponder("house", "Water",  handleWaterMessage);
    Mebm.addResponder("house", "Status", handleStatusRequest);

    // Delays in setup() are often added to let the power supply stabailize
    // This is done to prevent erroneous sensor readings
    delay(500);

    Serial.println("setup() done, system running");
}

void loop()
{
    // wait one second
    delay(1000);
    ticks++;

    // once every 30 seconds, we'll simulate a variation in temperature and humidity
    if (ticks % 30 == 0)
        simulateEnvironment(); 
        
    // once every minute send the temperature
    if (ticks % 60 == 0)
        Mebm.sendToIP(houseIP, "temp", getTemperature());

    // once every five minutes send the humidity
    if (ticks % (60*5) == 0)
        Mebm.sendToIP(houseIP, "humidity", getHumidity());

    // safeguards so we don't make the greenhouse TOO hot or wet
    if (getTemperature() > 25) 
        turnOffHeater();
        
    if (getHumidity() > 90)
        turnOffSprinkler();
        
    // process incoming messages
    Mebm.listen();
    
    // check the sprinkler timer
    sprinklerTimer.tick();

}

void handleHeaterMessage(MebmClass &node, const t_mebmMessage *message)
{
    if (strncasecmp(message->msgData, "On", 48) == 0)
        turnOnHeater();
    else if (strncasecmp(message->msgData, "Off", 48) == 0)
        turnOffHeater();
    else {
        Serial.print("handleHeaterMessage: message->msgData invalid: ");
        Serial.println(message->msgData);
    }
}

void handleWaterMessage(MebmClass &node, const t_mebmMessage *message)
{
    int duration = atoi(message->msgData);
    turnOnSprinkler();
    sprinklerTimer.set(60*duration);
}

void handleStatusRequest(MebmClass &node, const t_mebmMessage *message)
{
    Serial.println("handleStatusRequest()");
    char desc[48];
    memset(desc, 0, 48);
    snprintf(desc, 48, "temp=%d, humidity=%d, heater=%d, sprinkler=%d", 
             getTemperature(), 
             getHumidity(), 
             heaterIsOn, 
             sprinklerIsOn);
             
    Mebm.sendToIP(houseIP, "status", desc);
}

void simulateEnvironment()
{
    // add -3, -2, -1, 0 or 1 to the temperature
    // it is biased downwards so the heater will become necessary from time to time...
    greenhouseTemperature += random(-3, 2);

    // if the heater is on, we will increase the temperature
    if (heaterIsOn)
        greenhouseTemperature += 3;

    // and we'll adjust the soil humidity depending on the sprinkler status
    if (sprinklerIsOn)
        soilHumidity += 5;
    else
        soilHumidity -= 1;
}

// In a real implementation, these functions would probaby use digitalWrite to
// set a pin which would trigger a relay to turn on and off the actual heater
// In our example, we just set heaterIsOn / sprinklerIsOn to simuate this, but 
// it would be a trivial matter to actually control a device instead.
void turnOnHeater()
{
    heaterIsOn = true;
    Serial.println("heater on");
}

void turnOffHeater()
{
    heaterIsOn = false;
    Serial.println("heater off");
}

void turnOnSprinkler()
{
    sprinklerIsOn = true;
    Serial.println("sprinkler on");
}

void turnOffSprinkler()
{
    sprinklerIsOn = false;
    Serial.println("sprinkler off");
}

int getTemperature()
{
    return greenhouseTemperature;
}

int getHumidity()
{
    return soilHumidity;
}

