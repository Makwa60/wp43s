// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

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

  /**
   * Returns the status of a flag
   *
   * \param[in] flag
   */
  bool getFlag                 (uint16_t flag);

  /**
   * Returns the status of a system flag
   *
   * \param[in] systemFlag
   */
  void fnGetSystemFlag         (uint16_t systemFlag);

  /**
   * Sets a flag
   *
   * \param[in] flag
   */
  void fnSetFlag               (uint16_t flag);

  /**
   * Clears a flag
   *
   * \param[in] flags
   */
  void fnClearFlag             (uint16_t flag);

  /**
   * Flips a flag
   *
   * \param[in] flags
   */
  void fnFlipFlag              (uint16_t flag);

  /**
   * Clear all global from 00 to 99 and the local flags
   *
   * \param[in] flags
   */
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
