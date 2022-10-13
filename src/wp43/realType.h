// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file realType.h
 */
#if !defined(REALTYPE_H)
  #define REALTYPE_H

  #include "defines.h"

  #include "decimal128.h"
  #include "decimal64.h"
  #include "decDouble.h"
  #include "decQuad.h"
  extern int decGetInt(const decNumber *x); // Because decNumberToInt32 seems buggy! Needs more investigation

  typedef struct {
    int32_t digits;      // Count of digits in the coefficient; >0
    int32_t exponent;    // Unadjusted exponent, unbiased, in
                         // range: -1999999997 through 999999999
    uint8_t bits;        // Indicator bits (see above)
                         // Coefficient, from least significant unit
    decNumberUnit lsu[(39+DECDPUN-1)/DECDPUN]; // 39 = 27 + 1*12
  } real39_t; // used for trigonometric functions borrowed from WP34S


  typedef struct {
    int32_t digits;      // Count of digits in the coefficient; >0
    int32_t exponent;    // Unadjusted exponent, unbiased, in
                         // range: -1999999997 through 999999999
    uint8_t bits;        // Indicator bits (see above)
                         // Coefficient, from least significant unit
    decNumberUnit lsu[(51+DECDPUN-1)/DECDPUN]; // 51 = 39 + 1*12
  } real51_t; // used for trigonometric functions borrowed from WP34S


  //typedef struct {
  //  int32_t digits;      // Count of digits in the coefficient; >0
  //  int32_t exponent;    // Unadjusted exponent, unbiased, in
  //                       // range: -1999999997 through 999999999
  //  uint8_t bits;        // Indicator bits (see above)
  //                       // Coefficient, from least significant unit
  //  decNumberUnit lsu[(75+DECDPUN-1)/DECDPUN]; // 75 = 39 + 3*12
  //} real75_t; // used in SLVQ

  typedef struct {
    int32_t digits;      // Count of digits in the coefficient; >0
    int32_t exponent;    // Unadjusted exponent, unbiased, in
                         // range: -1999999997 through 999999999
    uint8_t bits;        // Indicator bits (see above)
                         // Coefficient, from least significant unit
    decNumberUnit lsu[(1071+DECDPUN-1)/DECDPUN]; // 1071 = 39 + 86*12
  } real1071_t; // used for radian angle reduction


  //typedef struct {
  //  int32_t digits;      // Count of digits in the coefficient; >0
  //  int32_t exponent;    // Unadjusted exponent, unbiased, in
  //                       // range: -1999999997 through 999999999
  //  uint8_t bits;        // Indicator bits (see above)
  //                       // Coefficient, from least significant unit
  //  decNumberUnit lsu[(2139+DECDPUN-1)/DECDPUN]; // 2139 = 39 + 175*12
  //} real2139_t;

  typedef decContext realContext_t;
  typedef decQuad    real34_t; // 34 digits and 128 bits = 16 bytes

  typedef struct {
    real34_t real, imag;
  } complex34_t;

  typedef decNumber  real_t;

  typedef enum rounding realRoundingMode_t;

  static const size_t REAL_SIZE      = TO_BLOCKS(sizeof(real_t));
  static const size_t REAL34_SIZE    = TO_BLOCKS(sizeof(real34_t));
  static const size_t REAL39_SIZE    = TO_BLOCKS(sizeof(real39_t));
  static const size_t REAL51_SIZE    = TO_BLOCKS(sizeof(real51_t));
  static const size_t REAL1071_SIZE  = TO_BLOCKS(sizeof(real1071_t));
  static const size_t COMPLEX34_SIZE = TO_BLOCKS(sizeof(complex34_t));

  extern realContext_t          ctxtReal4;    //   Limited digits: used for high speed internal calcs
  extern realContext_t          ctxtReal34;   //   34 digits
  extern realContext_t          ctxtReal39;   //   39 digits: used for 34 digits intermediate calculations
  extern realContext_t          ctxtReal51;   //   51 digits: used for 34 digits intermediate calculations
  extern realContext_t          ctxtReal75;   //   75 digits: used in SLVQ
  extern realContext_t          ctxtReal1071; // 1071 digits: used in radian angle reduction
  //extern realContext_t          ctxtReal2139; // 2139 digits: used for really big modulo

  #define VARIABLE_REAL34_DATA(a)                                ((real34_t    *)(a))
  #define VARIABLE_IMAG34_DATA(a)                                ((real34_t    *)((dataBlock_t *)(a) + REAL34_SIZE))


  #define complex34ChangeSign(operand)                           {real34ChangeSign((real34_t *)(operand)); \
                                                                  real34ChangeSign((real34_t *)((dataBlock_t *)(operand) + REAL34_SIZE)); \
                                                                 }
  #define complex34Copy(source, destination)                     {  *(uint64_t *)(destination)     =   *(uint64_t *)(source); \
                                                                  *(((uint64_t *)(destination))+1) = *(((uint64_t *)(source))+1); \
                                                                  *(((uint64_t *)(destination))+2) = *(((uint64_t *)(source))+2); \
                                                                  *(((uint64_t *)(destination))+3) = *(((uint64_t *)(source))+3); \
                                                                 }
  #define int32ToReal34(source, destination)                     decQuadFromInt32         ((real34_t *)(destination), source)
  #define real34Add(operand1, operand2, res)                     decQuadAdd               ((real34_t *)(res), (real34_t *)(operand1), (real34_t *)(operand2), &ctxtReal34)
  #define real34ChangeSign(operand)                              ((real34_t *)(operand))->bytes[15] ^= 0x80
  #define real34Compare(operand1, operand2, res)                 decQuadCompare           ((real34_t *)(res), (real34_t *)(operand1), (real34_t *)(operand2), &ctxtReal34)
  //#define real34Copy(source, destination)                        decQuadCopy            (destination, source)
  //#define real34Copy(source, destination)                        xcopy(destination, source, TO_BYTES(REAL34_SIZE))
  #define real34Copy(source, destination)                        {*(uint64_t *)(destination) = *(uint64_t *)(source); \
                                                                  *(((uint64_t *)(destination))+1) = *(((uint64_t *)(source))+1); \
                                                                 }
  #define real34CopyAbs(source, destination)                     decQuadCopyAbs           (destination, source)
  #define real34Divide(operand1, operand2, res)                  decQuadDivide            ((real34_t *)(res), (real34_t *)(operand1), (real34_t *)(operand2), &ctxtReal34)
  #define real34DivideRemainder(operand1, operand2, res)         decQuadRemainder         ((real34_t *)(res), (real34_t *)(operand1), (real34_t *)(operand2), &ctxtReal34)
  #define real34FMA(factor1, factor2, term, res)                 decQuadFMA               ((real34_t *)(res), (real34_t *)(factor1),  (real34_t *)(factor2),  (real34_t *)(term), &ctxtReal34)
  #define real34GetCoefficient(source, destination)              decQuadGetCoefficient    ((real34_t *)(source), (uint8_t *)(destination))
  #define real34GetExponent(source)                              decQuadGetExponent       ((real34_t *)(source))
  #define real34IsInfinite(source)                               decQuadIsInfinite        ((real34_t *)(source))
  #define real34IsNaN(source)                                    decQuadIsNaN             ((real34_t *)(source))
  #define real34IsNegative(source)                               (((((real34_t *)(source))->bytes[15]) & 0x80) == 0x80)
  #define real34IsPositive(source)                               (((((real34_t *)(source))->bytes[15]) & 0x80) == 0x00)
  #define real34IsSpecial(source)                                (decQuadIsNaN((real34_t *)(source))   || decQuadIsSignaling((real34_t *)(source))   || decQuadIsInfinite((real34_t *)(source)))
  #define real34IsZero(source)                                   decQuadIsZero            ((real34_t *)(source))
  #define real34Minus(operand, res)                              decQuadMinus             ((real34_t *)(res), (real34_t *)(operand), &ctxtReal34)
  #define real34Multiply(operand1, operand2, res)                decQuadMultiply          ((real34_t *)(res), (real34_t *)(operand1), (real34_t *)(operand2), &ctxtReal34)
  #define real34NextMinus(operand, res)                          decQuadNextPlus          ((real34_t *)(res), (real34_t *)(operand), &ctxtReal34)
  #define real34NextPlus(operand, res)                           decQuadNextPlus          ((real34_t *)(res), (real34_t *)(operand), &ctxtReal34)
  #define real34Plus(operand, res)                               decQuadPlus              ((real34_t *)(res), (real34_t *)(operand), &ctxtReal34)
  #define real34SetNegativeSign(operand)                         ((real34_t *)(operand))->bytes[15] |= 0x80
  #define real34SetPositiveSign(operand)                         ((real34_t *)(operand))->bytes[15] &= 0x7F
  #define real34Subtract(operand1, operand2, res)                decQuadSubtract          ((real34_t *)(res), (real34_t *)(operand1), (real34_t *)(operand2), &ctxtReal34)
  #define real34ToInt32(source)                                  decQuadToInt32           ((real34_t *)(source), &ctxtReal34, DEC_ROUND_DOWN)
  static inline void real34ToIntegralValue(const real34_t *source, real34_t *destination, realRoundingMode_t mode) { decQuadToIntegralValue(destination, source, &ctxtReal34, mode); }
  #define real34ToReal(source, destination)                      decQuadToNumber          ((real34_t *)(source), destination)
  #define real34ToString(source, destination)                    decQuadToString          ((real34_t *)(source), destination)
  #define real34ToUInt32(source)                                 decQuadToUInt32          ((real34_t *)(source), &ctxtReal34, DEC_ROUND_DOWN)
  #define real34Reduce(source, destination)                      decQuadReduce            (destination, source, &ctxtReal34)
  #define real34Zero(destination)                                decQuadZero              (destination)
  //#define real34Zero(destination)                                xcopy                    (destination, const34_0, TO_BYTES(REAL34_SIZE))
  /*#define real34Zero(destination)                                {  *(uint64_t *)(destination)     =   *(uint64_t *)const34_0; \
                                                                    *(((uint64_t *)(destination))+1) = *(((uint64_t *)const34_0)+1); \
                                                                 }*/
  #define stringToReal34(source, destination)                    decQuadFromString        ((real34_t *)(destination), source, &ctxtReal34)
  #define uInt32ToReal34(source, destination)                    decQuadFromUInt32        ((real34_t *)(destination), source)








  #define int32ToReal(source, destination)                       decNumberFromInt32       (destination, source)
  #define realAdd(operand1, operand2, res, ctxt)                 decNumberAdd             (res, operand1, operand2, ctxt)
  #define realChangeSign(operand)                                (operand)->bits ^= 0x80
  #define realCompare(operand1, operand2, res, ctxt)             decNumberCompare         (res, operand1, operand2, ctxt)
  #define realCopy(source, destination)                          decNumberCopy            (destination, source)
  #define realCopyAbs(source, destination)                       decNumberCopyAbs         (destination, source)
  #define realDivide(operand1, operand2, res, ctxt)              decNumberDivide          (res, operand1, operand2, ctxt)
  #define realDivideRemainder(operand1, operand2, res, ctxt)     decNumberRemainder       (res, operand1, operand2, ctxt)
  #define realExp(operand, res, ctxt)                            decNumberExp             (res, operand, ctxt)
  #define realFMA(factor1, factor2, term, res, ctxt)             decNumberFMA             (res, factor1,  factor2,  term, ctxt)
  #define realGetCoefficient(source, destination)                decNumberGetBCD          (source, (uint8_t *)(destination))
  //#define realGetExponent(source)                                ((source)->digits)
  #define realGetExponent(source)                                ((source)->digits + (source)->exponent - 1)
  #define realGetSign(source)                                    (((source)->bits) & 0x80) // 0x80=negative and 0x00=positive
  #define realIsInfinite(source)                                 decNumberIsInfinite      (source)
  #define realIsNaN(source)                                      decNumberIsNaN           (source)
  #define realIsNegative(source)                                 ((((source)->bits) & 0x80) == 0x80)
  #define realIsPositive(source)                                 ((((source)->bits) & 0x80) == 0x00)
  #define realIsSpecial(source)                                  decNumberIsSpecial       (source)
  #define realIsZero(source)                                     decNumberIsZero          (source)
  #define realMinus(operand, res, ctxt)                          decNumberMinus           (res, operand, ctxt)
  #define realMultiply(operand1, operand2, res, ctxt)            decNumberMultiply        (res, operand1, operand2, ctxt)
  #define realNextToward(from, toward, res, ctxt)                decNumberNextToward      (res, from,     toward,   ctxt)
  #define realPlus(operand, res, ctxt)                           decNumberPlus            (res, operand, ctxt)
  #define realPower(operand1, operand2, res, ctxt)               decNumberPower           (res, operand1, operand2, ctxt)
  #define realSetNegativeSign(operand)                           (operand)->bits |= 0x80
  #define realSetPositiveSign(operand)                           (operand)->bits &= 0x7F
  #define realSquareRoot(operand, res, ctxt)                     decNumberSquareRoot      (res, operand, ctxt)
  #define realSubtract(operand1, operand2, res, ctxt)            decNumberSubtract        (res, operand1, operand2, ctxt)
  static inline int32_t realToInt32(const real_t *source) {
    realRoundingMode_t savedRoundingMode;
    real_t             tmp;
    int32_t            result;

    savedRoundingMode = ctxtReal39.round;
    ctxtReal39.round  = DEC_ROUND_DOWN;
    ctxtReal39.status = 0;
    decNumberToIntegralValue(&tmp, source, &ctxtReal39);
    result = decGetInt(&tmp);
    ctxtReal39.round = savedRoundingMode;
    return result;
  }
  static inline void realToReal34(const real_t *source, real34_t *destination) { decQuadFromNumber(destination, source, &ctxtReal34); }
  static inline void realToString(const real_t *source, char     *destination) { decNumberToString(source, destination); }
  static inline void realZero    (real_t *destination)                         { decNumberZero(destination); }
  #define stringToReal(source, destination, ctxt)                decNumberFromString      (destination, source, ctxt)
  #define uInt32ToReal(source, destination)                      decNumberFromUInt32      (destination, source)
#endif // !REALTYPE_H
