//****************************************************************
//* Nom        :  carteSD        DATE : 21/10/2021               *  
//* Paramètres :  Aucun                                          *
//* Fonction   :  Ecriture & Lecture sur la carte SD             *
//* Retour     :  Aucun                                          *   
//****************************************************************

/*
  Lecture/écriture de carte SD

Cet exemple montre comment lire et écrire des données sur une carte SD.
Le circuit :
   La carte SD est connectée au bus SPI suivant:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** SCK - pin 13
 ** CS - pin 4 

*/




//--------------------------------------//   Ajouts des bibliothèques  //-------------------------------------//


#include <SPI.h> //Cette bibliothèque vous permet de communiquer avec des périphériques SPI, avec l'Arduino comme périphérique maître.
#include <SD.h> //La bibliothèque SD permet de lire et d'écrire sur des cartes SD
#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SparkFunBME280.h>
#include "functions.c"
#include "initialisation.c"
#include "functions.h"


//--------------------------------------//   Nomination du fichier ouvert  //-------------------------------------//


File myFile; //Nommez l'instance du fichier ouvert "myFile".



//--------------------------------------//   Fonction d'écriture des données //-------------------------------------//



void datawrite(){
	// Ouvrez le fichier. Notez qu'un seul fichier peut être ouvert à la fois,
  // donc vous devez fermer celui-ci avant d'en ouvrir un autre.
  myFile = SD.open("data.txt", FILE_WRITE);

  // si le fichier s'est ouvert sans problème, écrire dans ce fichier :
  if (myFile) {
    Serial.print("Ecriture dans texte.txt...");
    myFile.println("Test 1");
    // Fermeture du fichier
    myFile.close();
    Serial.println("Fini.");
  } else {
    // si le fichier ne s'est pas ouvert, ecrire une erreur
    Serial.println("Erreur d'ouverture de test.txt");
  }

}



//--------------------------------------//   Fonction de lecture des données //-------------------------------------//


void dataread(){
	// Re-ouvrir le fichier pour le lire:
  myFile = SD.open("data.txt");
  if (myFile) {
    Serial.println("Contenue du fichier:");

    // lire le fichier jusqu'à ce qu'il n'y ait rien d'autre dedans :
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // Fermer le fichier:
    myFile.close();
  } else {
    // si le fichier ne s'est pas ouvert, affiche une erreur :
    Serial.println("Erreur d'ouverture de test.txt");
  }
}


