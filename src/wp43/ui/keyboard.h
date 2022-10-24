// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/keyboard.h
 */
#if !defined(KEYBOARD_H)
  #define KEYBOARD_H

  #include <stdint.h>

  typedef enum {
    kcNoKey     =  0,
    kcInv       =  1,
    kcExp       =  2,
    kcTri       =  3,
    kcLn        =  4,
    kcEToX      =  5,
    kcSqrt      =  6,
    kcSto       =  7,
    kcRcl       =  8,
    kcRdown     =  9,
    kcCC        = 10,
    kcShiftF    = 11,
    kcShiftG    = 12,
    kcEnter     = 13,
    kcSwap      = 14,
    kcChs       = 15,
    kcE         = 16,
    kcBackspace = 17,
    kcDiv       = 18,
    kc7         = 19,
    kc8         = 20,
    kc9         = 21,
    kcXeq       = 22,
    kcMul       = 23,
    kc4         = 24,
    kc5         = 25,
    kc6         = 26,
    kcUp        = 27,
    kcSub       = 28,
    kc1         = 29,
    kc2         = 30,
    kc3         = 31,
    kcDown      = 32,
    kcAdd       = 33,
    kc0         = 34,
    kcDot       = 35,
    kcRun       = 36,
    kcExit      = 37,
    kcF1        = 38,
    kcF2        = 39,
    kcF3        = 40,
    kcF4        = 41,
    kcF5        = 42,
    kcF6        = 43
  } keyCode_t;

  extern keyCode_t lastKeyCode;

  void leavePem        (void);
  void showShiftState  (void);
  void processKeyAction(int16_t item);

  /**
   * Processing ENTER key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyEnter      (uint16_t unusedButMandatoryParameter);

  /**
   * Processing EXIT key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyExit       (uint16_t unusedButMandatoryParameter);

  /**
   * Processing CC key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyCC         (uint16_t unusedButMandatoryParameter);

  /**
   * Processing BACKSPACE key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyBackspace  (uint16_t unusedButMandatoryParameter);

  /**
   * Processing UP key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyUp         (uint16_t unusedButMandatoryParameter);

  /**
   * Processing DOWN key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyDown       (uint16_t unusedButMandatoryParameter);

  /**
   * Processing .d key.
   *
   * \param[in] unusedButMandatoryParameter
   */
  void fnKeyDotD       (uint16_t unusedButMandatoryParameter);

  void execAutoRepeat  (uint16_t key);

  #if !defined(TESTSUITE_BUILD)
    /**
     * A calc button was pressed.
     *
     * \param data pointer to a string containing the key number pressed: 00=1/x, ..., 36=EXIT
     */
    void btnPressed   (keyCode_t keyCode);

    /**
     * A calc button was released.
     *
     * \param string containing the key number pressed: 00=1/x, ..., 36=EXIT
     */
    void btnReleased  (keyCode_t keyCode);

    /**
     * A calc button was clicked.
     *
     * \param data String containing the key ID
     */
    void btnClicked   (keyCode_t keyCode);
  #endif // !TESTSUITE_BUILD

  keyCode_t kbKeyCodeFromRowColumn(uint8_t rowColumn);

  uint8_t   kbRowColumnFromKeyCode(keyCode_t keyCode);

#endif // !KEYBOARD_H
