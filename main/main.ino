#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
//#include "DS1307.h"

//Déclaration des variables du programme

const int redButtonPort = 2;
const int greenButtonPort = 3;

/*

    Fonctionnement de la variable mode :
    - Mode standard : 1
    - Mode maintenance : 2
    - Mode économique : 3
    - Mode configuration : 4

    Par défaut, au début on initialise le mode à 0.
    L'utilisateur doit appuyer pour se mettre en mode configuration, si il n'appuie pas il se met en mode standard.
    Il à 5 secondes pour choisir, une simple pression suffit au début.

*/
int mode = 0;
int lastMode = 0;

bool checkGetData = false;

unsigned long currentMillis;
unsigned long previousMillis = 0;

ChainableLED leds(7 ,8, 1);

//DS1307 clock;

void initialisation()
{
    Serial.begin(9600);

    pinMode(redButtonPort, INPUT);
    pinMode(greenButtonPort, INPUT);
    pinMode(A1, INPUT); //Initialisation du capteur de luminosité

    analogWrite(A1, LOW);
/*
    clock.begin();
    clock.fillByYMD(2021, 10, 17); // On initialise le jour de départ de la clock
    clock.fillByHMS(18,11,00); // On initialise l'heure de départ
    clock.fillDayOfWeek(SUN); //On entre le nom du jour de départ
    clock.setTime(); //Ecriture de la date sur l'horloge RTC
*/
    initInterrupt();
}


void initInterrupt()
{
    attachInterrupt(digitalPinToInterrupt(redButtonPort), interruptionRed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(greenButtonPort), interruptionGreen, CHANGE);
}

void interruptionRed()
{
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

void modeStandard()
{
    //Allumage de la LED verte
    leds.setColorRGB(0, 0, 255, 0);

    checkGetData = true;
}

void modeMaintenance()
{
    //Allumage de la LED orange
    leds.setColorRGB(0, 255, 75, 0);

    checkGetData = false;
}

void modeEconomique()
{
    //Allumage de la LED bleue
    leds.setColorRGB(0, 0, 170, 255);

    checkGetData = true;
}

void modeConfiguration()
{
    //Allumage de la LED jaune
    leds.setColorRGB(0, 255, 255, 0);

    checkGetData = false;
}

int selectionMode(bool redButtonValue, bool greenButtonValue)
{
    Serial.println("Selection en cours !");
    if (redButtonValue)
    {
        Serial.println("Red button pressed !");
        //Si nous venons d'activer le dispositif et que nous appuyons sur le bouton rouge, nous passons en mode configuration
        if (mode == 0)
        {
            mode = 4;
            modeConfiguration();
            Serial.println("Mode configuration !");
            return 0;
        }

        //Si nous sommes en mode standard et que nous appuyons sur le bouton rouge, nous passons en mode maintenance
        if (mode == 1)
        {
            mode = 2;
            lastMode = 1;
            modeMaintenance();
            Serial.println("Mode maintenance !");
            return 0;
        }

        //Si nous sommes en mode maintenance et que nous appuyons sur le bouton rouge
        //Si le mode précédent était le mode économique, alors nous repassons en mode économique
        //Si le mode précédent était le mode était le mode standard, alors nous repassons en mode standard
        if (mode == 2)
        {
            if (lastMode == 1)
            {
                mode = 1;
                lastMode = 2;
                modeStandard();
                Serial.println("Mode standard !");
                return 0;
            }
            if (lastMode == 3)
            {
                mode = 3;
                lastMode = 2;
                modeEconomique();
                Serial.println("Mode économique !");
                return 0;
            }
        }

        //Si nous sommes en mode économique et que le bouton rouge est pressé, nous passons en mode maintenance
        if (mode == 3)
        {
            mode = 2;
            modeMaintenance();
            Serial.println("Mode maintenance !");
            return 0;
        }
    }

    if (greenButtonValue)
    {
        Serial.println("Green button pressed !");
        Serial.print("Mode = ");
        Serial.print(mode);
        //Si nous venons d'activer le dispositif et que nous appuyons sur le bouton vert, nous passons en mode standard
        if (mode == 0)
        {
            mode = 1;
            modeStandard();
            Serial.println("Mode standard !");
            return 0;
        }

        //Si nous sommes en mode standard et que le bouton vert est pressé, nous passons en mode économique
        if (mode == 1)
        {
            mode = 3;
            lastMode = 1;
            modeEconomique();
            Serial.println("Mode économique !");
            return 0;
        }

        //Si nous sommes en mode économique et que le bouton vert est pressé, nous passons en mode standard
        if (mode == 3)
        {
            mode = 1;
            lastMode = 3;
            modeStandard();
            Serial.println("Mode standard !");
            return 0;
        }
    }

    if (redButtonValue && greenButtonValue)
    {
        leds.setColorRGB(0, 255, 255, 255);
        return 0;
    }
}

void getData()
{   
    //Récupération de la luminosité
    //On récupère les données brutes du capteurs

    int light = analogRead(A1); 
    
    //On traite les données ici
    //light = (light/10)^10;

    Serial.println(light);
    //Serial.println(" lux.");

}

void setup()
{
    initialisation();
}

void loop()
{
    while (checkGetData)
    {
        getData();
    }
}
