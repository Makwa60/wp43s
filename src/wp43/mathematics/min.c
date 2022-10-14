// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/min.h"

#include "mathematics/compare.h"
#include "registers.h"

#include "wp43.h"

void fnMin(uint16_t unusedButMandatoryParameter) {
  if(!saveLastX()) {
    return;
  }

  registerMin(REGISTER_X, REGISTER_Y, REGISTER_X);

  adjustResult(REGISTER_X, true, false, REGISTER_X, -1, -1);
}
