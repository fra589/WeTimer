/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2023 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: handleWiFi.cpp is part of WeTimer                             */
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

void handleWiFi() {

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  //--------------------------------------------------------------------------------
  String Page;
  Page = htmlEntete();
  //--------------------------------------------------------------------------------
  Page += F(
    "<form id=\"mainForm\" action=\"setparams?retour=wifi\" method=\"POST\">\n"
    "<div style=\"text-align: center !important;\">\n"

    "<label for=\"ap_ssid\" style=\"display: inline-block; text-align: left; width: 57%;\">AP SSID</label>\n"
    "<input name=\"ap_ssid\" type=\"text\" onfocus=\"this.select();\" value=\""
  );
  Page += String(ap_ssid);
  Page += F(
    "\"/><br />\n"

    "<label for=\"ap_pwd\" style=\"display: inline-block; text-align: left; width: 57%;\">AP password</label>\n"
    "<input name=\"ap_pwd\" type=\"password\" autocomplete=\"off\" onfocus=\"this.select();\" value=\""
  );
  Page += String(ap_pwd);
  Page += F(
    "\"/><br />\n"

    "<label for=\"cli_ssid\" style=\"display: inline-block; text-align: left; width: 57%;\">CLI SSID</label>\n"
    "<input name=\"cli_ssid\" type=\"text\" onfocus=\"this.select();\" value=\""
  );
  Page += String(cli_ssid);
  Page += F(
    "\"/><br />\n"

    "<label for=\"cli_pwd\" style=\"display: inline-block; text-align: left; width: 57%;\">CLI password</label>\n"
    "<input name=\"cli_pwd\" type=\"password\" autocomplete=\"off\" onfocus=\"this.select();\" value=\""
  );
  Page += String(cli_pwd);
  Page += F(
    "\"/><br />\n"
    "<!--<p style=\"text-align:center;\"><input type='submit' value='&#13;&#10;Envoi paramètres&#13;&#10;&nbsp;'/></p>-->\n"
    "<p>\n"
    "<div class=\"centreur\">\n"
    "<div class=\"bouton\" onclick=\"document.write(''); document.location.reload(true);\">Relire</div>\n"
    "<div style=\"width: 5px;\"></div>"
    "<div class=\"bouton\" onclick=\"document.forms['mainForm'].submit();\">Envoi</div>\n"
    "</div>\n"
    "</p>\n"
    "</form>\n"
  );
  //--------------------------------------------------------------------------------
  Page += netStatus();
  //--------------------------------------------------------------------------------
  Page += F(
    "<p style=\"text-align: center;\"><a href=\"/\">Retour...</a></p>\n"
  );
  //--------------------------------------------------------------------------------
  Page += piedPage();
  //--------------------------------------------------------------------------------

  server.send(200, "text/html", Page);  
}
