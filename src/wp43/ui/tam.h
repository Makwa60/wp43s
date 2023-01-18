// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file ui/tam.h
 */
#if !defined(TAM_H)
  #define TAM_H

  #include "defines.h"
  #include <stdbool.h>
  #include <stdint.h>

  // These values must not conflict with normal items, or anything else
  // that might be a parameter to an item
  typedef enum {
    tmValue     = 10001, // tmValue must be the 1st in this list
    tmValueChb  = 10002, // same as tmValue but for ->INT (#) change base
    tmRegister  = 10003,
    tmFlagR     = 10004,
    tmFlagW     = 10005,
    tmStoRcl    = 10006,
    tmMDim      = 10007,
    tmShuffle   = 10008,
    tmLabel     = 10009,
    tmSolve     = 10010,
    tmNewMenu   = 10011,
    tmKey       = 10012,
    tmIntegrate = 10013,
    tmDelItem   = 10014,
    tmCmp       = 10015  // tmCmp must be the last in this list
  } tamMode_t;

  /**
   * \struct tamState_t
   * State for TAM mode. Most of this state is internal and so not documented.
   */
  typedef struct {
    /**
     * The mode used by TAM processing. If non-zero then TAM mode is active.
     * This should not be used directly, but instead use ::tamIsActive to
     * determine whether in TAM mode.
     *
     *     if(tamIsActive()) {
     *       // the calculator is in TAM mode
     *     }
     */
    tamMode_t  mode;
    int16_t    function;
    /**
     * Whether input is a string rather than a number. For example, a named variable.
     * If the calculator is in alpha mode then additional details apply. See
     * ::tamProcessInput for further details.
     */
    bool       alpha;
    int16_t    currentOperation;
    bool       dot;
    bool       indirect;
    int16_t    digitsSoFar;
    int16_t    value;
    int16_t    min;
    int16_t    max;
    /**
     * Only used for KEYG and KEYX
     */
    int16_t    key;
    bool       keyAlpha;
    bool       keyDot;
    bool       keyIndirect;
    bool       keyInputFinished;
  } tamState_t;

  extern tamState_t tam;

  /**
   * Reset the TAM state.
   */
  void tamReset        (void);

  /**
   * Return whether the calculator is currently waiting for TAM input.
   *
   * \return true if currently in TAM
   */
  static inline bool tamIsActive(void) {
    return tam.mode;
  }

  #if !defined(TESTSUITE_BUILD)
    /**
     * Return whether TAM is waiting for a key assignment.
     *
     * \return true if waiting for key assignment
     */
    static inline bool tamIsWaitingKey(void) {
      return tam.mode == tmKey && !tam.keyInputFinished;
    }

    /**
     * Enters TAM mode.
     * This initialises TAM entry for the given command and sets the `tamBuffer`
     * to the appropriate text. In GUI mode the keyboard is set-up. Once this
     * function has been called TAM mode can be left by input processed by
     * ::tamProcessInput or by calling ::tamLeaveMode. If TAM is left the command
     * that triggered TAM is implicitly cancelled (no further action is needed).
     * For the command to be executed the input must be processed by
     * ::tamProcessInput.
     *
     * This function should be called instead of the command that requires TAM
     * input. The command that requires TAM should be passed as a parameter.
     *
     * \param[in] func the `indexOfItems` index for the command that
     *                 requires TAM mode
     */
    void tamEnterMode    (int16_t func);

    /**
     * Leaves TAM mode. TAM mode is closed and the pending operation is cancelled.
     */
    void tamLeaveMode    (void);

    /**
     * Processes input for the TAM buffer.
     * Almost all input is handled by this function. The exceptions are:
     * - alpha input when in 'alpha' mode (tamState_t::alpha), where input of alpha
     *   characters must be put into the AIM buffer before calling this function with
     *   the input item
     * - `EXIT` and other external functions where TAM should be closed externally
     *   using ::tamLeaveMode
     *
     * After calling this function the ::tamBuffer will be updated and it should be
     * redrawn to the relevant part of the display.
     *
     * \param[in] item the ITM value to process
     */
    void tamProcessInput (uint16_t item);


    /**
     * Returns actual function for current TAM.
     * TODO: this shouldn't be public
     *
     * \return operation code
     */
    int16_t tamOperation (void);
  #endif // !TESTSUITE_BUILD

#endif // !TAM_H
