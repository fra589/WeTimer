/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: flash.h is part of WeTimer / WeDT                             */
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

#ifndef flash_h
  #define flash_h

  #define ON_TIME    35 //  35 milliseconds
  #define OFF_TIME   25 //  25 milliseconds
  #define T_CYCLE  2000 //   2 secondes
  #define N_FLASH     5 // Number of flash by cycles

  enum flashMode {
    FLASH_OFF,            // 0 Flasher éteint
    FLASH_POWER,          // 1 Flasher alimenté en continu
    FLASH_DEVERROUILLAGE, // 2 Clignotement très rapide flasher
    FLASH_VOL,            // 3 Clignottement normal flasher
  };

  void setFlasher(flashMode mode);
  void flasherLoop(void);
  
#endif
