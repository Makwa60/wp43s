// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/toRect.h"

#include "apps/bugScreen.h"
#include "constantPointers.h"
#include "conversionAngles.h"
#include "debug.h"
#include "error.h"
#include "mathematics/wp34s.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"

void fnToRect(uint16_t unusedButMandatoryParameter) {
  uint32_t dataTypeX, dataTypeY;
  real_t x, y;

  dataTypeX = getRegisterDataType(REGISTER_X);
  dataTypeY = getRegisterDataType(REGISTER_Y);

  if((dataTypeX == dtReal34 || dataTypeX == dtLongInteger) && (dataTypeY == dtReal34 || dataTypeY == dtLongInteger)) {
    angularMode_t yAngularMode = getRegisterAngularMode(REGISTER_Y);

    if(!saveLastX()) {
    return;
  }

    switch(dataTypeX) {
      case dtLongInteger: {
        convertLongIntegerRegisterToReal(REGISTER_X, &x, &ctxtReal39);
        break;
      }
      case dtReal34: {
        real34ToReal(REGISTER_REAL34_DATA(REGISTER_X), &x);
        break;
      }
      default: {
        sprintf(errorMessage, "In function fnToRect: %" PRIu32 " is an unexpected dataTypeX value!", dataTypeX);
        bugScreen(errorMessage);
      }
    }

    if(yAngularMode == amNone) {
      yAngularMode = currentAngularMode;
    }

    switch(dataTypeY) {
      case dtLongInteger: {
        convertLongIntegerRegisterToReal(REGISTER_Y, &y, &ctxtReal39);
        convertAngleFromTo(&y, currentAngularMode, amRadian, &ctxtReal39);
        break;
      }

      case dtReal34: {
        real34ToReal(REGISTER_REAL34_DATA(REGISTER_Y), &y);
        convertAngleFromTo(&y, yAngularMode, amRadian, &ctxtReal39);
        break;
      }

      default: {
        sprintf(errorMessage, "In function fnToRect: %" PRIu32 " is an unexpected dataTypeY value!", dataTypeY);
        bugScreen(errorMessage);
      }
    }

    realPolarToRectangular(&x, &y, &x, &y, &ctxtReal39);

    reallocateRegister(REGISTER_X, dtReal34, REAL34_SIZE, amNone);
    reallocateRegister(REGISTER_Y, dtReal34, REAL34_SIZE, amNone);
    convertRealToReal34ResultRegister(&x, REGISTER_X);
    convertRealToReal34ResultRegister(&y, REGISTER_Y);

    temporaryInformation = TI_X_Y;
  }
  else {
    displayCalcErrorMessage(ERROR_INVALID_DATA_TYPE_FOR_OP, ERR_REGISTER_LINE, REGISTER_X);
    #if (EXTRA_INFO_ON_CALC_ERROR == 1)
      sprintf(errorMessage, "cannot convert (%s, %s) to rectangular coordinates!", getDataTypeName(getRegisterDataType(REGISTER_X), false, false), getDataTypeName(getRegisterDataType(REGISTER_Y), false, false));
      moreInfoOnError("In function fnToRect:", errorMessage, NULL, NULL);
    #endif // (EXTRA_INFO_ON_CALC_ERROR == 1)
  }
}



/* never used
void real34PolarToRectangular(const real34_t *magnitude34, const real34_t *theta34, real34_t *real34, real34_t *imag34) {
  real_t real, imag, magnitude, theta;

  real34ToReal(magnitude34, &magnitude);
  real34ToReal(theta34, &theta);

  realPolarToRectangular(&magnitude, &theta, &real, &imag, &ctxtReal39);  // theta in radian

  realToReal34(&real, real34);
  realToReal34(&imag, imag34);
}
*/



void realPolarToRectangular(const real_t *mag, const real_t *the, real_t *real, real_t *imag, realContext_t *realContext) {
  ///////////////////////////////////////////
  //
  //  +----+----+-------------+------+------+
  //  | ρ  | θ  | Condition   | Re   | Im   |
  //  +----+----+-------------+------+------+
  //  |NaN |any |             |NaN   |NaN   |  1
  //  |any |NaN |             |NaN   |NaN   |  2
  //  |-∞  |-∞  |             |NaN   |NaN   |  3
  //  |-∞  |-π  |             |+∞    |0     |  4
  //  |-∞  |-π/2|             |0     |+∞    |  5
  //  |-∞  |0   |             |-∞    |0     |  6
  //  |-∞  |π/2 |             |0     |-∞    |  7
  //  |-∞  |π   |             |+∞    |0     |  8
  //  |-∞  |θ   |0 < θ < π/2  |-∞    |-∞    |  9
  //  |-∞  |θ   |π/2 < θ < π  |+∞    |-∞    | 10
  //  |-∞  |θ   |-π < θ < -π/2|+∞    |+∞    | 11
  //  |-∞  |θ   |-π/2 < θ < 0 |-∞    |+∞    | 12
  //  |-∞  |+∞  |             |NaN   |NaN   | 13
  //  |0   |-∞  |             |NaN   |NaN   | 14
  //  |0   |θ   |             |0     |0     | 15
  //  |0   |+∞  |             |NaN   |NaN   | 16
  //  |r   |-∞  |             |NaN   |NaN   | 17
  //  |r   |θ   |             |r·cosθ|r·sinθ| 18
  //  |r   |+∞  |             |NaN   |NaN   | 19
  //  |+∞  |-∞  |             |NaN   |NaN   | 20
  //  |+∞  |-π  |             |-∞    |0     | 21
  //  |+∞  |-π/2|             |0     |-∞    | 22
  //  |+∞  |0   |             |+∞    |0     | 23
  //  |+∞  |π/2 |             |0     |+∞    | 24
  //  |+∞  |π   |             |-∞    |0     | 25
  //  |+∞  |θ   |0 < θ < π/2  |+∞    |+∞    | 26
  //  |+∞  |θ   |π/2 < θ < π  |-∞    |+∞    | 27
  //  |+∞  |θ   |-π < θ < -π/2|-∞    |-∞    | 28
  //  |+∞  |θ   |-π/2 < θ < 0 |+∞    |-∞    | 29
  //  |+∞  |+∞  |             |NaN   |NaN   | 30

  real_t sin, cos, magnitude, theta;

  // Testing NaNs and infinities
  if(realIsNaN(mag) || realIsNaN(the) || realIsInfinite(the)) {
    //  +----+----+-------------+------+------+
    //  | ρ  | θ  | Condition   | Re   | Im   |
    //  +----+----+-------------+------+------+
    //  |NaN |any |             |NaN   |NaN   |  1
    //  |any |NaN |             |NaN   |NaN   |  2
    //  |-∞  |-∞  |             |NaN   |NaN   |  3
    //  |-∞  |+∞  |             |NaN   |NaN   | 13
    //  |+∞  |-∞  |             |NaN   |NaN   | 20
    //  |+∞  |+∞  |             |NaN   |NaN   | 30
    //  |0   |-∞  |             |NaN   |NaN   | 14
    //  |0   |+∞  |             |NaN   |NaN   | 16
    //  |r   |-∞  |             |NaN   |NaN   | 17
    //  |r   |+∞  |             |NaN   |NaN   | 19
    realCopy(const_NaN, real);
    realCopy(const_NaN, imag);
    return;
  }

  realCopy(mag, &magnitude);
  WP34S_Mod(the, const1071_2pi, &theta, &ctxtReal75);  // here   0 ≤ theta < 2pi

  // Magnitude is infinite
  if(realIsInfinite(&magnitude)) {
    if(realIsZero(&theta)) { // theta = 0
      //  +----+----+-------------+------+------+
      //  | ρ  | θ  | Condition   | Re   | Im   |
      //  +----+----+-------------+------+------+
      //  |+∞  |0   |             |+∞    |0     | 23
      realCopy(const_plusInfinity, real);
      realZero(imag);
    }
    else {
      realSubtract(&theta, const_piOn2, real, &ctxtReal39);
      if(realIsZero(real)) { // theta = pi/2
        //  +----+----+-------------+------+------+
        //  | ρ  | θ  | Condition   | Re   | Im   |
        //  +----+----+-------------+------+------+
        //  |+∞  |π/2 |             |0     |+∞    | 24
        realZero(real);
        realCopy(const_plusInfinity, imag);
      }
      else {
        realSubtract(&theta, const_3piOn2, real, &ctxtReal39);
        if(realIsZero(real)) { // theta = -pi/2
          //  +----+----+-------------+------+------+
          //  | ρ  | θ  | Condition   | Re   | Im   |
          //  +----+----+-------------+------+------+
          //  |+∞  |-π/2|             |0     |-∞    | 22
          realZero(real);
          realCopy(const_minusInfinity, imag);
        }
        else {
          realSubtract(&theta, const_pi, real, &ctxtReal39);
          if(realIsZero(real)) { // theta = pi
            //  +----+----+-------------+------+------+
            //  | ρ  | θ  | Condition   | Re   | Im   |
            //  +----+----+-------------+------+------+
            //  |+∞  |-π  |             |-∞    |0     | 21
            //  |+∞  |π   |             |-∞    |0     | 25
            realCopy(const_minusInfinity, real);
            realZero(imag);
          }
          else {
            realSubtract(&theta, const_piOn2, &theta, &ctxtReal39);
            if(realIsNegative(&theta)) { //  0 < theta < pi/2
              //  +----+----+-------------+------+------+
              //  | ρ  | θ  | Condition   | Re   | Im   |
              //  +----+----+-------------+------+------+
              //  |+∞  |θ   |0 < θ < π/2  |+∞    |+∞    | 26
              realCopy(const_plusInfinity, real);
              realCopy(const_plusInfinity, imag);
            }
            else {
              realSubtract(&theta, const_piOn2, &theta, &ctxtReal39);
              if(realIsNegative(&theta)) { //  pi/2 < theta < pi
                //  +----+----+-------------+------+------+
                //  | ρ  | θ  | Condition   | Re   | Im   |
                //  +----+----+-------------+------+------+
                //  |+∞  |θ   |π/2 < θ < π  |-∞    |+∞    | 27
                realCopy(const_minusInfinity, real);
                realCopy(const_plusInfinity, imag);
              }
              else {
                realSubtract(&theta, const_piOn2, &theta, &ctxtReal39);
                if(realIsNegative(&theta)) { //  pi < theta < 3pi/2
                  //  +----+----+-------------+------+------+
                  //  | ρ  | θ  | Condition   | Re   | Im   |
                  //  +----+----+-------------+------+------+
                  //  |+∞  |θ   |-π < θ < -π/2|-∞    |-∞    | 28
                  realCopy(const_minusInfinity, real);
                  realCopy(const_minusInfinity, imag);
                }
                else { //  3pi/2 < theta < 2pi
                  //  +----+----+-------------+------+------+
                  //  | ρ  | θ  | Condition   | Re   | Im   |
                  //  +----+----+-------------+------+------+
                  //  |+∞  |θ   |-π/2 < θ < 0 |+∞    |-∞    | 29
                  realCopy(const_plusInfinity, real);
                  realCopy(const_minusInfinity, imag);
                }
              }
            }
          }
        }
      }
    }

    if(realIsNegative(&magnitude)) {
      //  +----+----+-------------+------+------+
      //  | ρ  | θ  | Condition   | Re   | Im   |
      //  +----+----+-------------+------+------+
      //  |-∞  |-π  |             |+∞    |0     |  4
      //  |-∞  |-π/2|             |0     |+∞    |  5
      //  |-∞  |0   |             |-∞    |0     |  6
      //  |-∞  |π/2 |             |0     |-∞    |  7
      //  |-∞  |π   |             |+∞    |0     |  8
      //  |-∞  |θ   |0 < θ < π/2  |-∞    |-∞    |  9
      //  |-∞  |θ   |π/2 < θ < π  |+∞    |-∞    | 10
      //  |-∞  |θ   |-π < θ < -π/2|+∞    |+∞    | 11
      //  |-∞  |θ   |-π/2 < θ < 0 |-∞    |+∞    | 12
      if(!realIsNaN(real) && !realIsZero(real)) {
        realChangeSign(real);
      }
      if(!realIsNaN(imag) && !realIsZero(imag)) {
        realChangeSign(imag);
      }
    }

    return;
  }

    //  +----+----+-------------+------+------+
    //  | ρ  | θ  | Condition   | Re   | Im   |
    //  +----+----+-------------+------+------+
    //  |0   |θ   |             |0     |0     | 15
    if(realIsZero(&magnitude)) {
      realZero(real);
      realZero(imag);
      return;
    }

    //  +----+----+-------------+------+------+
    //  | ρ  | θ  | Condition   | Re   | Im   |
    //  +----+----+-------------+------+------+
    //  |r   |θ   |             |r·cosθ|r·sinθ| 18

  WP34S_Cvt2RadSinCosTan(&theta, amRadian, &sin, &cos, NULL, realContext);
  realMultiply(&magnitude, &cos, real, realContext);
  realMultiply(&magnitude, &sin, imag, realContext);
}
