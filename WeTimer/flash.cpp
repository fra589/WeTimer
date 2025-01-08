/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: flash.cpp is part of WeTimer / WeDT                           */
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

flashMode flashEnCours   = FLASH_OFF;
unsigned long _tCycle    = tCycle;
unsigned long nCycle     = 0;
unsigned long debutCycle = 0;
int debugCount = 0;

void setFlasher(flashMode mode) {
  // Active le flash dans le mode demandé
  WT_PRINTF("Entrée dans setFlasher(%d)\n", mode);
  switch (mode) {
    case FLASH_OFF:
      // Eteint le flasher
      digitalWrite(PIN_LED, LOW);
      break;
    case FLASH_POWER:
      // Allume le flasher de manière continue
      digitalWrite(PIN_LED, HIGH);
      break;
    case FLASH_DEVERROUILLAGE:
      if (flashEnCours != FLASH_DEVERROUILLAGE) {
        _tCycle = (nFlash + 1) * (tOn + tOff);
        nCycle = 0;
        debutCycle   = millis();
      }
      break;
    case FLASH_VOL:
      if (flashEnCours != FLASH_VOL) {
        _tCycle = tCycle;
        nCycle = 0;
        debutCycle   = millis();
      }
  }
  flashEnCours = mode;
}
void flasherLoop(void) {
  // nCycle == 0 :
  // Entre debutCycle et debutCycle + tOn, la led doit être allumée
  // Entre debutCycle + tOn et debutCycle + tOn + tOff, la led doit être éteinte
  // nCycle == 1 :
  // Entre debutCycle + tOn + tOff et debutCycle + tOn + tOff + tOn, la led doit être allumée
  // Entre debutCycle + tOn + tOff + tOn et debutCycle + tOn + tOff + tOn + tOff, la led doit être éteinte
  // => jusqu'à nFlash
  // Ensuite, on reste éteint jusqu'à _tCycle
  if ((flashEnCours == FLASH_DEVERROUILLAGE) || (flashEnCours == FLASH_VOL)) {
    unsigned long now    = millis();
    unsigned long tFlash = now - debutCycle;
    unsigned long nextOn  = (nCycle * (tOn + tOff));
    unsigned long nextOff = (nCycle * (tOn + tOff) + tOn);
    if (nCycle <= nFlash) {
      if ((tFlash >= nextOn) && (tFlash < nextOff)) {
        if (digitalRead(PIN_LED) == LOW) {
          // Allume la LED
          digitalWrite(PIN_LED, HIGH);
        }
      } else if ((tFlash >= nextOff) && (tFlash < (nextOff + tOff))) {
          if (digitalRead(PIN_LED) == HIGH) {
            // Eteint la LED
            digitalWrite(PIN_LED, LOW);
          }
          // Passe au cycle suivant
          nCycle++;
      }
    } else {
      if (tFlash >= _tCycle) {
        // On lance un nouveau cycle
        debutCycle = millis();
        nCycle = 0;
      }
    }
  }
}

bool isFlashEnCours(void) {
  return (flashEnCours != FLASH_OFF);
}

