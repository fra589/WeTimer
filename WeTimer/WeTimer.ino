/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: WeTimer.ino is part of WeTimer                             */
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

#include "WeTimer.h"

//--------------------------------------------------------------------
// Variables globales
//--------------------------------------------------------------------

char cli_ssid[32] = DEFAULT_CLI_SSID;
char cli_pwd[63]  = DEFAULT_CLI_PWD;
char ap_ssid[32]  = DEFAULT_AP_SSID;
char ap_pwd[63]   = DEFAULT_AP_PWD;

unsigned int  delaiArmement = 2;     // Secondes
unsigned int  tempsVol      = 180;   // Secondes
unsigned int  servoDepart   = 1000;  // Microsecondes
unsigned int  servoDT       = 2000;  // Microsecondes

/* hostname for mDNS. Should work at least on windows. Try http://minuterie.local */
const char *myHostname = APP_NAME;

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(10, 10, 10, 10);
IPAddress netMsk(255, 255, 255, 0);

unsigned long debut;
int crochet = 0;

Servo servo;         // Variable globale DT servo

int timerStatus = STATUS_DT;

unsigned long blinkInterval = 1000; // 1 secondes
unsigned long previousBlink = 0;
int ledState = HIGH;


//--------------------------------------------------------------------
// Initialisation
//--------------------------------------------------------------------
void setup() {

  #ifdef debug
    Serial.begin(115200);
    delay(500);
    Serial.flush();
    Serial.println("\n");
    Serial.flush();
  #endif

  EEPROM.begin(EEPROM_LENGTH);
  
  // Récupération des paramètres dans la Flash ou de leur valeur par défaut
  delaiArmement = EEPROM_readInt(ADDR_TEMPS_ARMEMENT); if (delaiArmement == 0xFFFF) delaiArmement = DEFAULT_TEMPS_ARMEMENT;
  tempsVol      = EEPROM_readInt(ADDR_TEMPS_VOL);      if (tempsVol      == 0xFFFF) tempsVol      = DEFAULT_TEMPS_VOL;
  servoDepart   = EEPROM_readInt(ADDR_SERVO_DEPART);   if (servoDepart   == 0xFFFF) servoDepart   = DEFAULT_SERVO_DEPART;
  servoDT       = EEPROM_readInt(ADDR_SERVO_DT);       if (servoDT       == 0xFFFF) servoDT       = DEFAULT_SERVO_DT;
  char charTmp = char(EEPROM.read(ADDR_CLI_SSID));
  if (charTmp != 0xFF) {
    cli_ssid[0] = charTmp;
    for (int i=1; i<32; i++) {
      cli_ssid[i] = char(EEPROM.read(ADDR_CLI_SSID + i));
    }
  }
  charTmp = char(EEPROM.read(ADDR_CLI_PWD));
  if (charTmp != 0xFF) {
    cli_pwd[0] = charTmp;
    for (int i=1; i<32; i++) {
      cli_pwd[i] = char(EEPROM.read(ADDR_CLI_PWD + i));
    }
  }
  charTmp = char(EEPROM.read(ADDR_AP_SSID));
  if (charTmp != 0xFF) {
    ap_ssid[0] = charTmp;
    for (int i=1; i<32; i++) {
      ap_ssid[i] = char(EEPROM.read(ADDR_AP_SSID + i));
    }
  } else {
    String SSID_MAC = String(DEFAULT_AP_SSID + WiFi.softAPmacAddress().substring(9));
    SSID_MAC.toCharArray(ap_ssid, 32);
  }
  charTmp = char(EEPROM.read(ADDR_AP_PWD));
  if (charTmp != 0xFF) {
    ap_pwd[0] = charTmp;
    for (int i=1; i<32; i++) {
      ap_pwd[i] = char(EEPROM.read(ADDR_AP_PWD + i));
    }
  }

  #ifdef debug
    Serial.print("delaiArmement = ");
    Serial.println(delaiArmement);
    Serial.print("tempsVol      = ");
    Serial.println(tempsVol);
    Serial.print("servoDepart   = ");
    Serial.println(servoDepart);
    Serial.print("servoDT       = ");
    Serial.println(servoDT);
    Serial.print("cli_ssid      = ");
    Serial.println(cli_ssid);
    Serial.print("cli_pwd       = ");
    Serial.println(cli_pwd);
    Serial.print("ap_ssid       = ");
    Serial.println(ap_ssid);
    Serial.print("ap_pwd        = ");
    Serial.println(ap_pwd);
  #endif
  
  servo.attach(PIN_SERVO);
  servo.writeMicroseconds(servoDT);
  
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(GND_SWITCH, OUTPUT);
  digitalWrite(GND_SWITCH, LOW);
  
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, ledState);

  // Connection Acces Point si SSID défini
  if (cli_ssid[0] != '\0') {
    #ifdef debug
      Serial.print("Connexion à "); Serial.print(cli_ssid);
      Serial.flush();
    #endif
    debut = millis();
    WiFi.begin(cli_ssid, cli_pwd);
    while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      #ifdef debug
        Serial.print(".");
        Serial.flush();
      #endif
      if (millis() - debut > 10000) {
        break; // Timeout = 10 secondes
      }
    }
    #ifdef debug
      if(WiFi.status() == WL_CONNECTED) {
        Serial.println("OK");
        Serial.print("IP = ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("FAIL");
      } 
      Serial.print("\nConfiguring access point, SSID = ");
      Serial.print(ap_ssid);
      Serial.println("...");
      Serial.flush();
    #endif
  }
  
  /* AP ouverte si pas de mot de passe. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  if (ap_pwd[0] == '\0') {
    WiFi.softAP(ap_ssid); 
  } else {
    WiFi.softAP(ap_ssid, ap_pwd);
  }
  delay(500); // Without delay I've seen the IP address blank
  #ifdef debug
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    Serial.flush();
  #endif

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.on("/", handleRoot);
  server.on("/setparams", handleSetParams);
  server.on("/wifi", handleWiFi);
  server.on("/rdt", handleRDT);
  server.on("/factory", handleFactory);
  server.onNotFound(handleNotFound);
  server.begin(); // Web server start
  #ifdef debug
    Serial.println("HTTP server started");
    Serial.println("\ntimerStatus = STATUS_DT");
    Serial.flush();
  #endif

  ledState = LOW;
  digitalWrite(BUILTIN_LED, ledState);

}

//--------------------------------------------------------------------
// Main loop
//--------------------------------------------------------------------
void loop() {

  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
  
  // Actions de la minuterie
  int new_crochet = digitalRead(PIN_SWITCH);
  if (new_crochet != crochet) {
    // Détecte les changement d'état du switch du crochet
    if (new_crochet == CROCHET_TENDU) {
      // On vient d'enfoncer la pédale du crochet
      if (timerStatus == STATUS_DT) {
        // On était en DT, on arme la minuterie
        servo.writeMicroseconds(servoDepart);
        timerStatus = STATUS_ARMEE;
        debut = millis();
        blinkInterval = 1000; // 1 secondes
        #ifdef debug
          Serial.println("Servo en position départ.");
          Serial.println("timerStatus = STATUS_ARMEE");
          Serial.print("debut = "); Serial.println(debut);
          Serial.flush();
        #endif
      } else if (timerStatus == STATUS_ARMEE) {
        debut = millis();
        #ifdef debug
          Serial.print("Armement, debut = "); Serial.println(debut);
          Serial.flush();
        #endif
      } else if (timerStatus == STATUS_VOL) {
        // Annulation du vol => reset en position Treuil
        timerStatus = STATUS_TREUIL;
        blinkInterval = 125; // 1/8 secondes
        #ifdef debug
          Serial.println("timerStatus = STATUS_TREUIL");
          Serial.flush();
        #endif
      }
    } else { // new_crochet == CROCHET_RELACHE
      // Le cable vient de se relacher
      if (timerStatus == STATUS_TREUIL) {
        // Début du vol
        timerStatus = STATUS_VOL;
        debut = millis();
        blinkInterval = 500; // 1/2 secondes
        #ifdef debug
          Serial.println("timerStatus = STATUS_VOL");
          Serial.print("Vol, debut = "); Serial.println(debut);
          Serial.flush();
        #endif
      }
      ////servo.writeMicroseconds(servoDT);
    }
    #ifdef debug
      Serial.print("crochet=");
      Serial.println(crochet);
      Serial.flush();
    #endif
    crochet = new_crochet;
  }
  // Si le crochet est tendu depuis plus de "delaiArmement" et le status "STATUS_ARMEE", on passe en status STATUS_TREUIL
  if ((timerStatus == STATUS_ARMEE) and (crochet == CROCHET_TENDU) and ((millis() - debut) > delaiArmement * 1000)) {
    timerStatus = STATUS_TREUIL;
    blinkInterval = 125; // 1/8 secondes
    #ifdef debug
      Serial.println("timerStatus = STATUS_TREUIL");
      Serial.flush();
    #endif
  }

  // Si le crochet est relâché depuis plus de "tempsVol" et le status "STATUS_VOL", on déthermalise
  if ((timerStatus == STATUS_VOL) and (crochet == CROCHET_RELACHE) and ((millis() - debut) > tempsVol * 1000)) {
    servo.writeMicroseconds(servoDT);
    timerStatus = STATUS_DT;
    #ifdef debug
      Serial.println("Déthermalise...");
      Serial.println("Servo en position DT.");
      Serial.println("timerStatus = STATUS_DT");
      Serial.flush();
    #endif
  }
  
  // Clignottement de la LED sauf STATUS_DT
  if (timerStatus != STATUS_DT) {
    unsigned long T = millis();
    if (T - previousBlink >= blinkInterval) {
      previousBlink = T;
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
      digitalWrite(BUILTIN_LED, ledState);
    }
  } else {
    ledState = HIGH;
    digitalWrite(BUILTIN_LED, ledState);
  }

}
