/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: handleSetParams.cpp is part of WeTimer                     */
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

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleSetParams() {
  String updateSentence = "";
  bool parmUpdated = false;
  
  #ifdef debug
    Serial.println("Mise à jour des paramètres...");
    Serial.print("Number of args received:");
    Serial.println(server.args());
  #endif
  
  for (int i = 0; i < server.args(); i++) {
    #ifdef debug
      Serial.print("Arg n°" + (String)i + " –> ");
      Serial.print(server.argName(i));
      Serial.print(":");
      Serial.println(server.arg(i));
    #endif
    if (strncasecmp(server.argName(i).c_str(), "delaiArmement", (size_t)13) == 0) {
      unsigned int newDelaiArmement = server.arg(i).toInt();
      if ((delaiArmement != newDelaiArmement) and (newDelaiArmement != 0)) {
        delaiArmement = newDelaiArmement;
        parmUpdated = true;
        EEPROM_writeInt(ADDR_TEMPS_ARMEMENT, delaiArmement);
        #ifdef debug
          Serial.print("Mise à jour delaiArmement = ");
          Serial.println(delaiArmement);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "tempsVol", (size_t)8) == 0) {
      unsigned int newTempsVol = server.arg(i).toInt();
      if ((tempsVol != newTempsVol) and (newTempsVol != 0)) {
        tempsVol = newTempsVol;
        parmUpdated = true;
        EEPROM_writeInt(ADDR_TEMPS_VOL, tempsVol);
        #ifdef debug
          Serial.print("Mise à jour tempsVol = ");
          Serial.println(tempsVol);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "servoDepart", (size_t)11) == 0) {
      unsigned int newServoDepart = server.arg(i).toInt();
      if ((servoDepart != newServoDepart) and (newServoDepart >= 1000) and (newServoDepart <= 2000)) {
        servoDepart = newServoDepart;
        parmUpdated = true;
        EEPROM_writeInt(ADDR_SERVO_DEPART, servoDepart);
        #ifdef debug
          Serial.print("Mise à jour servoDepart = ");
          Serial.println(servoDepart);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "servoDT", (size_t)7) == 0) {
      unsigned int newServoDT = server.arg(i).toInt();
      if ((servoDT != newServoDT) and (newServoDT >= 1000) and (newServoDT <= 2000)) {
        servoDT = newServoDT;
        parmUpdated = true;
        EEPROM_writeInt(ADDR_SERVO_DT, servoDT);
        #ifdef debug
          Serial.print("Mise à jour servoDT = ");
          Serial.println(servoDT);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "ap_ssid", (size_t)7) == 0) {
      if (strncmp(ap_ssid, server.arg(i).c_str(), 32) != 0) {
        if (server.arg(i).length() == 0) {
          // Restaure la valeur par défaut du SSID de l'AP si vide
          String SSID_MAC = String(DEFAULT_AP_SSID + WiFi.softAPmacAddress().substring(9));
          SSID_MAC.toCharArray(ap_ssid, 32);
        } else {
          strncpy(ap_ssid, server.arg(i).c_str(), 32);
        }
        parmUpdated = true;
        EEPROM_writeStr(ADDR_AP_SSID, ap_ssid, 32);
        #ifdef debug
          Serial.print("Mise à jour APP SSID = ");
          Serial.println(ap_ssid);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "ap_pwd", (size_t)7) == 0) {
      if (strncmp(ap_pwd, server.arg(i).c_str(), 63) != 0) {
        strncpy(ap_pwd, server.arg(i).c_str(), 63);
        parmUpdated = true;
        EEPROM_writeStr(ADDR_AP_PWD, ap_pwd, 63);
        #ifdef debug
          Serial.print("Mise à jour APP password = ");
          Serial.println(ap_pwd);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "cli_ssid", (size_t)7) == 0) {
      if (strncmp(cli_ssid, server.arg(i).c_str(), 32) != 0) {
        strncpy(cli_ssid, server.arg(i).c_str(), 32);
        parmUpdated = true;
        EEPROM_writeStr(ADDR_CLI_SSID, cli_ssid, 32);
        #ifdef debug
          Serial.print("Mise à jour client SSID = ");
          Serial.println(cli_ssid);
          Serial.flush();
        #endif
      }
    } else if (strncasecmp(server.argName(i).c_str(), "cli_pwd", (size_t)7) == 0) {
      if (strncmp(cli_pwd, server.arg(i).c_str(), 63) != 0) {
        strncpy(cli_pwd, server.arg(i).c_str(), 63);
        parmUpdated = true;
        EEPROM_writeStr(ADDR_CLI_PWD, cli_pwd, 63);
        #ifdef debug
          Serial.print("Mise à jour APP SSID = ");
          Serial.println(ap_ssid);
          Serial.flush();
        #endif
      }
    }
  }

  if (parmUpdated) {
    #ifdef debug
      Serial.print("Committing RAM changes to EEPROM...");
      Serial.flush();
    #endif
    EEPROM.commit();
    #ifdef debug
      Serial.println("OK");
      Serial.flush();
    #endif
  }

  // Retour à la page principale après traitements
  server.sendHeader("Location", "/", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  ////saveCredentials();
  ////connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}
