/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: webserver.cpp is part of WeTimer / WeDT                       */
/*                                                                          */
/* WeTimer / WeDT is free software: you can redistribute it and/or modify   */
/* it under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or        */
/* (at your option) any later version.                                      */
/*                                                                          */
/* WeTimer / WeDT is distributed in the hope that it will be useful, but    */
/* WITHOUT ANY WARRANTY; without even the implied warranty of               */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*                                                                          */
/****************************************************************************/

#include "WeTimer.h"

void webServerInit(void) {

  // Montage du système de fichier ou sont stockés les éléments web
  // Désactive le formatage automatique
  LittleFSConfig cfg;
  cfg.setAutoFormat(false);
  LittleFS.setConfig(cfg);
  // Monte le système de fichier
  if (!LittleFS.begin()) {
    ;
    #ifdef DEBUG
      WT_PRINTF("Erreur lors du montage du système de fichier LittleFS\n");
      Serial.flush();
    #endif    
  }

  // Setup web pages
  server.enableCORS(true);
  server.on(ROOT_FILE,           handleRoot); // /index.html
  server.on("/",                 handleRoot);
  server.on("/connecttest.txt",  handleRoot);
  #ifdef GENERATE_204
    server.on("/generate_204",     generate_204);   //Android captive portal.
    server.on("/gen_204",          handleGen204);
  #endif
  server.on("/favicon.ico",      handleRoot); //Another Android captive portal. Maybe not needed. Might be handled By notFound handler. Checked on Sony Handy
  server.on("/fwlink",           handleRoot); //Microsoft captive portal. Maybe not needed. Might be handled By notFound handler.
  server.on("/getversion",       handleGetVersion);
  server.on("/getstatus",        handleGetStatus);
  server.on("/getconfig",        handleGetConfig);
  server.on("/setconfig",        handleSetConfig);
  server.on("/gettempvol",       handleGetTempVol);
  server.on("/settempvol",       handleSetTempVol);
  server.on("/getservoconfig",   handleGetServoConfig);
  server.on("/setservoconfig",   handleSetServoConfig);
  server.on("/servotest",        handleServoTest);
  server.on("/servopos",         handleServoTest);
  server.on("/getbatterie",      handleGetBatterie);
  server.on("/rdt",              handleRDT);
  server.on("/getjson",          handleGetJson);
  server.on("/uploadconfig",     handleUpload);
  server.on("/setapconfig",      handleSetApConfig);
  server.on("/setdescription",   handleSetDescription);
  server.on("/getflashdata",     handleGetFashData);
  server.on("/testflasher",      handleTestFlasher);
  server.on("/resetflashdata",   handleResetFlashData);
  server.on("/setflashdata",     handleSetFashData);
  
  server.onNotFound(handleNotFound);

  server.begin(); // Start http web server

  yield();
  delay(100);

  #ifdef DEBUG
    WT_PRINTF("Serveur web démarré.\n");
    Serial.flush();
  #endif    

}
void sendEmpty(void) {
  // Renvoi une réponse OK (HTTP 200) mais vide
  #ifdef DEBUG_WEB
    WT_PRINTF("Sending : (200, \"text/plain\", \"\")\n");
  #endif
  server.send(200, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.client().stop(); // Stop is needed because we sent no content length
}
bool captivePortal(void) {
  // Redirect to captive portal if we got a request for another domain.
  // Return true in that case so the page handler do not try to handle the request again.
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans captivePortal()...\n");
    WT_PRINTF("- server.hostHeader() = %s\n", server.hostHeader().c_str());
  #endif
  // Conversion de myHostname.local en minuscules
  String monNomDNS = String(myHostname) + ".local";
  monNomDNS.toLowerCase();
  // Conversion de hostHeader en minuscule
  String hostHeader = server.hostHeader();
  hostHeader.toLowerCase();
  
  if ((!isIp(server.hostHeader())) && (hostHeader != monNomDNS)) {
    #ifdef DEBUG_WEB
      WT_PRINTF("Redirection vers le portail captif\n");
    #endif
    server.sendHeader("Location", String("http://") + server.client().localIP().toString() + "/index.html", true);
    server.send(302, "text/plain", "");   // Empty content inhibits Content-length header so we have to close the socket ourselves.
    server.client().stop(); // Stop is needed because we sent no content length
    return true;
  }
  return false;
}
void handleRoot(void) {
  //Handles http request 

  String buffer = "";
  File file;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleRoot()\n");
    WT_PRINTF("- URL = %s%s\n", server.hostHeader().c_str(), server.uri().c_str());
  #endif
  if (server.hostHeader() != "") {
    if (captivePortal()) { // If captive portal redirect instead of displaying the page.
      return;
    }
  }

  #ifdef DEBUG_WEB
    WT_PRINTF("captivePortal() returned false, sending index.html\n");
  #endif

  handleFileRead("/index.html");

}
#ifdef GENERATE_204
  void generate_204(void) {

    #ifdef DEBUG_WEB
      WT_PRINTF("Entrée dans generate_204()\n");
      WT_PRINTF("- URL = %s%s\n", server.hostHeader().c_str(), server.uri().c_str());
    #endif
    
    server.send(204);       // Reply for Android captive portal detection
    server.client().stop(); // Stop is needed because we sent no content length
    
  }
  void handleGen204(void) {
    #ifdef DEBUG_WEB
      WT_PRINTF("Entrée dans handleGen204()\n");
      WT_PRINTF("- URL = %s%s\n", server.hostHeader().c_str(), server.uri().c_str());
    #endif
    String venueUrl = F("http://") + server.client().localIP().toString() + F("/index.html");
    String json = F("{\"captive\":false,\"venue-info-url\":\"") + venueUrl + F("\"}");
    WT_PRINTF("%s\n", json.c_str());
    server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    server.sendHeader("Pragma", "no-cache");
    server.sendHeader("Expires", "-1");
    server.sendHeader("Content-Type", "application/json; charset=utf-8");
    server.send(200, "application/json", "{}"); // Reply for Android captive portal detection
    server.client().stop();             // Stop is needed because we sent no content length
  }
#endif
void handleNotFound(void) {

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleNotFound()\n");
    WT_PRINTF("- URL = %s%s\n", server.hostHeader().c_str(), server.uri().c_str());
  #endif

  String uri = ESP8266WebServer::urlDecode(server.uri());  // required to read paths with blanks

  // Cherche le fichier sur LittleFS
  if (handleFileRead(uri)) {
    return;
  }

  String message = F("Page non trouvée : ");
  message += server.uri().c_str();
  message += F("\n");

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(404, "text/plain", message);

}
bool handleFileRead(String path) {
  String contentType;
  File file;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleFileRead(\"%s\")\n", path.c_str());
  #endif

  #ifdef GENERATE_204
    if (path == "/chat") {
      sendEmpty();
      return true;
    }
    if (strncmp(path.c_str(), "/generate_204", 13) == 0) {
      sendEmpty();
      return true;
    }
  #endif

  contentType = mime::getContentType(path);

  if (LittleFS.exists(path)) {
    #ifdef DEBUG_WEB
      WT_PRINTF("LittleFS.exists(\"%s\") OK.\n", path.c_str());
    #endif
    file = LittleFS.open(path, "r");
    if (server.streamFile(file, contentType) != file.size()) {
      ;
      #ifdef DEBUG_WEB
      WT_PRINTF("Sent less data than expected!\n");
      #endif
    }
    file.close();
    return true;
  #ifdef DEBUG_WEB
  } else {
    WT_PRINTF("LittleFS.exists(\"%s\") returned false !\n", path.c_str());
  #endif
  }

  return false;

}
void handleGetVersion(void) {

  String XML;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetVersion()\n");
  #endif

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  XML += F("  <name>");
  XML += F(APP_NAME);
  XML += F("</name>\n");
  XML += F("  <version>");
  XML += F(APP_VERSION_STRING);
  XML += F("</version>\n");
  XML += F("  <ssid>");
  XML += String(ap_ssid);
  XML += F("</ssid>\n");
  XML += F("  <pwd>");
  XML += String(ap_pwd);
  XML += F("</pwd>\n");
  XML += F("  <description>");
  XML += escapeXML(String(description));
  XML += F("</description>\n");
  XML += F("</WeTimer>\n");

  server.send(200,"text/xml",XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetVersion() : version envoyée.\n");
  #endif

}
void handleGetStatus(void) {

  String XML;
  unsigned long dureeVol = 0;

  #ifdef DEBUG_WEB_2
    WT_PRINTF("Entrée dans handleGetStatus()\n");
  #endif

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  XML += F("  <status>");
  XML += getStatusText(timerStatus);
  XML += F("</status>\n");
  if (temps.largage != 0) {
    dureeVol = (millis() - temps.largage) / 1000;
  }
  XML += F("  <dureevol>");
  XML += String(dureeVol);
  XML += F("</dureevol>\n");
  
  float tension = getTension();
  XML += F("  <tension>");
  XML += String(tension);
  XML += F("</tension>\n");
  XML += F("  <switch1>");
  XML += String(crochet[0]);
  XML += F("</switch1>\n");
  XML += F("  <switch2>");
  XML += String(crochet[1]);
  XML += F("</switch2>\n");

  XML += F("</WeTimer>\n");

  server.send(200,"text/xml",XML);

  #ifdef DEBUG_WEB_2
    WT_PRINTF("handleGetStatus() : status envoyée.\n");
  #endif

}
void handleGetConfig(void) {

  String XML;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetConfig()\n");
  #endif

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimerConfig>\n");
  for (int i=0; i<NB_DELAI; i++) {
    XML += F("  <delai><num>");
    XML += String(i);
    XML += F("</num><val>");
    XML += String(delai[i]);
    XML += F("</val></delai>\n");
  }
  for (int j=0; j<NB_POSITIONS; j++) {
    for (int i=0; i<NB_SERVOS; i++) {
      XML += F("  <pservo><num>");
      XML += String(i);
      XML += F("</num><pos>");
      XML += String(j);
      XML += F("</pos><microsecondes>");
      XML += String(pservo[i][j]);
      XML += F("</microsecondes></pservo>\n");
    }
  }
  XML += F("</WeTimerConfig>\n");

  server.send(200,"text/xml",XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetConfig() : config envoyée.\n");
  #endif

}
void handleSetConfig(void) {

  String XML;
  StaticJsonDocument<512> doc;
  StaticJsonDocument<1024> arr;
  DeserializationError error;
  bool erreur = false;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleSetConfig()\n");
  #endif

  // Format des données envoyées dans le POST :
  // json={"delai":"[val0,val1,...,val9]","pservo":"[[s0p0,s0p1,...,s0p10],[s1p0,s1p1,...,s1p10],[s2p0,s2p1,...,s2p10]]"}
  // Les données contiennent des tableaux JSON dans un document JSON. 
  // Il faut donc appeler deserializeJson() plusieurs fois.
  // Une première fois pour le document complet (doc), et ensuite pour chaque tableau (arr) JSON
  if (server.args() > 0) {
    for (int i=0; i < server.args(); i++) {
      #ifdef DEBUG_WEB
        WT_PRINTF("  handleSetConfig() - Arg n°%d –> %s = %s\n", i, server.argName(i), server.arg(i).c_str());
      #endif
      if (strncasecmp(server.argName(i).c_str(), "json", (size_t)4) == 0) {
        const char* jsonData = server.arg(i).c_str();
        // Deserialization JSON
        error = deserializeJson(doc, jsonData);
        if (error) {
          #ifdef DEBUG_WEB
            WT_PRINTF("handleSetConfig(): deserializeJson(jsonData) failed: %s\n", error.f_str());
          #endif
          erreur = true;
        } else {
          const char* tableauDelai = doc["delai"];
          #ifdef DEBUG_WEB_2
            WT_PRINTF("tableauDelai=%s\n", tableauDelai);
          #endif
          // Deserialization tableau des délais
          error = deserializeJson(arr, tableauDelai);
          if (error) {
            #ifdef DEBUG_WEB
              WT_PRINTF("handleSetConfig(): deserializeJson(tableauDelai) failed: %s\n", error.f_str());
            #endif
            erreur = true;
          } else {
            // Récupération des délais
            for (int i=0; i<NB_DELAI; i++) {
              delai[i] = arr[i];
              #ifdef DEBUG_WEB_2
                WT_PRINTF("delai[%d]=%d\n", i, delai[i]);
              #endif
            }
          }
          const char* tableauServo = doc["pservo"];
          #ifdef DEBUG_WEB_2
            WT_PRINTF("tableauServo=%s\n", tableauServo);
          #endif
          // Deserialization tableau des positions de servo
          error = deserializeJson(arr, tableauServo);
          if (error) {
            #ifdef DEBUG_WEB
              WT_PRINTF("handleSetConfig(): deserializeJson(tableauServo) failed: %s\n", error.f_str());
            #endif
            erreur = true;
          } else {
            // Récupération des positions de servos
            for (int j=0; j<NB_POSITIONS; j++) {
              for (int i=0; i<NB_SERVOS; i++) {
                pservo[i][j] = arr[i][j];
                #ifdef DEBUG_WEB_2
                  WT_PRINTF("pservo[%d][%d]=%d\n", i, j, pservo[i][j]);
                #endif
              }
            }
          }
        }
        if (!erreur) {
          // Mise à jour de l'EEPROM
          updateEepromData();
        }
      }
    }
  }

  // Renvoi la réponse au client http
  if (!erreur) {
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <setconfig>OK</setconfig>\n");
    XML += F("</WeTimer>\n");
  } else {
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <setconfig>Erreur</setconfig>\n");
    XML += F("</WeTimer>\n");
  }

  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleSetConfig() : OK envoyé.\n");
  #endif

}
void handleGetTempVol(void) {

  String XML;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetTempVol()\n");
  #endif

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  XML += F("  <tempvol>");
  XML += String(delai[7]);
  XML += F("  </tempvol>");
  XML += F("</WeTimer>\n");

  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetTempVol() : Temp de vol envoyé.\n");
  #endif

}
void handleSetTempVol(void) {

  String XML;
  unsigned int tmpVal;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleSetTempVol()\n");
  #endif

  //Traitement des données POST
  if (server.args() > 0) {
    for (int i=0; i < server.args(); i++) {
      #ifdef DEBUG_WEB
        WT_PRINTF("  handleSetTempVol() - Arg n°%d –> %s = [%s]\n", i, server.argName(i), server.arg(i).c_str());
      #endif
      if (strncasecmp(server.argName(i).c_str(), "tempvol", (size_t)7) == 0) {
        tmpVal = atoi(server.arg(i).c_str()) * 100;
        if (delai[7] != tmpVal) {
          #ifdef DEBUG_WEB
            WT_PRINTF("  handleSetTempVol() - tempvol = %d\n", tmpVal);
          #endif
          // Changement de durée en cour de vol
          if (temps.dt != 0) {
            #ifdef DEBUG_WEB
              WT_PRINTF("  handleSetTempVol() Changement de durée en cour de vol ! temps.dt %u => ", temps.dt);
            #endif
            // On applique sur le vol en cours
            int delta = tmpVal - delai[7];
            temps.dt   = temps.dt   + (delta * 10);
            temps.parc = temps.parc + (delta * 10);
            #ifdef DEBUG_WEB
              WT_PRINTF("%u\n", temps.dt);
            #endif
          }
          delai[7] = tmpVal;
          updateEepromData();
        } else {
          ;
          #ifdef DEBUG_WEB
            WT_PRINTF("  handleSetTempVol() - tempvol = %d (pas de changement)\n", tmpVal);
          #endif
        }
      } else {
        ;
        #ifdef DEBUG_WEB
          WT_PRINTF("  handleSetTempVol() - %s argument inconnu !\n", server.argName(i));
        #endif
      }
    }
  }

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  XML += F("  <tempvol>");
  XML += String(delai[7]);
  XML += F("  </tempvol>");
  XML += F("</WeTimer>\n");

  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetTempVol() : Temp de vol envoyé.\n");
  #endif

}
void handleGetServoConfig(void) {

  String XML;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetServoConfig()\n");
  #endif

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  for (int i=0; i<NB_SERVOS; i++) {
    for (int j=0; j<NB_CONFIG; j++) {
      XML += F("  <cservo><num>");
      XML += String(i);
      XML += F("</num><conf>");
      XML += String(j);
      XML += F("</conf><val>");
      XML += String(cservo[i][j]);
      XML += F("</val></cservo>\n");
    }
  }
  XML += F("</WeTimer>\n");

  server.send(200,"text/xml",XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetServoConfig() : config envoyée.\n");
  #endif

}
void handleSetServoConfig(void) {

  String XML;
  StaticJsonDocument<512> doc;
  StaticJsonDocument<1024> arr;
  DeserializationError error;
  bool erreur = false;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleSetServoConfig()\n");
  #endif

  // Format des données envoyées dans le POST :
  // json={"cservo":"[[s0p0,s0p1,s0p2,s0p3],[s1p0,s1p1,s1p2,s1p3],[s2p0,s2p1,s2p2,s2p3]]","pservo":"[[s0p0,s0p1,...,s0p10],[s1p0,s1p1,...,s1p10],[s2p0,s2p1,...,s2p10]]"}
  // Les données contiennent des tableaux JSON dans un document JSON. 
  // Il faut donc appeler deserializeJson() plusieurs fois.
  // Une première fois pour le document complet (doc), et ensuite pour chaque tableau (arr) JSON
  if (server.args() > 0) {
    for (int i=0; i < server.args(); i++) {
      #ifdef DEBUG_WEB
        WT_PRINTF("  handleSetServoConfig() - Arg n°%d –> %s = %s\n", i, server.argName(i), server.arg(i).c_str());
      #endif
      if (strncasecmp(server.argName(i).c_str(), "json", (size_t)4) == 0) {
        const char* jsonData = server.arg(i).c_str();
        // Deserialization JSON
        error = deserializeJson(doc, jsonData);
        if (error) {
          #ifdef DEBUG_WEB
            WT_PRINTF("handleSetServoConfig(): deserializeJson(jsonData) failed: %s\n", error.f_str());
          #endif
          erreur = true;
        } else {
          const char* tableauCServo = doc["cservo"];
          #ifdef DEBUG_WEB_2
            WT_PRINTF("tableauCServo(cservo)=%s\n", tableauCServo);
          #endif
          // Deserialization tableau des positions de servo
          error = deserializeJson(arr, tableauCServo);
          if (error) {
            #ifdef DEBUG_WEB
              WT_PRINTF("handleSetServoConfig(): deserializeJson(tableauCServo) failed: %s\n", error.f_str());
            #endif
            erreur = true;
          } else {
            // Récupération des positions de servos
            for (int j=0; j<NB_CONFIG; j++) {
              for (int i=0; i<NB_SERVOS; i++) {
                cservo[i][j] = arr[i][j];
                #ifdef DEBUG_WEB_2
                  WT_PRINTF("cservo[%d][%d]=%d\n", i, j, cservo[i][j]);
                #endif
              }
            }
          }
          const char* tableauPServo = doc["pservo"];
          #ifdef DEBUG_WEB_2
            WT_PRINTF("tableauPServo(pservo)=%s\n", tableauPServo);
          #endif
          // Deserialization tableau des positions de servo
          error = deserializeJson(arr, tableauPServo);
          if (error) {
            #ifdef DEBUG_WEB
              WT_PRINTF("handleSetServoConfig(): deserializeJson(tableauPServo) failed: %s\n", error.f_str());
            #endif
            erreur = true;
          } else {
            // Récupération des positions de servos
            for (int j=0; j<NB_POSITIONS; j++) {
              for (int i=0; i<NB_SERVOS; i++) {
                pservo[i][j] = arr[i][j];
                #ifdef DEBUG_WEB_2
                  WT_PRINTF("pservo[%d][%d]=%d\n", i, j, pservo[i][j]);
                #endif
              }
            }
          }
        }
        if (!erreur) {
          // Mise à jour de l'EEPROM
          updateEepromConf();
          updateEepromData();
        }
      }
    }
  }

  // Renvoi la réponse au client http
  if (!erreur) {
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <setservoconfig>OK</setservoconfig>\n");
    XML += F("</WeTimer>\n");
  } else {
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <setservoconfig>Erreur</setservoconfig>\n");
    XML += F("</WeTimer>\n");
  }

  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleSetServoConfig() : OK envoyé.\n");
  #endif

}
void handleServoTest(void) {

  String XML;
  unsigned int mode     = 0;
  unsigned int servonum = 0;
  int limit1   = 0;
  int limit2   = 0;
  int pwm1     = 0;
  int pwm2     = 0;
  int val      = 0;
  bool erreur = false;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleServoTest()\n");
  #endif

  //Traitement des données POST mode, servonum, limit1, limit2, pwm1, pwm2, val
  if (server.args() == 7) {
    for (int i=0; i < server.args(); i++) {
      #ifdef DEBUG_WEB_2
        WT_PRINTF("  handleServoTest() - Arg n°%d –> %s = [%s]\n", i, server.argName(i), server.arg(i).c_str());
      #endif
      if (strncasecmp(server.argName(i).c_str(), "mode", (size_t)4) == 0) {
        mode = atoi(server.arg(i).c_str());
      } else if (strncasecmp(server.argName(i).c_str(), "servonum", (size_t)8) == 0) {
        servonum = atoi(server.arg(i).c_str());
        if (servonum < 0) servonum = 0;
        if (servonum >=NB_SERVOS) servonum = NB_SERVOS - 1;
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - servonum = %d\n", servonum);
        #endif
      } else if (strncasecmp(server.argName(i).c_str(), "limit1", (size_t)6) == 0) {
        limit1 = atoi(server.arg(i).c_str());
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - limit1   = %d\n", limit1);
        #endif
      } else if (strncasecmp(server.argName(i).c_str(), "limit2", (size_t)6) == 0) {
        limit2 = atoi(server.arg(i).c_str());
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - limit2   = %d\n", limit2);
        #endif
      } else if (strncasecmp(server.argName(i).c_str(), "pwm1",   (size_t)4) == 0) {
        pwm1 = atoi(server.arg(i).c_str());
        // Assure que les valeurs PWM restent dans les limites
        if (pwm1 < MIN_SERVO_MICROSECONDS) pwm1 = MIN_SERVO_MICROSECONDS;
        if (pwm1 > MAX_SERVO_MICROSECONDS) pwm1 = MAX_SERVO_MICROSECONDS;
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - pwm1     = %d\n", pwm1);
        #endif
      } else if (strncasecmp(server.argName(i).c_str(), "pwm2",   (size_t)4) == 0) {
        pwm2 = atoi(server.arg(i).c_str());
        // Assure que les valeurs PWM restent dans les limites
        if (pwm2 < MIN_SERVO_MICROSECONDS) pwm2 = MIN_SERVO_MICROSECONDS;
        if (pwm2 > MAX_SERVO_MICROSECONDS) pwm2 = MAX_SERVO_MICROSECONDS;
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - pwm2     = %d\n", pwm2);
        #endif
      } else if (strncasecmp(server.argName(i).c_str(), "val",    (size_t)3) == 0) {
        val = atoi(server.arg(i).c_str());
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - val      = %d\n", val);
        #endif
      } else {
        erreur = true;
        #ifdef DEBUG_WEB_2
          WT_PRINTF("  handleServoTest() - %s argument inconnu !\n", server.argName(i));
        #endif
      }
    }
  } else { // if (server.args() == 6)
    erreur = true;
    #ifdef DEBUG_WEB
      WT_PRINTF("  handleServoTest() - nombre d'argument incorrect (%d != 6) !\n", server.args());
    #endif
  }

  // Assure que val est dans l'interval de valeur programmé
  if (mode == 0) { // Mode valeur de programmation minuterie
    if (limit1 < limit2) {
      if (val < limit1) val = limit1;
      if (val > limit2) val = limit2;
    } else if (limit2 < limit1) {
      if (val > limit1) val = limit1;
      if (val < limit2) val = limit2;
    } else {
      erreur = true;
      #ifdef DEBUG_WEB
        WT_PRINTF("  handleServoTest() - erreur : limit1 = limit2 !\n");
      #endif
    }
  } else { // mode == 1 : Mode valeur impulsion
    if (pwm1 < pwm1) {
      if (val < pwm1) val = pwm1;
      if (val > pwm2) val = pwm2;
    } else if (pwm2 < pwm1) {
      if (val > pwm1) val = pwm1;
      if (val < pwm2) val = pwm2;
    }
  }
  // Assure une plage PWM non nulle
  if (pwm1 == pwm2) {
    erreur = true;
    #ifdef DEBUG_WEB
      WT_PRINTF("  handleServoTest() - erreur : pwm1 = pwm2 !\n");
    #endif
  }

  if (!erreur) {
    #ifdef DEBUG_WEB_2
      WT_PRINTF("  handleServoTest() - appel de servoTest()\n");
    #endif
    servoTest(mode, servonum, limit1, limit2, pwm1, pwm2, val);
    #ifdef DEBUG_WEB_2
      WT_PRINTF("  handleServoTest() - retour de servoTest()\n");
    #endif
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <servotest>OK</servotest>\n");
    XML += F("</WeTimer>\n");
  } else {
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <servotest>Erreur</servotest>\n");
    XML += F("</WeTimer>\n");
  }

  // Renvoi la réponse au client http
  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleServoTest() : réponse envoyée.\n");
  #endif

}
void handleGetBatterie(void) {

  String XML;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetBatterie()\n");
  #endif

  float tension = getTension();

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  XML += F("  <tension>");
  XML += String(tension);
  XML += F("  </tension>");
  XML += F("</WeTimer>\n");

  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetBatterie() : Tension batterie envoyée.\n");
  #endif

}
void handleRDT(void) {

  String XML;

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleRDT()\n");
  #endif

  runDT();

  // Renvoi la réponse au client http
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimer>\n");
  XML += F("  <result>OK</result>\n");
  XML += F("</WeTimer>\n");

  server.send(200, "text/xml", XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleRDT() : OK envoyé.\n");
  #endif

}
void handleGetJson(void) {

  char buff[8];
  char j_version[64];
  char j_cservo[128];
  char j_delai[128];
  char j_pservo[256];
  String buffer;
  char j_description[DESCRIPTION_LEN]; // DESCRIPTION_LEN = 64
  char j_flash[32];
  char JSON[672];
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetJson()\n");
  #endif

  // Format du fichier de config JSON ;
  // {
  // "version":"WeTimer-v2.0.20250106",
  // "cservo":"[[0,255,544,2400],[0,255,544,2400],[0,255,544,2400]]",
  // "delai":"[200,20,15,60,90,300,1000,18000,300,0]",
  // "pservo":"[[100,100,100,150,170,130,120,30,90,95,10],[100,100,200,110,105,105,105,150,120,120,100],[100,150,100,150,100,100,100,100,120,120,120]]",
  // "description":"&lt;La description...&gt;"
  // "flash":"[1,1,2,35,25,3]"
  // }
  
  sprintf(j_version, "\"version\":\"[%s,%s]\",\n", APP_NAME, APP_VERSION_STRING);
  sprintf(j_cservo, "\"cservo\":\"[[%d,%d,%d,%d],[%d,%d,%d,%d],[%d,%d,%d,%d]]\",\n",
    cservo[0][0], cservo[0][1], cservo[0][2], cservo[0][3],
    cservo[0][0], cservo[0][1], cservo[0][2], cservo[0][3],
    cservo[0][0], cservo[0][1], cservo[0][2], cservo[0][3]
  );
  sprintf(j_delai, "\"delai\":\"[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\",\n",
    delai[0], delai[1], delai[2], delai[3], delai[4],
    delai[5], delai[6], delai[7], delai[8], delai[9]
  );
  sprintf(j_pservo, "\"pservo\":\"[[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d],[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]]\",\n",
    pservo[0][0], pservo[0][1], pservo[0][2], pservo[0][3], pservo[0][4], pservo[0][5], pservo[0][6], pservo[0][7], pservo[0][8], pservo[0][9], pservo[0][10],
    pservo[1][0], pservo[1][1], pservo[1][2], pservo[1][3], pservo[1][4], pservo[1][5], pservo[1][6], pservo[1][7], pservo[1][8], pservo[1][9], pservo[1][10],
    pservo[2][0], pservo[2][1], pservo[2][2], pservo[2][3], pservo[2][4], pservo[2][5], pservo[2][6], pservo[2][7], pservo[2][8], pservo[2][9], pservo[2][10]
  );
  buffer = escapeXML(String(description));
  sprintf(j_description, "\"description\":\"%s\"", buffer.c_str());
  sprintf(j_flash, "\"flash\":\"[%d,%d,%d,%d,%d,%d]\"", flash_verrou, flash_vol_on, tOn, tOff, tCycle / 1000, nFlash);

  // Assemblage du JSON complet
  sprintf(JSON, "{\n%s%s%s%s%s%s\n}", j_version, j_cservo, j_delai, j_pservo, j_description, j_flash);

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  ////server.sendHeader("Content-Type", "application/octet-stream; charset=utf-8");
  server.sendHeader("Content-Type", "text/json; charset=utf-8");
  server.sendHeader("Content-Disposition", "attachment; filename=\"wetimer.json\"");

  server.send(200, "application/octet-stream", JSON);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetJson() : données envoyées.\n");
  #endif

}
void handleUpload(void) {
  
  String XML;
  StaticJsonDocument<1024> doc;
  StaticJsonDocument<1024> arr;
  DeserializationError error;
  String errorMessage = "erreur inconnue.";
  bool erreur = true; // Sera mis à false si OK

  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleUpload()\n");
  #endif

  // Format des données envoyées dans le POST :
  // envoyées avec Content-Type: text/plain
  // {
  // "version":"[WeTimer,v2.0.20252106]",
  // "cservo":"[[0,255,544,2400],[0,255,544,2400],[0,255,544,2400,]]",
  // "delai":"[200,200,200,200,200,200,200,200,200,200]",
  // "pservo":"[[100,100,100,150,170,130,120,30,90,95,10],[100,100,200,110,105,105,105,150,120,120,100],[100,150,100,150,100,100,100,100,120,120,120]]",
  // "description":"&lt;La description...&gt;"
  // "flash":"[1,1,2,35,25,3]"
  // }
  // Les données contiennent des tableaux JSON dans un document JSON. 
  // Il faut donc appeler deserializeJson() plusieurs fois.
  // Une première fois pour le document complet (doc), et ensuite pour chaque tableau (arr) JSON

  if (server.hasArg("plain")) {
    const char* jsonData = server.arg("plain").c_str();
    #ifdef DEBUG_WEB
      WT_PRINTF("----------------------------------------------------------------------\n");
      WT_PRINTF("jsonData = %s\n", jsonData);
      WT_PRINTF("----------------------------------------------------------------------\n");
    #endif
    erreur = false; // On à des données, erreur sera remis à true si erreur JSON.

    // DeSerialize le document complet :
    //--------------------------------------------------------------
    error = deserializeJson(doc, jsonData);
    if (error) {
      #ifdef DEBUG_WEB
        WT_PRINTF("handleUpload(): deserializeJson(jsonData) failed: %s\n", error.f_str());
      #endif
      errorMessage = String(error.f_str());
      erreur = true;
    }

    // Verification chaine de version dans le fichier JSON
    //--------------------------------------------------------------
    if (!erreur) {
      const char* verifVersion  = doc["version"];
      #ifdef DEBUG_WEB
        WT_PRINTF("verifVersion  = %s\n", verifVersion);
      #endif
      // Comparaison
      if (strncmp(APP_VERSION_VERIF, verifVersion, (size_t)strlen(APP_VERSION_VERIF)) != 0) {
      #ifdef DEBUG_WEB
        WT_PRINTF("verifVersion erreur : %s[0 -> %d] != %s\n", verifVersion, strlen(APP_VERSION_VERIF), APP_VERSION_VERIF);
      #endif
        errorMessage = "Signature fichier invalide.";
        erreur = true;
      }
    }

    // Récupération du tableau de config des servos
    //--------------------------------------------------------------
    if (!erreur) {
      const char* tableauCServo = doc["cservo"];
      #ifdef DEBUG_WEB
        WT_PRINTF("tableauCServo(cservo)=%s\n", tableauCServo);
      #endif
      // Deserialization tableau des configuration de servo
      error = deserializeJson(arr, tableauCServo);
      if (error) {
        #ifdef DEBUG_WEB
          WT_PRINTF("handleUpload(): deserializeJson(tableauCServo) failed: %s\n", error.f_str());
        #endif
        errorMessage = String(error.f_str());
        erreur = true;
      } else {
        // Récupération des configuration de servos
        for (int j=0; j<NB_CONFIG; j++) {
          for (int i=0; i<NB_SERVOS; i++) {
            cservo[i][j] = arr[i][j];
            #ifdef DEBUG_WEB
              WT_PRINTF("cservo[%d][%d]=%d\n", i, j, cservo[i][j]);
            #endif
          }
        }
      }
    }

    // Récupération du tableau des délais
    //--------------------------------------------------------------
    if (!erreur) {
      const char* tableauDelai = doc["delai"];
      #ifdef DEBUG_WEB
        WT_PRINTF("tableauDelai = %s\n", tableauDelai);
      #endif
      // Deserialization tableau des délais
      error = deserializeJson(arr, tableauDelai);
      if (error) {
        #ifdef DEBUG_WEB
          WT_PRINTF("handleUpload(): deserializeJson(tableauDelai) failed: %s\n", error.f_str());
        #endif
        errorMessage = String(error.f_str());
        erreur = true;
      } else {
        // Récupération des délais
        for (int i=0; i<NB_DELAI; i++) {
          delai[i] = arr[i];
          #ifdef DEBUG_WEB
            WT_PRINTF("delai[%d]=%d\n", i, delai[i]);
          #endif
        }
      }
    }

    // Récupération du tableau des positions des servos
    //--------------------------------------------------------------
    if (!erreur) {
      const char* tableauPServo = doc["pservo"];
      #ifdef DEBUG_WEB
        WT_PRINTF("tableauPServo(pservo)=%s\n", tableauPServo);
      #endif
      // Deserialization tableau des positions de servo
      error = deserializeJson(arr, tableauPServo);
      if (error) {
        #ifdef DEBUG_WEB
          WT_PRINTF("handleUpload(): deserializeJson(tableauPServo) failed: %s\n", error.f_str());
        #endif
        errorMessage = String(error.f_str());
        erreur = true;
      } else {
        // Récupération des positions de servos
        for (int j=0; j<NB_POSITIONS; j++) {
          for (int i=0; i<NB_SERVOS; i++) {
            pservo[i][j] = arr[i][j];
            #ifdef DEBUG_WEB
              WT_PRINTF("pservo[%d][%d]=%d\n", i, j, pservo[i][j]);
            #endif
          }
        }
      }
    }

    // Récupération de la description
    //--------------------------------------------------------------
    if (!erreur) {
      const char* data = doc["description"];
      #ifdef DEBUG_WEB_2
        WT_PRINTF("description data = %s\n", data);
      #endif
      String decode = unescapeXML(String(data));
      #ifdef DEBUG_WEB_2
        WT_PRINTF("decode = %s\n", decode);
      #endif
      strncpy(description, decode.c_str(), DESCRIPTION_LEN);
      #ifdef DEBUG_WEB
        WT_PRINTF("description=%s\n", description);
      #endif
    }

    // Récupération de la configuration flasher
    //--------------------------------------------------------------
    if (!erreur) {
      const char* tableauFlash = doc["flash"];
      #ifdef DEBUG_WEB_2
        WT_PRINTF("tableauFlash = %s\n", data);
      #endif
      // Deserialization tableau flash
      error = deserializeJson(arr, tableauFlash);
      if (error) {
        #ifdef DEBUG_WEB
          WT_PRINTF("handleUpload(): deserializeJson(tableauFlash) failed: %s\n", error.f_str());
        #endif
        errorMessage = String(error.f_str());
        erreur = true;
      } else {
        // Récupération de la config flash
        flash_verrou = arr[0];
        flash_vol_on = arr[1];
        tOn          = arr[2];
        tOff         = arr[3];
        tCycle       = arr[4];
        nFlash       = arr[5];
      }
    }

  } else { // if (server.hasArg("plain"))
    errorMessage = "Pas de données";
    erreur = true;
  }

  if (!erreur) {
    // Mise à jour de l'EEPROM
    updateEepromConf();
    updateEepromData();
    updateDescription();
    updateFlashData();
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <uploadconfig>OK</uploadconfig>\n");
    XML += F("</WeTimer>\n");
    // Renvoi la réponse au client http
    server.send(200, "text/xml", XML);
    #ifdef DEBUG_WEB
      WT_PRINTF("handleUpload() : OK envoyé.\n");
    #endif
  } else {
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <uploadconfig>Erreur : ") + errorMessage + F("</uploadconfig>\n");
    XML += F("</WeTimer>\n");
    // Renvoi la réponse au client http
    server.send(400, "text/xml", XML);
    #ifdef DEBUG_WEB
      WT_PRINTF("handleUpload() : Erreur envoyé.\n");
    #endif
  }

}
void handleSetApConfig(void) {
  String XML;
  String ssid = "";
  String pwd  = "";
  bool settingChange = false;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleSetAPconfig() --- %d\n", millis()/1000);
  #endif

  if (server.args() > 0) {
    //Traitement des données POST
    for (int i = 0; i < server.args(); i++) {
      #ifdef DEBUG_WEB
        WT_PRINTF("  handleSetAPconfig() - Arg n°%d –> %s = [%s]\n", i, server.argName(i), server.arg(i).c_str());
      #endif
      if (strncasecmp(server.argName(i).c_str(), "ssid", (size_t)4) == 0) {
        #ifdef DEBUG_WEB
          WT_PRINTF("  handleSetAPconfig() - SSID = %s\n", server.arg(i).c_str());
        #endif
        if (strncmp(ap_ssid, server.arg(i).c_str(), MAX_SSID_LEN) != 0) {
          strncpy(ap_ssid, server.arg(i).c_str(), MAX_SSID_LEN);
          settingChange = true;
        }
        // Si SSID vide, on rétabli la valeur par défaut
        if (server.arg(i) == "") {
          String SSID_MAC = String(DEFAULT_AP_SSID + WiFi.softAPmacAddress().substring(9));
          SSID_MAC.toCharArray(ap_ssid, MAX_SSID_LEN);
          #ifdef DEBUG_WEB
            WT_PRINTF("  handleSetAPconfig() - reset SSID to default: %s\n", ap_ssid);
          #endif
        }
      } else if (strncasecmp(server.argName(i).c_str(), "pwd", (size_t)3) == 0) {
        #ifdef DEBUG_WEB
          WT_PRINTF("  handleSetAPconfig() - pwd  = %s\n", server.arg(i).c_str());
        #endif
        if (strncmp(ap_pwd, server.arg(i).c_str(), MAX_PWD_LEN) != 0) {
          strncpy(ap_pwd, server.arg(i).c_str(), MAX_PWD_LEN);
          settingChange = true;
        }
      }
    }

    // Reconfigure le point d'accès
    WiFi.softAP(ap_ssid, ap_pwd, DEFAULT_AP_CHANNEL); // (AP ouverte si de mot de passe vide ou null)

    if (settingChange) {
      #ifdef DEBUG_WEB
        WT_PRINTF(" Sauvegarde paramètres AP en EEPROM : SSID=[%s], pwd=[%s]\n", ap_ssid, ap_pwd);
      #endif
      // Sauvegarde les nouveaux paramètres dans l'EEPROM
      EEPROM_writeStr(ADDR_AP_SSID, ap_ssid, MAX_SSID_LEN);
      EEPROM_writeStr(ADDR_AP_PWD, ap_pwd, MAX_PWD_LEN);
      EEPROM.commit();
    }


    // Réponse au client
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<setapconfig>\n");
    XML += F("  <result>OK</result>\n");
    XML += F("</setapconfig>\n");

  } else { // if (server.args() > 0)
    // Réponse au client
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<setapconfig>\n");
    XML += F("  <result>setapconfig: missing parameters!</result>\n");
    XML += F("</setapconfig>\n");
  }
  
  // Renvoi la réponse au client http
  server.send(200,"text/xml",XML);

}
void handleSetDescription(void) {

  String XML;
  StaticJsonDocument<512> doc;
  DeserializationError error;
  String errorMessage;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleSetDescription()\n");
  #endif

  if (server.hasArg("plain")) {
    const char* jsonData = server.arg("plain").c_str();
    #ifdef DEBUG_WEB
      WT_PRINTF("server.arg(\"plain\") = %s\n", jsonData);
    #endif
    error = deserializeJson(doc, jsonData);
    if (error) {
      #ifdef DEBUG_WEB
        WT_PRINTF("handleSetDescription(): deserializeJson(jsonData) failed: %s\n", error.f_str());
      #endif
      // Réponse au client
      errorMessage = String(error.f_str());
      XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
      XML += F("<WeTimer>\n");
      XML += F("  <setdescription>Erreur : ") + errorMessage + F("</setdescription>\n");
      XML += F("/<WeTimer>\n");
      // Renvoi la réponse au client http
      server.send(200,"text/xml",XML);
      return;
    }
    const char* data = doc["description"];
    String buffer = String(data);
    String decode = unescapeXML(buffer);
    const char* newDescription = decode.c_str();
    #ifdef DEBUG_WEB
      WT_PRINTF("newDescription  = %s\n", newDescription);
    #endif
    if (strncmp(description, newDescription, DESCRIPTION_LEN) != 0) {
      // Description changée, on l'enregistre dans l'EEPROM
      strncpy(description, newDescription, DESCRIPTION_LEN);
      EEPROM_writeStr(ADDR_DESCRIPTION, description, DESCRIPTION_LEN);
      EEPROM.commit();
      #ifdef DEBUG_WEB
        WT_PRINTF("Description enregistrée = %s\n", description);
      #endif
    }
    // Réponse au client
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <setdescription>OK</setdescription>\n");
    XML += F("</WeTimer>\n");
  } else { // if (server.hasArg("plain"))
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<WeTimer>\n");
    XML += F("  <setdescription>Erreur : Description absente !</setdescription>\n");
    XML += F("</WeTimer>\n");
  }
  
  // Renvoi la réponse au client http
  server.send(200,"text/xml",XML);

}
void handleResetFlashData(void) {
  // Recharge les données de l'EEPROM
  getEepromFlashData();
  // Renvoie les données au client http
  handleGetFashData();
}
void handleGetFashData(void) {

  String XML;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleGetFashData()\n");
  #endif

  // Construit la réponse
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<WeTimerFlash>\n");
  XML += F("  <flash_verrou>") + String(flash_verrou)  + F("</flash_verrou>\n");
  XML += F("  <flash_vol_on>") + String(flash_vol_on)  + F("</flash_vol_on>\n");
  XML += F("  <tOn>")          + String(tOn)           + F("</tOn>\n");
  XML += F("  <tOff>")         + String(tOff)          + F("</tOff>\n");
  XML += F("  <tCycle>")       + String(tCycle / 1000) + F("</tCycle>\n");
  XML += F("  <nFlash>")       + String(nFlash)        + F("</nFlash>\n");
  XML += F("</WeTimerFlash>\n");

  // Renvoi la réponse au client http
  server.send(200,"text/xml",XML);

  #ifdef DEBUG_WEB
    WT_PRINTF("handleGetFashData() : données envoyées.\n");
  #endif

}
void handleTestFlasher(void) {

  String XML;
  unsigned long tmpVal = 0;
  
  #ifdef DEBUG_WEB
    WT_PRINTF("Entrée dans handleTestFlasher() --- %d\n", millis()/1000);
    WT_PRINTF("Parametres html = [%s]\n", server.arg("plain").c_str());
  #endif

  if (!server.hasArg("mode")){
    #ifdef DEBUG_WEB
      WT_PRINTF("handleTestFlasher() : Flash mode absent !\n");
    #endif
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<testflasher>\n");
    XML += F("  <result>testflasher: missing parameters mode!</result>\n");
    XML += F("</testflasher>\n");
    server.send(200,"text/xml",XML);
    return;
  }

  if (server.hasArg("cycle")){
    tmpVal = strtoul(server.arg("cycle").c_str(), NULL, 10);
    if (tmpVal > 0) {;
      tCycle = tmpVal * 1000;
      #ifdef DEBUG_WEB
        WT_PRINTF("handleTestFlasher() : tCycle = %d (%s)\n", tCycle, server.arg("cycle").c_str());
      #endif
    }
  }

  if (server.hasArg("allume")){
    tmpVal = strtoul(server.arg("allume").c_str(), NULL, 10);
    if (tmpVal > 0) {;
      tOn = tmpVal;
      #ifdef DEBUG_WEB
        WT_PRINTF("handleTestFlasher() : tOn    = %d (%s)\n", tOn, server.arg("allume").c_str());
      #endif
    }
  }

  if (server.hasArg("eteind")){
    tmpVal = strtoul(server.arg("eteind").c_str(), NULL, 10);
    if (tmpVal > 0) {;
      tOff = tmpVal;
      #ifdef DEBUG_WEB
        WT_PRINTF("handleTestFlasher() : tOff   = %d (%s)\n", tOff, server.arg("eteind").c_str());
      #endif
    }
  }

  if (server.hasArg("nombre")){
    tmpVal = strtoul(server.arg("nombre").c_str(), NULL, 10);
    if (tmpVal > 0) {;
      nFlash = tmpVal;
      #ifdef DEBUG_WEB
        WT_PRINTF("handleTestFlasher() : nFlash = %d (%s)\n", nFlash, server.arg("nombre").c_str());
      #endif
    }
  }

  if (server.hasArg("flash_verrou")){
    flash_verrou = (server.arg("flash_verrou") == "1");
    #ifdef DEBUG_WEB
      WT_PRINTF("handleTestFlasher() : flash_verrou = %s\n", flash_verrou ? "true" : "false");
    #endif
  }

  if (server.hasArg("flash_vol_on")){
    flash_vol_on = (server.arg("flash_vol_on") == "1");
    #ifdef DEBUG_WEB
      WT_PRINTF("handleTestFlasher() : flash_vol_on = %s\n", flash_vol_on ? "true" : "false");
    #endif
  }

  if (server.arg("mode") == "0") {
    setFlasher(FLASH_OFF);
  } else if (server.arg("mode") == "1") {
    setFlasher(FLASH_POWER);
  } else if (server.arg("mode") == "2") {
    setFlasher(FLASH_DEVERROUILLAGE);
  } else if (server.arg("mode") == "3") {
    setFlasher(FLASH_VOL);
  } else {
    #ifdef DEBUG_WEB
      WT_PRINTF("handleTestFlasher() : Flash mode invalide (%s)\n", server.arg("mode").c_str());
    #endif
    XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
    XML += F("<testflasher>\n");
    XML += F("  <result>testflasher: invalid parameter mode!</result>\n");
    XML += F("</testflasher>\n");
    server.send(200,"text/xml",XML);
    return;
  }

  // Réponse au client
  XML  = F("<?xml version=\"1.0\" encoding=\"UTF-8\"\?>\n");
  XML += F("<testflasher>\n");
  XML += F("  <result>OK</result>\n");
  XML += F("</testflasher>\n");
  // Renvoi la réponse au client http
  server.send(200,"text/xml",XML);

}
void handleSetFashData(void) {
  // Même données que pour les tests
  handleTestFlasher();
  // Sauvegarde les données en EEPROM
  updateFlashData();
}












