/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: WeTimer.cpp is part of WeTimer                                */
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

void readSerial(void) {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a cariage return, set a flag so the main loop can
    // do something about it:
    if (inChar == '\r') {
      serialComplete = true;
      if (telnet.isConnected()) telnet.printf("%s", serialInput);
      WT_PRINTF("\n");
    } else {
      // add it to the inputString:
      serialInput[serialPointer++] = inChar;
      serialInput[serialPointer] = '\0';
    }
  }
}
void processSerial(void) {
  readSerial();
  if (serialComplete) {
    
    #ifdef DEBUG_2
      WT_PRINTF("processSerial() : serialInput = <%s>\n", serialInput);
    #endif
    
    if ((strncmp(serialInput, "help", 4) == 0) || ((strlen(serialInput) == 1) && (serialInput[0] == '?'))) {
      //----------------------------------------------------------------
      WT_PRINTF("reboot................... : Redémarre la minuterie\n");
      WT_PRINTF("factory.................. : Restaure tous les paramètres d'usine\n");
      WT_PRINTF("status................... : Renvoie le status en cours\n");
      WT_PRINTF("cservo................... : Renvoie la configuration d'amplitude des servos\n");
      WT_PRINTF("delai.................... : Renvoi la configuration des délais\n");
      WT_PRINTF("pservo................... : Renvoi la configuration de position des servos\n");
      WT_PRINTF("config................... : Renvoi toute la configuration de la minuterie\n");
      WT_PRINTF("wifiscan................. : Liste les réseaux Wifi disponibles\n");
      WT_PRINTF("connect <SSID> <Password> : Connecte la minuterie au réseau \"SSID\"\n");
      WT_PRINTF("wifisave................. : Sauvegarde le réseau Wifi actif en EEPROM\n");
      WT_PRINTF("ls....................... : liste des fichiers LittleFS\n");
      WT_PRINTF("boucle................... : affiche le temps d'exécution de la boucle loop()\n");
      WT_PRINTF("millis................... : affiche le temps millis()\n");
      WT_PRINTF("flash <0|1|2|3>.......... : Active le flash en mode <off|power|deverrouillage|vol>\n");

    } else if (strncmp(serialInput, "reboot", 6) == 0) {
      //----------------------------------------------------------------
      WT_PRINTF("Restarting ESP...\n");
      ESP.restart();

    } else if (strncmp(serialInput, "factory", 7) == 0) {
      //----------------------------------------------------------------
      WT_PRINTF("Reset timer to factory defaults...\n");
      resetFactory();
      WT_PRINTF("Restarting ESP...\n");
      ESP.restart();

    } else if (strncmp(serialInput, "status", 6) == 0) {
      //----------------------------------------------------------------
      WT_PRINTF("cli_ssid.................. = %s\n", cli_ssid);
      WT_PRINTF("cli_pwd................... = %s\n", cli_pwd);
      if (WiFi.status() == WL_CONNECTED) {
        WT_PRINTF("Client IP................. = %s\n", WiFi.localIP().toString().c_str());
        WT_PRINTF("Client gateway IP......... = %s\n", WiFi.gatewayIP().toString().c_str());
        WT_PRINTF("mDNS status............... = %s\n", MDNS.isRunning() ? "RUNNING" : "NOT RUNNING");
      } else {
        WT_PRINTF("WiFi not connected.\n");
      }
      WT_PRINTF("\nWiFi access point:\n");
      WT_PRINTF("ap_ssid................... = %s\n", ap_ssid);
      WT_PRINTF("ap_pwd.................... = %s\n", ap_pwd);
      WT_PRINTF("description............... = %s\n", description);
      apListClients();
      WT_PRINTF("\nTimer status:\n");
      WT_PRINTF("Status en cours........... = %s\n", getStatusText(timerStatus));
      WT_PRINTF("Etat du switch crochet n°1 = %s\n", (crochet[0] == SWITCH_ON) ? "SWITCH_ON" : "SWITCH_OFF");
      WT_PRINTF("Etat du switch crochet n°2 = %s\n", (crochet[1] == SWITCH_ON) ? "SWITCH_ON" : "SWITCH_OFF");

    } else if (strncmp(serialInput, "cservo", 6) == 0) {
      //----------------------------------------------------------------
      for (int i=0; i<NB_SERVOS; i++) {
        for (int j=0; j<NB_CONFIG; j++) {
          WT_PRINTF("Config.servo, cservo[%d][%d].. = %d\n", i, j , cservo[i][j]);
        }
      }

    } else if (strncmp(serialInput, "delai", 5) == 0) {
      //----------------------------------------------------------------
      for (int i=0; i<NB_DELAI; i++) {
        WT_PRINTF("Conf.delai, delai[%d]........ = %d\n", i, delai[i]);
      }

    } else if (strncmp(serialInput, "pservo", 6) == 0) {
      //----------------------------------------------------------------
      for (int i=0; i<NB_SERVOS; i++) {
        for (int j=0; j<NB_POSITIONS; j++) {
          WT_PRINTF("Position.servo, pservo[%d][%d] = %d\n", i, j , pservo[i][j]);
        }
      }

    } else if (strncmp(serialInput, "config", 6) == 0) {
      //----------------------------------------------------------------
      for (int i=0; i<NB_SERVOS; i++) {
        for (int j=0; j<NB_CONFIG; j++) {
          WT_PRINTF("Config.servo, cservo[%d][%d].. = %d\n", i, j , cservo[i][j]);
        }
      }
      for (int i=0; i<NB_DELAI; i++) {
        WT_PRINTF("Conf.delai, delai[%d]........ = %d\n", i, delai[i]);
      }
      for (int i=0; i<NB_SERVOS; i++) {
        for (int j=0; j<NB_POSITIONS; j++) {
          WT_PRINTF("Position.servo, pservo[%d][%d] = %d\n", i, j , pservo[i][j]);
        }
      }

    } else if (strncmp(serialInput, "wifiscan", 8) == 0) {
      //----------------------------------------------------------------
      getWifiNetworks();

    } else if (strncmp(serialInput, "connect", 7) == 0) {
      //----------------------------------------------------------------
      connexion cWifi = getConnexion(serialInput);
      WT_PRINTF("Nouveau SSID = <%s>\n", cWifi.ssid);
      WT_PRINTF("Nouveau pwd  = <%s>\n", cWifi.pwd);
      if (WiFi.isConnected()) {
        // Déconnexion du réseau en cours
        WT_PRINTF("Déconnexion du réseau en cours (%s)\n", ap_ssid);
        MDNS.end();
        WiFi.disconnect(false);
      }
      strcpy(cli_ssid, cWifi.ssid);
      strcpy(cli_pwd, cWifi.pwd);
      if (cli_ssid[0] != '\0') {
        // Connexion au nouveau SSID
        wifiClientInit();
      }

    } else if (strncmp(serialInput, "wifisave", 8) == 0) {
      //----------------------------------------------------------------
      // Sauvegarde les paramètres wifi dans l'EEPROM
      EEPROM_writeStr(ADDR_CLI_SSID, cli_ssid, MAX_SSID_LEN);
      EEPROM_writeStr(ADDR_CLI_PWD, cli_pwd, MAX_PWD_LEN);
      EEPROM.commit();

    } else if ((strlen(serialInput) == 2) && (strncmp(serialInput, "ls", 2) == 0)) {
      //----------------------------------------------------------------
      ls("/");

    } else if ((strlen(serialInput) == 6) && (strncmp(serialInput, "boucle", 6) == 0)) {
      //----------------------------------------------------------------
      WT_PRINTF("Durée de la boucle principale = %lu millisecondes (%0.3fs)\n", dureeBoucle, ((float)dureeBoucle/1000));

    } else if ((strlen(serialInput) == 6) && (strncmp(serialInput, "millis", 6) == 0)) {
      //----------------------------------------------------------------
      WT_PRINTF("%lu millisecondes depuis le derniers démarrage\n", millis());

    } else if ((strlen(serialInput) > 6) && (strncmp(serialInput, "flash", 5) == 0)) {
      //----------------------------------------------------------------
      if        ((strlen(&serialInput[6]) == 1) && (serialInput[6] == '0')) {
        setFlasher(FLASH_OFF);
      } else if ((strlen(&serialInput[6]) == 1) && (serialInput[6] == '1')) {
        setFlasher(FLASH_POWER);
      } else if ((strlen(&serialInput[6]) == 1) && (serialInput[6] == '2')) {
        setFlasher(FLASH_DEVERROUILLAGE);
      } else if ((strlen(&serialInput[6]) == 1) && (serialInput[6] == '3')) {
        setFlasher(FLASH_VOL);
      } else {
        WT_PRINTF("Flash mode invalide (%s)\n", &serialInput[6]);
        WT_PRINTF("Les modes valides sont : 0: off, 1: power, 2: deverrouillage et 3: vol\n");
      }

    } else {
      //----------------------------------------------------------------
      if (serialInput[0] != '\0') {
        WT_PRINTF("%s: Unknow command.\n", serialInput);
      }
    }
    // clear the input string:
    serialInput[0] = '\0';
    serialPointer = 0;
    serialComplete = false;
    // Print prompt
    WT_PRINTF("> ");
  }
}
connexion getConnexion(const char* chaine) {
  // Retrouve le 2ème champ et la fin de la chaine passée en argument
  // le séparateur utilisé est l'espace
  // la chaine est du type "connect SSID mot de passe.
  
  char tmpStr[SERIAL_BUFF_LEN];
  char* p;
  connexion result;
  
  WT_PRINTF("Entrée dans getConnexion(%s)\n", chaine);

  // Copie la chaine d'entrée après le mot "connect "
  strcpy(tmpStr, &chaine[8]);
  if (strlen(tmpStr) > 0) {
    // Cherche la position du premier caractère espace
    if (p = strchr(tmpStr, ' ')) {;
      // On sépare la chaine en 2 sur l'espace trouvé
      // et on envoi les 2 dans ssid et pwd
      *p++ = '\0';
      strcpy(result.ssid, tmpStr);
      strcpy(result.pwd, p);
    } else {
      // l'espace n'a pas été trouvé, mot de passe vide
      strcpy(result.ssid, tmpStr);
      result.pwd[0]  = '\0';
    }
  } else {
    result.ssid[0] = '\0';
    result.pwd[0]  = '\0';
  }
  return result;
}
void setServoPos(unsigned int servoNum, int servoPos) {

  // Positionnement d'un servo en tenant compte de la configuration d'échelle
  // L'echelle est définie par les variables cservo[servoNum][0-3] avec :
  // cservo[servoNum][0] position extrémité 1 programmée
  // cservo[servoNum][1] position extrémité 2 programmée
  // cservo[servoNum][2] position microseconds extrémité 1 
  // cservo[servoNum][3] position microseconds extrémité 2

  #ifdef DEBUG_2
    WT_PRINTF("Entrée dans setServoPos(%u, %u)\n", servoNum, servoPos);
  #endif
  
  if ((servoNum <0) || (servoNum >= NB_SERVOS)) {
    WT_PRINTF("setServoPos(%u, %u) : ERREUR servoNum !!!", servoNum, servoPos);
    return;
  }
  if ((servoPos <0) || (servoNum >= NB_POSITIONS)) {
    WT_PRINTF("setServoPos(%u, %u) : ERREUR servoPos !!!", servoNum, servoPos);
    return;
  }
  
  int theMap = map(pservo[servoNum][servoPos], cservo[servoNum][0], cservo[servoNum][1], cservo[servoNum][2], cservo[servoNum][3]);

  #ifdef DEBUG_2
    WT_PRINTF("Mapping position = map(%u, %u, %u, %u, %u) = %u\n", pservo[servoNum][servoPos], cservo[servoNum][0], cservo[servoNum][1], cservo[servoNum][2], cservo[servoNum][3], theMap);
  #endif

  servo[servoNum].writeMicroseconds(theMap);

}
void servoTest(unsigned int mode, unsigned int servoNum, int limit1, int limit2, int pwm1, int pwm2, int val) {

  // Teste la config d'un servo
  WT_PRINTF("Entrée dans servoTest(%u, %u, %u, %u, %u, %u, %u)\n", mode , servoNum, limit1, limit2, pwm1, pwm2, val);
  // Positionne le servo à la valeur à tester
  if (mode == 0) { // Mode valeur de programmation minuterie
    servo[servoNum].writeMicroseconds(map(val, limit1, limit2, pwm1, pwm2));
  } else {
    servo[servoNum].writeMicroseconds(val);
  }
  WT_PRINTF("servoTest() : servo positionné\n");
  // Attend un peu
  delay(10);
  yield();

}
float getTension() {
  // Lecture du convertisseur analogique / numérique
  int sensorValue = analogRead(PIN_BATTERIE);
  // Conversion en fonction du diviseur de tension interne à la carte Wemos
  float volt0 = sensorValue * 3.3 / 1024;
  // Conversion en fonction du diviseur externe (R1 et R2) + erreur de mesure
  float tension = volt0 * V_COEF * V_ERR;
  #ifdef DEBUG_WEB_2
    WT_PRINTF("getTension() : sensorValue = %d - voltage = %f\n", sensorValue, tension);
  #endif
  return tension;
}
String getStatusText(const int status) {
  const char tblStatusText[][16] = {
    "PARC",
    "ARMEMENT",
    "TREUIL_MONTEE",
    "TREUIL_VIRAGE",
    "DEVERROUILLE",
    "LARGUE",
    "PITCHUP",
    "MONTEE_1",
    "MONTEE_2",
    "BUNT",
    "PLANE_1",
    "PLANE_2",
    "DT",
  };
  return String(tblStatusText[status]);
}
void IRAM_ATTR externRDT(void) {
    // TODO Code à écrire;
    recuExternRDT = true;
}
void runDT(void) {
  // On commence par l'action sur les servos
  for (int i=0; i<NB_SERVOS; i++) {
    setServoPos(i, POSITION_DT);
  }

  // Mise à jour de l'instant pour revenir au mode PARC en cas de RDT
  temps.dt   = millis();
  temps.parc = temps.dt + delai[8] * 10;

  // Acquitement du signal RDT
  recuExternRDT = false;

  timerStatus = STATUS_DT;
  webSocketSend("STATUS", getStatusText(timerStatus));
  #ifdef DEBUG
    WT_PRINTF("runDT() : Passage au STATUS_DT\n");
    Serial.flush();
  #endif
}
void calculInstants(void) {
  // Calcul des instants (millis() de chaque phases en fonction du moment de départ
  // millis() top largage = temps.largage; 
  temps.pitchup = temps.largage + delai[1] * 10;
  temps.montee1 = temps.pitchup + delai[2] * 10;
  temps.montee2 = temps.montee1 + delai[3] * 10;
  temps.bunt    = temps.montee2 + delai[4] * 10;
  temps.plane1  = temps.bunt    + delai[5] * 10;
  temps.plane2  = temps.plane1  + delai[6] * 10;
  temps.dt      = temps.largage + delai[7] * 10;
  temps.parc    = temps.dt      + delai[8] * 10;
}
String escapeXML(const String buffer) {
  // Caractère ne devant pas apparaître dans un XML :
  // "   &quot;
  // '   &apos;
  // <   &lt;
  // >   &gt;
  // &   &amp;
  String result = buffer;
  result.replace("&", "&amp;");
  result.replace("\"", "&quot;");
  result.replace("'", "&apos;");
  result.replace("<", "&lt;");
  result.replace(">", "&gt;");
  return result;
}
String unescapeXML(const String buffer) {
  String result = buffer;
  result.replace("&gt;", ">");
  result.replace("&lt;", "<");
  result.replace("&apos;", "'");
  result.replace("&quot;", "\"");
  result.replace("&amp;", "&");
  return result;
}
void ls(String path) {
  // Liste les fichiers sur la partie Littlefs
  Dir dir;
  File f;
  dir = LittleFS.openDir(path);
  if (dir.isDirectory()) {
    WT_PRINTF("d %s\n",dir.fileName().c_str());
  }
  while(dir.next()){
    WT_PRINTF("- %s%s\t%d\n", path.c_str(), dir.fileName().c_str(), dir.fileSize());
    if (dir.isDirectory()) {
      ls(path + dir.fileName() + "/");
    }
  }
  yield();
}
