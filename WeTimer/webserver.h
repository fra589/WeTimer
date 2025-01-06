/****************************************************************************/
/*                                                                          */
/* Copyright (C) 2021-2025 Gauthier Brière (gauthier.briere "at" gmail.com) */
/*                                                                          */
/* This file: webserver.h is part of WeTimer / WeDT                         */
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

#ifndef webserver_h
  #define webserver_h
  
  #define GENERATE_204

  #define ROOT_FILE "/index.html"

  void webServerInit(void);
  void sendEmpty(void);
  bool captivePortal(void);
  void handleRoot(void);
  #ifdef GENERATE_204
    void generate_204(void);
    void handleGen204(void);
  #endif
  void handleNotFound(void);
  bool handleFileRead(String path);
  void handleGetVersion(void);
  void handleGetStatus(void);
  void handleGetConfig(void);
  void handleSetConfig(void);
  void handleGetTempVol(void);
  void handleSetTempVol(void);
  void handleGetServoConfig(void);
  void handleSetServoConfig(void);
  void handleServoTest(void);
  void handleGetBatterie(void);
  void handleRDT(void);
  void handleGetJson(void);
  void handleUpload(void);
  void handleSetApConfig(void);
  void handleSetDescription(void);

#endif
