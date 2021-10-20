#include <ChainableLED.h>
#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"
#include <SPI.h>
#include <SD.h>
#include <SparkFunBME280.h>
#include "functions.c"
#include "initialisation.c"
#include "getData.c"
#include "functions.h"



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