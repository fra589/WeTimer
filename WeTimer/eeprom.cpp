/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: eeprom.cpp is part of WeTimer / WeDT                          */
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

void getEepromStartupData(void) {
  // Récupération des paramètres dans l'EEPROM ou de leur valeur par défaut

  char charTmp;
  int dTmp;
  int data_address = ADDR_DATA;
  int conf_address = ADDR_CONF_SERVOS;
  
  #ifdef DEBUG
    bool default_value;
  #endif
  
  EEPROM.begin(EEPROM_LENGTH);
  { // Lecture des paramètres EEPROM
    charTmp = char(EEPROM.read(ADDR_AP_SSID));
    if (charTmp != 0xFF) {
      ap_ssid[0] = charTmp;
      for (int i=1; i<MAX_SSID_LEN; i++) {
        ap_ssid[i] = char(EEPROM.read(ADDR_AP_SSID + i));
      }
    }
    if (strcmp(ap_ssid, DEFAULT_AP_SSID) == 0) {
      String SSID_MAC = String(DEFAULT_AP_SSID + WiFi.softAPmacAddress().substring(9));
      SSID_MAC.toCharArray(ap_ssid, MAX_SSID_LEN);
    }

    charTmp = char(EEPROM.read(ADDR_AP_PWD));
    if (charTmp != 0xFF) {
      ap_pwd[0] = charTmp;
      for (int i=1; i<MAX_PWD_LEN; i++) {
        ap_pwd[i] = char(EEPROM.read(ADDR_AP_PWD + i));
      }
    }

    charTmp = char(EEPROM.read(ADDR_CLI_SSID));
    if (charTmp != 0xFF) {
      cli_ssid[0] = charTmp;
      for (int i=1; i<MAX_SSID_LEN; i++) {
        cli_ssid[i] = char(EEPROM.read(ADDR_CLI_SSID + i));
      }
    }
    
    charTmp = char(EEPROM.read(ADDR_CLI_PWD));
    if (charTmp != 0xFF) {
      cli_pwd[0] = charTmp;
      for (int i=1; i<MAX_PWD_LEN; i++) {
        cli_pwd[i] = char(EEPROM.read(ADDR_CLI_PWD + i));
      }
    }
    #ifdef DEBUG
      WT_PRINTF("ap_ssid................ = %s\n", ap_ssid);
      WT_PRINTF("ap_pwd................. = %s\n", ap_pwd);
      WT_PRINTF("cli_ssid............... = %s\n", cli_ssid);
      WT_PRINTF("cli_pwd................ = %s\n", cli_pwd);
    #endif
  } // Fin lecture des paramètres EEPROM
  { // Lecture configurations servos
    for (int i=0; i<NB_SERVOS; i++, conf_address += sizeof(int)) {
      for (int j=0; j<NB_CONFIG; j++, conf_address += sizeof(int)) {
        EEPROM.get(conf_address, dTmp);
        if (dTmp != 0xFFFFFFFF) {
          cservo[i][j] = dTmp;
          #ifdef DEBUG
            default_value = false;
          } else {
            default_value = true;
          #endif
        }
        #ifdef DEBUG_EEPROM
          WT_PRINTF("conf_address = %d - cservo[%d][%d] = %d%s\n", conf_address, i, j , cservo[i][j], default_value?" (default)":"");
        #endif
      }
    }
  } // Fin lecture configurations servos
  { // Lecture des positions
    for (int i=0; i<NB_DELAI; i++, data_address += sizeof(int)) {
      EEPROM.get(data_address, dTmp);
      if (dTmp != 0xFFFFFFFF) {
        delai[i] = dTmp;
        #ifdef DEBUG
        default_value = false;
      } else {
        default_value = true;
        #endif
      }
      #ifdef DEBUG_EEPROM
        WT_PRINTF("data_address = %d - delai[%d] = %d%s\n", data_address, i, delai[i], default_value?" (default)":"");
      #endif
    }
    for (int i=0; i<NB_SERVOS; i++, data_address += sizeof(int)) {
      for (int j=0; j<NB_POSITIONS; j++, data_address += sizeof(int)) {
        EEPROM.get(data_address, dTmp);
        if (dTmp != 0xFFFFFFFF) {
          pservo[i][j] = dTmp;
          #ifdef DEBUG
          default_value = false;
        } else {
          default_value = true;
          #endif
        }
        #ifdef DEBUG_EEPROM
          WT_PRINTF("data_address = %d - pservo[%d][%d] = %d%s\n", data_address, i, j , pservo[i][j], default_value?" (default)":"");
        #endif
      }
    }
  } // Fin lecture des positions
  { // Lecture de la description
    charTmp = char(EEPROM.read(ADDR_DESCRIPTION));
    if (charTmp != 0xFF) {
      description[0] = charTmp;
      for (int i=1; i<DESCRIPTION_LEN; i++) {
        description[i] = char(EEPROM.read(ADDR_DESCRIPTION + i));
      }
    }
    #ifdef DEBUG
      WT_PRINTF("description............ = %s\n", description);
    #endif
  }
}

void updateEepromConf(void) {

  int conf_address = ADDR_CONF_SERVOS;
  int dTmp;
  bool dataUpdated = false;

  for (int i=0; i<NB_SERVOS; i++, conf_address += sizeof(int)) {
    for (int j=0; j<NB_CONFIG; j++, conf_address += sizeof(int)) {
      EEPROM.get(conf_address, dTmp);
      if (dTmp != cservo[i][j]) {
        EEPROM.put(conf_address, cservo[i][j]);
        dataUpdated = true;
        #ifdef DEBUG_EEPROM
          WT_PRINTF("Mise à jour @address = %d - cservo[%d][%d] = %d\n", conf_address, i, j , cservo[i][j]);
        #endif
      }
    }
  }

  if (dataUpdated) {
    // Valide l'enregistrement si une donnée à changé.
    EEPROM.commit();
  }

}

void updateEepromData(void) {

  int data_address = ADDR_DATA;
  int dTmp;
  bool dataUpdated = false;
  
  for (int i=0; i<NB_DELAI; i++, data_address += sizeof(int)) {
    EEPROM.get(data_address, dTmp);
    if (dTmp != delai[i]) {
      EEPROM.put(data_address, delai[i]);
      dataUpdated = true;
      #ifdef DEBUG_EEPROM
        WT_PRINTF("Mise à jour @address = %d delai[%d] = %d\n", data_address, i, delai[i]);
      #endif
    }
  }
  for (int i=0; i<NB_SERVOS; i++, data_address += sizeof(int)) {
    for (int j=0; j<NB_POSITIONS; j++, data_address += sizeof(int)) {
      EEPROM.get(data_address, dTmp);
      if (dTmp != pservo[i][j]) {
        EEPROM.put(data_address, pservo[i][j]);
        dataUpdated = true;
        #ifdef DEBUG_EEPROM
          WT_PRINTF("Mise à jour @address = %d - pservo[%d][%d] = %d\n", data_address, i, j , pservo[i][j]);
        #endif
      }
    }
  }

  if (dataUpdated) {
    // Valide l'enregistrement si une donnée à changé.
    EEPROM.commit();
  }

}
void updateDescription(void) {
  char charTmp;
  char oldDescription[DESCRIPTION_LEN];
  // Lecture de la description déjà dans l'EEPROM
  charTmp = char(EEPROM.read(ADDR_DESCRIPTION));
  if (charTmp != 0xFF) {
    oldDescription[0] = charTmp;
    for (int i=1; i<DESCRIPTION_LEN; i++) {
      oldDescription[i] = char(EEPROM.read(ADDR_DESCRIPTION + i));
    }
  }
  if (strncmp(description, oldDescription, DESCRIPTION_LEN) != 0) {
    // Description changée, on l'enregistre dans l'EEPROM
    EEPROM_writeStr(ADDR_DESCRIPTION, description, DESCRIPTION_LEN);
    EEPROM.commit();
    #ifdef DEBUG_EEPROM
      WT_PRINTF("Description enregistrée = %s\n", description);
    #endif
  }
}
void resetFactory(void) {
  // Reset de tous les paramètres à leur valeur par défaut
  // et reinitialisation EEPROM

  #ifdef DEBUG
    WT_PRINTF("Entrée dans resetFactory()\n");
  #endif

  strcpy(ap_ssid,  DEFAULT_AP_SSID);
  strcpy(ap_pwd,   DEFAULT_AP_PWD);
  strcpy(cli_ssid, DEFAULT_CLI_SSID);
  strcpy(cli_pwd,  DEFAULT_CLI_PWD);

  // Sauvegarde en EEPROM
  EEPROM_format(); // On efface tout

  // Pas besoin car si l'EEPROM ne contient pas de données, 
  // on utilise les valeurs définies par défaut
  /*
  EEPROM_writeStr(ADDR_AP_SSID,  ap_ssid,  MAX_SSID_LEN);
  EEPROM_writeStr(ADDR_AP_PWD,   ap_pwd,   MAX_PWD_LEN);
  EEPROM_writeStr(ADDR_CLI_SSID, cli_ssid, MAX_SSID_LEN);
  EEPROM_writeStr(ADDR_CLI_PWD,  cli_pwd,  MAX_PWD_LEN);

  #ifdef DEBUG
    WT_PRINTF("  EEPROM.commit()\n");
  #endif
  EEPROM.commit();
  */

}

void EEPROM_format(void) {
  // Efface tout le contenu de l'EEPROM
  
  #ifdef DEBUG_EEPROM
    WT_PRINTF("  EEPROM_format()\n");
  #endif
  
  for ( int i = 0 ; i < EEPROM_LENGTH ; i++ ) {
    EEPROM.write(i, 0xFF);
  }
  
  EEPROM.commit();
  
}

void EEPROM_writeStr(int address, char *value, int len) {
  // Write string char to eeprom
  
  #ifdef DEBUG_EEPROM
    WT_PRINTF("  EEPROM_writeStr(%d, \"%s\", %d)\n", address, value, len);
  #endif
  
  for (int i=0; i<len; i++) {
    EEPROM.write(address + i, value[i]);
  }
  
}
