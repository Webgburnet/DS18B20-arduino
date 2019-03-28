// Inclure les bibliothèques dont nous avons besoin
#include <OneWire.h>
#include <DallasTemperature.h>

// Le fil de données est branché sur le port 2 de l'Arduino
#define ONE_WIRE_BUS 2

// Configuration d'une instance OneWire pour communiquer avec tout périphérique OneWire (pas uniquement les CI de température Maxim / Dallas)
OneWire oneWire(ONE_WIRE_BUS);

// Passons notre référence oneWire à Dallas Temperature.
DallasTemperature sensors(&oneWire);

// tableaux pour contenir l'adresse du périphérique
DeviceAddress insideThermometer;

/*
 * Fonction d'installation. Ici nous faisons les bases
 */
void setup(void)
{
  // démarre le port série
  Serial.begin(9600);
  Serial.println("Démonstration de la bibliothèque de contrôle de température Dallas");

  // localiser les appareils sur le bus
  Serial.print("Localisation des appareils ...");
  sensors.begin();
  Serial.print(" Trouvé ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" dispositifs.");

  // signaler les besoins en énergie parasite
  Serial.print("Le pouvoir parasite est : "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Attribuer l'adresse manuellement. Les adresses ci-dessous seront modifiées
  // pour valider les adresses de périphérique sur votre bus. L'adresse de l'appareil peut être récupérée
  // en utilisant oneWire.search (deviceAddress) ou individuellement via
  // sensors.getAddress (deviceAddress, index)
  // Notez que vous devrez utiliser votre adresse spécifique ici
  // insideThermometer = {0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0};

  // Méthode 1:
  // Rechercher des périphériques sur le bus et les affecter en fonction d'un index. Idéalement,
  // vous le feriez pour découvrir d'abord les adresses sur le bus, puis
  // utilise ces adresses et les attribue manuellement (voir ci-dessus) une fois que vous savez
  // les appareils sur votre bus (et en supposant qu'ils ne changent pas).
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

  // montre les adresses trouvées sur le bus
  Serial.print("Adresse du périphérique 0:");
  printAddress(insideThermometer);
  Serial.println();

  // définit la résolution sur 9 bits (chaque périphérique Dallas / Maxim est capable de plusieurs résolutions différentes)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Résolution du périphérique 0:");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();
}

// fonction pour imprimer la température d'un appareil
void printTemperature(DeviceAddress deviceAddress)
{
  // méthode 1 - plus lent
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print ("Temp F:");
  //Serial.print(sensors.getTempF(deviceAddress)); // Fait un deuxième appel à getTempC puis se convertit en Fahrenheit

  // méthode 2 - plus rapide
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Erreur: Impossible de lire les données de température");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC));// Convertit tempC en Fahrenheit
}
/*
 * Fonction principale. Il demandera la température aux capteurs et s’affichera en série.
 */
void loop(void)
{ 
  // appelle sensors.requestTemperatures () pour émettre une température globale
  // demande à tous les périphériques sur le bus
  Serial.print("Demander des températures ...");
  sensors.requestTemperatures(); // Envoyer la commande pour obtenir les températures
  Serial.println("TERMIN");
  
  // Il répond presque immédiatement. Imprimons les données
  printTemperature(insideThermometer); // Utilise une fonction simple pour imprimer les données
}

// fonction pour imprimer une adresse de périphérique
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
