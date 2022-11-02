// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "gtkScreen.h"

#include "charString.h"
#include "defines.h"
#include "display.h"
#include "error.h"
#include "flags.h"
#include "fonts.h"
#include "longIntegerType.h"
#include "registers.h"
#include "registerValueConversions.h"

#include "wp43.h"



gboolean drawScreen(GtkWidget *widget, cairo_t *cr, gpointer data) {
  cairo_surface_t *imageSurface;

  imageSurface = cairo_image_surface_create_for_data((unsigned char *)screenData, CAIRO_FORMAT_RGB24, SCREEN_WIDTH, SCREEN_HEIGHT, screenStride * 4);
  #if defined(RASPBERRY) && (SCREEN_800X480 == 1)
    cairo_scale(cr, 2.0, 2.0);
  #endif // defined(RASPBERRY) && (SCREEN_800X480 == 1)
  cairo_set_source_surface(cr, imageSurface, 0, 0);
  cairo_surface_mark_dirty(imageSurface);
  #if defined(RASPBERRY) && (SCREEN_800X480 == 1)
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_FAST);
  #endif // defined(RASPBERRY) && (SCREEN_800X480 == 1)
  cairo_paint(cr);
  cairo_surface_destroy(imageSurface);

  screenChange = false;

  return FALSE;
}



void copyScreenToClipboard(void) {
  cairo_surface_t *imageSurface;
  GtkClipboard *clipboard;

  clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_clipboard_clear(clipboard);
  gtk_clipboard_set_text(clipboard, "", 0); //JM FOUND TIP TO PROPERLY CLEAR CLIPBOARD: https://stackoverflow.com/questions/2418487/clear-the-system-clipboard-using-the-gtk-lib-in-c/2419673#2419673

  imageSurface = cairo_image_surface_create_for_data((unsigned char *)screenData, CAIRO_FORMAT_RGB24, SCREEN_WIDTH, SCREEN_HEIGHT, screenStride * 4);
  gtk_clipboard_set_image(clipboard, gdk_pixbuf_get_from_surface(imageSurface, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
}



static void angularUnitToString(angularMode_t angularMode, char *string) {
  switch(angularMode) {
    case amRadian: {
      strcpy(string, "r");
      break;
    }
    case amMultPi: {
      strcpy(string, STD_pi);
      break;
    }
    case amGrad: {
      strcpy(string, "g");
      break;
    }
    case amDegree: {
      strcpy(string, STD_DEGREE);
      break;
    }
    case amDMS: {
      strcpy(string, "d.ms");
      break;
    }
    case amNone: {
      break;
    }
    default: {
      strcpy(string, "?");
    }
  }
}



void copyRegisterToClipboardString(calcRegister_t regist, char *clipboardString) {
  longInteger_t lgInt;
  int16_t base, sign, n;
  uint64_t shortInt;
  char string[30000];

  switch(getRegisterDataType(regist)) {
    case dtLongInteger: {
      convertLongIntegerRegisterToLongInteger(regist, lgInt);
      longIntegerToAllocatedString(lgInt, string, 30000);
      longIntegerFree(lgInt);
      break;
    }

    case dtTime: {
      timeToDisplayString(regist, string, false);
      break;
    }

    case dtDate: {
      dateToDisplayString(regist, string);
      break;
    }

    case dtString: {
      xcopy(string, REGISTER_STRING_DATA(regist), stringByteLength(REGISTER_STRING_DATA(regist)) + 1);
      break;
    }

    case dtReal34Matrix: {
      dataBlock_t* dblock = REGISTER_REAL34_MATRIX_DBLOCK(regist);
      real34_t *real34 = REGISTER_REAL34_MATRIX_M_ELEMENTS(regist);
      real34_t reduced;
      int rows, columns, len;

      rows = dblock->matrixRows;
      columns = dblock->matrixColumns;
      sprintf(string, "%dx%d", rows, columns);

      for(int i=0; i<rows*columns; i++) {
        strcat(string, LINEBREAK);
        len = strlen(string);

        real34Reduce(real34++, &reduced);
        real34ToString(&reduced, string + len);

        if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
          strcat(string + len, ".");
        }
      }
      break;
    }

    case dtComplex34Matrix: {
      dataBlock_t* dblock = REGISTER_COMPLEX34_MATRIX_DBLOCK(regist);
      complex34_t *complex34 = REGISTER_COMPLEX34_MATRIX_M_ELEMENTS(regist);
      real34_t reduced;
      int rows, columns, len;

      rows = dblock->matrixRows;
      columns = dblock->matrixColumns;
      sprintf(string, "%dx%d", rows, columns);

      for(int i=0; i<rows*columns; i++, complex34++) {
        strcat(string, LINEBREAK);
        len = strlen(string);

        // Real part
        real34Reduce((real34_t *)complex34, &reduced);
        real34ToString(&reduced, string + len);
        if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
          strcat(string + len, ".");
        }
        len = strlen(string);

        // Imaginary part
        real34Reduce(((real34_t *)complex34) + 1, &reduced);
        if(real34IsNegative(&reduced)) {
          sprintf(string + len, " - %sx", COMPLEX_UNIT);
          len += 5;
          real34SetPositiveSign(&reduced);
          real34ToString(&reduced, string + len);
        }
        else {
          sprintf(string + len + strlen(string + len), " + %sx", COMPLEX_UNIT);
          len += 5;
          real34ToString(&reduced, string + len);
        }
        if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
          strcat(string + len, ".");
        }
      }
      break;
    }

    case dtShortInteger: {
      convertShortIntegerRegisterToUInt64(regist, &sign, &shortInt);
      base = getRegisterShortIntegerBase(regist);

      n = ERROR_MESSAGE_LENGTH - 100;
      sprintf(errorMessage + n--, "#%d (word size = %u)", base, shortIntegerWordSize);

      if(shortInt == 0) {
        errorMessage[n--] = '0';
      }
      else {
        while(shortInt != 0) {
          errorMessage[n--] = digits[shortInt % base];
          shortInt /= base;
        }
        if(sign) {
          errorMessage[n--] = '-';
        }
      }
      n++;

      strcpy(string, errorMessage + n);
      break;
    }

    case dtReal34: {
      real34_t reduced;

      real34Reduce(REGISTER_REAL34_DATA(regist), &reduced);
      real34ToString(&reduced, string);
      if(strchr(string, '.') == NULL && strchr(string, 'E') == NULL) {
        strcat(string, ".");
      }
      angularUnitToString(getRegisterAngularMode(regist), string + strlen(string));
      break;
    }

    case dtComplex34: {
      real34_t reduced;
      int len;
      char tmpStr[100];

      // Real part
      real34Reduce(REGISTER_REAL34_DATA(regist), &reduced);
      real34ToString(&reduced, tmpStr);
      if(strchr(tmpStr, '.') == NULL && strchr(tmpStr, 'E') == NULL) {
        strcat(tmpStr, ".");
      }
      len = strlen(tmpStr);

      // Imaginary part
      real34Reduce(REGISTER_IMAG34_DATA(regist), &reduced);
      if(real34IsNegative(&reduced)) {
        sprintf(string, "%s - %sx", tmpStr, COMPLEX_UNIT);
        len += 5;
        real34SetPositiveSign(&reduced);
        real34ToString(&reduced, string + len);
      }
      else {
        sprintf(string, "%s + %sx", tmpStr, COMPLEX_UNIT);
        len += 5;
        real34ToString(&reduced, string + len);
      }
      if(strchr(string + len, '.') == NULL && strchr(string + len, 'E') == NULL) {
        strcat(string + len, ".");
      }

      break;
    }

    case dtConfig: {
      xcopy(string, "Configuration data", 19);
      break;
    }

    default: {
      sprintf(string, "In function copyRegisterXToClipboard, the data type %" PRIu32 " is unknown! Please try to reproduce and submit a bug.", getRegisterDataType(regist));
    }
  }

  stringToUtf8(string, (uint8_t *)clipboardString);
}



void copyRegisterXToClipboard(void) {
  GtkClipboard *clipboard;
  char clipboardString[30000];

  clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_clipboard_clear(clipboard);
  gtk_clipboard_set_text(clipboard, "", 0); //JM FOUND TIP TO PROPERLY CLEAR CLIPBOARD: https://stackoverflow.com/questions/2418487/clear-the-system-clipboard-using-the-gtk-lib-in-c/2419673#2419673

  copyRegisterToClipboardString(REGISTER_X, clipboardString);
  gtk_clipboard_set_text(clipboard, clipboardString, -1);
}



void copyStackRegistersToClipboardString(char *clipboardString) {
  char *ptr = clipboardString;

  strcpy(ptr, "K = ");
  ptr += 4;
  copyRegisterToClipboardString(REGISTER_K, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "J = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_J, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "I = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_I, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "L = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_L, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "D = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_D, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "C = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_C, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "B = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_B, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "A = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_A, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "T = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_T, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "Z = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_Z, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "Y = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_Y, ptr);

  ptr += strlen(ptr);
  strcpy(ptr, LINEBREAK "X = ");
  ptr += strlen(ptr);
  copyRegisterToClipboardString(REGISTER_X, ptr);
}



void copyStackRegistersToClipboard(void) {
  GtkClipboard *clipboard;
  char clipboardString[10000];

  clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_clipboard_clear(clipboard);
  gtk_clipboard_set_text(clipboard, "", 0); //JM FOUND TIP TO PROPERLY CLEAR CLIPBOARD: https://stackoverflow.com/questions/2418487/clear-the-system-clipboard-using-the-gtk-lib-in-c/2419673#2419673

  copyStackRegistersToClipboardString(clipboardString);

  gtk_clipboard_set_text(clipboard, clipboardString, -1);
}



void copyAllRegistersToClipboard(void) {
  GtkClipboard *clipboard;
  char clipboardString[15000], *ptr = clipboardString;

  clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  gtk_clipboard_clear(clipboard);
  gtk_clipboard_set_text(clipboard, "", 0); //JM FOUND TIP TO PROPERLY CLEAR CLIPBOARD: https://stackoverflow.com/questions/2418487/clear-the-system-clipboard-using-the-gtk-lib-in-c/2419673#2419673

  copyStackRegistersToClipboardString(ptr);

  for(int32_t regist=99; regist>=0; --regist) {
    ptr += strlen(ptr);
    sprintf(ptr, LINEBREAK "R%02d = ", regist);
    ptr += strlen(ptr);
    copyRegisterToClipboardString(regist, ptr);
  }

  for(int32_t regist=currentNumberOfLocalRegisters-1; regist>=0; --regist) {
    ptr += strlen(ptr);
    sprintf(ptr, LINEBREAK "R.%02d = ", regist);
    ptr += strlen(ptr);
    copyRegisterToClipboardString(FIRST_LOCAL_REGISTER + regist, ptr);
  }

  if(statisticalSumsPointer != NULL) {
    char sumName[40];
    for(int32_t sum=0; sum<NUMBER_OF_STATISTICAL_SUMS; sum++) {
      ptr += strlen(ptr);

      switch(sum) {
        case 0: {
          strcpy(sumName,           "n             "           );
          break;
        }
        case 1: {
          strcpy(sumName, STD_SIGMA "(x)          "            );
          break;
        }
        case 2: {
          strcpy(sumName, STD_SIGMA "(y)          "            );
          break;
        }
        case 3: {
          strcpy(sumName, STD_SIGMA "(x" STD_SUP_2 ")         ");
          break;
        }
        case 4: {
          strcpy(sumName, STD_SIGMA "(x" STD_SUP_2 "y)        ");
          break;
        }
        case 5: {
          strcpy(sumName, STD_SIGMA "(y" STD_SUP_2 ")         ");
          break;
        }
        case 6: {
          strcpy(sumName, STD_SIGMA "(xy)         "            );
          break;
        }
        case 7: {
          strcpy(sumName, STD_SIGMA "(ln(x)" STD_CROSS "ln(y))");
          break;
        }
        case 8: {
          strcpy(sumName, STD_SIGMA "(ln(x))      "            );
          break;
        }
        case 9: {
          strcpy(sumName, STD_SIGMA "(ln" STD_SUP_2 "(x))     ");
          break;
        }
        case 10: {
          strcpy(sumName, STD_SIGMA "(y ln(x))    "            );
          break;
        }
        case 11: {
          strcpy(sumName, STD_SIGMA "(ln(y))      "            );
          break;
        }
        case 12: {
          strcpy(sumName, STD_SIGMA "(ln" STD_SUP_2 "(y))     ");
          break;
        }
        case 13: {
          strcpy(sumName, STD_SIGMA "(x ln(y))    "            );
          break;
        }
        case 14: {
          strcpy(sumName, STD_SIGMA "(ln(y)/x)    "            );
          break;
        }
        case 15: {
          strcpy(sumName, STD_SIGMA "(x" STD_SUP_2 "/y)       ");
          break;
        }
        case 16: {
          strcpy(sumName, STD_SIGMA "(1/x)        "            );
          break;
        }
        case 17: {
          strcpy(sumName, STD_SIGMA "(1/x" STD_SUP_2 ")       ");
          break;
        }
        case 18: {
          strcpy(sumName, STD_SIGMA "(x/y)        "            );
          break;
        }
        case 19: {
          strcpy(sumName, STD_SIGMA "(1/y)        "            );
          break;
        }
        case 20: {
          strcpy(sumName, STD_SIGMA "(1/y" STD_SUP_2 ")       ");
          break;
        }
        case 21: {
          strcpy(sumName, STD_SIGMA "(x" STD_SUP_3 ")         ");
          break;
        }
        case 22: {
          strcpy(sumName, STD_SIGMA "(x" STD_SUP_4 ")         ");
          break;
        }
        case 23: {
          strcpy(sumName,           "x min         "           );
          break;
        }
        case 24: {
          strcpy(sumName,           "x max         "           );
          break;
        }
        case 25: {
          strcpy(sumName,           "y min         "           );
          break;
        }
        case 26: {
          strcpy(sumName,           "y max         "           );
          break;
        }
        default: {
          strcpy(sumName,           "?             "           );
        }
      }

      sprintf(ptr, LINEBREAK "SR%02d = ", sum);
      ptr += strlen(ptr);
      stringToUtf8(sumName, (uint8_t *)ptr);
      ptr += strlen(ptr);
      strcpy(ptr, " = ");
      ptr += strlen(ptr);
      realToString((real_t *)(statisticalSumsPointer + REAL_SIZE_IN_BLOCKS * sum), tmpString);
      if(strchr(tmpString, '.') == NULL && strchr(tmpString, 'E') == NULL) {
        strcat(tmpString, ".");
      }
      strcpy(ptr, tmpString);
    }
  }

  gtk_clipboard_set_text(clipboard, clipboardString, -1);
}
