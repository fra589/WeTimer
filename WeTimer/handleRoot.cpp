/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: handleRoot.cpp is part of WeTimer                          */
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

/** Handle root or redirect to captive portal */
void handleRoot() {
  
  const String inputNames[] = {
    F("delaiArmement"),
    F("tempsVol"),
    F("servoStabVol"),
    F("servoStabTreuil"),
    F("servoStabDT"),
    F("servoDeriveVol"),
    F("servoDeriveTreuil")
  };
  const String inputLabels[] = {
    F("Armement (s)"),
    F("Temps de vol (s)"),
    F("Servo stab vol (μs)"),
    F("Servo stab treuil (μs)"),
    F("Servo stab DT (μs)"),
    F("Servo derive vol (μs)"),
    F("Servo derive treuil (μs)")
  };
  String inputValues[7];
  inputValues[0] = String(delaiArmement);
  inputValues[1] = String(tempsVol);
  inputValues[2] = String(servoStabVol);
  inputValues[3] = String(servoStabTreuil);
  inputValues[4] = String(servoStabDT);
  inputValues[5] = String(servoDeriveVol);
  inputValues[6] = String(servoDeriveTreuil);
  String minValues[7];
  minValues[0] = String(0);
  minValues[1] = String(0);
  minValues[2] = String(MIN_SERVO_MICROSECONDS);
  minValues[3] = String(MIN_SERVO_MICROSECONDS);
  minValues[4] = String(MIN_SERVO_MICROSECONDS);
  minValues[5] = String(MIN_SERVO_MICROSECONDS);
  minValues[6] = String(MIN_SERVO_MICROSECONDS);
  String maxValues[7];
  maxValues[0] = String(9999);
  maxValues[1] = String(9999);
  maxValues[2] = String(MAX_SERVO_MICROSECONDS);
  maxValues[3] = String(MAX_SERVO_MICROSECONDS);
  maxValues[4] = String(MAX_SERVO_MICROSECONDS);
  maxValues[5] = String(MAX_SERVO_MICROSECONDS);
  maxValues[6] = String(MAX_SERVO_MICROSECONDS);

  
  const byte buffSize = 128;
  char       buff[buffSize];
  char       rc;
  int        recu=0;

  int   valueInt;

  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }
  
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");

  String Page;
  // Entêtes
  Page += F(
    "<html>\n"
    "<head>\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" charset=\"utf-8\">\n"    
    "<title>WeTimer</title>\n"
  );
  // Ajout des styles et des javascripts
  Page += app_style();
  Page += app_script();
  Page += F(
    "</head>\n"
    "<body>\n"
    "<h1 style=\"margin-bottom: 0;\">WeTimer</h1>\n"
    "<p style=\"margin-top: 0; margin-bottom: 5px; font-style: italic;\">Minuterie planeur Papi - "
  );
  Page += APP_VERSION_STRING;
  Page += F(
    "</p>\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<p style=\"text-align:center; margin: 0px;\">Minuterie status = "
  );
  if (timerStatus == STATUS_DT)          Page += F("<b>STATUS_DT</b>");
  else if (timerStatus == STATUS_ARMEE)  Page += F("<b>STATUS_ARMEE</b>");
  else if (timerStatus == STATUS_TREUIL) Page += F("<b>STATUS_TREUIL</b>");
  else if (timerStatus == STATUS_VOL)    Page += F("<b>STATUS_VOL</b>");
  Page += F(
    "</p>\n"
    "<hr>\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<form id=\"mainForm\" action=\"setparams\" method=\"POST\">\n"
    "<div style=\"text-align: center !important;\">\n"
  );
  //--------------------------------------------------------------------------------
  for (int i=0; i<7; i++) {
    Page += F("<div class=\"nomargin\"><label for=\"");
    Page += inputNames[i];
    Page += F("\">");
    Page += inputLabels[i];
    Page += F("</label>");
    Page += F("<input type=\"button\" onclick=\"changeVal('");
    Page += inputNames[i];
    Page += F("', -5);\" value=\"&lt;&lt;\">");
    Page += F("<input type=\"button\" onclick=\"changeVal('");
    Page += inputNames[i];
    Page += F("', -1);\" value=\"&nbsp;&lt;&nbsp;\">");
    Page += F("<input name=\"");
    Page += inputNames[i];
    Page += F("\" type=\"number\"  min=\"");
    Page += minValues[i];
    Page += F("\" max=\"");
    Page += maxValues[i];
    if (i < 2) {
      Page += F("\" onfocus=\"this.select();\" value=\"");
    } else if (i < 5) {
      Page += F("\" onfocus=\"this.select(); setServoPos('stab', this.value);\" oninput=\"setServoPos('stab', this.value);\" value=\"");
    } else {
      Page += F("\" onfocus=\"this.select(); setServoPos('derive', this.value);\" oninput=\"setServoPos('derive', this.value);\" value=\"");
    }
    Page += inputValues[i];
    Page += F("\"/>");
    Page += F("<input type=\"button\" onclick=\"changeVal('");
    Page += inputNames[i];
    Page += F("', 1);\" value=\"&nbsp;&gt;&nbsp;\">");
    Page += F("<input type=\"button\" onclick=\"changeVal('");
    Page += inputNames[i];
    Page += F("', 5);\" value=\"&gt;&gt;\">");
    Page += F("</div>\n");
  }
  //--------------------------------------------------------------------------------
  Page += F(
    "<p>\n"
    "<div class=\"centreur\">\n"
    "<div class=\"bouton\" onclick=\"document.write(''); document.location.reload(true);\">Relire</div>\n"
    "&nbsp;"
    "<div class=\"bouton\" onclick=\"document.forms['mainForm'].submit();\">Envoi</div>\n"
    "&nbsp;"
    "<div class=\"bouton rouge\" onclick=\"window.location.assign('/rdt');\">RDT</div>\n"
    "</div>\n"
    "</p>\n"

    "</form>\n"
  );
  //--------------------------------------------------------------------------------
  Page += F("<hr>");
  //--------------------------------------------------------------------------------
  Page += F("    <p style=\"text-align: center;\">AP SSID  = ");
  Page += String(ap_ssid);
  Page += F("<br />\n");
  Page += F("AP MAC = ");
  Page += WiFi.softAPmacAddress();
  Page += F("<br />\n");
  Page += F("AP IP addr = ");
  Page += toStringIp(WiFi.softAPIP());  
  Page += F("</p>\n");
  if(WiFi.status() == WL_CONNECTED) {
    Page += F("    <p style=\"text-align: center;\">CLI SSID  = ");
    Page += String(cli_ssid);
    Page += F("<br />\n");
    Page += F("CLI MAC = ");
    Page += WiFi.macAddress();
    Page += F("<br />\n");
    Page += F("CLI IP addr = ");
    Page += toStringIp(WiFi.localIP());
    Page += F("</p>\n");
  }
  Page += F(
    "<p style=\"text-align: center;\"><a href=\"/wifi\">Configurer le Wifi...</a></p>\n"
    "<p style=\"text-align: center;\">Copyright &copy; 2021 - Gauthier Brière</p>\n"
  );
  
  Page += F("</body>\n</html>\n");

  server.send(200, "text/html", Page);
}
