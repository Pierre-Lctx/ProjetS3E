#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>
#include "initialisation.c"

//Déclaration des variables du programme



void initInterrupt()
{
    const int redButtonPort = 2;
    const int greenButtonPort = 3;

    int mode = 0;
    int lastMode = 0;

    int valeurModification;

    int FILE_MAX_SIZE = 4096;

    long LOG_INTERVAL = 500;

    bool checkGetData = false;

    unsigned long currentMillis;
    unsigned long previousMillis = 0;

    BME280 capteur;

    ChainableLED leds(7 ,8, 1);

    DS1307 clock;

    attachInterrupt(digitalPinToInterrupt(redButtonPort), interruptionRed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(greenButtonPort), interruptionGreen, CHANGE);
}

void interruptionRed()
{
    const int redButtonPort = 2;
    const int greenButtonPort = 3;

    int mode = 0;
    int lastMode = 0;

    int valeurModification;

    int FILE_MAX_SIZE = 4096;

    long LOG_INTERVAL = 500;

    bool checkGetData = false;

    unsigned long currentMillis;
    unsigned long previousMillis = 0;

    BME280 capteur;

    ChainableLED leds(7 ,8, 1);

    DS1307 clock;

    if (digitalRead(redButtonPort))
    {
        Serial.println("Red interruption !");
        currentMillis = millis();

        if ((unsigned long)(currentMillis - previousMillis) >= 5000)
        {
            selectionMode(digitalRead(redButtonPort), 0);
            previousMillis = currentMillis;
        }
    }
}

void interruptionGreen()
{

    const int redButtonPort = 2;
    const int greenButtonPort = 3;

    int mode = 0;
    int lastMode = 0;

    int valeurModification;

    int FILE_MAX_SIZE = 4096;

    long LOG_INTERVAL = 500;

    bool checkGetData = false;

    unsigned long currentMillis;
    unsigned long previousMillis = 0;

    BME280 capteur;

    ChainableLED leds(7 ,8, 1);

    DS1307 clock;

    if (digitalRead(greenButtonPort))
    {
        Serial.println("Green interruption !");
        currentMillis = millis();

        if ((unsigned long)(currentMillis - previousMillis) >= 5000)
        {
            selectionMode(0, digitalRead(greenButtonPort));
            previousMillis = currentMillis;
        }
    }
}

