/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: handleHttp.cpp is part of WeTimer                          */
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

/** Redirect to captive portal if we got a request for another domain. Return true in that case so the page handler do not try to handle the request again. */
bool captivePortal() {
  if (!isIp(server.hostHeader()) && server.hostHeader() != (String(myHostname) + ".local")) {
    #ifdef debug
    Serial.println("Request redirected to captive portal");
    #endif
    server.sendHeader("Location", String("http://") + toStringIp(server.client().localIP()), true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}

void handleNotFound() {
  if (captivePortal()) { // If caprive portal redirect instead of displaying the error page.
    return;
  }
  String message = F("File Not Found : ");
  //message += F("URI: ");
  message += server.uri();
  //message += F("\nMethod: ");
  //message += (server.method() == HTTP_GET) ? "GET" : "POST";
  //message += F("\nArguments: ");
  //message += server.args();
  message += F("\n");
  //for (uint8_t i = 0; i < server.args(); i++) {
  //  message += String(F(" ")) + server.argName(i) + F(": ") + server.arg(i) + F("\n");
  //}
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);
}


void handleRDT() {

  // Force le DT
  servo.writeMicroseconds(servoDT);
  timerStatus = STATUS_DT;
  #ifdef debug
    Serial.println("RDT : Déthermalise...");
    Serial.println("Servo en position DT.");
    Serial.println("timerStatus = STATUS_DT");
    Serial.flush();
  #endif

  server.sendHeader("Location", "/", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  ////saveCredentials();
  ////connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}


void handleFactory() {

  #ifdef debug
    Serial.print("Formatage EEPROM...");
    Serial.flush();
  #endif

  // Formate l'EEPROM pour rétablir les paramètres par défaut
  EEPROM_format();

  #ifdef debug
    Serial.println("OK\nEEPROM : formatage terminé");
    Serial.flush();
  #endif

  server.sendHeader("Location", "/", true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
  ////saveCredentials();
  ////connect = strlen(ssid) > 0; // Request WLAN connect with new credentials if there is a SSID
}
