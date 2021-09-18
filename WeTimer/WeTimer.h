/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: WeTimer.h is part of WeTimer                               */
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

#ifndef WeTimer_h
  #define WeTimer_h

  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <DNSServer.h>
  #include <EEPROM.h>
  #include <string.h>
  #include <Servo.h>

  #include "tools.h"
  #include "handleHttp.h"
  #include "htmlStyle.h"
  #include "handleRoot.h"
  #include "handleSetParams.h"
  #include "handleWiFi.h"

  // Pour debug sur port série
  //#define debug
  
  #define ORG_NAME           "fra589"
  #define APP_NAME           "WeTimer\0"
  #define APP_VERSION_STRING "v0.9.0"
  #define APP_VERSION_DATE   "20210918"
  
  
  //----------------------------------------------------------------------------------------------------
  // Adresses EEProm pour sauvegarde des paramètres
  //----------------------------------------------------------------------------------------------------
  #define EEPROM_LENGTH 512
  //----------------------------------------------------------------------------------------------------
  #define ADDR_TEMPS_ARMEMENT       0
  #define ADDR_TEMPS_VOL            2
  #define ADDR_SERVO_STAB_VOL       4
  #define ADDR_SERVO_STAB_TREUIL    6
  #define ADDR_SERVO_STAB_DT        8
  #define ADDR_SERVO_DERIVE_VOL    10
  #define ADDR_SERVO_DERIVE_TREUIL 12
  #define ADDR_CLI_SSID            14
  #define ADDR_CLI_PWD             46 //  14 + 32
  #define ADDR_AP_SSID            109 //  46 + 63
  #define ADDR_AP_PWD             141 // 109 + 32
  
  //----------------------------------------------------------------------------------------------------
  // Valeurs des paramètres par défauts
  //----------------------------------------------------------------------------------------------------
  #define DEFAULT_TEMPS_ARMEMENT         2   // Délai d'armement de la minuterie = 2 seconde
  #define DEFAULT_TEMPS_VOL            183   // Temps de vol par défaut 3 minutes
  #define DEFAULT_SERVO_STAB_VOL      1000   // Position servo au départ du vol (1000 micro secondes = 0°)
  #define DEFAULT_SERVO_STAB_TREUIL   1200   // Position servo quand le switch crochet est actionné (câble tendu)
  #define DEFAULT_SERVO_STAB_DT       2000   // Position servo au DT (2000 micro secondes = angle max du servo)
  #define DEFAULT_SERVO_DERIVE_VOL    1100   // Position servo de dérive en vol spirale
  #define DEFAULT_SERVO_DERIVE_TREUIL 1500   // Position servo de dérive câble tendu (montée droite)
  #define DEFAULT_CLI_SSID "\0"              // SSID client (la minuterie se connecte si défini)
  #define DEFAULT_CLI_PWD  "\0"              // WPA-PSK/WPA2-PSK client
  #define DEFAULT_AP_SSID  "WeTimer_\0"      // SSID de l'AP minuterie
  #define DEFAULT_AP_PWD   "\0"              // WPA-PSK/WPA2-PSK AP

  #define MIN_SERVO_MICROSECONDS 1000
  #define MAX_SERVO_MICROSECONDS 2000
  
  //----------------------------------------------------------------------------------------------------
  // Etats possibles de la minuterie
  //----------------------------------------------------------------------------------------------------
  #define STATUS_DT     0
  #define STATUS_ARMEE  1
  #define STATUS_TREUIL 2
  #define STATUS_VOL    4

  //----------------------------------------------------------------------------------------------------
  // Valeurs d'état du switch crochet
  //----------------------------------------------------------------------------------------------------
  #define CROCHET_TENDU 0
  #define CROCHET_RELACHE 1


  //----------------------------------------------------------------------------------------------------
  // Hardware mapping :
  // Proto N°2 Servo stab sur D2, servi dérive sur D1 et Switch crochet entre D7 et D8
  //----------------------------------------------------------------------------------------------------
  #define PIN_SERVO_STAB    4 // D2 == GPIO4
  #define PIN_SERVO_DERIVE  5 // D1 == GPIO5
  #define PIN_SWITCH       13 // D7 == GPI13
  #define GND_SWITCH       15 // D8 == GPI15


  //----------------------------------------------------------------------------------------------------
  // Variables globales
  //----------------------------------------------------------------------------------------------------
  extern char cli_ssid[32];
  extern char cli_pwd[63];
  extern char ap_ssid[32];
  extern char ap_pwd[63];
  
  extern unsigned int delaiArmement;
  extern unsigned int tempsVol;
  extern unsigned int servoStabVol;
  extern unsigned int servoStabTreuil;
  extern unsigned int servoStabDT;
  extern unsigned int servoDeriveVol;
  extern unsigned int servoDeriveTreuil;
  
  // hostname for mDNS. Should work at least on windows. Try http://minuterie.local
  extern const char *myHostname;
  
  // DNS server
  extern const byte DNS_PORT;
  extern DNSServer dnsServer;
  
  // Web server
  extern ESP8266WebServer server;
  
  /* Soft AP network parameters */
  extern IPAddress apIP;
  extern IPAddress netMsk;
  
  extern unsigned long debut;
  extern int crochet;
  
  extern Servo servoStab;   // Variable globale servo stabilisateur
  extern Servo servoDerive; // Variable globale servo dérive
  
  extern int timerStatus;
  
  extern unsigned long blinkInterval;
  extern unsigned long previousBlink;
  extern int ledState;

#endif // WeTimer_h
