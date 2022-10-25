// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "wp43.h"

#include "apps/timerApp.h"
#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "core/timer.h"
#include "flags.h"
#include "items.h"
#include "longIntegerType.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include "ui/softmenus.h"
#include <stdbool.h>

bool     backToDMCP;
uint32_t nextTimerRefresh;
uint32_t nextScreenRefresh; // timer substitute for refreshLcd(), which does cursor blinking and other stuff
bool     wp43KbdLayout;

int convertKeyCode(int key) {
  if(!wp43KbdLayout) {
    return key;
  }

  // For key reassignment see:
  // https://technical.swissmicros.com/dm42/devel/dmcp_devel_manual/#_system_key_table
  //
  // Output of keymap2layout keymap.txt
  //
  //    +-----+-----+-----+-----+-----+-----+
  // 1: | F1  | F2  | F3  | F4  | F5  | F6  |
  //    |38:38|39:39|40:40|41:41|42:42|43:43|
  //    +-----+-----+-----+-----+-----+-----+
  // 2: | 1/x |Sum+ | SIN | LN  | LOG |SQRT |
  //    | 1: 2| 2: 1| 3:10| 4: 5| 5: 4| 6: 3|
  //    +-----+-----+-----+-----+-----+-----+
  // 3: | STO | RCL | RDN | COS | TAN |SHIFT|
  //    | 7: 7| 8: 8| 9: 9|10:11|11:12|12:28|
  //    +-----+-----+-----+-----+-----+-----+
  // 4: |   ENTER   |x<>y | CHS |  E  | <-- |
  //    |   13:13   |14:14|15:15|16:16|17:17|
  //    +-----------+-----+-----+-----+-----+
  // 5: |  DIV |   7  |   8  |   9  |  XEQ  |
  //    | 18:22| 19:19| 20:20| 21:21| 22: 6 |
  //    +------+------+------+------+-------+
  // 6: |  MUL |   4  |   5  |   6  |  UP   |
  //    | 23:27| 24:24| 25:25| 26:26| 27:18 |
  //    +------+------+------+------+-------+
  // 7: |  SUB |   1  |   2  |   3  | DOWN  |
  //    | 28:32| 29:29| 30:30| 31:31| 32:23 |
  //    +------+------+------+------+-------+
  // 8: |  ADD |   0  |  DOT |  RUN | EXIT  |
  //    | 33:37| 34:34| 35:35| 36:36| 37:33 |
  //    +------+------+------+------+-------+

  // The keys from DMCP are ordered from top left (excluding function keys) in rows, which is exactly the
  // order we want. However, we apply a keymap so that the keys works as expected in the DMCP menus.
  // If we have applied this keymap, we need to reverse this translation to get them back in the order we
  // expect.
  // The layout above shows the original number followed by the new assignment for each key. To reverse the
  // map we take the key codes and find the WP43 key that corresponds to the new position for that key code.
  // Since the map is to preserve meaning, this mapping has a close correspondence between the DMCP key
  // code and the WP43 key code.
  switch(key) {
    case KEY_SIGMA: return kcExp;
    case KEY_INV:   return kcInv;
    case KEY_SQRT:  return kcSqrt;
    case KEY_LOG:   return kcEToX;
    case KEY_LN:    return kcLn;
    case KEY_XEQ:   return kcXeq;
  //case KEY_STO:   return kcSto;
  //case KEY_RCL:   return kcRcl;
  //case KEY_RDN:   return kcRdown;
    case KEY_SIN:   return kcTri;
    case KEY_COS:   return kcCC;
    case KEY_TAN:   return kcShiftF;
  //case KEY_ENTER: return kcEnter;
  //case KEY_SWAP:  return kcSwap;
  //case KEY_CHS:   return kcChs;
  //case KEY_E:     return kcE;
  //case KEY_BSP:   return kcBackspace;
    case KEY_UP:    return kcUp;
  //case KEY_7:     return kc7;
  //case KEY_8:     return kc8;
  //case KEY_9:     return kc9;
    case KEY_DIV:   return kcDiv;
    case KEY_DOWN:  return kcDown;
  //case KEY_4:     return kc4;
  //case KEY_5:     return kc5;
  //case KEY_6:     return kc6;
    case KEY_MUL:   return kcMul;
    case KEY_SHIFT: return kcShiftG;
  //case KEY_1:     return kc1;
  //case KEY_2:     return kc2;
  //case KEY_3:     return kc3;
    case KEY_SUB:   return kcSub;
    case KEY_EXIT:  return kcExit;
  //case KEY_0:     return kc0;
  //case KEY_DOT:   return kcDot;
  //case KEY_RUN:   return kcRun;
    case KEY_ADD:   return kcAdd;
    default:        return key;
  }
}

void cbRefreshLcd(uint16_t param) {
  if(calcMode != cmTimerApp) {
    refreshLcd();
    lcd_refresh();
  }
  timerStart(tidRefreshLcd, NOPARAM, SCREEN_REFRESH_PERIOD);
}

void program_main(void) {
  const int TimerId = 0;
  int key = kcNoKey;
  int lastKey = kcNoKey;

  wp43MemInBlocks = 0;
  gmpMemInBytes = 0;
  mp_set_memory_functions(allocGmp, reallocGmp, freeGmp);

  lcd_clear_buf();
  lcd_putsAt(t24, 4, "Press the bottom left key."); lcd_refresh();
  while(key != 33 && key != 37) {
    key = key_pop();
    while(key == -1) {
      sys_sleep();
      key = key_pop();
    }
  }

  wp43KbdLayout = (key == 37); // bottom left key
  key = kcNoKey;

  lcd_clear_buf();
  fnReset(CONFIRMED);
  refreshScreen();

  #if 0
    longInteger_t li;
    uint32_t addr, min, max, *ptr;

    min = 1;
    max = 100000000;
    while(min+1 < max) {
      ptr = malloc((max + min) >> 1);
      if(ptr) {
        free(ptr);
        min = (max + min) >> 1;
      }
      else {
        max = (max + min) >> 1;
      }
    }

    ptr = malloc(min);
    xcopy(&addr, &ptr, 4);
    free(ptr);
    longIntegerInit(li);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 50);

    uIntToLongInteger(min, li);
    convertLongIntegerToShortIntegerRegister(li, 10, 51);

    ptr = (uint32_t *)qspi_user_addr();
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 52);

    addr = (uint32_t)qspi_user_size(); // QSPI user size in bytes
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 10, 53);

    ptr = (uint32_t *)&ram;
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 54);

    ptr = (uint32_t *)&indexOfItems;
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 55);

    ptr = (uint32_t *)ppgm_fp;
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 56);

    ptr = (uint32_t *)get_reset_state_file();
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 57);

    addr = 0x38; // RESET_STATE_FILE_SIZE;
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 10, 58);

    ptr = (uint32_t *)aux_buf_ptr();
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 59);

    addr = AUX_BUF_SIZE;
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 10, 60);

    ptr = (uint32_t *)write_buf_ptr();
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 61);

    addr = (uint32_t)write_buf_size();
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 10, 62);

    addr = (uint32_t)get_hw_id();
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 10, 63);

    ptr = (uint32_t *)resizeProgramMemory;
    xcopy(&addr, &ptr, 4);
    uIntToLongInteger(addr, li);
    convertLongIntegerToShortIntegerRegister(li, 16, 64);

    longIntegerFree(li);
  #endif // 1

  backToDMCP = false;

  lcd_refresh();
  nextScreenRefresh = sys_current_ms() + SCREEN_REFRESH_PERIOD;

  configSetUpTimers();
  timerConfig(tidRefreshLcd, cbRefreshLcd);
  timerStart(tidRefreshLcd, NOPARAM, SCREEN_REFRESH_PERIOD);
  nextTimerRefresh = 0;

  // Status flags:
  //   ST(STAT_PGM_END)   - Indicates that program should go to off state (set by auto off timer)
  //   ST(STAT_SUSPENDED) - Program signals it is ready for off and doesn't need to be woken-up again
  //   ST(STAT_OFF)       - Program in off state (OS goes to sleep and only [EXIT] key can wake it up again)
  //   ST(STAT_RUNNING)   - OS doesn't sleep in this mode
  //   SET_ST(STAT_CLK_WKUP_SECONDS)
  SET_ST(STAT_CLK_WKUP_ENABLE); // Enable wakeup each minute (for clock update)

  while(!backToDMCP) {
    if(ST(STAT_PGM_END) && ST(STAT_SUSPENDED)) {
      // Already in off mode and suspended
      CLR_ST(STAT_RUNNING);
      sys_sleep();
    }
    else if((!ST(STAT_PGM_END) && key_empty())) {
      // No keys available, wait for timers
      CLR_ST(STAT_RUNNING);

      if(nextTimerRefresh == 0) {
        sys_sleep();
      }
      else {
        sys_timer_start(TimerId, max(nextTimerRefresh, 1)); // wake up for refresh
        sys_sleep();
        sys_timer_disable(TimerId);
      }
    }

    // Externally forced LCD repaint
    if(ST(STAT_CLK_WKUP_FLAG)) {
      if(!ST(STAT_OFF) && (nextTimerRefresh == 0)) {
        refreshLcd();
        lcd_refresh_wait();
      }
      CLR_ST(STAT_CLK_WKUP_FLAG);
      continue;
    }
    if(ST(STAT_POWER_CHANGE)) {
      CLR_ST(STAT_POWER_CHANGE);
      refreshLcd();
      lcd_refresh_dma();
      continue;
    }

    // Wakeup in off state or going to sleep
    if(ST(STAT_PGM_END) || ST(STAT_SUSPENDED)) {
      if(!ST(STAT_SUSPENDED)) {
        // Going to off mode
        lcd_set_buf_cleared(0); // Mark no buffer change region
        draw_power_off_image(1);

        LCD_power_off(0);
        SET_ST(STAT_SUSPENDED);
        SET_ST(STAT_OFF);
      }
      // Already in OFF -> just continue to sleep above
      continue;
    }

    // Well, we are woken-up
    SET_ST(STAT_RUNNING);

    // Clear suspended state, because now we are definitely reached the active state
    CLR_ST(STAT_SUSPENDED);

    // Get up from OFF state
    if(ST(STAT_OFF)) {
      LCD_power_on();
      rtc_wakeup_delay(); // Ensure that RTC readings after power off will be OK

      CLR_ST(STAT_OFF);

      if(!lcd_get_buf_cleared()) {
        lcd_forced_refresh(); // Just redraw from LCD buffer
      }

      if(getSystemFlag(FLAG_AUTXEQ)) { // Run the program if AUTXEQ is set
        clearSystemFlag(FLAG_AUTXEQ);
        if(programRunStop != PGM_RUNNING) {
          screenUpdatingMode = SCRUPD_AUTO;
          runFunction(ITM_RS);
        }
        refreshScreen();
      }
    }

    // Key is ready -> clear auto off timer
    if(!key_empty() || (calcMode == cmTimerApp)) {
      reset_auto_off();
    }

    // Fetch the key
    //  < 0 -> No key event
    //  > 0 -> Key pressed
    // == 0 -> Key released
    key = key_pop();
    key = convertKeyCode(key);

    if(1 <= key && key <= 43) {
      btnPressed(key);
      lastKey = key;
    }
    else if(key == 0) { // Key released
      btnReleased(lastKey);
      if(calcMode == cmPem && shiftF && (lastKey == kcUp || lastKey == kcDown)) {
        shiftF = false;
        refreshScreen();
      }
    }

    // TODO: There was code that would create a delay and then refresh the screen after the keyboard
    // was active (and a few other events). This has been removed and replaced with always refreshing
    // after a fixed interval. This is worse for performance and should be fixed.
    // The fix should be to only refresh when something has changed and all changes should be from an
    // event (timer or key press).

    // Execute pending timer jobs
    nextTimerRefresh = timerRun();
    lcd_refresh_dma();
  }
}
