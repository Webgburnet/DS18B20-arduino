// Inclure les bibliothèques dont nous avons besoin
#include <OneWire.h>
#include <DallasTemperature.h>

// Le fil de données est branché sur le port 2 de l'Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9 // Lower resolution

// Configuration d'une instance OneWire pour communiquer avec tout périphérique OneWire (pas uniquement les CI de température Maxim / Dallas)
OneWire oneWire(ONE_WIRE_BUS);

// Passons notre référence oneWire à Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Nombre d'appareils de température trouvés

DeviceAddress tempDeviceAddress; // Nous utiliserons cette variable pour stocker une adresse de périphérique trouvée

void setup(void)
{
  // démarre le port série
  Serial.begin(9600);
  Serial.println("Démonstration de la bibliothèque de contrôle de température Dallas");

  // Démarrer la bibliothèque
  sensors.begin();
  
  // Compter un nombre d'appareils sur le fil
  numberOfDevices = sensors.getDeviceCount();
  
  // localiser les appareils sur le bus
  Serial.print("Localisation des appareils ...");
  
  Serial.print(" Trouvé ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" dispositifs.");

 // signaler les besoins en énergie parasite
  Serial.print("Le pouvoir parasite est: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Boucle à travers chaque périphérique, adresse d'impression
  for(int i=0;i<numberOfDevices; i++)
  {
    // Recherche l'adresse sur le fil
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Appareil trouvé ");
		Serial.print(i, DEC);
		Serial.print(" avec adresse : ");
		printAddress(tempDeviceAddress);
		Serial.println();
		
		Serial.print("Réglage de la résolution sur ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		
		// définit la résolution sur le bit TEMPERATURE_PRECISION (chaque périphérique Dallas / Maxim est capable de plusieurs résolutions différentes)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		Serial.print("Résolution réglée à : ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Serial.println();
	}else{
		Serial.print("Appareil fantôme trouvé à ");
		Serial.print(i, DEC);
		Serial.print(" mais n'a pas pu détecter l'adresse. Vérifiez l'alimentation et le câblage");
	}
  }

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
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Convertit tempC en Fahrenheit
}

void loop(void)
{ 
  // appelle sensors.requestTemperatures () pour émettre une température globale
  // demande à tous les périphériques sur le bus
  Serial.print("Demander des températures ...");
  sensors.requestTemperatures(); // Envoyer la commande pour obtenir les températures
  Serial.println("TERMINE");
  
  
  // boucle en boucle sur chaque appareil, impression des données de température
  for(int i=0;i<numberOfDevices; i++)
  {
    // Recherche l'adresse sur le fil
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		// Affiche l'identifiant de l'appareil
		Serial.print("Température pour appareil:");
		Serial.println(i,DEC);
		
		// Il répond presque immédiatement. Imprimons les données
		printTemperature(tempDeviceAddress); // Utilise une fonction simple pour imprimer les données
	} 
	// autre appareil fantôme! Vérifiez vos exigences en matière d'alimentation et de câblage
	
  }
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
