/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: handleConfig.cpp is part of WeTimer                          */
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
void handleConfig() {
  
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

  //--------------------------------------------------------------------------------
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  //--------------------------------------------------------------------------------
  String Page;
  // Entêtes
  Page = htmlEntete();
  //--------------------------------------------------------------------------------
  Page += F(
    "<form id=\"mainForm\" action=\"setparams?retour=config\" method=\"POST\">\n"
    "<div style=\"text-align: center !important;\">\n"
  );
  //--------------------------------------------------------------------------------
  for (int i=0; i<7; i++) {
    Page += F("<div class=\"nomargin\"><label for=\"");
    Page += inputNames[i];
    Page += F("\" onclick=\"doubleHeight('");
    Page += inputNames[i];
    Page += F("');\">");
    Page += inputLabels[i];
    Page += F("</label>");
    Page += F("<input type=\"button\" tabindex=\"-1\" onclick=\"doubleHeight('");
    Page += inputNames[i];
    Page += F("'); changeVal('");
    Page += inputNames[i];
    Page += F("', -5);\" value=\"&lt;&lt;\">");
    Page += F("<input type=\"button\" tabindex=\"-1\" onclick=\"doubleHeight('");
    Page += inputNames[i];
    Page += F("'); changeVal('");
    Page += inputNames[i];
    Page += F("', -1);\" value=\"&nbsp;&lt;&nbsp;\">");
    Page += F("<input name=\"");
    Page += inputNames[i];
    Page += F("\" type=\"number\"  min=\"");
    Page += minValues[i];
    Page += F("\" max=\"");
    Page += maxValues[i];
    if (i < 2) {
      Page += F("\" onfocus=\"doubleHeight('");
      Page += inputNames[i];
      Page += F("'); this.select();\" value=\"");
    } else if (i < 5) {
      Page += F("\" onfocus=\"doubleHeight('");
      Page += inputNames[i];
      Page += F("'); this.select(); setServoPos('stab', this.value);\" oninput=\"setServoPos('stab', this.value);\" value=\"");
    } else {
      Page += F("\" onfocus=\"doubleHeight('");
      Page += inputNames[i];
      Page += F("'); this.select(); setServoPos('derive', this.value);\" oninput=\"setServoPos('derive', this.value);\" value=\"");
    }
    Page += inputValues[i];
    Page += F("\"/>");
    Page += F("<input type=\"button\" tabindex=\"-1\" onclick=\"doubleHeight('");
    Page += inputNames[i];
    Page += F("'); changeVal('");
    Page += inputNames[i];
    Page += F("', 1);\" value=\"&nbsp;&gt;&nbsp;\">");
    Page += F("<input type=\"button\" tabindex=\"-1\" onclick=\"doubleHeight('");
    Page += inputNames[i];
    Page += F("'); changeVal('");
    Page += inputNames[i];
    Page += F("', 5);\" value=\"&gt;&gt;\">");
    Page += F("</div>\n");
  }
  //--------------------------------------------------------------------------------
  Page += F(
    "<p>\n"
    "<div class=\"centreur\">\n"
    "<div class=\"bouton\" onclick=\"doubleHeight(''); document.write(''); document.location.reload(true);\">Relire</div>\n"
    "&nbsp;"
    "<div class=\"bouton\" onclick=\"doubleHeight(''); document.forms['mainForm'].submit();\">Envoi</div>\n"
    "&nbsp;"
    "<div class=\"bouton rouge\" onclick=\"doubleHeight(''); window.location.assign('/rdt?retour=config');\">RDT</div>\n"
    "</div>\n"
    "</p>\n"

    "</form>\n"
  );
  //--------------------------------------------------------------------------------
  Page += netStatus();
  //--------------------------------------------------------------------------------
  Page += F(
    "<p style=\"text-align: center;\"><a href=\"/\">Retour...</a></p>\n"
    "<p style=\"text-align: center;\"><a href=\"/wifi\">Configurer le Wifi...</a></p>\n"
  );
  //--------------------------------------------------------------------------------
  Page += piedPage();
  //--------------------------------------------------------------------------------

  server.send(200, "text/html", Page);
}
