#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>

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

float dataTable[4];

BME280 capteur;

ChainableLED leds(7 ,8, 1);

DS1307 clock;

File carteSD;

//****************************************************************
//* Nom        :  initialisation        DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Initialisation de notre Arduino                *
//* Retour     :  Aucun                                          *   
//****************************************************************

void initialisation()
{
    Serial.begin(9600);
    while (!Serial) {
    ; // Attendre que le port série se connecte. Nécessaire pour le port USB natif uniquement
    }


    Serial.print("Initialisation avec la carte SD...");

    if (!SD.begin(4)) {
        Serial.println("Initialisation échoué!");
        while (1);
    }
    Serial.println("Initialisation réussi");


    pinMode(redButtonPort, INPUT);
    pinMode(greenButtonPort, INPUT);
    pinMode(A1, INPUT);                 //Initialisation du capteur de luminosité

    analogWrite(A1, LOW);

    //Configuration de l'horloge
    clock.begin();
    clock.fillByYMD(2021, 10, 22);            // On initialise le jour de départ de la clock
    clock.fillByHMS(14,31,00);              // On initialise l'heure de départ
    clock.fillDayOfWeek(SUN);               //On entre le nom du jour de départ
    clock.setTime();                  //Ecriture de la date sur l'horloge RTC

    //Configuration du capteur
    capteur.settings.commInterface = I2C_MODE; 
    capteur.settings.I2CAddress = 0x76;
    capteur.settings.runMode = 3; 
    capteur.settings.tStandby = 0;
    capteur.settings.filter = 0;
    capteur.settings.tempOverSample = 1 ;
    capteur.settings.pressOverSample = 1;
    capteur.settings.humidOverSample = 1;

    delay(10);

    //Chargement de la configuration du capteur
    capteur.begin();

    initInterrupt();
}

//****************************************************************
//* Nom        :  getData               DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Récupération et traitement des données         *
//* Retour     :  Aucun                                          *   
//****************************************************************

void getData()
{   
    String *dateString;
    String *dataString;

    clock.getTime();
    *dateString = String(clock.dayOfMonth, DEC);
    *dateString += "/";
    *dateString += String(clock.month, DEC);
    *dateString += "/";
    *dateString += String(clock.year + 2000, DEC);
    *dateString += " ";
    *dateString += String(clock.hour, DEC);
    *dateString += ":";
    *dateString += String(clock.minute, DEC);
    *dateString += ":";
    *dateString += String(clock.year + 2000, DEC);
    *dateString += " -> ";

    

    //Récupération de la luminosité
    //On récupère les données brutes du capteurs

    int temperature = (float)analogRead(A0);
    //On traite les données ici
    temperature = (temperature/10)^10;
    dataTable[0] = temperature;

    *dataString = "Luminosité : ";
    *dataString += String(dataTable[0], DEC);
    *dataString += " lux | ";

    //Récupération de la température de l'air en °C
    dataTable[1] = capteur.readTempC();
    *dataString += "Température : ";
    *dataString += String(dataTable[1], DEC);
    *dataString += "°C | ";

    //Récupération de la pression atmosphérique en Pascals
    dataTable[2] = capteur.readFloatPressure();
    *dataString += "Pression : ";
    *dataString += String(dataTable[2], DEC);
    *dataString += "Pa | ";

    //Récupération de l'humidité en %
    dataTable[3] = capteur.readFloatHumidity();
    *dataString += "Humidité : ";
    *dataString += String(dataTable[3], DEC);
    *dataString += "%.\n";

    carteSD = (SD.open("data.txt", FILE_WRITE));

    if (carteSD)
    {
        Serial.println(F("Enregistrement des données ! Fichier ouvert !"));

        carteSD.print(*dateString);
        if (dataTable[0] != NULL)
        {
            Serial.println(F("Ecriture de la luminosité !"));
            carteSD.print("Luminosité : ");
            carteSD.print(String(dataTable[0], DEC));
            carteSD.print(" lux | ");
        }

        if (dataTable[1] != NULL)
        {
            Serial.println(F("Ecriture de la température !"));
            carteSD.print("Température : ");
            carteSD.print(String(dataTable[1], DEC));
            carteSD.print("°C | ");
        }

        if (dataTable[2] != NULL)
        {
            Serial.println(F("Ecriture de la pression !"));
            carteSD.print("Pression : ");
            carteSD.print(String(dataTable[2], DEC));
            carteSD.print("Pa | ");
        }

        if (dataTable[3] != NULL)
        {
            Serial.println(F("Ecriture de l'humidité !"));
            carteSD.print("Humidité : ");
            carteSD.print(String(dataTable[3], DEC));
            carteSD.print("%.\n");
        }

        carteSD.close();
    }
    else 
        Serial.println(F("Erreur !!! Aucune écriture possible sur la carte SD !"));

    *dateString += *dataString;

    Serial.println(*dateString);

    *dateString = " ";
    *dataString = " ";

    delay(LOG_INTERVAL);
}

//****************************************************************
//* Nom        :  interruptionRed       DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Initialisation de l'interruption du bouton     *
//* Rouge                                                        *
//* Retour     :  Aucun                                          *   
//****************************************************************

void interruptionRed()
{
    if (digitalRead(redButtonPort))
    {
        Serial.println(F("Red interruption !"));
        currentMillis = millis();

        if ((unsigned long)(currentMillis - previousMillis) >= 5000)
        {
            selectionMode(digitalRead(redButtonPort), 0);
            previousMillis = currentMillis;
        }
    }
}

//****************************************************************
//* Nom        :  interruptionGreen     DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Initialisation de l'interruption du bouton     *
//* Vert                                                         *
//* Retour     :  Aucun                                          *   
//****************************************************************

void interruptionGreen()
{
    if (digitalRead(greenButtonPort))
    {
        Serial.println(F("Green interruption !"));
        currentMillis = millis();

        if ((unsigned long)(currentMillis - previousMillis) >= 5000)
        {
            selectionMode(0, digitalRead(greenButtonPort));
            previousMillis = currentMillis;
        }
    }
}

//****************************************************************
//* Nom        :  initInterrupt         DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Initialisation de l'interruption               *
//* Retour     :  Aucun                                          *   
//****************************************************************

void initInterrupt()
{
    attachInterrupt(digitalPinToInterrupt(redButtonPort), interruptionRed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(greenButtonPort), interruptionGreen, CHANGE);
}

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

    LOG_INTERVAL = LOG_INTERVAL * 2;
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
    Serial.println(F("Interface de modification des paramètres : "));
    Serial.println(F("1 - Temps d'intervalle entre deux mesure mode standard."));
    Serial.println(F("2 - Changer la taille maximale d'un fichier."));
    Serial.println(F("3 - Réinitialiser l'ensemble des paramètres en valeur par défaut."));
    Serial.println(F("4 - Affichage de la version du programme."));
    Serial.println(F(""));
    Serial.println(F("Veuillez choisir la modification à effectuer : "));

    bool checkDataInput = false;

    while (checkDataInput == false)
    {
        valeurModification = Serial.read();
    }

    if (valeurModification < 1 || valeurModification > 4)
    {
        Serial.println(F("Ce choix n'existe pas !"));
        valeurModification = Serial.read();
    }

    switch (valeurModification)
    {
        case 1 :
            Serial.println(F("Veuillez mettre le nouveau temps entre deux mesures (en minutes) : "));
            LOG_INTERVAL =  Serial.read();
            LOG_INTERVAL *= 60000;
            break;
        case 2 :
            Serial.println(F("Veuillez mettre la nouvelle taille de fichier : "));
            FILE_MAX_SIZE =  Serial.read();
            break;
        case 3 :
            LOG_INTERVAL = 600000;
            FILE_MAX_SIZE;
            break;
        case 4:
            Serial.println(F("Version du programme : "));
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
    Serial.println(F("Selection en cours !"));
    if (redButtonValue)
    {
        Serial.println(F("Red button pressed !"));
        //Si nous venons d'activer le dispositif et que nous appuyons sur le bouton rouge, nous passons en mode configuration
        if (mode == 0)
        {
            mode = 4;
            modeConfiguration();
            Serial.println(F("Mode configuration !"));
            return 0;
        }

        //Si nous sommes en mode standard et que nous appuyons sur le bouton rouge, nous passons en mode maintenance
        if (mode == 1)
        {
            mode = 2;
            lastMode = 1;
            modeMaintenance();
            Serial.println(F("Mode maintenance !"));
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
                
                return 0;
            }
            if (lastMode == 3)
            {
                mode = 3;
                lastMode = 2;
                modeEconomique();
                
                return 0;
            }
        }

        //Si nous sommes en mode économique et que le bouton rouge est pressé, nous passons en mode maintenance
        if (mode == 3)
        {
            mode = 2;
            modeMaintenance();
            
            return 0;
        }
    }

    if (greenButtonValue)
    {
        Serial.println(F("Green button pressed !"));

        //Si nous venons d'activer le dispositif et que nous appuyons sur le bouton vert, nous passons en mode standard
        if (mode == 0)
        {
            mode = 1;
            modeStandard();
            
            return 0;
        }

        //Si nous sommes en mode standard et que le bouton vert est pressé, nous passons en mode économique
        if (mode == 1)
        {
            mode = 3;
            lastMode = 1;
            modeEconomique();
            
            return 0;
        }

        //Si nous sommes en mode économique et que le bouton vert est pressé, nous passons en mode standard
        if (mode == 3)
        {
            mode = 1;
            lastMode = 3;
            modeStandard();
           
            return 0;
        }
    }

    if (redButtonValue && greenButtonValue)
    {
        leds.setColorRGB(0, 255, 255, 255);
        return 0;
    }
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
