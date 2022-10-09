/* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file flags.h
 */
#if !defined(FLAGS_H)
  #define FLAGS_H

  #include <stdbool.h>
  #include <stdint.h>

  extern uint64_t systemFlags;

  void systemFlagAction        (uint16_t systemFlag, uint16_t action);
  void synchronizeLetteredFlags(void);
  bool getFlag                 (uint16_t flag);
  void fnGetSystemFlag         (uint16_t systemFlag);
  void fnSetFlag               (uint16_t flag);
  void fnClearFlag             (uint16_t flag);
  void fnFlipFlag              (uint16_t flag);
  void fnClFAll                (uint16_t confirmation);
  void fnIsFlagClear           (uint16_t flag);
  void fnIsFlagSet             (uint16_t flag);
  void fnIsFlagClearClear      (uint16_t flag);
  void fnIsFlagSetClear        (uint16_t flag);
  void fnIsFlagClearSet        (uint16_t flag);
  void fnIsFlagSetSet          (uint16_t flag);
  void fnIsFlagClearFlip       (uint16_t flag);
  void fnIsFlagSetFlip         (uint16_t flag);
  void fnIsFlagSetFlip         (uint16_t flag);

  static inline bool isSystemFlagWriteProtected(uint16_t sf) {return (sf & 0x4000) != 0;}
  static inline bool getSystemFlag             (uint16_t sf) {return (systemFlags & ((uint64_t)1 << (sf & 0x3fff))) != 0;}
  static inline void setSystemFlag             (uint16_t sf) {systemFlags |=  ((uint64_t)1 << (sf & 0x3fff)); systemFlagAction(sf, 1);}
  static inline void clearSystemFlag           (uint16_t sf) {systemFlags &= ~((uint64_t)1 << (sf & 0x3fff)); systemFlagAction(sf, 0);}
  static inline void flipSystemFlag            (uint16_t sf) {systemFlags ^=  ((uint64_t)1 << (sf & 0x3fff)); systemFlagAction(sf, 2);}

#endif // !FLAGS_H
