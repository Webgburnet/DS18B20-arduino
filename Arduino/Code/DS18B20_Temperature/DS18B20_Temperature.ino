#include <OneWire.h>

// Exemple de température OneWire DS18S20, DS18B20, DS1822
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// La bibliothèque DallasTemperature peut faire tout ce travail pour vous!
// https://github.com/milesburton/Arduino-Temperature-Control-Library

OneWire  ds(2);  // sur la broche 2 (une résistance de 4,7K est nécessaire)

void setup(void) {
  Serial.begin(9600);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    Serial.println("Pas plus d'adresses.");
    Serial.println();
    ds.reset_search();
    //delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("Le CRC n'est pas valide!");
      return;
  }
  Serial.println();
 
  // le premier octet de la ROM indique quelle puce
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Puce = DS18S20");  // ou ancien DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Puce = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Puce = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Le périphérique n'est pas un périphérique de la famille DS18x20.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // démarre la conversion, avec une alimentation parasite à la fin
  
  //delay(1000);     // peut-être que 750ms suffisent, peut-être pas
  // nous pourrions faire un ds.depower () ici, mais la réinitialisation s'en chargera.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Lire le Scratchpad

  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // nous avons besoin de 9 octets
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // Convertir les données en température réelle
  // parce que le résultat est un entier signé de 16 bits, il devrait
  // être stocké dans un type "int16_t", qui a toujours 16 bits
  // même lorsqu'il est compilé sur un processeur 32 bits.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // résolution 9 bits par défaut
    if (data[7] == 0x10) {
      // "compte restant" donne une résolution complète de 12 bits
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    // en basse résolution, les bits bas sont indéfinis, remettons-les donc à zéro
    if (cfg == 0x00) raw = raw & ~7;  // résolution 9 bits, 93,75 ms
    else if (cfg == 0x20) raw = raw & ~3; // résolution 10 bits, 187,5 ms
    else if (cfg == 0x40) raw = raw & ~1; // Résolution 11 bits, 375 ms
    //// la résolution par défaut est 12 bits, temps de conversion de 750 ms
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
}
