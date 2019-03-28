// Inclure les bibliothèques dont nous avons besoin
#include <OneWire.h>
#include <DallasTemperature.h>

// Le fil de données est branché sur le port 2 de l'Arduino
#define DS18B20_Dallas 2
#define TEMPERATURE_PRECISION 12
// Configuration d'une instance OneWire pour communiquer avec tout périphérique OneWire (pas uniquement les CI de température Maxim / Dallas)
OneWire oneWire(DS18B20_Dallas);

// Passons notre référence oneWire à Dallas Temperature.
DallasTemperature sensor(&oneWire);

// tableaux pour contenir l'adresse du périphérique
DeviceAddress deviceAddress;

void setup(void)
{
    Serial.begin(9600);
    Serial.println("Démo de la bibliothèque de contrôle de la température de Dallas - DS18B20");
    sensor.begin();
}

void loop(void)
{
    Serial.print("Aquisition temperature...");
    sensor.requestTemperatures();
    Serial.println(" Fait");
    
    float temp_C_slow = sensor.getTempCByIndex(0);
    float temp_F_slow = sensor.getTempFByIndex(0);
    
    Serial.print("Temperature (Celius) : ");
    Serial.print(temp_C_slow);
    Serial.println("C");
    Serial.print("Temperature (Far) : ");
    Serial.print(temp_F_slow);
    Serial.println("F");
    
}
