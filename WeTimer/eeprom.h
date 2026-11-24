/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2026 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: eeprom.h is part of WeTimer                                   */
/*                                                                          */
/* WeTimer is free software: you can redistribute it and/or modify          */
/* it under the terms of the GNU General Public License as published by     */
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

#ifndef eeprom_h
  #define eeprom_h

  //----------------------------------------------------------------------------------------------------
  // Adresses EEPROM pour sauvegarde des paramètres
  //----------------------------------------------------------------------------------------------------
  #define EEPROM_LENGTH 1024
  //----------------------------------------------------------------------------------------------------
  #define ADDR_AP_SSID          0 //   0 + 32 =  32
  #define ADDR_AP_PWD          32 //  32 + 63 =  95
  #define ADDR_CLI_SSID        95 //  95 + 32 = 127
  #define ADDR_CLI_PWD        127 // 127 + 63 = 190
  // Configuration servos
  #define ADDR_CONF_SERVOS    200 // 200 + 4*(4*4) = 200 + 64 = 264
                                  // 4 bytes (int) x (4 servos x 4 valeurs)
  // Timing et positions
  #define ADDR_DATA           284 // 284 + 4*(11+(4*11)) = 284 + 216 = 504 
                                  // 4 bytes(int) x (11 délais + (4 servos x 11 positions))
  // Configuration flash (booleens)
  #define ADDR_FLASH_VERROU   510 // 510 + 1 = 511
  #define ADDR_FLASH_VOL_ON   511 // 511 + 1 = 522
  #define ADDR_FLASH_ON_TIME  512 // 512 + 4 = 516
  #define ADDR_FLASH_OFF_TIME 516 // 516 + 4 = 520
  #define ADDR_FLASH_T_CYCLE  520 // 520 + 4 = 524
  #define ADDR_FLASH_N_FLASH  524 // 524 + 4 = 528

  // Description de la config
  #define ADDR_DESCRIPTION   532
  
  void getEepromStartupData(void);
  void getEepromFlashData(void);
  void updateEepromConf(void);
  void updateEepromData(void);
  void updateFlashData(void);
  void updateDescription (void);
  void resetFactory(void);
  void EEPROM_format(void);
  void EEPROM_writeStr(int address, char *value, int len);

#endif
