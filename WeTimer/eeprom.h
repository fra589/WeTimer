/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: eeprom.h is part of WeTimer / WeDT                            */
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

#ifndef eeprom_h
  #define eeprom_h

  //----------------------------------------------------------------------------------------------------
  // Adresses EEPROM pour sauvegarde des paramètres
  //----------------------------------------------------------------------------------------------------
  #define EEPROM_LENGTH 1024
  //----------------------------------------------------------------------------------------------------
  #define ADDR_AP_SSID         0 //   0 + 32 =  32
  #define ADDR_AP_PWD         32 //  32 + 63 =  95
  #define ADDR_CLI_SSID       95 //  95 + 32 = 127
  #define ADDR_CLI_PWD       127 // 126 + 63 = 190
  // Configuration servos
  #define ADDR_CONF_SERVOS   200 // 132 + 4*(3*4) = 132 + 48 = 180
  // Timing et positions
  #define ADDR_DATA          256 // 256 + 4*(10+(3*11)) = 256 + 172 = 428 
                                 // 4 bytes(int) x (10 délais + (3 servos x 11 positions))
  #define ADDR_DESCRIPTION   512 // Description de la config
  
  void getEepromStartupData(void);
  void updateEepromConf(void);
  void updateEepromData(void);
  void updateDescription (void);
  void resetFactory(void);
  void EEPROM_format(void);
  void EEPROM_writeStr(int address, char *value, int len);

#endif
