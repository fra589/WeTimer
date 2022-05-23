/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: tools.cpp is part of WeTimer                               */
/*                                                                       */
/* WeTimer is free software: you can redistribute it and/or modify it    */
/* under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* WeTimer is distributed in the hope that it will be useful, but        */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*                                                                       */
/*************************************************************************/

#include <Arduino.h>
#include "WeTimer.h"

/** Is this an IP? */
bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

/*--------------------------------------------------------------------------------------------------*/
/*                                      EEPROM functions                                            */
/*--------------------------------------------------------------------------------------------------*/
// Efface tout le contenu de l'EEPROM
void EEPROM_format() {
  // Allume la LED
  digitalWrite(LED_BUILTIN, HIGH);
  for ( int i = 0 ; i < EEPROM_LENGTH ; i++ )
    EEPROM.write(i, 0xFF);
   EEPROM.commit();
  // Eteint la LED
  digitalWrite(LED_BUILTIN, LOW);
}

//Ecriture d'un entier (16 bits) en mémoire EEPROM à l'adresse spécifiée jusqu'à adresse+1
void EEPROM_writeInt(int adresse, int val) {   
  //découpage de la variable val qui contient la valeur à sauvegarder en mémoire en deux variables de 8 bits. 
  uint8_t faible = val & 0x00FF;       //récupère les 8 bits de droite (poids faible)
  uint8_t fort = (val >> 8) & 0x00FF;  //décale puis récupère les 8 bits de gauche (poids fort)
  //puis on enregistre les deux variables obtenues en mémoire
  EEPROM.write(adresse, fort) ;     //on écrit les bits de poids fort en premier
  EEPROM.write(adresse+1, faible) ; //puis on écrit les bits de poids faible à la case suivante
}

int EEPROM_readInt(int adresse) {
  // Lecture des 2 demi-valeurs
  uint8_t fort = EEPROM.read(adresse);     //récupère les 8 bits de gauche (poids fort)
  uint8_t faible = EEPROM.read(adresse+1); //récupère les 8 bits de droite (poids faible)
  // Renvoie l'entier assemblé
  return ((fort << 8) | faible);
}

//Ecriture d'un entier long (32bits) en mémoire EEPROM à l'adresse spécifiée jusqu'à adresse+3
void EEPROM_writeLong(int address, long value) {
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  uint8_t four = (value & 0xFF);
  uint8_t three = ((value >> 8) & 0xFF);
  uint8_t two = ((value >> 16) & 0xFF);
  uint8_t one = ((value >> 24) & 0xFF);
  
  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROM_readLong(int address) {
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);
  // Renvoie l'entier long assemblé
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void EEPROM_writeStr(int address, char *value, int len) {
  // Write string char to eeprom
  for (int i=0; i<len; i++) {
    EEPROM.write(address + i, value[i]);
  }  
}

double getTensionBatterie() {
  // Détermine la tension de batterie si connecté...
  const int analogInPin = A0;           // ESP8266 Analog Pin ADC0 = A0
  int analogValue = 0;                  // valeur lue sur A0 (0 to 1024 for 0 to 3.3v)
  // Calcul du voltage sur la pin.
  const double valueToVolts = 3.3/1024;
  // Diviseur de tension :
  // Résistance 22k (rouge/rouge/orange) au moins,
  // résistance 10k (marron/noir/orange) au plus,
  // point millieu sur A0
  const double correctionTension = (22000.0+10000.0)/22000.0; // 1.4545...
  // Determination empirique d'un coeeficient pour avoir la "bonne" valeur...
  const double correctionErreur = 0.934;

  double voltage;

  analogValue = analogRead(analogInPin);
  voltage = double(analogValue) * valueToVolts * correctionTension * correctionErreur;

  return voltage;
}
