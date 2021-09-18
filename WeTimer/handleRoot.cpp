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
  Page += F(
    "<html>\n"
    "<head>\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" charset=\"utf-8\">\n"    
    "<title>WeTimer</title>\n"
    "<style type=\"text/css\">\n"
    ":root {\n"
    "  background-color: #f0f0f0;\n"
    "}\n"
    ".centreur {\n"
    "  display: table;\n"
    "  margin: 0 auto;\n"
    "  width: 75%;\n"
    "}\n"
    ".bouton {\n"
    "  display: table-cell;\n"
    "  font-family: sans-serif;\n"
    "    font-size: 1.6em;\n"
    "  color: #000;\n"
    "  text-shadow: 0px 1px 0px #ffffff80;\n"
    "  background: #7f7f7f;\n"
    "    background: linear-gradient(#d2d2d2, #2c2c2c);\n"
    "  width: 30%;\n"
    "  height: 3em;\n"
    "  text-align: center;\n"
    "  vertical-align: middle;\n"
    "  border-radius: 7px;\n"
    "  box-shadow: 0 0 5px rgba(0, 0, 0, 0.5),\n"
    "                0 -1px 0 rgba(255, 255, 255, 0.4);\n"
    "}\n"
    ".bouton:hover {\n"
    "  color: #222;\n"
    "  background: #9f9f9f;\n"
    "    background: linear-gradient(#f2f2f2, #4c4c4c);\n"
    "}\n"
    ".bouton:active {\n"
    "  color: #000;\n"
    "  background: #7f7f7f;\n"
    "    background: linear-gradient(#d2d2d2, #2c2c2c);\n"
    "  box-shadow: 1px 1px 10px black inset,\n"
    "                0 1px 0 rgba( 255, 255, 255, 0.4);\n"
    "}\n"
    ".rouge {\n"
    "  background: #7f0000;\n"
    "    background: linear-gradient(#d20000, #2c0000);\n"      
    "}\n"
    ".rouge:hover {\n"
    "  background: #9f0000;\n"
    "    background: linear-gradient(#f20000, #4c0000);\n"
    "}\n"
    ".rouge:active {\n"
    "  background: #7f0000;\n"
    "    background: linear-gradient(#d20000, #2c0000);\n"
    "}\n"
    ".nodisplay {\n"
    "  display: none;\n"
    "}\n"
    ".erreur {\n"
    "color: red;\n"
    "font-weight: bold;\n"
    "}\n"
    "input[type=text] {\n"
    "  text-align: left;\n"
    "  width: 50%;\n"
    "  background-color: #f0f0f0;\n"
    "}\n"
    "input[type=number] {\n"
    "  text-align: right;\n"
    "  width: 35%;\n"
    "  background-color: #f0f0f0;\n"
    "}\n"
    "</style>\n"
    "<script>\n"
    "function setServoPos(servo, valeur) {\n"
    "  var xhttp = getXMLHttpRequest();\n"
    "  xhttp.onreadystatechange = function() {\n"
    "    if (xhttp.readyState == 4) {\n"
    "      if ((xhttp.status == 200) || (xhttp.status == 0)) {\n"
    "        //alert(xhttp.responseText);\n"
    "        xmlrep = xhttp.responseXML;\n"
    "        xmldoc = xmlrep.getElementsByTagName('response');\n"
    "        rep = xmldoc[0].firstChild.nodeValue;\n"
    "        if (rep != 'OK') {\n"
    "          alert(\"setServoPos() : mauvaise réponse de la minuterie !\");\n"
    "        }\n"
    "      } else {\n"
    "        alert(\"XMLHttpRequest_setservo() : Erreur \"+xhttp.status);\n"
    "      }\n"
    "    }\n"
    "  };\n"
    "  xhttp.open(\"POST\", \"/setservo\", true);\n"
    "  xhttp.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\n"
    "  xhttp.send(\"servo=\"+servo+\"&valeur=\"+valeur);\n"
    "}\n"
    "function getXMLHttpRequest() {\n"
    "    var xhr = null;\n"
    "    if (window.XMLHttpRequest || window.ActiveXObject) {\n"
    "        if (window.ActiveXObject) {\n"
    "            try {\n"
    "                xhr = new ActiveXObject(\"Msxml2.XMLHTTP\");\n"
    "            } catch(e) {\n"
    "                xhr = new ActiveXObject(\"Microsoft.XMLHTTP\");\n"
    "            }\n"
    "        } else {\n"
    "            xhr = new XMLHttpRequest();\n"
    "        }\n"
    "    } else {\n"
    "        alert(\"Votre navigateur ne supporte pas l'objet XMLHTTPRequest...\");\n"
    "        return null;\n"
    "    }\n"
    "    return xhr;\n"
    "}\n"
    "</script>\n"
    "</head>\n"
    "<body>\n"
    "<h1 style=\"margin-bottom: 0;\">WeTimer</h1>\n"
    "<p style=\"margin-top: 0;\">Minuterie planeur Papi - "
  );
  Page += APP_VERSION_STRING;
  Page += F(
    "</p>\n"
    "<hr>\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<p>Minuterie status = "
  );
  if (timerStatus == STATUS_DT)          Page += F("STATUS_DT");
  else if (timerStatus == STATUS_ARMEE)  Page += F("STATUS_ARMEE");
  else if (timerStatus == STATUS_TREUIL) Page += F("STATUS_TREUIL");
  else if (timerStatus == STATUS_VOL)    Page += F("STATUS_VOL");
  Page += F(
    "</p>\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<hr>\n"
    "<div class=\"nodisplay\">\n"
  );
  //--------------------------------------------------------------------------------
  Page += F("<pre style=\"text-align: left;\">\n");
  Page += F("Délai armement         = ") + String(delaiArmement) + F(" s\n");
  Page += F("Durée de vol           = ") + String(tempsVol) + F(" s\n");
  Page += F("Pos. servo stab vol    = ") + String(servoStabVol) + F(" μs\n");
  Page += F("Pos. servo stab treuil = ") + String(servoStabTreuil) + F(" μs\n");
  Page += F("Pos. servo stab DT     = ") + String(servoStabDT) + F(" μs\n");
  Page += F("Pos. derive treuil     = ") + String(servoDeriveTreuil) + F(" μs\n");
  Page += F("Pos. derive vol        = ") + String(servoDeriveVol) + F(" μs\n");
  Page += F("</pre>\n");
  //--------------------------------------------------------------------------------
  Page += F(
    "<hr>\n"
    "</div>\n"
  );
  Page += F(
    "<!--<p>\n"
    "<div class=\"centreur\">\n"
    "<div class=\"bouton\" onclick=\"document.write(''); document.location.reload(true);\">Relire</div>\n"
    "</div>\n"
    "</p>-->\n"
    "<form id=\"mainForm\" action=\"setparams\" method=\"POST\">\n"
    "<div style=\"text-align: center !important;\">\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"delaiArmement\" style=\"display: inline-block; text-align: left; width: 57%;\">Armement (s)</label>\n"
    "<input name=\"delaiArmement\" type=\"number\" onfocus=\"this.select();\" value=\""
  );
  Page += String(delaiArmement);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"tempsVol\" style=\"display: inline-block; text-align: left; width: 57%;\">Temps de vol (s)</label>\n"
    "<input name=\"tempsVol\" type=\"number\" onfocus=\"this.select();\" value=\""
  );
  Page += String(tempsVol);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"servoStabVol\" style=\"display: inline-block; text-align: left; width: 57%;\">Servo stab vol (μs)</label>\n"
    "<input name=\"servoStabVol\" type=\"number\" min=\""
  );
  Page += String(MIN_SERVO_MICROSECONDS);
  Page += F(
    "\" max=\""
  );
  Page += String(MAX_SERVO_MICROSECONDS);
  Page += F(
    "\" onfocus=\"this.select(); setServoPos('stab', this.value);\" oninput=\"setServoPos('stab', this.value);\" value=\""
  );
  Page += String(servoStabVol);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"servoStabTreuil\" style=\"display: inline-block; text-align: left; width: 57%;\">Servo stab treuil (μs)</label>\n"
    "<input name=\"servoStabTreuil\" type=\"number\" min=\""
  );
  Page += String(MIN_SERVO_MICROSECONDS);
  Page += F(
    "\" max=\""
  );
  Page += String(MAX_SERVO_MICROSECONDS);
  Page += F(
    "\" onfocus=\"this.select(); setServoPos('stab', this.value);\" oninput=\"setServoPos('stab', this.value);\" value=\""
  );
  Page += String(servoStabTreuil);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"servoStabDT\" style=\"display: inline-block; text-align: left; width: 57%;\">Servo stab DT (μs)</label>\n"
    "<input name=\"servoStabDT\" type=\"number\" min=\""
  );
  Page += String(MIN_SERVO_MICROSECONDS);
  Page += F(
    "\" max=\""
  );
  Page += String(MAX_SERVO_MICROSECONDS);
  Page += F(
    "\" onfocus=\"this.select(); setServoPos('stab', this.value);\" oninput=\"setServoPos('stab', this.value);\" value=\""
  );
  Page += String(servoStabDT);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"servoDeriveVol\" style=\"display: inline-block; text-align: left; width: 57%;\">Servo derive vol (μs)</label>\n"
    "<input name=\"servoDeriveVol\" type=\"number\" min=\""
  );
  Page += String(MIN_SERVO_MICROSECONDS);
  Page += F(
    "\" max=\""
  );
  Page += String(MAX_SERVO_MICROSECONDS);
  Page += F(
    "\" onfocus=\"this.select(); setServoPos('derive', this.value);\" oninput=\"setServoPos('derive', this.value);\" value=\""
  );
  Page += String(servoDeriveVol);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<label for=\"servoDeriveTreuil\" style=\"display: inline-block; text-align: left; width: 57%;\">Servo derive treuil (μs)</label>\n"
    "<input name=\"servoDeriveTreuil\" type=\"number\" min=\""
  );
  Page += String(MIN_SERVO_MICROSECONDS);
  Page += F(
    "\" max=\""
  );
  Page += String(MAX_SERVO_MICROSECONDS);
  Page += F(
    "\" onfocus=\"this.select(); setServoPos('derive', this.value);\" oninput=\"setServoPos('derive', this.value);\" value=\""
  );
  Page += String(servoDeriveTreuil);
  Page += F(
    "\"/><br />\n"
  );
  //--------------------------------------------------------------------------------
  Page += F(
    "<!--<p style=\"text-align:center;\"><input type='submit' value='&#13;&#10;Envoi paramètres&#13;&#10;&nbsp;'/></p>-->\n"
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
  Page += F("IP addr = ");
  Page += toStringIp(WiFi.softAPIP());  
  Page += F("</p>\n");
  if(WiFi.status() == WL_CONNECTED) {
    Page += F("    <p style=\"text-align: center;\">CLI SSID  = ");
    Page += String(cli_ssid);
    Page += F("<br />\n");
    Page += F("CLI MAC = ");
    Page += WiFi.macAddress();
    Page += F("<br />\n");
    Page += F("IP addr = ");
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
