/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: wifi.cpp is part of WeTimer / WeDT                            */
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

/*
 * A voir pour la mise hors tension du wifi pour économiser le courant...
 * https://github.com/esp8266/Arduino/issues/644#issuecomment-174932102
*/


#include "WeTimer.h"

#ifdef DEBUG_WIFI
  // Prise en charge des évennements
  WiFiEventHandler stationConnectedHandler;
  WiFiEventHandler stationDisconnectedHandler;
  WiFiEventHandler probeRequestPrintHandler;
  
  void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
    WT_PRINTF("Station connectée : %s aid = %d\n", macToString(evt.mac).c_str(), evt.aid);
  }

  void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
    WT_PRINTF("Station déconnectée : %s\n", macToString(evt.mac).c_str());
  }
#endif

// Configuration WiFi AP 
void wifiApInit(void) {

  // Soft AP network parameters
  IPAddress apIP(10, 10, 10, 10);
  IPAddress gateway(10, 10, 10, 10);
  IPAddress netMsk(255, 255, 255, 0);
  int ssid_hidden = 0;    // 1 pour masquer le SSID
  int max_connection = 4; // 8 au maximum possible, 4 max préconisé pour les perfos et la stabilité

  #ifdef DEBUG
    WT_PRINTF("\nConfiguring access point, SSID = <%s>...\n", ap_ssid);
    Serial.flush();
  #endif

  WiFi.softAPConfig(apIP, gateway, netMsk);
  // AP ouverte si mot de passe vide ou null
  WiFi.softAP(ap_ssid, ap_pwd, DEFAULT_AP_CHANNEL, ssid_hidden, max_connection);
  delay(500);

  #ifdef DEBUG
    WT_PRINTF("AP IP address  = %s\n", WiFi.softAPIP().toString().c_str());
    WT_PRINTF("AP MAC address = %s\n", WiFi.softAPmacAddress().c_str());
    WT_PRINTF("Wifi channel   = %d\n", WiFi.channel());
    Serial.flush();
  #endif
  #ifdef DEBUG_WIFI
    // Register event handlers.
    // Callback functions will be called as long as these handler objects exist.
    // Call "onStationConnected" each time a station connects
    stationConnectedHandler = WiFi.onSoftAPModeStationConnected(&onStationConnected);
    // Call "onStationDisconnected" each time a station disconnects
    stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onStationDisconnected);
  #endif

  // Setup the DNS server redirecting all domains to the apIP
  dnsServer.setTTL(300);
  dnsServer.start(DNS_PORT, "*", apIP);

}

void wifiClientInit(void) {

  unsigned long startWifiClient;

  #ifdef DEBUG
    WT_PRINTF("Entrée dans wifiClientInit(%s)\n", cli_ssid);
    Serial.flush();
  #endif

  // Empeche le wifi client de se connecter avec d'anciens paramètres résiduels en EEPROM.
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect (false );

  // Connection Cliente si SSID défini
  if (cli_ssid[0] != '\0') {
    #ifdef DEBUG
      WT_PRINTF("Connexion à %s...\n", cli_ssid);
      Serial.flush();
    #endif
    startWifiClient = millis();
    WiFi.begin(cli_ssid, cli_pwd);
    while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      if (millis() - startWifiClient > 15000) {
        break; // Timeout = 15 secondes
      }
    }
    if (WiFi.status() == WL_CONNECTED) {
      #ifdef DEBUG
        WT_PRINTF("OK\n");
        WT_PRINTF("Local  IP = %s\n", WiFi.localIP().toString().c_str());
        WT_PRINTF("Remote IP = %s\n", WiFi.gatewayIP().toString().c_str());
        Serial.flush();
      #endif
      WiFi.setAutoConnect(true);
      WiFi.setAutoReconnect(true);
      //Start mDNS with APP_NAME
      if (MDNS.begin(myHostname, WiFi.localIP())) {
        ;
        #ifdef DEBUG
          WT_PRINTF("MDNS started\n");
        #endif
      } else {
        ;
        #ifdef DEBUG
          WT_PRINTF("MDNS failed\n");
        #endif
      }
    } else {
      #ifdef DEBUG
        WT_PRINTF("FAIL, WiFi.status() = %s\n", (getWiFiStatus(WiFi.status())).c_str());
        switch (WiFi.status()) {
          case WL_IDLE_STATUS:
            WT_PRINTF("Erreur : Wi-Fi is in process of changing between statuses\n");
          break;
          case WL_NO_SSID_AVAIL:
            WT_PRINTF("Erreur : SSID cannot be reached\n");
          break;
          case WL_CONNECT_FAILED:
            WT_PRINTF("Erreur : Connexion failed\n");
          break;
          case WL_WRONG_PASSWORD:
            WT_PRINTF("Erreur : Password is incorrect\n");
          break;
          case WL_DISCONNECTED:
            WT_PRINTF("Erreur : Module is not configured in station mode\n");
          break;
        }
      #endif
      // Connection failed, on force le mode AP uniquement
      WiFi.mode(WIFI_AP);
    }
  } else {
    // cli_ssid est vide, pas de connection
    WiFi.mode(WIFI_AP);
  }
}
void getWifiNetworks(void) {
  String result = "";
  String ssid;
  uint8_t encryptionType;
  int32_t RSSI;
  uint8_t* BSSID;
  int32_t channel;
  bool isHidden;
  int nReseaux;
  int i;

  nReseaux = WiFi.scanNetworks();
  for (i = 0; i < nReseaux; i++) {
    WiFi.getNetworkInfo(i, ssid, encryptionType, RSSI, BSSID, channel, isHidden);
    WT_PRINTF("WiFi network #%d : %s, Ch:%d (%ddBm) %s %s\n", i + 1, ssid.c_str(), channel, RSSI, getEncryptionText(encryptionType).c_str(), isHidden ? "hidden" : "");
  }
  WiFi.scanDelete();
}
bool isIp(String str) {
  for (size_t i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}
String getEncryptionText(const uint8_t encryptionType) {
  const char tblEncryptionText[][22] = {
    "ENC_TYPE_NONE",
    "ENC_TYPE_WEP",
    "ENC_TYPE_WPA_PSK",
    "ENC_TYPE_WPA2_PSK",
    "ENC_TYPE_WPA_WPA2_PSK"
  };
  return String(tblEncryptionText[encryptionType]);
}

#ifdef DEBUG_WIFI
  String macToString(const unsigned char* mac) {
    char buf[18];
    snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buf);
  }
#endif

String getWiFiStatus(wl_status_t wifiStatus) {
 
  switch (wifiStatus) {
    case WL_NO_SHIELD:
      return(String("WL_NO_SHIELD"));
      break;
    case WL_IDLE_STATUS:
      return(String("WL_IDLE_STATUS"));
      break;
    case WL_NO_SSID_AVAIL:
      return(String("WL_NO_SSID_AVAIL"));
      break;
    case WL_SCAN_COMPLETED:
      return(String("WL_SCAN_COMPLETED"));
      break;
    case WL_CONNECTED:
      return(String("WL_CONNECTED"));
      break;
    case WL_CONNECT_FAILED:
      return(String("WL_CONNECT_FAILED"));
      break;
    case WL_CONNECTION_LOST:
      return(String("WL_CONNECTION_LOST"));
      break;
    case WL_WRONG_PASSWORD:
      return(String("WL_WRONG_PASSWORD"));
      break;
    case WL_DISCONNECTED:
      return(String("WL_DISCONNECTED"));
      break;
  }
  return(String(wifiStatus));
}

// IP to String.
String IPtoString(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String((ip >> (8 * 3)) & 0xFF);
  return res;
}
void apListClients(void) {
  unsigned char softap_stations_cnt;
  struct station_info *stat_info;
  struct ip4_addr *clientIP;
  IPAddress ip;

  softap_stations_cnt = wifi_softap_get_station_num(); // Count of stations which are connected to ESP8266 soft-AP
  WT_PRINTF("WiFi AP : %d station(s) connectée(s)\n", softap_stations_cnt);
  stat_info = wifi_softap_get_station_info();
  while (stat_info != NULL) {
    clientIP = &stat_info->ip;
    ip = clientIP->addr;
    WT_PRINTF("- Client IP = %s - MAC = %s\n", IPtoString(ip).c_str(), macToString(stat_info->bssid).c_str());
    stat_info = STAILQ_NEXT(stat_info, next);
  }
}
int apCountClients(void) {
  uint8_t result;
  result = wifi_softap_get_station_num();
  return (int)result;
}



