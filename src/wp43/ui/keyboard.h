// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/keyboard.h
 */
#if !defined(KEYBOARD_H)
  #define KEYBOARD_H

  #include <stdint.h>

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

  #if defined(PC_BUILD)
    extern char key[3];
  #endif // PC_BUILD

  void execAutoRepeat(uint16_t key);

  #if !defined(TESTSUITE_BUILD)
    /**
     * A calc button was pressed.
     *
     * \param data pointer to a string containing the key number pressed: 00=1/x, ..., 36=EXIT
     */
    void btnPressed   (char *data);

    /**
     * A calc button was released.
     *
     * \param string containing the key number pressed: 00=1/x, ..., 36=EXIT
     */
    void btnReleased   (char *data);

    /**
     * A calc button was clicked.
     *
     * \param data String containing the key ID
     */
    void btnClicked   (char *data);

    /**
     * A calc function key was pressed.
     *
     * \param data pointer to a string containing the key number pressed: 00=1/x, ..., 36=EXIT
     */
    void btnFnPressed (char *data);

    /**
     * A calc function key was released.
     *
     * \param string containing the key number pressed: 00=1/x, ..., 36=EXIT
     */
    void btnFnReleased(char *data);

    /**
     * A calc function key was clicked.
     *
     * \param data String containing the key ID
     */
    void btnFnClicked (char *data);
  #endif // !TESTSUITE_BUILD

  void setLastKeyCode  (int key);

#endif // !KEYBOARD_H
