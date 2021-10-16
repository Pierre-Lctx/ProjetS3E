#include <ChainableLED.h>
#include <Arduino.h>

//Déclaration des variables du programme

int greenButtonPort = 3;
int redButtonPort = 2;

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
int lastMode;

unsigned long currentMillis;
unsigned long previousMillis = 0;

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

            changeLEDMode(mode);

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

            changeLEDMode(mode);

            previousMillis = currentMillis;

        }
        
    }

}

void actionMode(int modeNbr)
{

    switch (modeNbr)
    {
    case 1:
        
        break;

    case 2:

        break;

    case 3:
        
        break;

    case 4:

        break;
    
    default:
        break;
    }

}

void checkStartingPressButton(bool stateButton)
{
    if (stateButton)
    {

        mode = 4;

        changeLEDMode(mode);
        actionMode(mode);

    }
    else 
    {

        mode = 1;

        changeLEDMode(mode);
        actionMode(mode);

    }
}

void checkMode(int modeNbr)
{

    if (modeNbr == 0)
    {
        if (digitalRead(redButtonPort))
        {
            
            changeLEDMode(4);

            actionMode(4);

            lastMode = modeNbr;

        }
        if (digitalRead(greenButtonPort))
        {

            changeLEDMode(1);

            actionMode(1);

            lastMode = modeNbr;

        }
    }

    else 
    {

        if (lastMode == 1)
        {
            if (modeNbr == 2)
            {
                changeLEDMode(modeNbr);

                actionMode(modeNbr);

                lastMode = modeNbr;
            }

            if (modeNbr == 3)
            {
                changeLEDMode(modeNbr);

                actionMode(modeNbr);

                lastMode = modeNbr;
            }
            
        }

        if (lastMode == 2)
        {
            if (modeNbr == 1)
            {

                changeLEDMode(modeNbr);

                actionMode(modeNbr);

                lastMode = modeNbr;

            }
        }

        if (lastMode == 3)
        {

            if ()

        }

    }

}

void changeLEDMode(int nbrMode)
{
    switch (nbrMode)
    {
    case 1:
        //Passage en mode standard
        leds.setColorRGB(0, 100, 100, 50);
        break;
    
    case 2 :
        //Passage en mode maintenance
        leds.setColorRGB(0, 255, 205, 0);
        break;

    case 3:
        //Passage en mode économique
        leds.setColorRGB(0, 0, 170, 255);
        break;
    
    case 4 :
        //Passage en mode configuration
        leds.setColorRGB(0, 60, 100, 50);
        break;
    
    default:
        //On éteint la led
        leds.setColorRGB(0, 0, 0, 0);
        break;
    }
}

int modeStandard(int modeNum)
{


    return modeNum;
}

int modeMaintenance(int modeNum)
{


    return modeNum;
}

int modeEconomique(int modeNum)
{


    return modeNum;
}

int modeConfiguration(int modeNum)
{


    return modeNum;
}

void capteursOFF()
{

    //On desactive les capteurs
    

}

void init()
{

    leds.init();

    pinMode(buttonPort, INPUT);

    initInterrupt();

}

