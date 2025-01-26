/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: WeTimer.ino is part of WeTimer                                */
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

#include "WeTimer.h"

//----------------------------------------------------------------------
// Variables globales
//----------------------------------------------------------------------

char cli_ssid[32] = DEFAULT_CLI_SSID;
char cli_pwd[63]  = DEFAULT_CLI_PWD;
char ap_ssid[32]  = DEFAULT_AP_SSID;
char ap_pwd[63]   = DEFAULT_AP_PWD;
char description[DESCRIPTION_LEN] = "";
bool wifiIsSlepping = false;

// hostname pour mDNS : http://minuterie.local
const char *myHostname = APP_NAME;
// DNS server
DNSServer dnsServer;
// Web server sur port 80 et WebSocket server sur port 81
ESP8266WebServer server(HTTP_PORT);
WebSocketsServer webSocket = WebSocketsServer(WEB_SOCKET_PORT);  
// Telnet server
ESPTelnet telnet;
// Soft AP network parameters
IPAddress apIP(10, 10, 10, 10);
IPAddress netMsk(255, 255, 255, 0);
// Servos
Servo servo[NB_SERVOS];              // Variable globale pour 3 servos
unsigned int delai[NB_DELAI];        // On stockeras en millisecondes
int cservo[NB_SERVOS][NB_CONFIG];    // Configuration des servos
int pservo[NB_SERVOS][NB_POSITIONS]; // positions des servos

int timerStatus    = STATUS_PARC;
int timerOldStatus = timerStatus;
struct temps temps = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned long timeArmement = 0;
uint8_t crochet[2] = {SWITCH_OFF, SWITCH_OFF};
volatile bool recuExternRDT = false;
char serialInput[SERIAL_BUFF_LEN]; // Incoming Serial data
int serialPointer = 0;             // last character counter of Serial data
bool serialComplete = false;       // whether the string is complete
unsigned long dureeBoucle = 0;
unsigned long bouclePrecedente;

bool flash_verrou        = DEFAULT_FLASH_VERROU;
bool flash_vol_on        = DEFAULT_FLASH_VOL_ON;
unsigned long tOn        = DEFAULT_ON_TIME;
unsigned long tOff       = DEFAULT_OFF_TIME;
unsigned long tCycle     = DEFAULT_T_CYCLE;
unsigned long nFlash     = DEFAULT_N_FLASH;

//----------------------------------------------------------------------
// Initialisation
//----------------------------------------------------------------------
void setup() {

  // Try pushing frequency to 160MHz.
  //bool update_cpu_freq = system_update_cpu_freq(160);
  // Set CPU frequency to 80MHz.
  bool update_cpu_freq = system_update_cpu_freq(80);

  ////Serial.begin(115200);
  Serial.begin(38400);
  #ifdef DEBUG
    delay(500);
    Serial.flush();
    WT_PRINTF("\n");
    Serial.flush();
    int cpuFreq = system_get_cpu_freq();
    WT_PRINTF("Starting %s on ESP8266@%dMHz (system_update_cpu_freq() = %s)...\n\n", APP_NAME_VERSION, cpuFreq, update_cpu_freq?"true":"false");
    Serial.flush();
  #endif

  // Initialisation entrées / sorties
  pinMode(PIN_LED,      OUTPUT);
  pinMode(PIN_RDT,      INPUT_PULLUP);
  pinMode(PIN_SWITCH_0, INPUT_PULLUP);
  pinMode(PIN_SWITCH_1, INPUT_PULLUP);
  pinMode(PIN_BATTERIE, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_RDT), externRDT, FALLING);

  pinMode(PIN_SERVO_0, OUTPUT); digitalWrite(PIN_SERVO_0, LOW);
  pinMode(PIN_SERVO_1, OUTPUT); digitalWrite(PIN_SERVO_1, LOW);
  pinMode(PIN_SERVO_2, OUTPUT); digitalWrite(PIN_SERVO_2, LOW);

  // Initialisation données de la minuterie par défaut
  defaultInit();
  // Récupération des paramètres EEPROM
  getEepromStartupData();
  
  // Connection cliente acces point si SSID défini
  wifiClientInit();

  // Initialisation point d'accès minuterie
  wifiApInit();

  // Setup web server
  webServerInit();

  // Serveur webSocket
  webSocketInit();
  // Serveur Telnet;
  telnetInit();
  // Initialisation des servos
  setServoPos(0, POSITION_PARC);
  servo[0].attach(PIN_SERVO_0, cservo[0][2], cservo[0][3]);
  #ifdef DEBUG
    WT_PRINTF("setup() : Position initiale Servo[0] en position parc (%d)\n", pservo[0][0]);
  #endif

  setServoPos(1, POSITION_PARC);
  servo[1].attach(PIN_SERVO_1, cservo[1][2], cservo[1][3]);
  #ifdef DEBUG
    WT_PRINTF("setup() : Position initiale Servo[1] en position parc (%d)\n", pservo[1][0]);
  #endif
  setServoPos(2, POSITION_PARC);
  servo[2].attach(PIN_SERVO_2, cservo[2][2], cservo[2][3]);
  #ifdef DEBUG
    WT_PRINTF("setup() : Position initiale Servo[2] en position parc (%d)\n", pservo[2][0]);
  #endif

}

//----------------------------------------------------------------------
// Main loop
//----------------------------------------------------------------------
void loop() {

  uint8_t new_crochet[2];

  // Mesure de la durée de la boucle
  dureeBoucle = millis() - bouclePrecedente;
  bouclePrecedente = millis();

  //--------------------------------------------------------------------
  // Actions de la minuterie
  //--------------------------------------------------------------------
    if (recuExternRDT) {
      #ifdef DEBUG
        WT_PRINTF("RDT externe reçu !\n");
      #endif
      runDT();
    }

  // Verification du délai d'armement de la minuterie
  //--------------------------------------------------------------------
  if (timeArmement != 0) {
    unsigned long duree = millis() - timeArmement;
    if ((timerStatus == STATUS_ARMEMENT) && (crochet[0] == SWITCH_ON) && (duree > (delai[0] * 10))) {
      timerStatus = STATUS_TREUIL_MONTEE;
      webSocketSend("STATUS", getStatusText(timerStatus));
      #ifdef DEBUG
        WT_PRINTF("Passage au STATUS_TREUIL_MONTEE\n");
        WT_PRINTF("timeArmement = %d, millis() = %d, delai[0] * 10 = %d\n", timeArmement, millis(), delai[0] * 10);
        Serial.flush();
      #endif
    }
  }

  // Lecture switch crochet avant
  //--------------------------------------------------------------------
  new_crochet[0] = digitalRead(PIN_SWITCH_0);
  if (new_crochet[0] != crochet[0]) {
    // Mouvement du crochet avant
    crochet[0] = new_crochet[0];
    webSocketSend("SWITCH_0", String(crochet[0]));
    if (isFlashEnCours()) {
      setFlasher(FLASH_OFF);
    }

    if (crochet[0] == SWITCH_ON) {
      // Crochet en position avant : câble tendu.
      // On positionne les servos en mode treuillage montée
      for (int i=0; i<NB_SERVOS; i++) {
        setServoPos(i, POSITION_TREUIL_MONTEE);
      }
      if ((timerStatus == STATUS_PARC) || (timerStatus == STATUS_DT)) {
        // On était en PARC ou DT, on lance le compte de temps
        // d'armement de la minuterie.
        timeArmement = millis();
        // Mémorise l'ancien status (PARC ou DT) pour y revenir en cas
        // de non armement.
        timerOldStatus = timerStatus;
        // Passe en status armement en cours
        timerStatus = STATUS_ARMEMENT;
        webSocketSend("STATUS", getStatusText(timerStatus));
        #ifdef DEBUG
          WT_PRINTF("Passage au STATUS_ARMEMENT\n");
          WT_PRINTF("Init. timeArmement = %d\n", timeArmement);
          Serial.flush();
        #endif
      } else if (timerStatus == STATUS_TREUIL_VIRAGE) {
        timerStatus = STATUS_TREUIL_MONTEE;
        webSocketSend("STATUS", getStatusText(timerStatus));
        #ifdef DEBUG
          WT_PRINTF("Passage au STATUS_TREUIL_MONTEE\n");
          Serial.flush();
        #endif
      }

    } else { // crochet[0] == SWITCH_OFF
      // Crochet en position arrière : câble détendu,
      if (timerStatus == STATUS_DEVERROUILLE) {
        //--------------------------------------------------------------
        //                   Moment du largage !
        //--------------------------------------------------------------
        // Pas de mouvement de servo maintenant. Le premier mouvement
        // sera le PITCHUP après le delai[1]
        temps.largage = millis();
        calculInstants(); // Calcul des phases temporelles
        timerStatus = STATUS_LARGUE;
        webSocketSend("STATUS", getStatusText(timerStatus));
        #ifdef DEBUG
          WT_PRINTF("Passage au STATUS_LARGUE (T = %d)\n", temps.largage);
          Serial.flush();
        #endif
        // Allume la LED en mode vol
        if (flash_vol_on) {
          setFlasher(FLASH_VOL);
        } else {
          // Au cas ou le flash serait en mode déverrouillage
          if (flash_verrou) {
            setFlasher(FLASH_OFF);
          }
        }
      }  else if (timerStatus == STATUS_TREUIL_MONTEE) {
        // On positionne les servos en mode treuillage virage
        for (int i=0; i<NB_SERVOS; i++) {
          setServoPos(i, POSITION_TREUIL_VIRAGE);
        }
        // Et on met à jour le status
        timerStatus = STATUS_TREUIL_VIRAGE;
        webSocketSend("STATUS", getStatusText(timerStatus));
        #ifdef DEBUG
          WT_PRINTF("Passage au STATUS_TREUIL_VIRAGE\n");
          Serial.flush();
        #endif
      } else if (timerStatus == STATUS_ARMEMENT) {
        // délai trop court pour l'armement, on reinitialise et on
        // revient à l'ancien status.
        timeArmement = 0;
        timerStatus = timerOldStatus;
        webSocketSend("STATUS", getStatusText(timerStatus));
        #ifdef DEBUG
          WT_PRINTF("Armement abandonné, retour status précédent\n");
          Serial.flush();
        #endif
      }
    }
  } // (new_crochet[0] != crochet[0])
  
  // Lecture switch crochet verouillage
  //--------------------------------------------------------------------
  new_crochet[1] = digitalRead(PIN_SWITCH_1);
  if (new_crochet[1] != crochet[1]) {
    crochet[1] = new_crochet[1];
    webSocketSend("SWITCH_1", String(new_crochet[1]));

    if (crochet[1] == SWITCH_ON) {
      if ((timerStatus > STATUS_ARMEMENT) && (crochet[0] == SWITCH_ON)) {
        // Switch largage activé => Déverouillage
        // On positionne les servos en mode dÉVERROUILLÉ
        for (int i=0; i<NB_SERVOS; i++) {
          setServoPos(i, POSITION_DEVERROUILLE);
        }
        timerStatus = STATUS_DEVERROUILLE;
        webSocketSend("STATUS", getStatusText(timerStatus));
        #ifdef DEBUG
          WT_PRINTF("Passage au STATUS_DEVERROUILLE\n");
          Serial.flush();
        #endif
        // Allume la LED en mode déverrouillage
        if (flash_verrou) {
          setFlasher(FLASH_DEVERROUILLAGE);
        }
      }
    } else { // crochet[1] == SWITCH_OFF
      ; // A priori, rien a faire pour l'instant...
      // TODO C'est probablement ici qu'on traitera la refermeture
      // d'un crochet piloté par servo
    }
  } // (new_crochet[1] != crochet[1])

  //--------------------------------------------------------------------
  //                 Séquence de vol après largage
  //--------------------------------------------------------------------

  if (timerStatus >= STATUS_LARGUE) {
    unsigned long T = millis();
    switch (timerStatus) {
      case STATUS_LARGUE:
        if (T >= temps.pitchup ) {
          // Déclenchement PITCHUP
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_PITCHUP);
          }
          timerStatus = STATUS_PITCHUP;
          webSocketSend("STATUS", getStatusText(timerStatus));
          #ifdef DEBUG
            WT_PRINTF("Passage au STATUS_PITCHUP (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_PITCHUP:
        if (T >= temps.montee1 ) {
          // Déclenchement MONTEE_1
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_MONTEE_1);
          }
          timerStatus = STATUS_MONTEE_1;
          webSocketSend("STATUS", getStatusText(timerStatus));
          #ifdef DEBUG
            WT_PRINTF("Passage au STATUS_MONTEE_1 (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_MONTEE_1:
        if (T >= temps.montee2 ) {
          // Déclenchement MONTEE_2
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_MONTEE_2);
          }
          timerStatus = STATUS_MONTEE_2;
          webSocketSend("STATUS", getStatusText(timerStatus));
          #ifdef DEBUG
            WT_PRINTF("Passage au STATUS_MONTEE_2 (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_MONTEE_2:
        if (T >= temps.bunt ) {
          // Déclenchement BUNT
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_BUNT);
          }
          timerStatus = STATUS_BUNT;
          webSocketSend("STATUS", getStatusText(timerStatus));
          #ifdef DEBUG
            WT_PRINTF("Passage au STATUS_BUNT (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_BUNT:
        if (T >= temps.plane1 ) {
          // Déclenchement PLANE_1
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_PLANE_1);
          }
          timerStatus = STATUS_PLANE_1;
          webSocketSend("STATUS", getStatusText(timerStatus));
          #ifdef DEBUG
            WT_PRINTF("Passage au STATUS_PLANE_1 (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_PLANE_1:
        if (T >= temps.plane2 ) {
          // Déclenchement PLANE_2
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_PLANE_2);
          }
          timerStatus = STATUS_PLANE_2;
          webSocketSend("STATUS", getStatusText(timerStatus));
          #ifdef DEBUG
            WT_PRINTF("Passage au STATUS_PLANE_2 (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_PLANE_2:
        if (T >= temps.dt) {
          // Déclenchement DT
          runDT();
        }
        break;
      case STATUS_DT:
        if (T >= temps.parc ) {
          // Retour au status parc
          for (int i=0; i<NB_SERVOS; i++) {
            setServoPos(i, POSITION_PARC);
          }
          timerStatus = STATUS_PARC;
          webSocketSend("STATUS", getStatusText(timerStatus));
          temps = (struct temps){ 0, 0, 0, 0, 0, 0, 0, 0, 0 };
          // Eteint la LED
          setFlasher(FLASH_OFF);
          #ifdef DEBUG
            WT_PRINTF("Retour au STATUS_PARC (T = %d)\n", T);
            Serial.flush();
          #endif
        }
        break;
      case STATUS_PARC:
        break;
    }
  }

  //--------------------------------------------------------------------
  // Services Process
  //--------------------------------------------------------------------
  // Flasher
  flasherLoop();
  // mDNS
  if (WiFi.status() == WL_CONNECTED) {
    MDNS.update();
  }
  //DNS
  dnsServer.processNextRequest();
  //HTTP
  server.handleClient();
  // WebSocket
  webSocket.loop();
  // Telnet
  telnet.loop();

  // On laisse du temps au système WiFi...
  delay(1);
  yield();

  //--------------------------------------------------------------------
  // TODO: Transférer les entrées du port série
  // vers un module interpréteur de commandes
  processSerial();

}

