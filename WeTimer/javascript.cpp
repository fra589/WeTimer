/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: javascript.cpp is part of WeTimer                           */
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

/*--------------------------------------------------------------------------------------------------*/
/*                                    HTML styles definitions                                       */
/*--------------------------------------------------------------------------------------------------*/
String app_script() {
  String script = F(
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
    "function changeVal(inputName, increment) {\n"
    "  var inp = document.getElementsByName(inputName);\n"
    "    inp[0].value = parseInt(inp[0].value) + increment;\n"
    "    if (inp[0].min != \"\") {\n"
    "      if (parseInt(inp[0].value) < inp[0].min) {\n"
    "        inp[0].value = inp[0].min\n"
    "      }\n"
    "    }\n"
    "    if (inp[0].max != \"\") {\n"
    "      if (parseInt(inp[0].value) > inp[0].max) {\n"
    "        inp[0].value = inp[0].max\n"
    "      }\n"
    "    }\n"
    "    inp[0].focus()\n"
    "}\n"
    "</script>\n"
  );
  return script;
}
