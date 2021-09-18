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

unsigned int  delaiArmement     = DEFAULT_TEMPS_ARMEMENT;      // Secondes
unsigned int  tempsVol          = DEFAULT_TEMPS_VOL;           // Secondes
unsigned int  servoStabVol      = DEFAULT_SERVO_STAB_VOL;      // Microsecondes
unsigned int  servoStabTreuil   = DEFAULT_SERVO_STAB_TREUIL;   // Microsecondes
unsigned int  servoStabDT       = DEFAULT_SERVO_STAB_DT;       // Microsecondes
unsigned int  servoDeriveVol    = DEFAULT_SERVO_DERIVE_VOL;    // Microsecondes
unsigned int  servoDeriveTreuil = DEFAULT_SERVO_DERIVE_TREUIL; // Microsecondes

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

Servo servoStab;   // Variable globale servo stabilisateur
Servo servoDerive; // Servo pour la dérive

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
  delaiArmement     = EEPROM_readInt(ADDR_TEMPS_ARMEMENT);      if (delaiArmement     == 0xFFFF) delaiArmement     = DEFAULT_TEMPS_ARMEMENT;
  tempsVol          = EEPROM_readInt(ADDR_TEMPS_VOL);           if (tempsVol          == 0xFFFF) tempsVol          = DEFAULT_TEMPS_VOL;
  servoStabVol      = EEPROM_readInt(ADDR_SERVO_STAB_VOL);      if (servoStabVol      == 0xFFFF) servoStabVol      = DEFAULT_SERVO_STAB_VOL;
  servoStabTreuil   = EEPROM_readInt(ADDR_SERVO_STAB_TREUIL);   if (servoStabTreuil   == 0xFFFF) servoStabTreuil   = DEFAULT_SERVO_STAB_TREUIL;
  servoStabDT       = EEPROM_readInt(ADDR_SERVO_STAB_DT);       if (servoStabDT       == 0xFFFF) servoStabDT       = DEFAULT_SERVO_STAB_DT;
  servoDeriveVol    = EEPROM_readInt(ADDR_SERVO_DERIVE_VOL);    if (servoDeriveVol    == 0xFFFF) servoDeriveVol    = DEFAULT_SERVO_DERIVE_VOL;
  servoDeriveTreuil = EEPROM_readInt(ADDR_SERVO_DERIVE_TREUIL); if (servoDeriveTreuil == 0xFFFF) servoDeriveTreuil = DEFAULT_SERVO_DERIVE_TREUIL;
  
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
    Serial.print("delaiArmement     = "); Serial.println(delaiArmement);
    Serial.print("tempsVol          = "); Serial.println(tempsVol);
    Serial.print("servoStabVol      = "); Serial.println(servoStabVol);
    Serial.print("servoStabTreuil   = "); Serial.println(servoStabTreuil);
    Serial.print("servoStabDT       = "); Serial.println(servoStabDT);
    Serial.print("servoDeriveVol    = "); Serial.println(servoDeriveVol);
    Serial.print("servoDeriveTreuil = "); Serial.println(servoDeriveTreuil);
    Serial.print("cli_ssid          = "); Serial.println(cli_ssid);
    Serial.print("cli_pwd           = "); Serial.println(cli_pwd);
    Serial.print("ap_ssid           = "); Serial.println(ap_ssid);
    Serial.print("ap_pwd            = "); Serial.println(ap_pwd);
  #endif
  
  servoStab.attach(PIN_SERVO_STAB);
  servoStab.writeMicroseconds(servoStabDT);
  servoDerive.attach(PIN_SERVO_DERIVE);
  servoDerive.writeMicroseconds(servoDeriveVol);
  #ifdef debug
    Serial.print("setup() : Servo stab en position DT ("); Serial.print(servoStabDT); Serial.println(")");
    Serial.print("setup() : Servo derive en position vol ("); Serial.print(servoDeriveVol); Serial.println(")");
  #endif
  
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(GND_SWITCH, OUTPUT);
  digitalWrite(GND_SWITCH, LOW);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, ledState);

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
  server.on("/setservo", handleSetservo);
  server.on("/factory", handleFactory);
  server.onNotFound(handleNotFound);
  server.begin(); // Web server start
  #ifdef debug
    Serial.println("HTTP server started");
    Serial.println("\ntimerStatus = STATUS_DT");
    Serial.flush();
  #endif

  ledState = LOW;
  digitalWrite(LED_BUILTIN, ledState);

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
      // On positionne les 2 servos en mode treuillage
      servoStab.writeMicroseconds(servoStabTreuil);
      servoDerive.writeMicroseconds(servoDeriveTreuil);
      #ifdef debug
        Serial.print("Tension du crochet : Stab en position treuil,   servoStabTreuil   = "); Serial.println(servoStabTreuil);
        Serial.print("Tension du crochet : Dérive en position treuil, servoDeriveTreuil = "); Serial.println(servoDeriveTreuil);
      #endif
      if (timerStatus == STATUS_DT) {
        // On était en DT, on arme la minuterie
        timerStatus = STATUS_ARMEE;
        debut = millis();
        blinkInterval = 1000; // 1 secondes
        #ifdef debug
          ////Serial.println("Servo en position départ.");
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
      // Le cable vient de se relacher,
      // On positionne les 2 servos en mode plané si on est pas déthermalisé
      if (timerStatus != STATUS_DT) {
        servoStab.writeMicroseconds(servoStabVol);
        servoDerive.writeMicroseconds(servoDeriveVol);
        #ifdef debug
          Serial.print("Relâchement du crochet : Stab en position vol,   servoStabVol   = "); Serial.println(servoStabVol);
          Serial.print("Relâchement du crochet : Dérive en position vol, servoDeriveVol = "); Serial.println(servoDeriveVol);
        #endif
      }
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
    }
    #ifdef debug
      Serial.print("crochet=");
      Serial.println(crochet);
      Serial.flush();
    #endif
    crochet = new_crochet;
  } // if (new_crochet != crochet)
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
    servoStab.writeMicroseconds(servoStabDT);
    timerStatus = STATUS_DT;
    #ifdef debug
      Serial.print("Déthermalise : Stab en position DT,   servoStabVol = "); Serial.println(servoStabDT);
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
      digitalWrite(LED_BUILTIN, ledState);
    }
  } else {
    ledState = HIGH;
    digitalWrite(LED_BUILTIN, ledState);
  }

}
