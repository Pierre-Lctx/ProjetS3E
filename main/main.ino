#include <ChainableLED.h>
#include <Arduino.h>

//Déclaration des variables du programme

int buttonPort = 3;

unsigned long beginTouch;

#define NUM_LED 5

ChainableLED leds(4 ,5, NUM_LED);

void setup()
{

    init();

}

void loop()
{

    getData();

}

void getData()
{

}

void initInterrupt()
{
    attachInterrupt(digitalPinToInterrupt(buttonPort), interruption, CHANGE);
}

void interruption()
{

    if (digitalRead(buttonPort))
    {
        beginTouch = millis();
    }
    else
    {
        leds.setColorRGB(0, 100, 0, 0);
    }

}

void init()
{

    leds.init();

    pinMode(buttonPort, INPUT);

    initInterrupt();

}

void changeLEDColor(int portLED)
{

}

