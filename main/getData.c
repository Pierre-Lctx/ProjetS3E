#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>
#include "functions.c"
#include "initialisation.c"
#include "functions.h"
#include "carteSD.c"

//****************************************************************
//* Nom        :  getData               DATE : 20/10/2021        *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Récupération et traitement des données         *
//* Retour     :  Aucun                                          *   
//****************************************************************

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

    Serial.print(dateString);

    //Récupération de la luminosité
    //On récupère les données brutes du capteurs

    float luminosite = analogRead(A0);
    //On traite les données ici
    luminosite =  (luminosite/10)^10;

    Serial.print(luminosite);
    Serial.print(" lux | ");

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

    Serial.print("Température : ");
    Serial.print(temperature);
    Serial.print("°C | ");
    Serial.print("Pression : ");
    Serial.print(pression);
    Serial.print(" Pa | ");
    Serial.print("Humidité : ");
    Serial.print(humidite);
    Serial.println(" %.");

    carteSD = (SD.open("data.txt", FILE_WRITE));

    if (carteSD)
    {
        Serial.println("Enregistrement des données ! Fichier ouvert !");
        if (light != NULL)
        {
            carteSD.print("Luminosité : ")
            carteSD.print(light);
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
    else 
    {
        Serial.println("Impossible de trouver la carte SD...");
    }

    delay(LOG_INTERVAL);
}