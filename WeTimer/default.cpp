/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: default.cpp is part of WeTimer / WeDT                         */
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

void defaultInit(void) {

  //--------------------------------------------------------------------------
  // Initialisation des données par défaut
  //--------------------------------------------------------------------------
  // Délais minuterie
  delai[0] = DEFAULT_DELAY_0;
  delai[1] = DEFAULT_DELAY_1;
  delai[2] = DEFAULT_DELAY_2;
  delai[3] = DEFAULT_DELAY_3;
  delai[4] = DEFAULT_DELAY_4;
  delai[5] = DEFAULT_DELAY_5;
  delai[6] = DEFAULT_DELAY_6;
  delai[7] = DEFAULT_DELAY_7;
  delai[8] = DEFAULT_DELAY_8;
  delai[9] = DEFAULT_DELAY_9;
  // Config servos
  cservo[0][0] = DEFAULT_CSERVO_0_0;
  cservo[1][0] = DEFAULT_CSERVO_1_0;
  cservo[2][0] = DEFAULT_CSERVO_2_0;
  cservo[0][1] = DEFAULT_CSERVO_0_1;
  cservo[1][1] = DEFAULT_CSERVO_1_1;
  cservo[2][1] = DEFAULT_CSERVO_2_1;
  cservo[0][2] = DEFAULT_CSERVO_0_2;
  cservo[1][2] = DEFAULT_CSERVO_1_2;
  cservo[2][2] = DEFAULT_CSERVO_2_2;
  cservo[0][3] = DEFAULT_CSERVO_0_3;
  cservo[1][3] = DEFAULT_CSERVO_1_3;
  cservo[2][3] = DEFAULT_CSERVO_2_3;
  // Positions servos
  pservo[0][0] = DEFAULT_PSERVO_0_0;
  pservo[1][0] = DEFAULT_PSERVO_1_0;
  pservo[2][0] = DEFAULT_PSERVO_2_0;
  pservo[0][1] = DEFAULT_PSERVO_0_1;
  pservo[1][1] = DEFAULT_PSERVO_1_1;
  pservo[2][1] = DEFAULT_PSERVO_2_1;
  pservo[0][2] = DEFAULT_PSERVO_0_2;
  pservo[1][2] = DEFAULT_PSERVO_1_2;
  pservo[2][2] = DEFAULT_PSERVO_2_2;
  pservo[0][3] = DEFAULT_PSERVO_0_3;
  pservo[1][3] = DEFAULT_PSERVO_1_3;
  pservo[2][3] = DEFAULT_PSERVO_2_3;
  pservo[0][4] = DEFAULT_PSERVO_0_4;
  pservo[1][4] = DEFAULT_PSERVO_1_4;
  pservo[2][4] = DEFAULT_PSERVO_2_4;
  pservo[0][5] = DEFAULT_PSERVO_0_5;
  pservo[1][5] = DEFAULT_PSERVO_1_5;
  pservo[2][5] = DEFAULT_PSERVO_2_5;
  pservo[0][6] = DEFAULT_PSERVO_0_6;
  pservo[1][6] = DEFAULT_PSERVO_1_6;
  pservo[2][6] = DEFAULT_PSERVO_2_6;
  pservo[0][7] = DEFAULT_PSERVO_0_7;
  pservo[1][7] = DEFAULT_PSERVO_1_7;
  pservo[2][7] = DEFAULT_PSERVO_2_7;
  pservo[0][8] = DEFAULT_PSERVO_0_8;
  pservo[1][8] = DEFAULT_PSERVO_1_8;
  pservo[2][8] = DEFAULT_PSERVO_2_8;
  pservo[0][9] = DEFAULT_PSERVO_0_9;
  pservo[1][9] = DEFAULT_PSERVO_1_9;
  pservo[2][9] = DEFAULT_PSERVO_2_9;
  pservo[0][10] = DEFAULT_PSERVO_0_10;
  pservo[1][10] = DEFAULT_PSERVO_1_10;
  pservo[2][10] = DEFAULT_PSERVO_2_10;
  // Description de la config
  strncpy(description, DEFAULT_DESCRIPTION, DESCRIPTION_LEN);

}
