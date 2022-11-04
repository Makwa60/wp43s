// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "constants.h"

#include "constantPointers.h"
#include "registers.h"
#include "registerValueConversions.h"
#include "stack.h"

#include "wp43.h"

void fnConstant(const uint16_t constant) {
  liftStack();
  currentSolverStatus &= ~SOLVER_STATUS_READY_TO_EXECUTE;

  if(constant < NUMBER_OF_CONSTANTS_39) { // 39 digit constants
    realToReal34((real_t *)(constants + constant * TO_BYTES(TO_BLOCKS(REAL39_SIZE_IN_BYTES))), REGISTER_REAL34_DATA(REGISTER_X));
  }
  else if(constant < NUMBER_OF_CONSTANTS_39 + NUMBER_OF_CONSTANTS_51) { // 51 digit constants (gamma coefficients)
    realToReal34((real_t *)(constants + NUMBER_OF_CONSTANTS_39 * TO_BYTES(TO_BLOCKS(REAL39_SIZE_IN_BYTES))
                                      + (constant - NUMBER_OF_CONSTANTS_39) * TO_BYTES(TO_BLOCKS(REAL51_SIZE_IN_BYTES))), REGISTER_REAL34_DATA(REGISTER_X));
  }
  else if(constant < NUMBER_OF_CONSTANTS_39 + NUMBER_OF_CONSTANTS_51 + NUMBER_OF_CONSTANTS_1071) { // 1071 digit constant
    realToReal34((real_t *)(constants + NUMBER_OF_CONSTANTS_39 * TO_BYTES(TO_BLOCKS(REAL39_SIZE_IN_BYTES)) + NUMBER_OF_CONSTANTS_51 * TO_BYTES(TO_BLOCKS(REAL51_SIZE_IN_BYTES))
                                      + (constant - NUMBER_OF_CONSTANTS_39 - NUMBER_OF_CONSTANTS_51) * TO_BYTES(TO_BLOCKS(REAL1071_SIZE_IN_BYTES))), REGISTER_REAL34_DATA(REGISTER_X));
  }
  else { // 34 digit constants
    real34Copy((real_t *)(constants + NUMBER_OF_CONSTANTS_39 * TO_BYTES(TO_BLOCKS(REAL39_SIZE_IN_BYTES)) + NUMBER_OF_CONSTANTS_51 * TO_BYTES(TO_BLOCKS(REAL51_SIZE_IN_BYTES)) + NUMBER_OF_CONSTANTS_1071 * TO_BYTES(TO_BLOCKS(REAL1071_SIZE_IN_BYTES))
                                    + (constant - NUMBER_OF_CONSTANTS_39 - NUMBER_OF_CONSTANTS_51 - NUMBER_OF_CONSTANTS_1071) * TO_BYTES(TO_BLOCKS(REAL34_SIZE_IN_BYTES))), REGISTER_REAL34_DATA(REGISTER_X));
  }

  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}



void fnPi(uint16_t unusedButMandatoryParameter) {
  liftStack();
  currentSolverStatus &= ~SOLVER_STATUS_READY_TO_EXECUTE;

  convertRealToReal34ResultRegister(const_pi, REGISTER_X);
  adjustResult(REGISTER_X, false, false, REGISTER_X, -1, -1);
}
