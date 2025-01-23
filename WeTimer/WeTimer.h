/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: WeTimer.h is part of WeTimer                                  */
/*                                                                          */
/* WeTimer is free software: you can redistribute it and/or modify it       */
/* under the terms of the GNU General Public License as published by        */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* WeTimer is distributed in the hope that it will be useful, but           */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*                                                                          */
/****************************************************************************/

#ifndef WeTimer_h
  #define WeTimer_h

  // Pour debug sur port série
  #define DEBUG
  //#define DEBUG_2
  //#define DEBUG_EEPROM
  //#define DEBUG_WIFI
  #define DEBUG_WEB
  //#define DEBUG_WEB_2

  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <WebSocketsServer.h>
  #include <DNSServer.h>
  #include <ESP8266mDNS.h>
  #include <EEPROM.h>
  #include <LittleFS.h>
  #include <string.h>
  #include <Servo.h>
  #include <ArduinoJson.h>
  #include "ESPTelnet.h"
  #include <stdarg.h>

  #include "default.h"
  #include "eeprom.h"
  #include "wifi.h"
  #include "webserver.h"
  #include "websocket.h"
  #include "telnet.h"
  #include "flash.h"

  // Printf wrapper pour envoi sur Serial et telnet
  #define WT_PRINTF(f_, ...) Serial.printf((f_), ##__VA_ARGS__); \
                             if (telnet.isConnected()) \
                             telnet.printf((f_), ##__VA_ARGS__);

  #define ORG_NAME              "fra589"
  #define COPYRIGHT             "G.Brière 2021-2025"
  #define APP_NAME              "WeTimer"
  #define APP_VERSION_MAJOR     "2"
  #define APP_VERSION_MINOR     "0"
  #define APP_VERSION_DATE      "20250132"
  #define APP_VERSION_STRING    "v" APP_VERSION_MAJOR "." APP_VERSION_MINOR "." APP_VERSION_DATE
  #define APP_VERSION_VERIF     "[" APP_NAME ",v" APP_VERSION_MAJOR "." APP_VERSION_MINOR
  #define APP_NAME_VERSION      APP_NAME " - " APP_VERSION_STRING "\0"

  //----------------------------------------------------------------------------------------------------
  // Hardware mapping :
  //----------------------------------------------------------------------------------------------------
  #define PIN_SCL      D1 // réservé I2C................ GPIO05 (futur accéléromètre ?)
  #define PIN_SDA      D2 // réservé I2C................ GPIO04 (futur accéléromètre ?)
  #define PIN_LED      D0 // LED externe................ GPIO16
  #define PIN_SERVO_0  D8 // Servo Stab................. GPIO13 
  #define PIN_SERVO_1  D7 // Servo Derive............... GPIO12
  #define PIN_SERVO_2  D6 // Servo Aile droite.......... GPIO14 (ou futur crochet)
  #define PIN_RDT      D5 // Signal RDT externe (BMK DT) GPIO15
  #define PIN_SWITCH_0 D4 // Switch crochet avant....... GPIO2
  #define PIN_SWITCH_1 D3 // Switch crochet largue...... GPIO0
  #define PIN_BATTERIE A0 // Conversion analogique mesure tension batterie

  //----------------------------------------------------------------------------------------------------
  // Convertisseur analogique / numérique pour mesure tension batterie
  //----------------------------------------------------------------------------------------------------  
  #define R1     2200.      //      + -------
  #define R2     4700.      //              R1
  #define V_COEF (R1+R2)/R2 //               +------ A0 (PIN_BATTERIE)
  #define V_ERR  0.945f     //              R2
                            //    GND -------

  //----------------------------------------------------------------------------------------------------
  // Valeurs min/max servos (MICROSECONDS = valeurs par défaut de la bibliothèque servo)
  //----------------------------------------------------------------------------------------------------
  #define MIN_SERVO_PROGRAMATION 0
  #define MAX_SERVO_PROGRAMATION 255
  #define MIN_SERVO_MICROSECONDS 544
  #define MAX_SERVO_MICROSECONDS 2400

  //----------------------------------------------------------------------------------------------------
  // Valeurs d'état d'un switch
  //----------------------------------------------------------------------------------------------------
  #define SWITCH_ON  0
  #define SWITCH_OFF 1

  //----------------------------------------------------------------------------------------------------
  // Etats possibles de la minuterie
  //----------------------------------------------------------------------------------------------------
  // Status :                      // Déclencheur :                                      // Servos :
  #define STATUS_PARC           0 // Position au repos (DT +5 secondes) et à l'allumage // Position[0]
  #define STATUS_ARMEMENT       1 // STATUS_PARC         + délai N°0 (délai armement)    // <No change>
  #define STATUS_TREUIL_MONTEE  2 // STATUS_ARMEMENT     + SWITCH_1 = ON                 // Position[1]
  #define STATUS_TREUIL_VIRAGE  3 // STATUS_ARMEMENT     + SWITCH_1 = OFF                // Position[2]
  #define STATUS_DEVERROUILLE   4 // STATUS_ARMEMENT     + SWITCH_1 = ON + SWITCH_2 = ON // Position[3]
  #define STATUS_LARGUE         5 // STATUS_DEVERROUILLE + SWITCH_1 = OFF                // <No change>
  #define STATUS_PITCHUP        6 // STATUS_LARGUE       + délai N°1                     // Position[4]
  #define STATUS_MONTEE_1       7 // STATUS_PITCHUP      + délai N°2                     // Position[5]
  #define STATUS_MONTEE_2       8 // STATUS_MONTEE_1     + délai N°3                     // Position[6]
  #define STATUS_BUNT           9 // STATUS_MONTEE_2     + délai N°4                     // Position[7]
  #define STATUS_PLANE_1       10 // STATUS_BUNT         + délai N°5                     // Position[8]
  #define STATUS_PLANE_2       11 // STATUS_PLANE_1      + délai N°6                     // Position[9]
  #define STATUS_DT            12 // STATUS_PLANE_2      + délai N°7                     // Position[10]
  //retour STATUS_PARC           0 // STATUS_DT            + délai N°8 (= 5 secondes)      // Position[0]
  // Délai N°9 = futur délai de refermeture pour crochet piloté par servo

  // Positions possibles des servos
  #define POSITION_PARC           0
  #define POSITION_TREUIL_MONTEE  1
  #define POSITION_TREUIL_VIRAGE  2
  #define POSITION_DEVERROUILLE   3
  #define POSITION_PITCHUP        4
  #define POSITION_MONTEE_1       5
  #define POSITION_MONTEE_2       6
  #define POSITION_BUNT           7
  #define POSITION_PLANE_1        8
  #define POSITION_PLANE_2        9
  #define POSITION_DT            10

  //----------------------------------------------------------------------------------------------------
  // Variables globales
  //----------------------------------------------------------------------------------------------------
  // Paramètres WiFi
  #define MAX_SSID_LEN 32             // Longueur maxi d'un SSID
  #define MAX_PWD_LEN  63             // Longueur maxi des mots de passe WiFi
  extern char cli_ssid[MAX_SSID_LEN];
  extern char cli_pwd[MAX_PWD_LEN];
  extern char ap_ssid[MAX_SSID_LEN];
  extern char ap_pwd[MAX_PWD_LEN];
  struct connexion {
    char ssid[MAX_SSID_LEN];
    char pwd[MAX_PWD_LEN];
  };
  extern bool wifiIsSlepping;
  
  // positions servos et délais
  #define NB_DELAI     10
  #define NB_SERVOS     3
  #define NB_POSITIONS 11
  #define NB_CONFIG     4
  extern unsigned int delai[NB_DELAI];        // On stockeras en centièmes de secondes
  extern int pservo[NB_SERVOS][NB_POSITIONS]; // Position des 3 servos (11 positions)
  extern int cservo[NB_SERVOS][NB_CONFIG];    // Configuration limites servos 3 x valeur prog min, valeur prog max,
                                                       // microseconds min, microsecond max.
  #define DESCRIPTION_LEN 64
  extern char description[DESCRIPTION_LEN];

  struct temps {
    unsigned long largage; // millis() top largage
    unsigned long pitchup;
    unsigned long montee1;
    unsigned long montee2;
    unsigned long bunt;
    unsigned long plane1;
    unsigned long plane2;
    unsigned long dt;
    unsigned long parc;
  };
  extern struct temps temps;
  
  // hostname for mDNS (http://WeTimer.local)
  extern const char *myHostname;
  // DNS server
  #define DNS_PORT 53
  extern DNSServer dnsServer;
  // Web server
  #define HTTP_PORT 80
  extern ESP8266WebServer server;
  #define WEB_SOCKET_PORT 81
  extern WebSocketsServer webSocket;
  // Telnet server
  #define TELNET_PORT 23
  extern ESPTelnet telnet;
  // Objets Servos
  extern Servo servo[3]; // Variable globale servos
  // Gestion port série
  #define SERIAL_BUFF_LEN 256
  extern char serialInput[SERIAL_BUFF_LEN];  // a String to hold incoming Serial data
  extern int  serialPointer;                 // last character counter of Serial data
  extern bool serialComplete;                // whether the string is complete
  // Statut de la minuterie
  extern int timerStatus;
  extern uint8_t crochet[2];
  extern volatile bool recuExternRDT;
  extern unsigned long dureeBoucle;
  // Flasher
  extern bool flash_verrou;
  extern bool flash_vol_on;
  extern unsigned long tOn;
  extern unsigned long tOff;
  extern unsigned long tCycle;
  extern unsigned long nFlash;

  //----------------------------------------------------------------------------------------------------
  // Fonctions de WeTimer.cpp
  //----------------------------------------------------------------------------------------------------
  void readSerial(void);
  void processSerial(void);
  connexion getConnexion(const char* chaine);
  void setServoPos(unsigned int servoNum, int servoPos);
  void servoTest(unsigned int mode, unsigned int servoNum, int limit1, int limit2, int pwm1, int pwm2, int val);
  float getTension();
  String getStatusText(const int status);
  void IRAM_ATTR externRDT(void);
  void runDT(void);
  void calculInstants(void);
  String escapeXML(const String buff);
  String unescapeXML(const String buffer);
  void ls(String path);
  
#endif // WeTimer_h
