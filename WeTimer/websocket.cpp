/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: websocket.cpp is part of WeTimer / WeDT                       */
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

void webSocketInit(void) {

  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  #ifdef DEBUG
    WT_PRINTF("Serveur webSocket démarré.\n");
    Serial.flush();
  #endif    

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  
  #ifdef DEBUG
    WT_PRINTF("Entrée dans webSocketEvent().\n");
    Serial.flush();
  #endif    

  switch (type) {
    case WStype_DISCONNECTED:
      #ifdef DEBUG
        WT_PRINTF("webSocketEvent(): [%u] Disconnected!\n", num);
        break;
      #endif
    case WStype_CONNECTED:
      #ifdef DEBUG
      {
        IPAddress ip = webSocket.remoteIP(num);
        WT_PRINTF("webSocketEvent(): [%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      #endif
      break;
    case WStype_TEXT:
      #ifdef DEBUG
        WT_PRINTF("[%u] Received text: %s\n", num, payload);
        // Send a response back to the client
        String echoMessage = "Received:  " + String((char*)payload);
        webSocket.sendTXT(num, echoMessage);
      #endif
      break;
  }
}

void webSocketSend(const char* varName, String value) {

  String json;

  json = "{\"key\":\"";
  json+=varName;
  json+="\", \"value\":\"";
  json+=value;
  json+="\"}";
  #ifdef DEBUG
    WT_PRINTF("webSocketSend() sent: %s\n", json.c_str());
  #endif

  webSocket.broadcastTXT(json);     // This function sends the message to all connected clients.

}




