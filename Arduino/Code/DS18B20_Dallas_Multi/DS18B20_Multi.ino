// Inclure les bibliothèques dont nous avons besoin
#include <OneWire.h>
#include <DallasTemperature.h>

// Le fil de données est branché sur le port 2 de l'Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 12

// Configuration d'une instance OneWire pour communiquer avec tout périphérique OneWire (pas uniquement les CI de température Maxim / Dallas)
OneWire oneWire(ONE_WIRE_BUS);

// Passons notre référence oneWire à Dallas Temperature.
DallasTemperature sensors(&oneWire);

// tableaux pour contenir les adresses de périphériques
DeviceAddress insideThermometer;

// Attribuer l'adresse manuellement. Les adresses ci-dessous seront modifiées
// pour valider les adresses de périphérique sur votre bus. L'adresse de l'appareil peut être récupérée
// en utilisant oneWire.search (deviceAddress) ou individuellement via
// sensors.getAddress (deviceAddress, index)
// DeviceAddress insideThermometer = {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0};
// DeviceAddress outsideThermometer = {0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2};

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Démonstration de la bibliothèque de contrôle de température Dallas");

  // Démarrer la bibliothèque
  sensors.begin();

  // localiser les appareils sur le bus
  Serial.print("Localisation des appareils ...");
  Serial.print("Trouvé ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" dispositifs.");

  // signaler les besoins en énergie parasite
  Serial.print("Le pouvoir parasite est:");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Rechercher des périphériques sur le bus et les affecter en fonction d'un index. Idéalement,
  // vous le feriez pour découvrir d'abord les adresses sur le bus, puis
  // utilise ces adresses et les attribue manuellement (voir ci-dessus) une fois que vous savez
  // les appareils sur votre bus (et en supposant qu'ils ne changent pas).
  //
  // méthode 1: par index
  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Impossible de trouver l'adresse pour le périphérique 0");
  
  // méthode 2: search ()
  // search () recherche le périphérique suivant. Retourne 1 si une nouvelle adresse a été
  // revenu. Un zéro peut signifier que le bus est en court-circuit, il n'y a pas de périphériques,
  // ou vous les avez déjà tous récupérés. Ce pourrait être une bonne idée de
  // vérifie le CRC pour t'assurer que tu n'as pas de déchets. La commande est
  // déterministe. Vous obtiendrez toujours les mêmes appareils dans le même ordre
  //
  // Doit être appelé avant search ()
  //oneWire.reset_search ();
  // assigne la première adresse trouvée à insideThermometer
  // if (! oneWire.search (insideThermometer)) Serial.println ("Impossible de trouver l'adresse pour insideThermometer");
  // assigne l'adresse des secondes trouvée à l'extérieur de Thermomètre
  // if (! oneWire.search (outsideThermometer)) Serial.println ("Impossible de trouver l'adresse pour outsideThermometer");

  // montre les adresses trouvées sur le bus
  Serial.print("Adresse du périphérique 0: ");
  printAddress(insideThermometer);
  Serial.println();

  // définit la résolution à 9 bits par périphérique
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();
}

// fonction pour imprimer une adresse de périphérique
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Erreur: Impossible de lire les données de température");
    return;
  }
  Serial.print("Temp C : ");
  Serial.print(tempC);
  Serial.print(" Temp F : ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Résolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Adresse du périphérique : ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Demander des températures ...");
  sensors.requestTemperatures();
  Serial.println("TERMINÉ");

  // print the device information
  printData(insideThermometer);
}
