/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: wifi.h is part of WeTimer / WeDT                              */
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

#ifndef wifi_h
  #define wifi_h

  //----------------------------------------------------------------------------------------------------
  // Valeurs des paramètres par défauts
  //----------------------------------------------------------------------------------------------------
  #define DEFAULT_AP_SSID  "WeTimer_" // SSID de l'AP minuterie
  #define DEFAULT_AP_PWD   ""         // WPA-PSK/WPA2-PSK AP
  #define DEFAULT_AP_CHANNEL 3
  #define DEFAULT_CLI_SSID ""         // SSID client (la minuterie se connecte si défini)
  #define DEFAULT_CLI_PWD  ""         // WPA-PSK/WPA2-PSK client

  void wifiApInit(void);
  void wifiClientInit(void);
  void getWifiNetworks(void);
  String getEncryptionText(const uint8_t encryptionType);
  bool isIp(String str);
  String getWiFiStatus(wl_status_t wifiStatus);
  String IPtoString(IPAddress ip);
  void apListClients(void);
  int apCountClients(void);
  #ifdef DEBUG_WIFI
    void onStationConnected(const WiFiEventSoftAPModeStationConnected& evt);
    void onStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt);
    String macToString(const unsigned char* mac);
  #endif

#endif
