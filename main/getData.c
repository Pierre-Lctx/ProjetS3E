#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>
#include "functions.c"
#include "initialisation.c"

//****************************************************************
//* Nom        :  getData               DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Récupération et traitement des données         *
//* Retour     :  Aucun                                          *   
//****************************************************************

void getData()
{   
    String dateString;

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

    Serial.print(dateString);

    //Récupération de la luminosité
    //On récupère les données brutes du capteurs

    int luminosite = analogRead(A0);
    //On traite les données ici
    luminosite =  (luminosite/10)^10;

    Serial.print(luminosite);
    Serial.print(" lux | ");

    //Récupération de la température de l'air en °C
    float temperature = capteur.readTempC();

    //Récupération de la pression atmosphérique en Pascals
    float pression = capteur.readFloatPressure();

    //Récupération de l'humidité en %
    float humidite = capteur.readFloatHumidity();

    Serial.print("Température : ");
    Serial.print(temperature);
    Serial.print("°C | ");
    Serial.print("Pression : ");
    Serial.print(pression);
    Serial.print(" Pa | ");
    Serial.print("Humidité : ");
    Serial.print(humidite);
    Serial.println(" %.");

    delay(LOG_INTERVAL);
}