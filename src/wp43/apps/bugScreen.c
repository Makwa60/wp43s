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

#include "apps/bugScreen.h"

#include "apps/apps.h"
#include "charString.h"
#include "flags.h"
#include "fonts.h"
#include "hal/lcd.h"
#include "ui/screen.h"
#include "typeDefinitions.h"
#include <string.h>

#include "wp43.h"

#if !defined(TESTSUITE_BUILD)
  static const char *_bugScreenMsg = NULL;



  static void _nextWord(const char *str, int16_t *pos, char *word) {
    int16_t i = 0;

    while(str[*pos] != 0 && str[*pos] != ' ') {
      word[i++] = str[(*pos)++];
    }

    word[i] = 0;

    while(str[*pos] == ' ') {
      (*pos)++;
    }
  }



  static bool_t _bugScreenKeyHandler(int16_t item) {
    return false;
  }



  static void _bugScreenDraw(void) {
    int16_t y, pos;
    char line[100], word[50], message[1000];
    bool_t firstWordOfLine;

    lcd_fill_rect(0, 20, SCREEN_WIDTH, 220, LCD_SET_VALUE);

    y = 20;
    showString("This is most likely a bug in the firmware!", &standardFont, 1, y, vmNormal, true, false);
    y += 20;

    strcpy(message, _bugScreenMsg);
    strcat(message, " Try to reproduce this and report a bug. Press EXIT to leave." );

    pos = 0;
    line[0] = 0;
    firstWordOfLine = true;

    _nextWord(message, &pos, word);
    while(word[0] != 0) {
      if(stringWidth(line, &standardFont, true, true) + (firstWordOfLine ? 0 : 4) + stringWidth(word, &standardFont, true, true) >= SCREEN_WIDTH) { // 4 is the width of STD_SPACE_PUNCTUATION
        showString(line, &standardFont, 1, y, vmNormal, true, false);
        y += 20;
        line[0] = 0;
        firstWordOfLine = true;
      }

      if(firstWordOfLine) {
        strcpy(line, word);
        firstWordOfLine = false;
      }
      else {
        strcat(line, STD_SPACE_PUNCTUATION);
        strcat(line, word);
      }

      _nextWord(message, &pos, word);
    }

    if(line[0] != 0) {
      showString(line, &standardFont, 1, y, vmNormal, true, false);
    }
  }



  void bugScreen(const char *msg) {
    clearSystemFlag(FLAG_ALPHA);
    hideCursor();
    cursorEnabled = false;
    _bugScreenMsg = msg;
    appsEnter(glFlagFontBrowser, _bugScreenKeyHandler, _bugScreenDraw);
  }
#else
  void bugScreen(const char *msg) {
    printf("\n-----------------------------------------------------------------------\n");
    printf("%s\n", msg);
    printf("\n-----------------------------------------------------------------------\n");
  }
#endif // !TESTSUITE_BUILD
