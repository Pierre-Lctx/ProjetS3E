#include <ChainableLED.h>
#include <Arduino.h>

//Déclaration des variables du programme

int greenButtonPort = 3;
int redButtonPort = 4;


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

unsigned long currentMillis;
unsigned long previousMillis = 0;

ChainableLED leds(7 ,8, 1);

void initialisation()
{
    Serial.begin(9600);
    pinMode(redButtonPort, INPUT);
    pinMode(greenButtonPort, INPUT);
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
}

void modeMaintenance()
{
    //Allumage de la LED orange
    leds.setColorRGB(0, 255, 75, 0);
}

void modeEconomique()
{
    //Allumage de la LED bleue
    leds.setColorRGB(0, 0, 170, 255);
}

void modeConfiguration()
{
    //Allumage de la LED jaune
    leds.setColorRGB(0, 255, 255, 0);
}

void selectionMode(bool redButtonValue, bool greenButtonValue)
{
    Serial.println("Selection en cours !");
    Serial.print("Red button value : ");
    Serial.println(redButtonValue);
    Serial.print("Green Button Value : ");
    Serial.println(greenButtonValue);
    if (redButtonValue)
    {
        //Si nous venons d'activer le dispositif et que nous appuyons sur le bouton rouge, nous passons en mode configuration
        if (mode == 0)
        {
            mode = 4;
            modeConfiguration();
            Serial.println("Mode configuration !");
        }

        //Si nous sommes en mode standard et que nous appuyons sur le bouton rouge, nous passons en mode maintenance
        if (mode == 1)
        {
            mode = 2;
            lastMode = 1;
            modeMaintenance();
            Serial.println("Mode maintenance !");
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

            }
            if (lastMode == 3)
            {
                mode = 3;
                lastMode = 2;
                modeEconomique();
                Serial.println("Mode économique !");
            }
        }

        //Si nous sommes en mode économique et que le bouton rouge est pressé, nous passons en mode maintenance
        if (mode == 3)
        {
            mode = 2;
            modeMaintenance();
            Serial.println("Mode maintenance !");
        }
    }

    if (greenButtonValue)
    {
        Serial.println("Green button pressed !");
        //Si nous venons d'activer le dispositif et que nous appuyons sur le bouton vert, nous passons en mode standard
        if (mode == 0)
        {
            mode = 1;
            modeStandard();
            Serial.println("Mode standard !");
        }

        //Si nous sommes en mode standard et que le bouton vert est pressé, nous passons en mode économique
        if (mode == 1)
        {
            mode = 3;
            lastMode = 1;
            modeEconomique();
            Serial.println("Mode économique !");
        }

        //Si nous sommes en mode économique et que le bouton vert est pressé, nous passons en mode standard
        if (mode == 3)
        {
            mode = 1;
            lastMode = 3;
            modeStandard();
            Serial.println("Mode standard !");
        }
    }

    if (!redButtonValue && !greenButtonValue)
    {
        leds.setColorRGB(0, 255, 255, 255);
    }
}

void setup()
{
    initialisation();
}

void loop()
{

    int stateGreenButton = digitalRead(greenButtonPort);
    int stateRedButton = digitalRead(redButtonPort);
/*
    if (!stateRedButton)
    {
        Serial.println("Green button value : ");
        Serial.print(stateGreenButton);
        
    }
    
    if (!stateGreenButton)
    {
        Serial.println("Red button value : ");
        Serial.print(stateRedButton);
    }
        */
}
