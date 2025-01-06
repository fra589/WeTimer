/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: websocket.h is part of WeTimer / WeDT                         */
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

#ifndef websocket_h
  #define websocket_h

  void webSocketInit(void);
  void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
  void webSocketSend(const char* varName, String value);
#endif
