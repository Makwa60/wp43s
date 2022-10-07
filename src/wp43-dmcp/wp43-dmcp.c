/* This file is part of 43S.
 *
 * 43S is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 43S is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 43S.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wp43.h"

#include "calcMode.h"
#include "charString.h"
#include "config.h"
#include "core/memory.h"
#include "flags.h"
#include "items.h"
#include "longIntegerType.h"
#include "timer.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include "ui/softmenus.h"

bool_t               backToDMCP;
//int                  keyAutoRepeat;
//int16_t              previousItem;
uint32_t             nextTimerRefresh;
uint32_t             nextScreenRefresh; // timer substitute for refreshLcd(), which does cursor blinking and other stuff
bool_t               wp43KbdLayout;

int convertKeyCode(int key) {
  if(wp43KbdLayout) {
    /////////////////////////////////////////////////
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

    //The switch instruction below is implemented as follows e.g. for the up arrow key on the WP43 layout:
    //  the output of keymap2layout for this key is UP 27:18 so we need the line:
    //    case 18: key = 27; break;
    switch(key) {               // Original
      case  1: key =  2; break; // SUM+
      case  2: key =  1; break; // 1/x
      case  3: key =  6; break; // SQRT
      case  4: key =  5; break; // LOG
      case  5: key =  4; break; // LN
      case  6: key = 22; break; // XEQ
    //case  7: key =  7; break; // STO
    //case  8: key =  8; break; // RCL
    //case  9: key =  9; break; // RDN
      case 10: key =  3; break; // SIN
      case 11: key = 10; break; // COS
      case 12: key = 11; break; // TAN
    //case 13: key = 13; break; // ENTER
    //case 14: key = 14; break; // x<>y
    //case 15: key = 15; break; // +/-
    //case 16: key = 16; break; // E
    //case 17: key = 17; break; // <--
      case 18: key = 27; break; // UP
    //case 19: key = 19; break; // 7
    //case 20: key = 20; break; // 8
    //case 21: key = 21; break; // 9
      case 22: key = 18; break; // /
      case 23: key = 32; break; // DOWN
    //case 24: key = 24; break; // 4
    //case 25: key = 25; break; // 5
    //case 26: key = 26; break; // 6
      case 27: key = 23; break; // x
      case 28: key = 12; break; // SHIFT
    //case 29: key = 29; break; // 1
    //case 30: key = 30; break; // 2
    //case 31: key = 31; break; // 3
      case 32: key = 28; break; // -
      case 33: key = 37; break; // EXIT
    //case 34: key = 34; break; // 0
    //case 35: key = 35; break; // .
    //case 36: key = 36; break; // R/S
      case 37: key = 33; break; // +
      default: {
      }
    }
  }
  return key;
}

void program_main(void) {
  int key = 0;
  char charKey[3];
  /*bool_t wp43KbdLayout, inFastRefresh = 0, inDownUpPress = 0, repeatDownUpPress = 0*/;
  //uint32_t now, previousRefresh, nextAutoRepeat = 0;

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
  key = 0;

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
  //previousRefresh = sys_current_ms();
  nextScreenRefresh = sys_current_ms() + SCREEN_REFRESH_PERIOD;
  //now = sys_current_ms();
  //runner_key_tout_init(0); // Enables fast auto repeat

  timerReset();
  timerConfig(TO_AUTO_REPEAT, execAutoRepeat, 0);
  timerConfig(TO_TIMER_APP, execTimerApp, 0);
  timerConfig(TO_KB_ACTV, timerDummyTest, TO_KB_ACTV);
  //timerConfig(TO_SHOW_NOP, execNOPTimeout, TO_SHOW_NOP);
  nextTimerRefresh = 0;

  // Status flags:
  //   ST(STAT_PGM_END)   - Indicates that program should go to off state (set by auto off timer)
  //   ST(STAT_SUSPENDED) - Program signals it is ready for off and doesn't need to be woken-up again
  //   ST(STAT_OFF)       - Program in off state (OS goes to sleep and only [EXIT] key can wake it up again)
  //   ST(STAT_RUNNING)   - OS doesn't sleep in this mode
  //SET_ST(STAT_CLK_WKUP_SECONDS);
  SET_ST(STAT_CLK_WKUP_ENABLE); // Enable wakeup each minute (for clock update)

  while(!backToDMCP) {
    if(ST(STAT_PGM_END) && ST(STAT_SUSPENDED)) { // Already in off mode and suspended
      CLR_ST(STAT_RUNNING);
      sys_sleep();
    }
    else if((!ST(STAT_PGM_END) && key_empty())) {         // Just wait if no keys available.
      CLR_ST(STAT_RUNNING);

      if(nextTimerRefresh == 0) {                         // no timeout available
        sys_sleep();
      }
      else {                                                                  // timeout available
        //uint32_t timeoutTime = max(1, nextTimerRefresh - sys_current_ms());
        uint32_t timeoutTime = sys_current_ms();
        if(nextTimerRefresh > timeoutTime) {
          timeoutTime = nextTimerRefresh - timeoutTime;
        }
        else {
          timeoutTime = 1;
        }

        //uint32_t sleepTime = max(1, nextScreenRefresh - sys_current_ms());
        uint32_t sleepTime = sys_current_ms();
        if(nextScreenRefresh > sleepTime) {
          sleepTime = nextScreenRefresh - sleepTime;
        }
        else {
          sleepTime = 1;
        }
        if(showFunctionNameCounter > 0) {
          sleepTime = min(sleepTime, FAST_SCREEN_REFRESH_PERIOD);
        }
        sleepTime = min(sleepTime, timeoutTime);

        sys_timer_start(TIMER_IDX_REFRESH_SLEEP, max(sleepTime, 1));          // wake up for refresh
        sys_sleep();
        sys_timer_disable(TIMER_IDX_REFRESH_SLEEP);
      }


      //sys_timer_start(TIMER_IDX_SCREEN_REFRESH, max(1, nextScreenRefresh - now));  // wake up for screen refresh
      //if(inDownUpPress) {
      //  sys_timer_start(TIMER_IDX_AUTO_REPEAT, max(1, nextAutoRepeat - now)); // wake up for key auto-repeat
      //}
      //sys_sleep();
      //sys_timer_disable(TIMER_IDX_SCREEN_REFRESH);
      //if(inDownUpPress) {
      //  repeatDownUpPress = (sys_current_ms() > nextAutoRepeat);
      //  sys_timer_disable(TIMER_IDX_AUTO_REPEAT);
      //}
    }

    //now = sys_current_ms();

    // =======================
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
      if(!ST(STAT_OFF) && (timerGetStatus(TO_KB_ACTV) != TMR_RUNNING)) {
        timerStart(TO_KB_ACTV, TO_KB_ACTV, SCREEN_REFRESH_PERIOD+50);
      }
      CLR_ST(STAT_POWER_CHANGE);
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
    if(!key_empty() || (nextTimerRefresh != 0)) {
      reset_auto_off();
    }

    // Fetch the key
    //  < 0 -> No key event
    //  > 0 -> Key pressed
    // == 0 -> Key released
    key = key_pop();

    //key = runner_get_key_delay(&keyAutoRepeat,
    //                           50,                            // timeout - this should be the fastest period between loops
    //                           KEY_AUTOREPEAT_FIRST_PERIOD,  // time before the first autorepeat
    //                           KEY_AUTOREPEAT_PERIOD,        // time between subsequent autorepeats
    //                           KEY_AUTOREPEAT_FIRST_PERIOD); // should be the same as time before first autorepeat
    //key = runner_get_key(&keyAutoRepeat);

    key = convertKeyCode(key);
    //The 3 lines below to see in the top left screen corner the pressed keycode
    //char sysLastKeyCh[5];
    //sprintf(sysLastKeyCh, " %02d", key);
    //showString(sysLastKeyCh, &standardFont, 0, 0, vmReverse, true, true);
    //The line below to emit a beep
    //while(get_beep_volume() < 11) beep_volume_up(); start_buzzer_freq(220000); sys_delay(200); stop_buzzer();

    // Increase the refresh rate if we are in an UP/DOWN key press so we pick up auto key repeats
    if(key == 27 || key == 32) {
      //inDownUpPress = 1;
      //nextAutoRepeat = now + KEY_AUTOREPEAT_FIRST_PERIOD;
      if(timerGetStatus(TO_AUTO_REPEAT) != TMR_RUNNING && (!shiftF || calcMode == cmPem) && !shiftG && (currentSoftmenuScrolls() || (calcMode != cmNormal && calcMode != cmNim && calcMode != cmAim))) {
        timerStart(TO_AUTO_REPEAT, key, KEY_AUTOREPEAT_FIRST_PERIOD);
      }
    }
    else if(key == 0) {
      //inDownUpPress = 0;
      //repeatDownUpPress = 0;
      //nextAutoRepeat = 0;
      timerStop(TO_AUTO_REPEAT);
    }
    //else if(repeatDownUpPress) {
    //  keyAutoRepeat = 1;
    //  key = 0;
    //  nextAutoRepeat = now + KEY_AUTOREPEAT_PERIOD;
    //  repeatDownUpPress = 0;
    //}

    //if(keyAutoRepeat) {
    //  if(key == 27 || key == 32) { // UP or DOWN keys
    //    //beep(2200, 50);
    //    key = 0; // to trigger btnReleased
    //  }
    //  else {
    //    key = -1;
    //  }
    //}

    if(38 <= key && key <=43) { // Function key
      sprintf(charKey, "%c", key+11);
      btnFnPressed(charKey);
      lcd_refresh();
    }
    else if(1 <= key && key <= 37) { // Not a function key
      sprintf(charKey, "%02d", key - 1);
      btnPressed(charKey);
      lcd_refresh();
    }
    else if(key == 0) { // Autorepeat of UP/DOWN or key released
      if(charKey[1] == 0) { // Last key pressed was one of the 6 function keys
        btnFnReleased(charKey);
      }
      else { // Last key pressed was not one of the 6 function keys
        //beep(440, 50);
        btnReleased(charKey);
        if(calcMode == cmPem && shiftF && ((charKey[0] == '2' && charKey[1] == '6') || (charKey[0] == '3' && charKey[1] == '1'))) {
          shiftF = false;
          refreshScreen();
        }
      }
      //keyAutoRepeat = 0;
      lcd_refresh();
    }

    if(key >= 0) {                                        // Temporary intermediate solution to get some refreshLcd and go to sleep afterwards
      if(key > 0) {
        timerStart(TO_KB_ACTV, TO_KB_ACTV, 60000);
      }
      else if(cursorEnabled == true) {
        timerStart(TO_KB_ACTV, TO_KB_ACTV, 4*FAST_SCREEN_REFRESH_PERIOD+50);
      }
      else
      {
        timerStart(TO_KB_ACTV, TO_KB_ACTV, FAST_SCREEN_REFRESH_PERIOD+50);
      }
    }

    //// Compute refresh period
    //if(showFunctionNameCounter > 0) {
    //  inFastRefresh = 1;
    //  nextScreenRefresh = previousRefresh + FAST_SCREEN_REFRESH_PERIOD;
    //}
    //else {
    //  inFastRefresh = 0;
    //}

    uint32_t now = sys_current_ms();

    if(nextTimerRefresh != 0 && nextTimerRefresh <= now) {
      timerRefresh();                                     // Executes pending timer jobs
    }
    now = sys_current_ms();
    if(nextScreenRefresh <= now) {
      //previousRefresh = now;
      nextScreenRefresh += ((showFunctionNameCounter > 0) ? FAST_SCREEN_REFRESH_PERIOD : SCREEN_REFRESH_PERIOD);
      if(nextScreenRefresh < now) {
        nextScreenRefresh = now + ((showFunctionNameCounter > 0) ? FAST_SCREEN_REFRESH_PERIOD : SCREEN_REFRESH_PERIOD);         // we were out longer than expected; just skip ahead.
      }
      if(calcMode != cmTimerApp) {
        refreshLcd();
        lcd_refresh();
      }
    }
  }
}
