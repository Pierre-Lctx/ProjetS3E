/*

    Fonctionnement de la variable mode :
    - Mode standard.........:   1
    - Mode maintenance......:   2
    - Mode économique.......:   3
    - Mode configuration....:   4

    Par défaut, au début on initialise le mode à 0.
    L'utilisateur doit appuyer pour se mettre en mode configuration, si il n'appuie pas il se met en mode standard.
    Il à 5 secondes pour choisir, une simple pression suffit au début.

*/

#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>
#include "functions.c"
#include "initialisation.c"
#include "carteSD.c"


//****************************************************************
//* Nom        :  modeStandard          DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Mise en marche du mode Standard                *
//* Retour     :  Aucun                                          *   
//****************************************************************

void modeStandard() 
{
    //Allumage de la LED verte
    leds.setColorRGB(0, 0, 255, 0);

    checkGetData = true;

    LOG_INTERVAL = 600000;
}

//****************************************************************
//* Nom        :  modeMaintenance       DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Mise en marche du mode Maintenance             *
//* Retour     :  Aucun                                          *   
//****************************************************************

void modeMaintenance()
{
    //Allumage de la LED orange
    leds.setColorRGB(0, 255, 75, 0);

    checkGetData = false;
}

//****************************************************************
//* Nom        :  modeEconomique        DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Mise en marche du mode Economique              *
//* Retour     :  Aucun                                          *   
//****************************************************************

void modeEconomique()
{
    //Allumage de la LED bleue
    leds.setColorRGB(0, 0, 170, 255);

    checkGetData = true;

    LOG_INTERVAL *= 2;
}

//****************************************************************
//* Nom        :  modeConfiguration     DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Mise en marche du mode Configuration           *
//* Retour     :  Aucun                                          *   
//****************************************************************

void modeConfiguration()
{
    //Allumage de la LED jaune
    leds.setColorRGB(0, 255, 255, 0);

    checkGetData = false;
    modificationParametre();

    if (millis() >= 1800000)
    {
        mode = 1;
        lastMode = 3;
        modeStandard();
    }
}

//****************************************************************
//* Nom        :  modificationParametre DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Fonction de modification de paramètres         *
//* Retour     :  Aucun                                          *   
//****************************************************************

void modificationParametre()
{
    Serial.println("Interface de modification des paramètres : ");
    Serial.println("1 - Temps d'intervalle entre deux mesure mode standard.");
    Serial.println("2 - Changer la taille maximale d'un fichier.");
    Serial.println("3 - Réinitialiser l'ensemble des paramètres en valeur par défaut.");
    Serial.println("4 - Affichage de la version du programme.");
    Serial.println("");
    Serial.println("Veuillez choisir la modification à effectuer : ");

    valeurModification = Serial.read();

    while (valeurModification < 1 || valeurModification > 4)
    {
        Serial.println("Ce choix n'existe pas !");
        valeurModification = Serial.read();
    }

    switch (valeurModification)
    {
        case 1 :
            Serial.println("Veuillez mettre le nouveau temps entre deux mesures (en minutes) : ");
            LOG_INTERVAL =  Serial.read();
            LOG_INTERVAL *= 60000;
            break;
        case 2 :
            Serial.println("Veuillez mettre la nouvelle taille de fichier : ");
            FILE_MAX_SIZE =  Serial.read();
            break;
        case 3 :
            LOG_INTERVAL = 600000;
            FILE_MAX_SIZE;
            break;
        case 4:
            Serial.println("Version du programme : ");
            break;
    }
}

//****************************************************************
//* Nom        :  selectionMode         DATE : 20/10/2021        *  
//* Paramètres :  Valeur des boutons rouge et vert               *
//* Fonction   :  Fonction de selection de mode                  *
//* Retour     :  0 pour s'assurer du changement de mode         *   
//****************************************************************

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