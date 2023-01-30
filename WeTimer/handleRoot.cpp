/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2023 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: handleRoot.cpp is part of WeTimer                             */
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

#include <Arduino.h>
#include "WeTimer.h"

/** Handle root or redirect to captive portal */
void handleRoot() {

  if (captivePortal()) { // If caprive portal redirect instead of displaying the page.
    return;
  }

  //--------------------------------------------------------------------------------
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  //--------------------------------------------------------------------------------
  String Page;
  Page = htmlEntete();
  //--------------------------------------------------------------------------------
  Page += F(
    "<form id=\"mainForm\" action=\"setparams\" method=\"POST\">\n"
    "<div style=\"text-align: center !important;\">\n"
  );
  //--------------------------------------------------------------------------------
  Page += F("<div class=\"nomargin\">");
  Page += F("<label for=\"tempsVol\" style=\"width: 100%; font-size: 1.6em; text-align: center; margin-bottom: 10px;\">Temps de vol (secondes)</label>\n<br />\n");
  
  Page += F("<input type=\"button\" style=\"font-size: 2.5em;\" tabindex=\"-1\" onclick=\"changeVal('tempsVol', -1); calculTemps();\" value=\"&nbsp;&lt;&nbsp;\">");
  Page += F("<input name=\"tempsVol\" style=\"font-size: 2.5em; width: 3em; text-align: center; margin: 0;\" type=\"number\" min=\"0\" max=\"9999\" onfocus=\"this.select();\" oninput=\"calculTemps();\" value=\"");
  Page += String(tempsVol);
  Page += F("\"/>");
  Page += F("<input type=\"button\" style=\"font-size: 2.5em;\" tabindex=\"-1\" onclick=\"changeVal('tempsVol', 1); calculTemps();\" value=\"&nbsp;&gt;&nbsp;\">");
  
  Page += F("<br /><br />\n");
  Page += F("<input type=\"button\" style=\"font-size: 2.75em;\" tabindex=\"-1\" onclick=\"changeVal('tempsVol', -60); calculTemps();\" value=\"&lt;m\">");
  Page += F("<input type=\"button\" style=\"font-size: 2.75em;\" tabindex=\"-1\" onclick=\"changeVal('tempsVol', -10); calculTemps();\" value=\"&lt;&lt;\">");
  Page += F("<input type=\"button\" style=\"font-size: 2.75em;\" tabindex=\"-1\" onclick=\"changeVal('tempsVol', 10); calculTemps();\" value=\"&gt;&gt;\">");
  Page += F("<input type=\"button\" style=\"font-size: 2.75em;\" tabindex=\"-1\" onclick=\"changeVal('tempsVol', 60); calculTemps();\" value=\"m&gt;\">");
  Page += F("</div>\n");
  Page += F("<p id=\"tempsMinutes\">0 minute(s) 0 seconde(s)</p>");
  //--------------------------------------------------------------------------------
  Page += F(
    "<p>\n"
    "<div class=\"centreur\">\n"
    "<div class=\"bouton\" style=\"height: 3em;\" onclick=\"document.write(''); document.location.reload(true);\">Relire</div>\n<!--"
    "--><div class=\"bouton\" style=\"height: 3em;\" onclick=\"document.forms['mainForm'].submit();\">Envoi</div>\n"
    "</div>\n"
    "<p></p>\n"
    "<div class=\"centreur\">\n"
    "<div class=\"bouton rouge\" style=\"height: 4em;\" onclick=\"window.location.assign('/rdt');\">RDT</div>\n"
    "</div>\n"
    "</p>\n"
    "</form>\n"
  );
  //--------------------------------------------------------------------------------
  Page += netStatus();
  //--------------------------------------------------------------------------------
  Page += F(
    "<p style=\"text-align: center;\"><a href=\"/config\">Configurer la minuterie...</a></p>\n"
    "<p style=\"text-align: center;\"><a href=\"/wifi\">Configurer le Wifi...</a></p>\n"
  );
  //--------------------------------------------------------------------------------
  Page += piedPage();
  //--------------------------------------------------------------------------------

  server.send(200, "text/html", Page);
}
