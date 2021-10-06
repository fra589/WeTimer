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

// Redirect to captive portal if we got a request for another domain.
// Return true in that case so the page handler do not try to handle the request again.
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


// Entêtes des pages HTML
String htmlEntete() {

  String Buff = "<!DOCTYPE html>\n";

  Buff += F(
    "<html>\n"
    "<head>\n"
    "<title>WeTimer</title>\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" charset=\"utf-8\">\n"
    "<link rel=\"icon\" href=\"data:,\">\n"
    "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">"
    "<script src=\"script.js\"></script>"
    "</head>\n"
    "<body onload=\"calculTemps()\">\n"
    "<h1><span class=\"span_title\">WeTimer</span></h1>\n"
  );
  //--------------------------------------------------------------------------------
  Buff += F(
    "<p class=\"p0\">Minuterie status = <span class=\"timer_status\">"
  );
  if (timerStatus == STATUS_DT)          Buff += F("STATUS_DT");
  else if (timerStatus == STATUS_ARMEE)  Buff += F("STATUS_ARMEE");
  else if (timerStatus == STATUS_TREUIL) Buff += F("STATUS_TREUIL");
  else if (timerStatus == STATUS_VOL)    Buff += F("STATUS_VOL");
  Buff += F(
    "</span></p>\n"
    "<hr>\n"
  );

  return Buff;

}


// Renvoi le text HTML du status WiFi
String netStatus() {

  String Buff;

  Buff  = F("<hr>\n");
  Buff += F("    <p style=\"text-align: center;\">AP SSID  = ");
  Buff += String(ap_ssid);
  Buff += F("<br />\n");
  Buff += F("AP MAC = ");
  Buff += WiFi.softAPmacAddress();
  Buff += F("<br />\n");
  Buff += F("AP IP addr = ");
  Buff += toStringIp(WiFi.softAPIP());  
  if(WiFi.status() == WL_CONNECTED) {
    Buff += F("<br />\n");
    Buff += F("CLI SSID  = ");
    Buff += String(cli_ssid);
    Buff += F("<br />\n");
    Buff += F("CLI MAC = ");
    Buff += WiFi.macAddress();
    Buff += F("<br />\n");
    Buff += F("CLI IP addr = ");
    Buff += toStringIp(WiFi.localIP());
  }
  Buff += F("</p>\n");

  return Buff;

}


String piedPage() {

  String Buff;

  Buff += F(
    "<p class=\"p1\">Minuterie planeur Papi - "
  );
  Buff += APP_VERSION_STRING;
  Buff += F("</p>\n");
  Buff += F(
    "<p style=\"text-align: center; margin: 0;\">Copyright &copy; 2021 - Gauthier Brière</p>\n"
  );
  Buff += F("</body>\n</html>\n");

  return Buff;  

}


void handleNotFound() {

  if (captivePortal()) { // If captive portal redirect instead of displaying the error page.
    return;
  }

  String message = F("File Not Found : ");
  message += server.uri();
  message += F("\n");

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);

}


void handleRDT() {

  String urlRetour = "/";

  // Force le DT
  servoStab.writeMicroseconds(servoStabDT);
  timerStatus = STATUS_DT;
  #ifdef debug
    Serial.println("RDT : Déthermalise...");
    Serial.println("Servo en position DT.");
    Serial.println("timerStatus = STATUS_DT");
    Serial.flush();
  #endif

  // Récupère l'adresse de retour dans les arguments GET
  for (int i = 0; i < server.args(); i++) {
    #ifdef debug
      Serial.print("Arg n°" + (String)i + " –> ");
      Serial.print(server.argName(i));
      Serial.print(":");
      Serial.println(server.arg(i));
    #endif
    if (strncasecmp(server.argName(i).c_str(), "retour", (size_t)6) == 0) {
      urlRetour += server.arg(i);
    }
  }

  server.sendHeader("Location", urlRetour, true);
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");    // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length

}


void handleSetservo() {

  unsigned int servoNum = 0; // 0=> Non connu, 1 => stab, 2 => derive
  unsigned int newValue = 0; // nouvelle valeur a affecter...
  String XML;

  #ifdef debug
    Serial.println("**************** handleSetservo() ****************");
  #endif

  for (int i = 0; i < server.args(); i++) {
    // décode les arguments
    #ifdef debug
      Serial.print("Arg n°" + (String)i + " –> ");
      Serial.print(server.argName(i));
      Serial.print(":");
      Serial.println(server.arg(i));
    #endif
    if (strncasecmp(server.argName(i).c_str(), "servo", (size_t)5) == 0) {
      if (strncasecmp(server.arg(i).c_str(), "stab", (size_t)4) == 0) {
        servoNum = 1;
      } else if (strncasecmp(server.arg(i).c_str(), "derive", (size_t)6) == 0) {
        servoNum = 2;
      }
    } else if (strncasecmp(server.argName(i).c_str(), "valeur", (size_t)6) == 0) {
      unsigned int newValueTemp = server.arg(i).toInt();
      if ((newValueTemp >= MIN_SERVO_MICROSECONDS) && (newValueTemp <= MAX_SERVO_MICROSECONDS)) {
        newValue = newValueTemp;
      }
    }
  }

  // positionne le servo
  if ((servoNum == 1) && (newValue != 0)) {
    servoStab.writeMicroseconds(newValue);
  } else if ((servoNum == 2) && (newValue != 0)) {
    servoDerive.writeMicroseconds(newValue);
  }

  // Renvoi la réponse au client http
  XML =F("<?xml version='1.0'?>");
  XML+=F("<response>");
  XML+=F("OK");
  XML+=F("</response>");
  server.send(200,"text/xml",XML);

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

}
