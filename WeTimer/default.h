/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: default.h is part of WeTimer / WeDT                           */
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

#ifndef default_h
  #define default_h

  // Délais en centièmes de secondes
  #define DEFAULT_DELAY_0   400 // Délai armement minuterie
  #define DEFAULT_DELAY_1    20 // Délai entre larguage et PITCHUP
  #define DEFAULT_DELAY_2    15 // Durée PITCHUP
  #define DEFAULT_DELAY_3    60 // Durée MONTEE_1
  #define DEFAULT_DELAY_4    90 // Durée MONTEE_2
  #define DEFAULT_DELAY_5   300 // Durée BUNT
  #define DEFAULT_DELAY_6  1000 // Durée PLANE_1
  #define DEFAULT_DELAY_7 18000 // Délai entre larguage et DT (PLANE_2)
  #define DEFAULT_DELAY_8  1500 // Durée DT avant PARC
  #define DEFAULT_DELAY_9     0 // Délai refermeture crochet (laisser 0, réservé usage futur)
  // Config servos - Configuration limites servos 3 x valeur prog min, valeur prog max,
  //                 microseconds min, microsecond max.
  #define DEFAULT_CSERVO_0_0  MIN_SERVO_PROGRAMATION // valeur prog min
  #define DEFAULT_CSERVO_1_0  MIN_SERVO_PROGRAMATION
  #define DEFAULT_CSERVO_2_0  MIN_SERVO_PROGRAMATION
  #define DEFAULT_CSERVO_0_1  MAX_SERVO_PROGRAMATION // valeur prog max
  #define DEFAULT_CSERVO_1_1  MAX_SERVO_PROGRAMATION
  #define DEFAULT_CSERVO_2_1  MAX_SERVO_PROGRAMATION
  #define DEFAULT_CSERVO_0_2  MIN_SERVO_MICROSECONDS // microseconds min
  #define DEFAULT_CSERVO_1_2  MIN_SERVO_MICROSECONDS
  #define DEFAULT_CSERVO_2_2  MIN_SERVO_MICROSECONDS
  #define DEFAULT_CSERVO_0_3  MAX_SERVO_MICROSECONDS // microsecond max.
  #define DEFAULT_CSERVO_1_3  MAX_SERVO_MICROSECONDS
  #define DEFAULT_CSERVO_2_3  MAX_SERVO_MICROSECONDS
  // Positions servos
  #define DEFAULT_PSERVO_0_0  100 // Parc
  #define DEFAULT_PSERVO_1_0  100
  #define DEFAULT_PSERVO_2_0  100
  #define DEFAULT_PSERVO_0_1  100 // Treuil montée
  #define DEFAULT_PSERVO_1_1  100
  #define DEFAULT_PSERVO_2_1  150
  #define DEFAULT_PSERVO_0_2  100 // Treuil virage
  #define DEFAULT_PSERVO_1_2  200
  #define DEFAULT_PSERVO_2_2  100
  #define DEFAULT_PSERVO_0_3  150 // Déverouillage
  #define DEFAULT_PSERVO_1_3  110
  #define DEFAULT_PSERVO_2_3  150
  #define DEFAULT_PSERVO_0_4  170 // Pitchup
  #define DEFAULT_PSERVO_1_4  105
  #define DEFAULT_PSERVO_2_4  100
  #define DEFAULT_PSERVO_0_5  130 // Montée 1
  #define DEFAULT_PSERVO_1_5  105
  #define DEFAULT_PSERVO_2_5  100
  #define DEFAULT_PSERVO_0_6  120 // Montée 2
  #define DEFAULT_PSERVO_1_6  105
  #define DEFAULT_PSERVO_2_6  100
  #define DEFAULT_PSERVO_0_7   30 // Bunt
  #define DEFAULT_PSERVO_1_7  150
  #define DEFAULT_PSERVO_2_7  100
  #define DEFAULT_PSERVO_0_8   90 // Plané 1
  #define DEFAULT_PSERVO_1_8  120
  #define DEFAULT_PSERVO_2_8  120
  #define DEFAULT_PSERVO_0_9   95 // Plané 2
  #define DEFAULT_PSERVO_1_9  120
  #define DEFAULT_PSERVO_2_9  120
  #define DEFAULT_PSERVO_0_10  10 // DT
  #define DEFAULT_PSERVO_1_10 100
  #define DEFAULT_PSERVO_2_10 120
  #define DEFAULT_DESCRIPTION "<Description configuration WeTimer>"
  void defaultInit(void);

#endif
