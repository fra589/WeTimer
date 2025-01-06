/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: telnet.cpp is part of WeTimer / WeDT                          */
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

void telnetInit(void) {

  // Initialize Telnet server
  telnet.onConnect(onTelnetConnect);
  telnet.onConnectionAttempt(onTelnetConnectionAttempt);
  telnet.onReconnect(onTelnetReconnect);
  telnet.onDisconnect(onTelnetDisconnect);
  telnet.onInputReceived(onTelnetInput);

  if (telnet.begin(TELNET_PORT)) {
    ;
    #ifdef DEBUG
      WT_PRINTF("Serveur telnet démarré.\n");
      Serial.flush();
    #endif
  } else {
    ;
    #ifdef DEBUG
      WT_PRINTF("Erreur de démarrage du serveur telnet !\n");
      Serial.flush();
    #endif
  }

}
void onTelnetConnect(String ip) {
  WT_PRINTF("Telnet : %s connecté\n", ip.c_str());
  telnet.printf("Telnet connecté sur %s\n", ap_ssid);
  telnet.printf("> ");
}
void onTelnetDisconnect(String ip) {
  WT_PRINTF("Telnet : %s déconnecté\n", ip.c_str());
  telnet.printf("> ");
}
void onTelnetReconnect(String ip) {
  WT_PRINTF("Telnet : %s reconnecté\n", ip.c_str());
  telnet.printf("> ");
}
void onTelnetConnectionAttempt(String ip) {
  WT_PRINTF("Telnet : %s tentative de connexion\n", ip.c_str());
  telnet.printf("> ");
}
void onTelnetInput(String str) {
  //WT_PRINTF("%s\n", str.c_str());
  if ((str == "exit") || (str == "quit") || (str == "bye")) {
    telnet.println("bye\n");
    telnet.disconnectClient();
  } else {
    Serial.printf("%s\n", str.c_str());
    // Remplace inputString:
    strncpy(serialInput, str.c_str(), SERIAL_BUFF_LEN);
    serialComplete = true;
  }
}



