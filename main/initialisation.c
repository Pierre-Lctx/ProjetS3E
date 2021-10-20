#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>
#include "functions.h"

//Déclaration des variables du programme

//****************************************************************
//* Nom        :  initialisation        DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Initialisation de notre Arduino                *
//* Retour     :  Aucun                                          *   
//****************************************************************

void initialisation()
{
    Serial.begin(9600);

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

    pinMode(redButtonPort, INPUT);
    pinMode(greenButtonPort, INPUT);
    pinMode(A1, INPUT); 								//Initialisation du capteur de luminosité

    analogWrite(A1, LOW);

    //Configuration de l'horloge
    clock.begin();
    clock.fillByYMD(2021, 10, 22); 						// On initialise le jour de départ de la clock
    clock.fillByHMS(14,31,00); 							// On initialise l'heure de départ
    clock.fillDayOfWeek(SUN); 							//On entre le nom du jour de départ
    clock.setTime(); 									//Ecriture de la date sur l'horloge RTC

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



