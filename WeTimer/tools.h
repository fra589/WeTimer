/*************************************************************************/
/*                                                                       */
/* Copyright (C) 2021 Gauthier Brière  (gauthier.briere "at" gmail.com)  */
/*                                                                       */
/* This file: tools.h is part of WeTimer                                 */
/*                                                                       */
/* WeTimer is free software: you can redistribute it and/or modify it    */
/* under the terms of the GNU General Public License as published by     */
/* the Free Software Foundation, either version 3 of the License, or     */
/* (at your option) any later version.                                   */
/*                                                                       */
/* WeTimer is distributed in the hope that it will be useful, but        */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */
/*                                                                       */
/*************************************************************************/

#ifndef tools_h
  #define tools_h
  
  bool isIp(String str);
  String toStringIp(IPAddress ip);
  void EEPROM_format();
  void EEPROM_writeInt(int adresse, int val);
  int EEPROM_readInt(int adresse);
  void EEPROM_writeLong(int address, long value);
  long EEPROM_readLong(int address);
  void EEPROM_writeStr(int address, char *value, int len);
#endif // tools_h
