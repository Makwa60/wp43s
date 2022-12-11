// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "mathematics/slvc.h"

#include "constantPointers.h"
#include "mathematics/comparisonReals.h"
#include "mathematics/cubeRoot.h"
#include "mathematics/multiplication.h"
#include "mathematics/squareRoot.h"
#include "typeDefinitions.h"

static bool _checkConditionNumberOfAddSub(const real_t *operand1, const real_t *operand2, const real_t *res, realContext_t *realContext) {
  real_t conditionNumber1, conditionNumber2;
  real_t *conditionNumber = &conditionNumber1;

  if(realIsZero(res)) {
    return false;
  }
  else {
    realDivide(res, operand1, &conditionNumber1, realContext); realSetPositiveSign(&conditionNumber1);
    realDivide(res, operand2, &conditionNumber2, realContext); realSetPositiveSign(&conditionNumber2);
    if(realIsZero(operand1)) {
      conditionNumber = &conditionNumber2;
    }
    else if(realIsZero(operand2)) {
      conditionNumber = &conditionNumber1;
    }
    else if(realCompareGreaterThan(&conditionNumber1, &conditionNumber2)) {
      conditionNumber = &conditionNumber2;
    }
    else {
      conditionNumber = &conditionNumber1;
    }
    return realCompareLessThan(conditionNumber, const_1e_37);
  }
}
static void _realCheckedAdd(const real_t *operand1, const real_t *operand2, real_t *res, realContext_t *realContext) {
  real_t r;
  realAdd(operand1, operand2, &r, realContext);
  if(_checkConditionNumberOfAddSub(operand1, operand2, &r, realContext)) {
    realZero(res);
  }
  else {
    realCopy(&r, res);
  }
}
static void _realCheckedSubtract(const real_t *operand1, const real_t *operand2, real_t *res, realContext_t *realContext) {
  real_t r;
  realSubtract(operand1, operand2, &r, realContext);
  if(_checkConditionNumberOfAddSub(operand1, operand2, &r, realContext)) {
    realZero(res);
  }
  else {
    realCopy(&r, res);
  }
}

void solveCubicEquation(const real_t *c2Real, const real_t *c2Imag, const real_t *c1Real, const real_t *c1Imag, const real_t *c0Real, const real_t *c0Imag, real_t *rReal, real_t *rImag, real_t *x1Real, real_t *x1Imag, real_t *x2Real, real_t *x2Imag, real_t *x3Real, real_t *x3Imag, realContext_t *realContext) {
  // x^3 + b x^2 + c x + d = 0
  // Abramowitz & Stegun §3.8.2
  real_t qr, qi, rr, ri, s1r, s1i, s2r, s2i, ar, ai;

  // q = (c - b^2 / 3) / 3
  mulComplexComplex(c2Real, c2Imag, c2Real, c2Imag, &qr, &qi, realContext);
  realDivide(&qr, const_3, &qr, realContext), realDivide(&qi, const_3, &qi, realContext);
  realSubtract(c1Real, &qr, &qr, realContext), realSubtract(c1Imag, &qi, &qi, realContext);
  realDivide(&qr, const_3, &qr, realContext), realDivide(&qi, const_3, &qi, realContext);

  // r = (b c - 3 d) / 6 - b^3 / 27
  mulComplexComplex(c2Real, c2Imag, c1Real, c1Imag, &rr, &ri, realContext);
  realMultiply(c0Real, const_3, &ar, realContext), realMultiply(c0Imag, const_3, &ai, realContext);
  realSubtract(&rr, &ar, &rr, realContext), realSubtract(&ri, &ai, &ri, realContext);
  realDivide(&rr, const_6, &rr, realContext), realDivide(&ri, const_6, &ri, realContext);
  mulComplexComplex(c2Real, c2Imag, c2Real, c2Imag, &ar, &ai, realContext);
  mulComplexComplex(&ar, &ai, c2Real, c2Imag, &ar, &ai, realContext);
  realDivide(&ar, const_27, &ar, realContext), realDivide(&ai, const_27, &ai, realContext);
  realSubtract(&rr, &ar, &rr, realContext), realSubtract(&ri, &ai, &ri, realContext);

  // q^3 + r^2
  mulComplexComplex(&qr, &qi, &qr, &qi, rReal, rImag, realContext);
  mulComplexComplex(rReal, rImag, &qr, &qi, rReal, rImag, realContext);
  mulComplexComplex(&rr, &ri, &rr, &ri, &ar, &ai, realContext);
  realAdd(rReal, &ar, rReal, realContext), realAdd(rImag, &ai, rImag, realContext);

  // s1, s2 = cbrt(r ± sqrt(q^3 + r^2))
  sqrtComplex(rReal, rImag, &s1r, &s1i, realContext);
  realSubtract(&rr, &s1r, &s2r, realContext), realSubtract(&ri, &s1i, &s2i, realContext);
  realAdd(&rr, &s1r, &s1r, realContext), realAdd(&ri, &s1i, &s1i, realContext);
  curtComplex(&s1r, &s1i, &s1r, &s1i, realContext);
  curtComplex(&s2r, &s2i, &s2r, &s2i, realContext);

  // reusing q, r for (s1 ± s2)
  realAdd(&s1r, &s2r, &qr, realContext), realAdd(&s1i, &s2i, &qi, realContext);
  realSubtract(&s1r, &s2r, &rr, realContext), realSubtract(&s1i, &s2i, &ri, realContext);
  mulComplexComplex(&rr, &ri, const_0, const_root3on2, &rr, &ri, realContext);

  // roots
  realDivide(c2Real, const_3, x2Real, realContext), realDivide(c2Imag, const_3, x2Imag, realContext);
  realSubtract(&qr, x2Real, x1Real, realContext), realSubtract(&qi, x2Imag, x1Imag, realContext);
  realMultiply(&qr, const_1on2, x3Real, realContext), realMultiply(&qi, const_1on2, x3Imag, realContext);
  realAdd(x3Real, x2Real, x3Real, realContext), realAdd(x3Imag, x2Imag, x3Imag, realContext);
  realChangeSign(x3Real); realChangeSign(x3Imag);
  _realCheckedAdd(x3Real, &rr, x2Real, realContext), _realCheckedAdd(x3Imag, &ri, x2Imag, realContext);
  _realCheckedSubtract(x3Real, &rr, x3Real, realContext), _realCheckedSubtract(x3Imag, &ri, x3Imag, realContext);

  // discriminant
  realMultiply(rReal, const__108, rReal, realContext);
  realMultiply(rImag, const__108, rImag, realContext);
}
