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

#include "gtkGui.h"

#include "charString.h"
#include "debug.h"
#include "error.h"
#include "gtkScreen.h"
#include "items.h"
#include "keyboard.h"
#include "timer.h"
#include "saveRestoreCalcState.h"
#include "screen.h"
#include "wp43.h"

GtkWidget *grid;
#if (SCREEN_800X480 == 0)
  GtkWidget *backgroundImage, *bezelImage[3], *behindScreenImage, *fgShiftedArea1, *fgShiftedArea2;
  GtkWidget *lblFSoftkeyArea, *lblGSoftkeyArea;
  int backgroundWidth, backgroundHeight;
  int lcdx, lcdy;
  int bezelX[3], bezelY[3];
  int behindScreenX, behindScreenY;
  int fgShiftedArea1X, fgShiftedArea1Y;
  int fgShiftedArea2X, fgShiftedArea2Y;

  #if (DEBUG_PANEL == 1)
    GtkWidget *lbl1[DEBUG_LINES], *lbl2[DEBUG_LINES];
    GtkWidget *btnBitFields, *btnFlags, *btnRegisters, *btnLocalRegisters, *btnStatisticalSums, *btnNamedVariables, *btnSavedStackRegisters;
    GtkWidget *chkHexaString;
    int16_t debugWidgetDx, debugWidgetDy;
  #endif // (DEBUG_PANEL == 1)

  char *cssData;
#endif // (SCREEN_800X480 == 0)



static gint destroyCalc(GtkWidget* w, GdkEventAny* e, gpointer data) {
  fnStopTimerApp();
  saveCalc();
  gtk_main_quit();

  return 0;
}



static void convertXYToKey(int x, int y) {
  int xMin, xMax, yMin, yMax;
  key[0] = 0;
  key[1] = 0;
  key[2] = 0;

  for(int i=0; i<43; i++) {
    xMin = calcKeyboard[i].x;
    yMin = calcKeyboard[i].y;
    if(i == 10 && currentBezel == 2 && (tam.mode == TM_LABEL || (tam.mode == TM_SOLVE && (tam.function != ITM_SOLVE || calcMode != CM_PEM)) || (tam.mode == TM_KEY && tam.keyInputFinished))) {
      xMax = xMin + calcKeyboard[10].width[3];
      yMax = yMin + calcKeyboard[10].height[3];
    }
    else {
      xMax = xMin + calcKeyboard[i].width[currentBezel];
      yMax = yMin + calcKeyboard[i].height[currentBezel];
    }

    if(   xMin <= x && x <= xMax
       && yMin <= y && y <= yMax) {
      if(i < 6) { // Function key
        key[0] = '1' + i;
      }
      else {
        key[0] = '0' + (i - 6)/10;
        key[1] = '0' + (i - 6)%10;
      }
      break;
    }
  }
}

void frmCalcMouseButtonPressed(GtkWidget *notUsed, GdkEvent *event, gpointer data) {
  if(key[0] == 0) { // The previous click must be released
    convertXYToKey((int)event->button.x, (int)event->button.y);
    if(key[0] == 0) {
      return;
    }

    if(event->type == GDK_DOUBLE_BUTTON_PRESS || event->type == GDK_TRIPLE_BUTTON_PRESS) { // return unprocessed for double or triple click
      return;
    }
    if(event->button.button == 2) { // Middle click
      shiftF = true;
      shiftG = false;
    }
    if(event->button.button == 3) { // Right click
      shiftF = false;
      shiftG = true;
    }

    if(key[1] == 0) { // Soft function key
      btnFnPressed(key);
    }
    else { // Not a soft function key
      btnPressed(key);
    }
  }
}

void frmCalcMouseButtonReleased(GtkWidget *notUsed, GdkEvent *event, gpointer data) {
  if(key[0] == 0) {
    return;
  }

  if(key[1] == 0) { // Soft function key
    btnFnReleased(key);
  }
  else { // Not a soft function key
    btnReleased(key);
  }

  key[0] = 0;
}



#if (SCREEN_800X480 == 0)
  static void getParameter(char *textLine, char *parameterName, char *parameterValue) {
    char *equalSign;

    *parameterName = 0;
    *parameterValue = 0;

    // removing comment and/or end of line
    for(size_t i=0; i<strlen(textLine); i++) {
      if(textLine[i] == '#' || textLine[i] == '\n' || textLine[i] == '\r') {
        textLine[i] = 0;
        break;
      }
    }
    if(textLine[0] == 0) {
      return;
    }

    // removing trailing spaces
    while(textLine[strlen(textLine) - 1] == ' ') {
      textLine[strlen(textLine) - 1] = 0;
    }
    if(textLine[0] == 0) {
      return;
    }

    equalSign = strchr(textLine, '=');
    if(equalSign == NULL) {
      return;
    }

    // remove spaces beheind parameter name
    *equalSign = 0;
    while(textLine[strlen(textLine) - 1] == ' ') {
      textLine[strlen(textLine) - 1] = 0;
    }
    strcpy(parameterName, textLine);

    // removing spaces in front of parameter value
    equalSign++;
    while(*equalSign == ' ') {
      equalSign++;
    }

    strcpy(parameterValue, equalSign);
  }

  static void prepareSkin(void) {
    FILE *skin;
    int calcKey;
    char textLine[1000], skinDirectory[213]="", fileName[412], parameter[100], value[200];

    skin = fopen("res/artwork/skin.cfg", "rb");
    if(skin == NULL) {
      moreInfoOnError("In function prepareSkin:", "error opening file res/artwork/skin.cfg!", NULL, NULL);
      exit(1);
    }

    fgets(textLine, 1000, skin);
    while(!feof(skin) && skinDirectory[0] == 0) {
      getParameter(textLine, parameter, value);

      if(!strcmp(parameter, "skinDirectory") && value[0] != 0) {
        sprintf(skinDirectory, "res/artwork/%s/", value);
      }

      fgets(textLine, 1000, skin);
    }

    if(skinDirectory[0] == 0) {
      moreInfoOnError("In function prepareSkin:", "cannot find skinDirectory in file res/artwork/skin.cfg!", NULL, NULL);
      exit(1);
    }

    fclose(skin);

    sprintf(fileName, "%s%s", skinDirectory, calcLandscape ? "landscapeSkin.cfg" : "portraitSkin.cfg");
    skin = fopen(fileName, "rb");
    if(skin == NULL) {
      moreInfoOnError("In function prepareSkin:", "error opening file", fileName, NULL);
      exit(1);
    }

    fgets(textLine, 1000, skin);
    while(!feof(skin)) {
      getParameter(textLine, parameter, value);

      if(parameter[0] != 0 && value[0] != 0) {
        if(!strcmp(parameter, "backgroundImage")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
            exit(1);
          }
          backgroundImage = gtk_image_new_from_file(fileName);
          const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(backgroundImage));
          backgroundWidth = gdk_pixbuf_get_width(pb);
          backgroundHeight = gdk_pixbuf_get_height(pb);
          continue;
        }

        if(!strcmp(parameter, "lcdx")) {
          lcdx = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "lcdy")) {
          lcdy = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "behindScreen")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
            exit(1);
          }
          behindScreenImage = gtk_image_new_from_file(fileName);
          continue;
        }

        if(!strcmp(parameter, "behindScreenx")) {
          behindScreenX = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "behindScreeny")) {
          behindScreenY = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "fgShiftedArea")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
            exit(1);
          }
          fgShiftedArea1 = gtk_image_new_from_file(fileName);
          fgShiftedArea2 = gtk_image_new_from_file(fileName);
          continue;
        }

        if(!strcmp(parameter, "fgShiftedArea1x")) {
          fgShiftedArea1X = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "fgShiftedArea1y")) {
          fgShiftedArea1Y = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "fgShiftedArea2x")) {
          fgShiftedArea2X = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "fgShiftedArea2y")) {
          fgShiftedArea2Y = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "bezelNormal")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
            exit(1);
          }
          bezelImage[0] = gtk_image_new_from_file(fileName);
          continue;
        }

        if(!strcmp(parameter, "bezelNormalx")) {
          bezelX[0] = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "bezelNormaly")) {
          bezelY[0] = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "bezelAIM")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
            exit(1);
          }
          bezelImage[1] = gtk_image_new_from_file(fileName);
          continue;
        }

        if(!strcmp(parameter, "bezelAIMx")) {
          bezelX[1] = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "bezelAIMy")) {
          bezelY[1] = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "bezelTAM")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
            exit(1);
          }
          bezelImage[2] = gtk_image_new_from_file(fileName);
          continue;
        }

        if(!strcmp(parameter, "bezelTAMx")) {
          bezelX[2] = atoi(value);
          continue;
        }

        if(!strcmp(parameter, "bezelTAMy")) {
          bezelY[2] = atoi(value);
          continue;
        }

        if(!strncmp(parameter, "key", 3)) {
          calcKey = 10*(parameter[3] - '0') + parameter[4] - '0';
          if(calcKey <= 16) {
            calcKey -= 11;
          }
          else if(calcKey <= 26) {
            calcKey -= 15;
          }
          else if(calcKey <= 36) {
            calcKey -= 19;
          }
          else if(calcKey <= 45) {
            calcKey -= 23;
          }
          else if(calcKey <= 55) {
            calcKey -= 28;
          }
          else if(calcKey <= 65) {
            calcKey -= 33;
          }
          else if(calcKey <= 75) {
            calcKey -= 38;
          }
          else if(calcKey <= 85) {
            calcKey -= 43;
          }

          if(!strcmp(parameter + 5, "x")) {
            calcKeyboard[calcKey].x = atoi(value);
            continue;
          }

          if(!strcmp(parameter + 5, "y")) {
            calcKeyboard[calcKey].y = atoi(value);
            continue;
          }

          if(!strcmp(parameter + 5, "Normal")) {
            sprintf(fileName, "%s%s", skinDirectory, value);
            if(access(fileName, F_OK) != 0) {
              moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
              exit(1);
            }
            calcKeyboard[calcKey].keyImage[0] = gtk_image_new_from_file(fileName);
            const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(calcKeyboard[calcKey].keyImage[0]));
            calcKeyboard[calcKey].width[0] = gdk_pixbuf_get_width(pb);
            calcKeyboard[calcKey].height[0] = gdk_pixbuf_get_height(pb);
            continue;
          }

          if(!strcmp(parameter + 5, "AIM")) {
            sprintf(fileName, "%s%s", skinDirectory, value);
            if(access(fileName, F_OK) != 0) {
              moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
              exit(1);
            }
            calcKeyboard[calcKey].keyImage[1] = gtk_image_new_from_file(fileName);
            const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(calcKeyboard[calcKey].keyImage[1]));
            calcKeyboard[calcKey].width[1] = gdk_pixbuf_get_width(pb);
            calcKeyboard[calcKey].height[1] = gdk_pixbuf_get_height(pb);
            continue;
          }

          if(!strcmp(parameter + 5, "TAM")) {
            sprintf(fileName, "%s%s", skinDirectory, value);
            if(access(fileName, F_OK) != 0) {
              moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
              exit(1);
            }
            calcKeyboard[calcKey].keyImage[2] = gtk_image_new_from_file(fileName);
            const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(calcKeyboard[calcKey].keyImage[2]));
            calcKeyboard[calcKey].width[2] = gdk_pixbuf_get_width(pb);
            calcKeyboard[calcKey].height[2] = gdk_pixbuf_get_height(pb);
            continue;
          }

          if(!strcmp(parameter + 5, "TAM_L")) {
            sprintf(fileName, "%s%s", skinDirectory, value);
            if(access(fileName, F_OK) != 0) {
              moreInfoOnError("In function prepareSkin:", "error: cannot access file", fileName, NULL);
              exit(1);
            }
            calcKeyboard[calcKey].keyImage[3] = gtk_image_new_from_file(fileName);
            const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(calcKeyboard[calcKey].keyImage[3]));
            calcKeyboard[calcKey].width[3] = gdk_pixbuf_get_width(pb);
            calcKeyboard[calcKey].height[3] = gdk_pixbuf_get_height(pb);
            continue;
          }
        }

        printf("Can't do anything with this parameter: %s=%s\n", parameter, value);
        exit(0);
      }

      fgets(textLine, 1000, skin);
    }

    fclose(skin);
  }

  /* Reads the CSS file to configure the calc's GUI style. */
  static void prepareCssData(void) {
    FILE *cssFile;
    char *toReplace, *replaceWith, needle[100], newNeedle[100];
    int  fileLg;

    // Convert the pre-CSS data to CSS data
    cssFile = fopen(CSSFILE, "rb");
    if(cssFile == NULL) {
      moreInfoOnError("In function prepareCssData:", "error opening file " CSSFILE "!", NULL, NULL);
      exit(1);
    }

    // Get the file length
    fseek(cssFile, 0L, SEEK_END);
    fileLg = ftell(cssFile);
    fseek(cssFile, 0L, SEEK_SET);

    cssData = malloc(2*fileLg); // To be sure there is enough space
    if(cssData == NULL) {
      moreInfoOnError("In function prepareCssData:", "error allocating 10000 bytes for CSS data", NULL, NULL);
      exit(1);
    }

    ignore_result(fread(cssData, 1, fileLg, cssFile));
    fclose(cssFile);
    cssData[fileLg] = 0;

    toReplace = strstr(cssData, "/* Replace $");
    while(toReplace != NULL) {
      int i = -1;
      toReplace += 11;
      while(toReplace[++i] != ' ') {
        needle[i] = toReplace[i];
      }
      needle[i] = 0;

      *toReplace = ' ';

      replaceWith = strstr(toReplace, " with ");
      if(replaceWith == NULL) {
        moreInfoOnError("In function prepareCssData:", "Can't find \" with \" after \"/* Replace $\" in CSS file " CSSFILE, NULL, NULL);
        exit(1);
      }

      replaceWith[1] = ' ';
      replaceWith += 6;
      i = -1;
      while(replaceWith[++i] != ' ') {
        newNeedle[i] = replaceWith[i];
      }
      newNeedle[i] = 0;

      strReplace(toReplace, needle, newNeedle);

      toReplace = strstr(cssData, "/* Replace $");
    }

    if(strstr(cssData, "$") != NULL) {
      moreInfoOnError("In function prepareCssData:", "There is still an unreplaced $ in the CSS file!\nPlease check file " CSSFILE, NULL, NULL);
      printf("%s\n", cssData);
      exit(1);
    }
  }
#endif // (SCREEN_800X480 == 0)



static gboolean keyPressed(GtkWidget *w, GdkEventKey *event, gpointer data) {
  //printf("%d\n", event->keyval);
  switch(event->keyval) {
    case 65470: // F1
      //printf("key pressed: F1\n");
      btnFnClicked("1");
      break;

    case 65471: // F2
      //printf("key pressed: F2\n");
      btnFnClicked("2");
      break;

    case 65472: // F3
      //printf("key pressed: F3\n");
      btnFnClicked("3");
      break;

    case 65473: // F4
      //printf("key pressed: F4\n");
      btnFnClicked("4");
      break;

    case 65474: // F5
      //printf("key pressed: F5\n");
      btnFnClicked("5");
      break;

    case 65475: // F6
      //printf("key pressed: F6\n");
      btnFnClicked("6");
      break;

    case 73:  // I
    case 105: // i
      //printf("key pressed: i 1/x\n");
      btnClicked("00");
      break;

    case 76:  // L
      //printf("key pressed: L EXP\n");
      btnClicked("01");
      break;

    case 84:  // T
    case 116: // t
      //printf("key pressed: T TRI\n");
      btnClicked("02");
      break;

    case 108: // l
      //printf("key pressed: l ln\n");
      btnClicked("03");
      break;

    case 101: // e
      //printf("key pressed: e e^x\n");
      btnClicked("04");
      break;

    case 81:  // Q
    case 113: // q
      //printf("key pressed: Q Quadrad\n");
      btnClicked("05");
      break;

    case 83:  // S
    case 115: // s
      //printf("key pressed: s STO\n");
      btnClicked("06");
      break;

    case 82:  // R
    case 114: // r
      //printf("key pressed: r RCL\n");
      btnClicked("07");
      break;

    case 65366: // PgDn
      //printf("key pressed: PgDn Roll down\n");
      btnClicked("08");
      break;

    case 67: // C
      //printf("key pressed: C CC\n");
      btnClicked("09");
      break;

    case 70:  // F
    case 102: // f
      //printf("key pressed: f\n");
      btnClicked("10");
      break;

    case 71:  // G
    case 103: // g
      //printf("key pressed: g\n");
      btnClicked("11");
      break;

    case 65293: // Enter
    case 65421: // Enter numeric pad
      //printf("key pressed: ENTER\n");
      btnClicked("12");
      break;

    case 65289: // Tab
      //printf("key pressed: Tab x<>y\n");
      btnClicked("13");
      break;

    case 99: // c
      //printf("key pressed: c CHS +/-\n");
      btnClicked("14");
      break;

    case 69: // E
      //printf("key pressed: E\n");
      btnClicked("15");
      break;

    case 65288: // Backspace
      //printf("key pressed: Backspace\n");
      btnClicked("16");
      break;

    case 47:    // /
    case 65455: // /
      //printf("key pressed: /\n");
      btnClicked("17");
      break;

    case 55:    // 7
    case 65463: // 7
      //printf("key pressed: 7\n");
      btnClicked("18");
      break;

    case 56:    // 8
    case 65464: // 8
      //printf("key pressed: 8\n");
      btnClicked("19");
      break;

    case 57:    // 9
    case 65465: // 9
      //printf("key pressed: 9\n");
      btnClicked("20");
      break;

    case 88:  // X
      //printf("key pressed: XEQ\n");
      btnClicked("21");
      break;

    case 42:    // *
    case 65450: // *
      //printf("key pressed: *\n");
      btnClicked("22");
      break;

    case 52:    // 4
    case 65460: // 4
      //printf("key pressed: 4\n");
      btnClicked("23");
      break;

    case 53:    // 5
    case 65461: // 5
      //printf("key pressed: 5\n");
      btnClicked("24");
      break;

    case 54:    // 6
    case 65462: // 6
      //printf("key pressed: 6\n");
      btnClicked("25");
      break;

    case 65362: // up arrow
      //printf("key pressed: up\n");
      btnClicked("26");
      break;

    case 45:    // -
    case 65453: // -
      //printf("key pressed: -\n");
      btnClicked("27");
      break;

    case 49:    // 1
    case 65457: // 1
      //printf("key pressed: 1\n");
      btnClicked("28");
      break;

    case 50:    // 2
    case 65458: // 2
      //printf("key pressed: 2\n");
      btnClicked("29");
      break;

    case 51:    // 3
    case 65459: // 3
      //printf("key pressed: 3\n");
      btnClicked("30");
      break;

    case 65364: // down arrow
      //printf("key pressed: down\n");
      btnClicked("31");
      break;

    case 43:    // +
    case 65451: // +
      //printf("key pressed: +\n");
      btnClicked("32");
      break;

    case 48:    // 0
    case 65456: // 0
      //printf("key pressed: 0\n");
      btnClicked("33");
      break;

    case 44:    // ,
    case 46:    // .
    case 65454: // .
      //printf("key pressed: .\n");
      btnClicked("34");
      break;

    case 65507: // left Ctrl
    case 65508: // right Ctrl
      //printf("key pressed: Ctrl R/S\n");
      btnClicked("35");
      break;

    case 65307: // Esc
      //printf("key pressed: EXIT\n");
      btnClicked("36");
      break;

    case 72:  // H
    case 104: // h
      //printf("key pressed: h Hardcopy to clipboard\n");
      copyScreenToClipboard();
      break;

    case 120: // x
      //printf("key pressed: x copy register X to clipboard\n");
      copyRegisterXToClipboard();
      break;

    case 122: // z
      //printf("key pressed: z copy stack registers to clipboard\n");
      copyStackRegistersToClipboard();
      break;

    case 90: // Z
      //printf("key pressed: Z copy all registers to clipboard\n");
      copyAllRegistersToClipboard();
      break;

    default: {
      break;
    }
  }
  return FALSE;
}



void setupUI(void) {
  #if (SCREEN_800X480 == 0)
    GError         *error;
    GtkCssProvider *cssProvider;
    GdkDisplay     *cssDisplay;
    GdkScreen      *cssScreen;

    // Get the monitor geometry to determine whether the calc is portrait or landscape
    GdkRectangle monitor;
    gdk_monitor_get_geometry(gdk_display_get_monitor(gdk_display_get_default(), 0), &monitor);

    if(calcAutoLandscapePortrait) {
      calcLandscape = (monitor.height < 1025);
    }

    prepareSkin();
    prepareCssData();

    cssProvider = gtk_css_provider_new();
    cssDisplay  = gdk_display_get_default();
    cssScreen   = gdk_display_get_default_screen(cssDisplay);
    gtk_style_context_add_provider_for_screen(cssScreen, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    error = NULL;
    gtk_css_provider_load_from_data(cssProvider, cssData, -1, &error);
    if(error != NULL) {
      moreInfoOnError("In function setupUI:", "error while loading CSS style sheet " CSSFILE, NULL, NULL);
      exit(1);
    }
    g_object_unref(cssProvider);
    free(cssData);

    // The main window
    frmCalc = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if(calcLandscape) {
      #if (DEBUG_PANEL == 1)
        gtk_window_set_default_size(GTK_WINDOW(frmCalc), backgroundWidth, backgroundHeight + 971);
        debugWidgetDx = 0;
        debugWidgetDy = backgroundHeight + 1;
      #else // (DEBUG_PANEL == 0)
        gtk_window_set_default_size(GTK_WINDOW(frmCalc), backgroundWidth,  backgroundHeight);
      #endif // (DEBUG_PANEL == 1)
    }
    else { // Portrait
      #if (DEBUG_PANEL == 1)
        gtk_window_set_default_size(GTK_WINDOW(frmCalc), backgroundWidth + 1006,  backgroundHeight);
        debugWidgetDx = backgroundWidth + 7;
        debugWidgetDy = 0;
      #else // (DEBUG_PANEL == 0)
        gtk_window_set_default_size(GTK_WINDOW(frmCalc),  backgroundWidth,  backgroundHeight);
      #endif // (DEBUG_PANEL == 1)
    }

    gtk_widget_set_name(frmCalc, "mainWindow");
    gtk_window_set_resizable (GTK_WINDOW(frmCalc), FALSE);
    gtk_window_set_title(GTK_WINDOW(frmCalc), "WP43");
    g_signal_connect(frmCalc, "destroy", G_CALLBACK(destroyCalc), NULL);
    g_signal_connect(frmCalc, "key_press_event", G_CALLBACK(keyPressed), NULL);
    #if defined(RASPBERRY)
      gtk_window_set_decorated(GTK_WINDOW(frmCalc), FALSE);
      gtk_window_set_position(GTK_WINDOW(frmCalc), GTK_WIN_POS_CENTER);
    #endif // RASPBERRY

    gtk_widget_add_events(GTK_WIDGET(frmCalc), GDK_CONFIGURE);

    // Fixed grid to freely put widgets on it
    grid = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(frmCalc), grid);
    g_signal_connect(frmCalc, "button-press-event",   G_CALLBACK(frmCalcMouseButtonPressed),  NULL);
    g_signal_connect(frmCalc, "button-release-event", G_CALLBACK(frmCalcMouseButtonReleased), NULL);
    gtk_fixed_put(GTK_FIXED(grid), backgroundImage, 0, 0);

    // Areas for the f and g shifted softkeys
    gtk_fixed_put(GTK_FIXED(grid), fgShiftedArea1, fgShiftedArea1X, fgShiftedArea1Y);
    gtk_fixed_put(GTK_FIXED(grid), fgShiftedArea2, fgShiftedArea2X, fgShiftedArea2Y);

    // Behind screen
    gtk_fixed_put(GTK_FIXED(grid), behindScreenImage, behindScreenX, behindScreenY);

    // LCD screen 400x240
    screen = gtk_drawing_area_new();
    gtk_widget_set_size_request(screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    gtk_widget_set_tooltip_text(GTK_WIDGET(screen), "Copy to clipboard:\nh -> screen image\nx -> register X\nz -> lettered registers\nZ -> all registers");
    gtk_fixed_put(GTK_FIXED(grid), screen, lcdx, lcdy);
    screenStride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, SCREEN_WIDTH)/4;
    int numBytes = screenStride * SCREEN_HEIGHT * 4;
    screenData = malloc(numBytes);
    if(screenData == NULL) {
      sprintf(errorMessage, "error allocating %d x %d = %d bytes for screenData", screenStride * 4, SCREEN_HEIGHT, numBytes);
      moreInfoOnError("In function setupUI:", errorMessage, NULL, NULL);
      exit(1);
    }

    g_signal_connect(screen, "draw", G_CALLBACK(drawScreen), NULL);

    for(int bezel=0; bezel<3; bezel++) {
      gtk_fixed_put(GTK_FIXED(grid), bezelImage[bezel], -999, -999);
    }
    for(int key=0; key<43; key++) {
      for(int bezel=0; bezel<3; bezel++) {
        gtk_fixed_put(GTK_FIXED(grid), calcKeyboard[key].keyImage[bezel], -999, -999);
      }
    }
    gtk_fixed_put(GTK_FIXED(grid), calcKeyboard[10].keyImage[3], -999, -999);

    #if (DEBUG_REGISTER_L == 1)
      lblRegisterL1 = gtk_label_new("");
      lblRegisterL2 = gtk_label_new("");
      gtk_widget_set_name(lblRegisterL1, "registerL");
      gtk_widget_set_name(lblRegisterL2, "registerL");
      gtk_fixed_put(GTK_FIXED(grid), lblRegisterL1, 5, 31);
      gtk_fixed_put(GTK_FIXED(grid), lblRegisterL2, 5, 49);
    #endif // (DEBUG_REGISTER_L == 1)

    #if (SHOW_MEMORY_STATUS == 1)
      lblMemoryStatus = gtk_label_new("");
      gtk_widget_set_name(lblMemoryStatus, "memoryStatus");
      gtk_fixed_put(GTK_FIXED(grid), lblMemoryStatus, 5, 1);
    #endif // (SHOW_MEMORY_STATUS == 1)


    for(int bezel=0; bezel<3; bezel++) {
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[ 6].keyImage[bezel]), "i");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[ 7].keyImage[bezel]), "L");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[ 8].keyImage[bezel]), "t");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[ 9].keyImage[bezel]), "l");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[10].keyImage[bezel]), "e");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[11].keyImage[bezel]), "q");

      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[12].keyImage[bezel]), "s");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[13].keyImage[bezel]), "r");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[14].keyImage[bezel]), "Page Down");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[15].keyImage[bezel]), "upper C");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[16].keyImage[bezel]), "f");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[17].keyImage[bezel]), "g");

      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[18].keyImage[bezel]), "Enter");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[19].keyImage[bezel]), "Tab");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[20].keyImage[bezel]), "c");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[21].keyImage[bezel]), "E");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[22].keyImage[bezel]), "Backspace");

      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[23].keyImage[bezel]), "/");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[24].keyImage[bezel]), "7");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[25].keyImage[bezel]), "8");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[26].keyImage[bezel]), "9");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[27].keyImage[bezel]), "upper X");

      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[28].keyImage[bezel]), "*");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[29].keyImage[bezel]), "4");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[30].keyImage[bezel]), "5");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[31].keyImage[bezel]), "6");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[32].keyImage[bezel]), "Up");

      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[33].keyImage[bezel]), "-");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[34].keyImage[bezel]), "1");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[35].keyImage[bezel]), "2");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[36].keyImage[bezel]), "3");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[37].keyImage[bezel]), "Down");

      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[38].keyImage[bezel]), "+");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[39].keyImage[bezel]), "0");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[40].keyImage[bezel]), ". ,");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[41].keyImage[bezel]), "Ctrl");
      gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[42].keyImage[bezel]), "Esc");
    }
    gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[10].keyImage[3]), "e");

    // The debug window
    #if (DEBUG_PANEL == 1)
      for(int i=0; i<DEBUG_LINES; i++) {
        lbl1[i] = gtk_label_new("");
        gtk_widget_set_name(lbl1[i], "debugDejaVu");
        gtk_fixed_put(GTK_FIXED(grid), lbl1[i], 1 + debugWidgetDx, 26 + i*14 + debugWidgetDy);
        lbl2[i] = gtk_label_new("");
        gtk_widget_set_name(lbl2[i], "debugWP43");
        gtk_fixed_put(GTK_FIXED(grid), lbl2[i], 270 + debugWidgetDx, 25 + i*14 + debugWidgetDy);
      }

      btnBitFields           = gtk_button_new_with_label("Bitfields");
      btnFlags               = gtk_button_new_with_label("Flags");
      btnRegisters           = gtk_button_new_with_label("Registers");
      btnLocalRegisters      = gtk_button_new_with_label("Local registers");
      btnStatisticalSums     = gtk_button_new_with_label("Statistical sums");
      btnNamedVariables      = gtk_button_new_with_label("Named variables");
      btnSavedStackRegisters = gtk_button_new_with_label("Saved stack registers");
      chkHexaString          = gtk_check_button_new_with_label("Strings in hexadecimal form");

      gtk_widget_set_name(btnBitFields,           "debugButton");
      gtk_widget_set_name(btnFlags,               "debugButton");
      gtk_widget_set_name(btnRegisters,           "debugButton");
      gtk_widget_set_name(btnLocalRegisters,      "debugButton");
      gtk_widget_set_name(btnStatisticalSums,     "debugButton");
      gtk_widget_set_name(btnNamedVariables,      "debugButton");
      gtk_widget_set_name(btnSavedStackRegisters, "debugButton");
      gtk_widget_set_name(chkHexaString,          "debugCheckbox");

      g_signal_connect(btnBitFields,           "clicked", G_CALLBACK(btnBitFieldsClicked),           NULL);
      g_signal_connect(btnFlags,               "clicked", G_CALLBACK(btnFlagsClicked),               NULL);
      g_signal_connect(btnRegisters,           "clicked", G_CALLBACK(btnRegistersClicked),           NULL);
      g_signal_connect(btnLocalRegisters,      "clicked", G_CALLBACK(btnLocalRegistersClicked),      NULL);
      g_signal_connect(btnStatisticalSums,     "clicked", G_CALLBACK(btnStatisticalSumsClicked),     NULL);
      g_signal_connect(btnNamedVariables,      "clicked", G_CALLBACK(btnNamedVariablesClicked),      NULL);
      g_signal_connect(btnSavedStackRegisters, "clicked", G_CALLBACK(btnSavedStackRegistersClicked), NULL);
      g_signal_connect(chkHexaString,          "clicked", G_CALLBACK(chkHexaStringClicked),          NULL);

      gtk_fixed_put(GTK_FIXED(grid), btnBitFields,             1 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), btnFlags,                60 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), btnRegisters,           101 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), btnLocalRegisters,      166 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), btnStatisticalSums,     260 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), btnNamedVariables,      360 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), btnSavedStackRegisters, 465 + debugWidgetDx, 1 + debugWidgetDy);
      gtk_fixed_put(GTK_FIXED(grid), chkHexaString,          630 + debugWidgetDx, 1 + debugWidgetDy);

      gtk_widget_show(btnBitFields);
      gtk_widget_show(btnFlags);
      gtk_widget_show(btnRegisters);
      gtk_widget_show(btnLocalRegisters);
      gtk_widget_show(btnStatisticalSums);
      gtk_widget_show(btnNamedVariables);
      gtk_widget_show(btnSavedStackRegisters);
      gtk_widget_show(chkHexaString);

      debugWindow = DBG_REGISTERS;
    #endif // DEBUG_PANEL == 0

    gtk_widget_show_all(frmCalc);

  #else // SCREEN_800X480 == 1
    // The main window
    frmCalc = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(frmCalc), 800, 480);
    gtk_window_set_decorated(GTK_WINDOW(frmCalc), FALSE);
    gtk_window_set_position(GTK_WINDOW(frmCalc), GTK_WIN_POS_CENTER);

    gtk_widget_set_name(frmCalc, "mainWindow");
    gtk_window_set_resizable(GTK_WINDOW(frmCalc), FALSE);
    g_signal_connect(frmCalc, "destroy", G_CALLBACK(destroyCalc), NULL);
    g_signal_connect(frmCalc, "key_press_event", G_CALLBACK(keyPressed), NULL);

    gtk_widget_add_events(GTK_WIDGET(frmCalc), GDK_CONFIGURE);

    // Fixed grid to freely put widgets on it
    grid = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(frmCalc), grid);

    // LCD screen 800x480
    screen = gtk_drawing_area_new();
    gtk_widget_set_size_request(screen, SCREEN_WIDTH*2, SCREEN_HEIGHT*2);
    gtk_fixed_put(GTK_FIXED(grid), screen, 0, 0);
    screenStride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, SCREEN_WIDTH)/4;
    int numBytes = screenStride * SCREEN_HEIGHT * 4;
    screenData = malloc(numBytes);
    if(screenData == NULL) {
      sprintf(errorMessage, "error allocating %d x %d = %d bytes for screenData", screenStride * 4, SCREEN_HEIGHT, numBytes);
      moreInfoOnError("In function setupUI:", errorMessage, NULL, NULL);
      exit(1);
    }

    g_signal_connect(screen, "draw", G_CALLBACK(drawScreen), NULL);

    gtk_widget_show_all(frmCalc);
  #endif //  (SCREEN_800X480 == 0)
}
