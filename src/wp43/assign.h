// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file assign.h
 */
#if !defined(ASSIGN_H)
  #define ASSIGN_H

  #include <stdint.h>

  void fnAssign             (uint16_t mode);

  void updateAssignTamBuffer(void);

  void assignToMyMenu       (uint16_t position);
  void assignToMyAlpha      (uint16_t position);
  void assignToUserMenu     (uint16_t position);
  void assignToKey          (const char *data);

  void setUserKeyArgument   (uint16_t position, const char *name);
  void createMenu           (const char *name);

  void assignEnterAlpha     (void);
  void assignLeaveAlpha     (void);
  void assignGetName1       (void);
  void assignGetName2       (void);

#endif // !ASSIGN_H
