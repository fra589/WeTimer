/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2023 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: htmlStyle.cpp is part of WeTimer                              */
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

/*--------------------------------------------------------------------------------------------------*/
/*                                    HTML styles definitions                                       */
/*--------------------------------------------------------------------------------------------------*/
void handleStyle() {
  server.send(200, "text/css", app_style());
}


String app_style() {
  String style = F(
    "<style type=\"text/css\">\n"
    ":root {\n"
    "  font-family: sans-serif;\n"
    "}\n"
    "body {\n"
    "  margin: 0;\n"
    "  background-color: #d0e5ff;\n"
    "}\n"
    "h1 {\n"
    "  margin-left: 7px;\n"
    "  margin-bottom: 0;\n"
    "}\n"
    ".span_title {\n"
    "  float; left;\n"
    "  background: rgb(0,0,62);\n"
    "  background: linear-gradient(90deg, rgb(0, 0, 62) 0%, rgb(0, 0, 127) 63%, rgb(255, 0, 0) 100%);\n"
    "  background-clip: border-box;\n"
    "  -webkit-background-clip: text;\n"
    "  -webkit-text-fill-color: transparent;\n"
    "}\n"
    ".p0 {\n"
    "  text-align:center;\n"
    "  margin: 0px;\n"
    "  margin-top: -10px;\n"
    "}\n"
    ".p1 {\n"
    "  text-align: center;\n"
    "  font-style: italic;\n"
    "  margin: 0;\n"
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
    ".nomargin {\n"
    "  margin: 0px;\n"
    "  padding: 0px;\n"
    "}\n"
    ".addPadding {\n"
    "  margin-top: 0px;\n"
    "  margin-bottom: 0px;\n"
    "  padding-top: 15px;\n"
    "  padding-bottom: 15px;\n"
    "}\n"
    "label {\n"
    "  display: inline-block;\n"
    "  text-align: left;\n"
    "  width: 49%;\n"
    "}\n"
    "input[type=text] {\n"
    "  text-align: left;\n"
    "  width: 40%;\n"
    "  background-color: #f0f0f0;\n"
    "}\n"
    "input[type=password] {\n"
    "  text-align: left;\n"
    "  width: 40%;\n"
    "  background-color: #f0f0f0;\n"
    "}\n"
    "input[type=number] {\n"
    "-moz-appearance: textfield;\n"
    "  text-align: right;\n"
    "  width: 15%;\n"
    "  background-color: #f0f0f0;\n"
    "}\n"
    "input[name=\"tempsVol\"] {\n"
    "  background-color: lightblue;\n"
    "  font-weight: bold;\n"
    "}\n"
    "label[for=\"tempsVol\"] {\n"
    "  font-weight: bold;\n"
    "}\n"
    ".timer_status {\n"
    "  font-weight: bold;\n"
    "  color: #900000;\n"
    "  font-size: 140%;\n"
    "}\n"
    "</style>\n"
  );
  return style;
}
