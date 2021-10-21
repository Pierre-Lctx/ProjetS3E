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

    BME280 capteur;

    ChainableLED leds(7 ,8, 1);

    DS1307 clock;

    File carteSD;


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

void getData()
{   
    String dateString;
    String dataString;

    clock.getTime();
    dateString += String(clock.dayOfMonth, DEC);
    dateString += "/";
    dateString += String(clock.month, DEC);
    dateString += "/";
    dateString += String(clock.year + 2000, DEC);
    dateString += " ";
    dateString += String(clock.hour, DEC);
    dateString += ":";
    dateString += String(clock.minute, DEC);
    dateString += ":";
    dateString += String(clock.year + 2000, DEC);
    dateString += " -> ";

    

    //Récupération de la luminosité
    //On récupère les données brutes du capteurs

    int luminosite = analogRead(A0);
    //On traite les données ici
    luminosite =  (luminosite/10)^10;

 

    //Récupération de la température de l'air en °C
    float temperature = capteur.readTempC();
    dataString += "Température : ";
    dataString += String(temperature, DEC);
    dataString += "°C | ";

    //Récupération de la pression atmosphérique en Pascals
    float pression = capteur.readFloatPressure();
    dataString += "Pression : ";
    dataString += String(pression, DEC);
    dataString += " Pa | ";

    //Récupération de l'humidité en %
    float humidite = capteur.readFloatHumidity();
    dataString += "Humidité : ";
    dataString += String(temperature, DEC);
    dataString += "° %.";


    carteSD = (SD.open("data.txt", FILE_WRITE));

    if (carteSD)
    {
        Serial.println("Enregistrement des données ! Fichier ouvert !");
        if (luminosite != NULL)
        {
            carteSD.print("Luminosité : ");
            carteSD.print(luminosite);
            carteSD.print(" lux | ");
        }

        if (temperature != NULL)
        {
            carteSD.print("Température : ");
            carteSD.print(temperature);
            carteSD.print("°C | ");
        }

        if (pression != NULL)
        {
            carteSD.print("Pression : ");
            carteSD.print(pression);
            carteSD.print("Pa | ");
        }

        if (humidite != NULL)
        {
            carteSD.print("Humidité : ");
            carteSD.print(humidite);
            carteSD.print("%.");
        }
    }

    delay(LOG_INTERVAL);
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
        Serial.println("Green interruption !");
        currentMillis = millis();

        if ((unsigned long)(currentMillis - previousMillis) >= 5000)
        {
            selectionMode(0, digitalRead(greenButtonPort));
            previousMillis = currentMillis;
        }
    }
}

void initInterrupt()
{
    attachInterrupt(digitalPinToInterrupt(redButtonPort), interruptionRed, CHANGE);
    attachInterrupt(digitalPinToInterrupt(greenButtonPort), interruptionGreen, CHANGE);
}


void modeStandard() 
{
    //Allumage de la LED verte
    leds.setColorRGB(0, 0, 255, 0);

    checkGetData = true;

    LOG_INTERVAL = 600000;
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

    LOG_INTERVAL *= 2;
}
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
        Serial.println("Green button pressed !");
        Serial.print("Mode = ");
        
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

