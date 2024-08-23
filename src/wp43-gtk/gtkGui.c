// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "gtkGui.h"

#include "apps/timerApp.h"
#include "calcMode.h"
#include "charString.h"
#include "debug.h"
#include "defines.h"
#include "error.h"
#include "flags.h"
#include "gtkDebug.h"
#include "gtkScreen.h"
#include "hal/lcd.h"
#include "items.h"
#include "programming/manage.h"
#include "saveRestoreCalcState.h"
#include "ui/bufferize.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include "ui/statusBar.h"
#include "ui/tam.h"
#include "wp43-gtk.h"
#include <stdbool.h>

#include "wp43.h"

GtkWidget      *grid;
calcKeyboard_t  calcKeyboard[MAX_KEYS];
guiLayout_t     currentBezel;
keyCode_t key;
#if (SCREEN_800X480 == 0)
  GtkWidget *backgroundImage, *bezelImage[MAX_GUI_LAYOUTS], *behindScreenImage, *fgShiftedArea1, *fgShiftedArea2;
  GtkWidget *lblFSoftkeyArea, *lblGSoftkeyArea;
  int backgroundWidth, backgroundHeight;
  int lcdx, lcdy;
  int bezelX[MAX_GUI_LAYOUTS], bezelY[MAX_GUI_LAYOUTS];
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

  static const char *_layoutNames[] = {
    "Normal",
    "AIM",
    "TAM",
    "RBR",
    "FBR",
    "Timer"
  };
#endif // (SCREEN_800X480 == 0)



static gint destroyCalc(GtkWidget* w, GdkEventAny* e, gpointer data) {
  timerAppStop();
  saveCalc();
  gtk_main_quit();

  return 0;
}



bool guiUseTamL(void) {
  return (tam.mode == tmLabel || (tam.mode == tmSolve && (tam.function != ITM_SOLVE || calcMode != cmPem)) || tamIsWaitingKey());
}



static void convertXYToKey(int x, int y) {
  int xMin, xMax, yMin, yMax;
  key = kcNoKey;

  for(int i=0; i<MAX_KEYS; i++) {
    xMin = calcKeyboard[i].x;
    yMin = calcKeyboard[i].y;
    if(i == 10 && currentBezel == glTam && guiUseTamL()) {
      xMax = xMin + calcKeyboard[10].width[TAM_L_LAYOUT];
      yMax = yMin + calcKeyboard[10].height[TAM_L_LAYOUT];
    }
    else {
      xMax = xMin + calcKeyboard[i].width[currentBezel];
      yMax = yMin + calcKeyboard[i].height[currentBezel];
    }

    if(   xMin <= x && x <= xMax
       && yMin <= y && y <= yMax) {
      if(i < 6) { // Function key
        key = kcF1 + i;
      }
      else {
        key = i - 5;
      }
      break;
    }
  }
}



void frmCalcMouseButtonPressed(GtkWidget *notUsed, GdkEvent *event, gpointer data) {
  if(key == kcNoKey) { // The previous click must be released
    convertXYToKey((int)event->button.x, (int)event->button.y);
    if(key == kcNoKey) {
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

    btnPressed(key);
    lcd_refresh();
  }
}



void frmCalcMouseButtonReleased(GtkWidget *notUsed, GdkEvent *event, gpointer data) {
  if(key != kcNoKey) {
    keyCode_t newKey = key;
    key = kcNoKey;
    btnReleased(newKey);
    lcd_refresh();
  }
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
    char textLine[1000], skinDirectory[222]="", fileName[421], parameter[100], value[200];

    skin = fopen(BASEPATH "res/artwork/skin.cfg", "rb");
    if(skin == NULL) {
      errorMoreInfo("error opening file " BASEPATH "res/artwork/skin.cfg!");
      exit(1);
    }

    fgets(textLine, 1000, skin);
    while(!feof(skin) && skinDirectory[0] == 0) {
      getParameter(textLine, parameter, value);

      if(!strcmp(parameter, "skinDirectory") && value[0] != 0) {
        sprintf(skinDirectory, BASEPATH "res/artwork/%s/", value);
      }

      fgets(textLine, 1000, skin);
    }

    if(skinDirectory[0] == 0) {
      errorMoreInfo("cannot find skinDirectory in file res/artwork/skin.cfg!");
      exit(1);
    }

    fclose(skin);

    sprintf(fileName, "%s%s", skinDirectory, calcLandscape ? "landscapeSkin.cfg" : "portraitSkin.cfg");
    skin = fopen(fileName, "rb");
    if(skin == NULL) {
      errorMoreInfo("error opening file '%s'", fileName);
      exit(1);
    }

    fgets(textLine, 1000, skin);
    while(!feof(skin)) {
      getParameter(textLine, parameter, value);

      if(parameter[0] != 0 && value[0] != 0) {
        if(!strcmp(parameter, "backgroundImage")) {
          sprintf(fileName, "%s%s", skinDirectory, value);
          if(access(fileName, F_OK) != 0) {
            errorMoreInfo("error: cannot access file '%s'", fileName);
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
            errorMoreInfo("error: cannot access file '%s'", fileName);
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
            errorMoreInfo("error: cannot access file '%s'", fileName);
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

        if(!strncmp(parameter, "bezel", 5)) {
          bool found = false;
          for(guiLayout_t layout = 0; layout < MAX_GUI_LAYOUTS; layout++) {
            if(!strcmp(parameter + 6, _layoutNames[layout])) {
              if(parameter[5] == 'i') {
                sprintf(fileName, "%s%s", skinDirectory, value);
                if(access(fileName, F_OK) != 0) {
                  errorMoreInfo("error: cannot access file '%s'", fileName);
                  exit(1);
                }
                bezelImage[layout] = gtk_image_new_from_file(fileName);
                found = true;
                break;
              }

              if(parameter[5] == 'x') {
                bezelX[layout] = atoi(value);
                found = true;
                break;
              }

              if(parameter[5] == 'y') {
                bezelY[layout] = atoi(value);
                found = true;
                break;
              }
            }
          }
          if(found) {
            continue;
          }
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

          bool found = false;
          for(guiLayout_t layout = 0; layout < MAX_GUI_LAYOUTS; layout++) {
            if(!strcmp(parameter + 5, _layoutNames[layout])) {
              sprintf(fileName, "%s%s", skinDirectory, value);
              if(access(fileName, F_OK) != 0) {
                errorMoreInfo("error: cannot access file '%s'", fileName);
                exit(1);
              }
              calcKeyboard[calcKey].keyImage[layout] = gtk_image_new_from_file(fileName);
              const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(calcKeyboard[calcKey].keyImage[layout]));
              calcKeyboard[calcKey].width[layout] = gdk_pixbuf_get_width(pb);
              calcKeyboard[calcKey].height[layout] = gdk_pixbuf_get_height(pb);
              found = true;
              break;
            }
          }
          if(found) {
            continue;
          }

          if(!strcmp(parameter + 5, "TAM_L")) {
            sprintf(fileName, "%s%s", skinDirectory, value);
            if(access(fileName, F_OK) != 0) {
              errorMoreInfo("error: cannot access file '%s'", fileName);
              exit(1);
            }
            calcKeyboard[calcKey].keyImage[TAM_L_LAYOUT] = gtk_image_new_from_file(fileName);
            const GdkPixbuf *pb = gtk_image_get_pixbuf(GTK_IMAGE(calcKeyboard[calcKey].keyImage[TAM_L_LAYOUT]));
            calcKeyboard[calcKey].width[TAM_L_LAYOUT] = gdk_pixbuf_get_width(pb);
            calcKeyboard[calcKey].height[TAM_L_LAYOUT] = gdk_pixbuf_get_height(pb);
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
      errorMoreInfo("error opening file " CSSFILE "!");
      exit(1);
    }

    // Get the file length
    fseek(cssFile, 0L, SEEK_END);
    fileLg = ftell(cssFile);
    fseek(cssFile, 0L, SEEK_SET);

    cssData = malloc(2*fileLg); // To be sure there is enough space
    if(cssData == NULL) {
      errorMoreInfo("error allocating 10000 bytes for CSS data");
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
        errorMoreInfo("Can't find \" with \" after \"/* Replace $\" in CSS file " CSSFILE);
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
      errorMoreInfo("There is still an unreplaced $ in the CSS file!\nPlease check file " CSSFILE);
      printf("%s\n", cssData);
      exit(1);
    }
  }
#endif // (SCREEN_800X480 == 0)

const gdkKeyMap_t gdkKeyMap[] = {
//  #define ITM_INDIRECTION                539
  { .item = ITM_0                          ,  .gdkKey = GDK_KEY_0                           },
  { .item = ITM_0                          ,  .gdkKey = GDK_KEY_KP_0                        },
  { .item = ITM_1                          ,  .gdkKey = GDK_KEY_1                           },
  { .item = ITM_1                          ,  .gdkKey = GDK_KEY_KP_1                        },
  { .item = ITM_2                          ,  .gdkKey = GDK_KEY_2                           },
  { .item = ITM_2                          ,  .gdkKey = GDK_KEY_KP_2                        },
  { .item = ITM_3                          ,  .gdkKey = GDK_KEY_3                           },
  { .item = ITM_3                          ,  .gdkKey = GDK_KEY_KP_3                        },
  { .item = ITM_4                          ,  .gdkKey = GDK_KEY_4                           },
  { .item = ITM_4                          ,  .gdkKey = GDK_KEY_KP_4                        },
  { .item = ITM_5                          ,  .gdkKey = GDK_KEY_5                           },
  { .item = ITM_5                          ,  .gdkKey = GDK_KEY_KP_5                        },
  { .item = ITM_6                          ,  .gdkKey = GDK_KEY_6                           },
  { .item = ITM_6                          ,  .gdkKey = GDK_KEY_KP_6                        },
  { .item = ITM_7                          ,  .gdkKey = GDK_KEY_7                           },
  { .item = ITM_7                          ,  .gdkKey = GDK_KEY_KP_7                        },
  { .item = ITM_8                          ,  .gdkKey = GDK_KEY_8                           },
  { .item = ITM_8                          ,  .gdkKey = GDK_KEY_KP_8                        },
  { .item = ITM_9                          ,  .gdkKey = GDK_KEY_9                           },
  { .item = ITM_9                          ,  .gdkKey = GDK_KEY_KP_9                        },
  { .item = ITM_A                          ,  .gdkKey = GDK_KEY_A                           },
  { .item = ITM_B                          ,  .gdkKey = GDK_KEY_B                           },
  { .item = ITM_C                          ,  .gdkKey = GDK_KEY_C                           },
  { .item = ITM_D                          ,  .gdkKey = GDK_KEY_D                           },
  { .item = ITM_E                          ,  .gdkKey = GDK_KEY_E                           },
  { .item = ITM_F                          ,  .gdkKey = GDK_KEY_F                           },
  { .item = ITM_G                          ,  .gdkKey = GDK_KEY_G                           },
  { .item = ITM_H                          ,  .gdkKey = GDK_KEY_H                           },
  { .item = ITM_I                          ,  .gdkKey = GDK_KEY_I                           },
  { .item = ITM_J                          ,  .gdkKey = GDK_KEY_J                           },
  { .item = ITM_K                          ,  .gdkKey = GDK_KEY_K                           },
  { .item = ITM_L                          ,  .gdkKey = GDK_KEY_L                           },
  { .item = ITM_M                          ,  .gdkKey = GDK_KEY_M                           },
  { .item = ITM_N                          ,  .gdkKey = GDK_KEY_N                           },
  { .item = ITM_O                          ,  .gdkKey = GDK_KEY_O                           },
  { .item = ITM_P                          ,  .gdkKey = GDK_KEY_P                           },
  { .item = ITM_Q                          ,  .gdkKey = GDK_KEY_Q                           },
  { .item = ITM_R                          ,  .gdkKey = GDK_KEY_R                           },
  { .item = ITM_S                          ,  .gdkKey = GDK_KEY_S                           },
  { .item = ITM_T                          ,  .gdkKey = GDK_KEY_T                           },
  { .item = ITM_U                          ,  .gdkKey = GDK_KEY_U                           },
  { .item = ITM_V                          ,  .gdkKey = GDK_KEY_V                           },
  { .item = ITM_W                          ,  .gdkKey = GDK_KEY_W                           },
  { .item = ITM_X                          ,  .gdkKey = GDK_KEY_X                           },
  { .item = ITM_Y                          ,  .gdkKey = GDK_KEY_Y                           },
  { .item = ITM_Z                          ,  .gdkKey = GDK_KEY_Z                           },
  { .item = ITM_a                          ,  .gdkKey = GDK_KEY_a                           },
  { .item = ITM_b                          ,  .gdkKey = GDK_KEY_b                           },
  { .item = ITM_c                          ,  .gdkKey = GDK_KEY_c                           },
  { .item = ITM_d                          ,  .gdkKey = GDK_KEY_d                           },
  { .item = ITM_e                          ,  .gdkKey = GDK_KEY_e                           },
  { .item = ITM_f                          ,  .gdkKey = GDK_KEY_f                           },
  { .item = ITM_g                          ,  .gdkKey = GDK_KEY_g                           },
  { .item = ITM_h                          ,  .gdkKey = GDK_KEY_h                           },
  { .item = ITM_i                          ,  .gdkKey = GDK_KEY_i                           },
  { .item = ITM_j                          ,  .gdkKey = GDK_KEY_j                           },
  { .item = ITM_k                          ,  .gdkKey = GDK_KEY_k                           },
  { .item = ITM_l                          ,  .gdkKey = GDK_KEY_l                           },
  { .item = ITM_m                          ,  .gdkKey = GDK_KEY_m                           },
  { .item = ITM_n                          ,  .gdkKey = GDK_KEY_n                           },
  { .item = ITM_o                          ,  .gdkKey = GDK_KEY_o                           },
  { .item = ITM_p                          ,  .gdkKey = GDK_KEY_p                           },
  { .item = ITM_q                          ,  .gdkKey = GDK_KEY_q                           },
  { .item = ITM_r                          ,  .gdkKey = GDK_KEY_r                           },
  { .item = ITM_s                          ,  .gdkKey = GDK_KEY_s                           },
  { .item = ITM_t                          ,  .gdkKey = GDK_KEY_t                           },
  { .item = ITM_u                          ,  .gdkKey = GDK_KEY_u                           },
  { .item = ITM_v                          ,  .gdkKey = GDK_KEY_v                           },
  { .item = ITM_w                          ,  .gdkKey = GDK_KEY_w                           },
  { .item = ITM_x                          ,  .gdkKey = GDK_KEY_x                           },
  { .item = ITM_y                          ,  .gdkKey = GDK_KEY_y                           },
  { .item = ITM_z                          ,  .gdkKey = GDK_KEY_z                           },
  { .item = ITM_ALPHA                      ,  .gdkKey = GDK_KEY_Greek_ALPHA                 },
  { .item = ITM_BETA                       ,  .gdkKey = GDK_KEY_Greek_BETA                  },
  { .item = ITM_GAMMA                      ,  .gdkKey = GDK_KEY_Greek_GAMMA                 },
  { .item = ITM_DELTA                      ,  .gdkKey = GDK_KEY_Greek_DELTA                 },
  { .item = ITM_EPSILON                    ,  .gdkKey = GDK_KEY_Greek_EPSILON               },
  { .item = ITM_ZETA                       ,  .gdkKey = GDK_KEY_Greek_ZETA                  },
  { .item = ITM_ETA                        ,  .gdkKey = GDK_KEY_Greek_ETA                   },
  { .item = ITM_THETA                      ,  .gdkKey = GDK_KEY_Greek_THETA                 },
  { .item = ITM_IOTA                       ,  .gdkKey = GDK_KEY_Greek_IOTA                  },
  { .item = ITM_IOTA_DIALYTIKA             ,  .gdkKey = GDK_KEY_Greek_IOTAdieresis          },
  { .item = ITM_KAPPA                      ,  .gdkKey = GDK_KEY_Greek_KAPPA                 },
  { .item = ITM_LAMBDA                     ,  .gdkKey = GDK_KEY_Greek_LAMBDA                },
  { .item = ITM_MU                         ,  .gdkKey = GDK_KEY_Greek_MU                    },
  { .item = ITM_NU                         ,  .gdkKey = GDK_KEY_Greek_NU                    },
  { .item = ITM_XI                         ,  .gdkKey = GDK_KEY_Greek_XI                    },
  { .item = ITM_OMICRON                    ,  .gdkKey = GDK_KEY_Greek_OMICRON               },
  { .item = ITM_PI                         ,  .gdkKey = GDK_KEY_Greek_PI                    },
  { .item = ITM_RHO                        ,  .gdkKey = GDK_KEY_Greek_RHO                   },
  { .item = ITM_SIGMA                      ,  .gdkKey = GDK_KEY_Greek_SIGMA                 },
  { .item = ITM_TAU                        ,  .gdkKey = GDK_KEY_Greek_TAU                   },
  { .item = ITM_UPSILON                    ,  .gdkKey = GDK_KEY_Greek_UPSILON               },
  { .item = ITM_UPSILON_DIALYTIKA          ,  .gdkKey = GDK_KEY_Greek_UPSILONdieresis       },
  { .item = ITM_PHI                        ,  .gdkKey = GDK_KEY_Greek_PHI                   },
  { .item = ITM_CHI                        ,  .gdkKey = GDK_KEY_Greek_CHI                   },
  { .item = ITM_PSI                        ,  .gdkKey = GDK_KEY_Greek_PSI                   },
  { .item = ITM_OMEGA                      ,  .gdkKey = GDK_KEY_Greek_OMEGA                 },
  { .item = ITM_alpha                      ,  .gdkKey = GDK_KEY_Greek_alpha                 },
  { .item = ITM_beta                       ,  .gdkKey = GDK_KEY_Greek_beta                  },
  { .item = ITM_gamma                      ,  .gdkKey = GDK_KEY_Greek_gamma                 },
  { .item = ITM_delta                      ,  .gdkKey = GDK_KEY_Greek_delta                 },
  { .item = ITM_epsilon                    ,  .gdkKey = GDK_KEY_Greek_epsilon               },
  { .item = ITM_zeta                       ,  .gdkKey = GDK_KEY_Greek_zeta                  },
  { .item = ITM_eta                        ,  .gdkKey = GDK_KEY_Greek_eta                   },
  { .item = ITM_theta                      ,  .gdkKey = GDK_KEY_Greek_theta                 },
  { .item = ITM_iota                       ,  .gdkKey = GDK_KEY_Greek_iota                  },
  { .item = ITM_iota_DIALYTIKA             ,  .gdkKey = GDK_KEY_Greek_iotadieresis          },
  { .item = ITM_kappa                      ,  .gdkKey = GDK_KEY_Greek_kappa                 },
  { .item = ITM_lambda                     ,  .gdkKey = GDK_KEY_Greek_lambda                },
  { .item = ITM_mu                         ,  .gdkKey = GDK_KEY_Greek_mu                    },
  { .item = ITM_nu                         ,  .gdkKey = GDK_KEY_Greek_nu                    },
  { .item = ITM_xi                         ,  .gdkKey = GDK_KEY_Greek_xi                    },
  { .item = ITM_omicron                    ,  .gdkKey = GDK_KEY_Greek_omicron               },
  { .item = ITM_pi                         ,  .gdkKey = GDK_KEY_Greek_pi                    },
  { .item = ITM_rho                        ,  .gdkKey = GDK_KEY_Greek_rho                   },
  { .item = ITM_sigma                      ,  .gdkKey = GDK_KEY_Greek_sigma                 },
  { .item = ITM_tau                        ,  .gdkKey = GDK_KEY_Greek_tau                   },
  { .item = ITM_upsilon                    ,  .gdkKey = GDK_KEY_Greek_upsilon               },
  { .item = ITM_upsilon_DIALYTIKA          ,  .gdkKey = GDK_KEY_Greek_upsilondieresis       },
  { .item = ITM_phi                        ,  .gdkKey = GDK_KEY_Greek_phi                   },
  { .item = ITM_chi                        ,  .gdkKey = GDK_KEY_Greek_chi                   },
  { .item = ITM_psi                        ,  .gdkKey = GDK_KEY_Greek_psi                   },
  { .item = ITM_omega                      ,  .gdkKey = GDK_KEY_Greek_omega                 },
  { .item = ITM_alpha_TONOS                ,  .gdkKey = GDK_KEY_Greek_alphaaccent           },
  { .item = ITM_epsilon_TONOS              ,  .gdkKey = GDK_KEY_Greek_epsilonaccent         },
  { .item = ITM_eta_TONOS                  ,  .gdkKey = GDK_KEY_Greek_etaaccent             },
  { .item = ITM_iotaTON                    ,  .gdkKey = GDK_KEY_Greek_iotaaccent            },
  { .item = ITM_iota_DIALYTIKA_TONOS       ,  .gdkKey = GDK_KEY_Greek_iotaaccentdieresis    },
  { .item = ITM_omicron_TONOS              ,  .gdkKey = GDK_KEY_Greek_omicronaccent         },
  { .item = ITM_sigma_end                  ,  .gdkKey = GDK_KEY_Greek_finalsmallsigma       },
  { .item = ITM_upsilon_TONOS              ,  .gdkKey = GDK_KEY_Greek_upsilonaccent         },
  { .item = ITM_upsilon_DIALYTIKA_TONOS    ,  .gdkKey = GDK_KEY_Greek_upsilonaccentdieresis },
  { .item = ITM_omega_TONOS                ,  .gdkKey = GDK_KEY_Greek_omegaaccent           },
  { .item = ITM_A_MACRON                   ,  .gdkKey = GDK_KEY_Amacron                     },
  { .item = ITM_A_ACUTE                    ,  .gdkKey = GDK_KEY_Aacute                      },
  { .item = ITM_A_BREVE                    ,  .gdkKey = GDK_KEY_Abreve                      },
  { .item = ITM_A_GRAVE                    ,  .gdkKey = GDK_KEY_Agrave                      },
  { .item = ITM_A_DIARESIS                 ,  .gdkKey = GDK_KEY_Adiaeresis                  },
  { .item = ITM_A_TILDE                    ,  .gdkKey = GDK_KEY_Atilde                      },
  { .item = ITM_A_CIRC                     ,  .gdkKey = GDK_KEY_Acircumflex                 },
  { .item = ITM_A_RING                     ,  .gdkKey = GDK_KEY_Aring                       },
  { .item = ITM_AE                         ,  .gdkKey = GDK_KEY_AE                          },
  { .item = ITM_A_OGONEK                   ,  .gdkKey = GDK_KEY_Aogonek                     },
  { .item = ITM_C_ACUTE                    ,  .gdkKey = GDK_KEY_Cacute                      },
  { .item = ITM_C_CARON                    ,  .gdkKey = GDK_KEY_Ccaron                      },
  { .item = ITM_C_CEDILLA                  ,  .gdkKey = GDK_KEY_Ccedilla                    },
  { .item = ITM_D_STROKE                   ,  .gdkKey = GDK_KEY_Dstroke                     },
  { .item = ITM_D_CARON                    ,  .gdkKey = GDK_KEY_Dcaron                      },
  { .item = ITM_E_MACRON                   ,  .gdkKey = GDK_KEY_Emacron                     },
  { .item = ITM_E_ACUTE                    ,  .gdkKey = GDK_KEY_Eacute                      },
//  #define ITM_E_BREVE                    681
  { .item = ITM_E_GRAVE                    ,  .gdkKey = GDK_KEY_Egrave                      },
  { .item = ITM_E_DIARESIS                 ,  .gdkKey = GDK_KEY_Ediaeresis                  },
  { .item = ITM_E_CIRC                     ,  .gdkKey = GDK_KEY_Ecircumflex                 },
  { .item = ITM_E_OGONEK                   ,  .gdkKey = GDK_KEY_Eogonek                     },
  { .item = ITM_G_BREVE                    ,  .gdkKey = GDK_KEY_Gbreve                      },
  { .item = ITM_I_MACRON                   ,  .gdkKey = GDK_KEY_Imacron                     },
  { .item = ITM_I_ACUTE                    ,  .gdkKey = GDK_KEY_Iacute                      },
  { .item = ITM_I_BREVE                    ,  .gdkKey = GDK_KEY_Ibreve                      },
  { .item = ITM_I_GRAVE                    ,  .gdkKey = GDK_KEY_Igrave                      },
  { .item = ITM_I_DIARESIS                 ,  .gdkKey = GDK_KEY_Idiaeresis                  },
  { .item = ITM_I_CIRC                     ,  .gdkKey = GDK_KEY_Icircumflex                 },
  { .item = ITM_I_OGONEK                   ,  .gdkKey = GDK_KEY_Iogonek                     },
//  #define ITM_I_DOT                      694
//  #define ITM_I_DOTLESS                  695
  { .item = ITM_L_STROKE                   ,  .gdkKey = GDK_KEY_Lstroke                     },
  { .item = ITM_L_ACUTE                    ,  .gdkKey = GDK_KEY_Lacute                      },
//  #define ITM_L_APOSTROPHE               698
  { .item = ITM_N_ACUTE                    ,  .gdkKey = GDK_KEY_Nacute                      },
  { .item = ITM_N_CARON                    ,  .gdkKey = GDK_KEY_Ncaron                      },
  { .item = ITM_N_TILDE                    ,  .gdkKey = GDK_KEY_Ntilde                      },
  { .item = ITM_O_MACRON                   ,  .gdkKey = GDK_KEY_Omacron                     },
  { .item = ITM_O_ACUTE                    ,  .gdkKey = GDK_KEY_Oacute                      },
//  #define ITM_O_BREVE                    704
  { .item = ITM_O_GRAVE                    ,  .gdkKey = GDK_KEY_Ograve                      },
  { .item = ITM_O_DIARESIS                 ,  .gdkKey = GDK_KEY_Odiaeresis                  },
  { .item = ITM_O_TILDE                    ,  .gdkKey = GDK_KEY_Otilde                      },
  { .item = ITM_O_CIRC                     ,  .gdkKey = GDK_KEY_Ocircumflex                 },
//  #define ITM_O_STROKE                   709
  { .item = ITM_OE                         ,  .gdkKey = GDK_KEY_OE                          },
  { .item = ITM_S_SHARP                    ,  .gdkKey = GDK_KEY_ssharp                      },
  { .item = ITM_S_ACUTE                    ,  .gdkKey = GDK_KEY_Sacute                      },
  { .item = ITM_S_CARON                    ,  .gdkKey = GDK_KEY_Scaron                      },
  { .item = ITM_S_CEDILLA                  ,  .gdkKey = GDK_KEY_Scedilla                    },
  { .item = ITM_T_CARON                    ,  .gdkKey = GDK_KEY_Tcaron                      },
  { .item = ITM_T_CEDILLA                  ,  .gdkKey = GDK_KEY_Tcedilla                    },
  { .item = ITM_U_MACRON                   ,  .gdkKey = GDK_KEY_Umacron                     },
  { .item = ITM_U_ACUTE                    ,  .gdkKey = GDK_KEY_Uacute                      },
  { .item = ITM_U_BREVE                    ,  .gdkKey = GDK_KEY_Ubreve                      },
  { .item = ITM_U_GRAVE                    ,  .gdkKey = GDK_KEY_Ugrave                      },
  { .item = ITM_U_DIARESIS                 ,  .gdkKey = GDK_KEY_Udiaeresis                  },
  { .item = ITM_U_TILDE                    ,  .gdkKey = GDK_KEY_Utilde                      },
  { .item = ITM_U_CIRC                     ,  .gdkKey = GDK_KEY_Ucircumflex                 },
  { .item = ITM_U_RING                     ,  .gdkKey = GDK_KEY_Uring                       },
  { .item = ITM_W_CIRC                     ,  .gdkKey = GDK_KEY_Wcircumflex                 },
  { .item = ITM_Y_CIRC                     ,  .gdkKey = GDK_KEY_Ycircumflex                 },
  { .item = ITM_Y_ACUTE                    ,  .gdkKey = GDK_KEY_Yacute                      },
  { .item = ITM_Y_DIARESIS                 ,  .gdkKey = GDK_KEY_Ydiaeresis                  },
  { .item = ITM_Z_ACUTE                    ,  .gdkKey = GDK_KEY_Zacute                      },
  { .item = ITM_Z_CARON                    ,  .gdkKey = GDK_KEY_Zcaron                      },
  { .item = ITM_Z_DOT                      ,  .gdkKey = GDK_KEY_Zabovedot                   },
  { .item = ITM_a_MACRON                   ,  .gdkKey = GDK_KEY_amacron                     },
  { .item = ITM_a_ACUTE                    ,  .gdkKey = GDK_KEY_aacute                      },
  { .item = ITM_a_BREVE                    ,  .gdkKey = GDK_KEY_abreve                      },
  { .item = ITM_a_GRAVE                    ,  .gdkKey = GDK_KEY_agrave                      },
  { .item = ITM_a_DIARESIS                 ,  .gdkKey = GDK_KEY_adiaeresis                  },
  { .item = ITM_a_TILDE                    ,  .gdkKey = GDK_KEY_atilde                      },
  { .item = ITM_a_CIRC                     ,  .gdkKey = GDK_KEY_acircumflex                 },
  { .item = ITM_a_RING                     ,  .gdkKey = GDK_KEY_aring                       },
  { .item = ITM_ae                         ,  .gdkKey = GDK_KEY_ae                          },
  { .item = ITM_a_OGONEK                   ,  .gdkKey = GDK_KEY_aogonek                     },
  { .item = ITM_c_ACUTE                    ,  .gdkKey = GDK_KEY_cacute                      },
  { .item = ITM_c_CARON                    ,  .gdkKey = GDK_KEY_ccaron                      },
  { .item = ITM_c_CEDILLA                  ,  .gdkKey = GDK_KEY_ccedilla                    },
  { .item = ITM_d_STROKE                   ,  .gdkKey = GDK_KEY_dstroke                     },
//  #define ITM_d_APOSTROPHE               746
  { .item = ITM_e_MACRON                   ,  .gdkKey = GDK_KEY_emacron                     },
  { .item = ITM_e_ACUTE                    ,  .gdkKey = GDK_KEY_eacute                      },
//  #define ITM_e_BREVE                    749
  { .item = ITM_e_GRAVE                    ,  .gdkKey = GDK_KEY_egrave                      },
  { .item = ITM_e_DIARESIS                 ,  .gdkKey = GDK_KEY_ediaeresis                  },
  { .item = ITM_e_CIRC                     ,  .gdkKey = GDK_KEY_ecircumflex                 },
  { .item = ITM_e_OGONEK                   ,  .gdkKey = GDK_KEY_eogonek                     },
  { .item = ITM_g_BREVE                    ,  .gdkKey = GDK_KEY_gbreve                      },
  { .item = ITM_h_STROKE                   ,  .gdkKey = GDK_KEY_hstroke                     },
  { .item = ITM_i_MACRON                   ,  .gdkKey = GDK_KEY_imacron                     },
  { .item = ITM_i_ACUTE                    ,  .gdkKey = GDK_KEY_iacute                      },
  { .item = ITM_i_BREVE                    ,  .gdkKey = GDK_KEY_ibreve                      },
  { .item = ITM_i_GRAVE                    ,  .gdkKey = GDK_KEY_igrave                      },
  { .item = ITM_i_DIARESIS                 ,  .gdkKey = GDK_KEY_idiaeresis                  },
  { .item = ITM_i_CIRC                     ,  .gdkKey = GDK_KEY_icircumflex                 },
  { .item = ITM_i_OGONEK                   ,  .gdkKey = GDK_KEY_iogonek                     },
//  #define ITM_i_DOT                      763
  { .item = ITM_i_DOTLESS                  ,  .gdkKey = GDK_KEY_idotless                    },
  { .item = ITM_l_STROKE                   ,  .gdkKey = GDK_KEY_lstroke                     },
  { .item = ITM_l_ACUTE                    ,  .gdkKey = GDK_KEY_lacute                      },
//  #define ITM_l_APOSTROPHE               767
  { .item = ITM_n_ACUTE                    ,  .gdkKey = GDK_KEY_nacute                      },
  { .item = ITM_n_CARON                    ,  .gdkKey = GDK_KEY_ncaron                      },
  { .item = ITM_n_TILDE                    ,  .gdkKey = GDK_KEY_ntilde                      },
  { .item = ITM_o_MACRON                   ,  .gdkKey = GDK_KEY_omacron                     },
  { .item = ITM_o_ACUTE                    ,  .gdkKey = GDK_KEY_oacute                      },
//  #define ITM_o_BREVE                    773
  { .item = ITM_o_GRAVE                    ,  .gdkKey = GDK_KEY_ograve                      },
  { .item = ITM_o_DIARESIS                 ,  .gdkKey = GDK_KEY_odiaeresis                  },
  { .item = ITM_o_TILDE                    ,  .gdkKey = GDK_KEY_otilde                      },
  { .item = ITM_o_CIRC                     ,  .gdkKey = GDK_KEY_ocircumflex                 },
//  #define ITM_o_STROKE                   778
  { .item = ITM_oe                         ,  .gdkKey = GDK_KEY_oe                          },
  { .item = ITM_r_CARON                    ,  .gdkKey = GDK_KEY_rcaron                      },
  { .item = ITM_r_ACUTE                    ,  .gdkKey = GDK_KEY_racute                      },
//  #define ITM_s_SHARP                    782
  { .item = ITM_s_ACUTE                    ,  .gdkKey = GDK_KEY_sacute                      },
  { .item = ITM_s_CARON                    ,  .gdkKey = GDK_KEY_scaron                      },
  { .item = ITM_s_CEDILLA                  ,  .gdkKey = GDK_KEY_scedilla                    },
//  #define ITM_t_APOSTROPHE               786
  { .item = ITM_t_CEDILLA                  ,  .gdkKey = GDK_KEY_tcedilla                    },
  { .item = ITM_u_MACRON                   ,  .gdkKey = GDK_KEY_umacron                     },
  { .item = ITM_u_ACUTE                    ,  .gdkKey = GDK_KEY_uacute                      },
  { .item = ITM_u_BREVE                    ,  .gdkKey = GDK_KEY_ubreve                      },
  { .item = ITM_u_GRAVE                    ,  .gdkKey = GDK_KEY_ugrave                      },
  { .item = ITM_u_DIARESIS                 ,  .gdkKey = GDK_KEY_udiaeresis                  },
  { .item = ITM_u_TILDE                    ,  .gdkKey = GDK_KEY_utilde                      },
  { .item = ITM_u_CIRC                     ,  .gdkKey = GDK_KEY_ucircumflex                 },
  { .item = ITM_u_RING                     ,  .gdkKey = GDK_KEY_uring                       },
  { .item = ITM_w_CIRC                     ,  .gdkKey = GDK_KEY_wcircumflex                 },
//  #define ITM_x_BAR                      797
//  #define ITM_x_CIRC                     798
//  #define ITM_y_BAR                      799
  { .item = ITM_y_CIRC                     ,  .gdkKey = GDK_KEY_ycircumflex                 },
  { .item = ITM_y_ACUTE                    ,  .gdkKey = GDK_KEY_yacute                      },
  { .item = ITM_y_DIARESIS                 ,  .gdkKey = GDK_KEY_ydiaeresis                  },
  { .item = ITM_z_ACUTE                    ,  .gdkKey = GDK_KEY_zacute                      },
  { .item = ITM_z_CARON                    ,  .gdkKey = GDK_KEY_zcaron                      },
  { .item = ITM_z_DOT                      ,  .gdkKey = GDK_KEY_zabovedot                   },
  { .item = ITM_SPACE                      ,  .gdkKey = GDK_KEY_space                       },
  { .item = ITM_EXCLAMATION_MARK           ,  .gdkKey = GDK_KEY_exclam                      },
  { .item = ITM_DOUBLE_QUOTE               ,  .gdkKey = GDK_KEY_quotedbl                    },
  { .item = ITM_NUMBER_SIGN                ,  .gdkKey = GDK_KEY_numbersign                  },
  { .item = ITM_DOLLAR                     ,  .gdkKey = GDK_KEY_dollar                      },
  { .item = ITM_PERCENT                    ,  .gdkKey = GDK_KEY_percent                     },
  { .item = ITM_AMPERSAND                  ,  .gdkKey = GDK_KEY_ampersand                   },
  { .item = ITM_QUOTE                      ,  .gdkKey = GDK_KEY_apostrophe                  },
  { .item = ITM_LEFT_PARENTHESIS           ,  .gdkKey = GDK_KEY_parenleft                   },
  { .item = ITM_RIGHT_PARENTHESIS          ,  .gdkKey = GDK_KEY_parenright                  },
  { .item = ITM_PROD_SIGN                  ,  .gdkKey = GDK_KEY_asterisk                    },
  { .item = ITM_PROD_SIGN                  ,  .gdkKey = GDK_KEY_KP_Multiply                 },
  { .item = ITM_PLUS                       ,  .gdkKey = GDK_KEY_plus                        },
  { .item = ITM_PLUS                       ,  .gdkKey = GDK_KEY_KP_Add                      },
  { .item = ITM_COMMA                      ,  .gdkKey = GDK_KEY_comma                       },
  { .item = ITM_COMMA                      ,  .gdkKey = GDK_KEY_KP_Decimal                  },
  { .item = ITM_MINUS                      ,  .gdkKey = GDK_KEY_minus                       },
  { .item = ITM_MINUS                      ,  .gdkKey = GDK_KEY_KP_Subtract                 },
  { .item = ITM_PERIOD                     ,  .gdkKey = GDK_KEY_period                      },
  { .item = ITM_SLASH                      ,  .gdkKey = GDK_KEY_slash                       },
  { .item = ITM_SLASH                      ,  .gdkKey = GDK_KEY_KP_Divide                   },
  { .item = ITM_COLON                      ,  .gdkKey = GDK_KEY_colon                       },
  { .item = ITM_SEMICOLON                  ,  .gdkKey = GDK_KEY_semicolon                   },
  { .item = ITM_LESS_THAN                  ,  .gdkKey = GDK_KEY_less                        },
  { .item = ITM_EQUAL                      ,  .gdkKey = GDK_KEY_equal                       },
  { .item = ITM_GREATER_THAN               ,  .gdkKey = GDK_KEY_greater                     },
  { .item = ITM_QUESTION_MARK              ,  .gdkKey = GDK_KEY_question                    },
  { .item = ITM_AT                         ,  .gdkKey = GDK_KEY_at                          },
  { .item = ITM_LEFT_SQUARE_BRACKET        ,  .gdkKey = GDK_KEY_bracketleft                 },
  { .item = ITM_BACK_SLASH                 ,  .gdkKey = GDK_KEY_backslash                   },
  { .item = ITM_RIGHT_SQUARE_BRACKET       ,  .gdkKey = GDK_KEY_bracketright                },
  { .item = ITM_CIRCUMFLEX                 ,  .gdkKey = GDK_KEY_asciicircum                 },
  { .item = ITM_UNDERSCORE                 ,  .gdkKey = GDK_KEY_underscore                  },
  { .item = ITM_LEFT_CURLY_BRACKET         ,  .gdkKey = GDK_KEY_braceleft                   },
  { .item = ITM_PIPE                       ,  .gdkKey = GDK_KEY_bar                         },
  { .item = ITM_RIGHT_CURLY_BRACKET        ,  .gdkKey = GDK_KEY_braceright                  },
  { .item = ITM_TILDE                      ,  .gdkKey = GDK_KEY_asciitilde                  },
  { .item = ITM_INVERTED_EXCLAMATION_MARK  ,  .gdkKey = GDK_KEY_exclamdown                  },
  { .item = ITM_CENT                       ,  .gdkKey = GDK_KEY_cent                        },
  { .item = ITM_POUND                      ,  .gdkKey = GDK_KEY_sterling                    },
  { .item = ITM_YEN                        ,  .gdkKey = GDK_KEY_yen                         },
  { .item = ITM_SECTION                    ,  .gdkKey = GDK_KEY_section                     },
//  #define ITM_OVERFLOW_CARRY             843
  { .item = ITM_LEFT_DOUBLE_ANGLE          ,  .gdkKey = GDK_KEY_guillemotleft               },
  { .item = ITM_NOT                        ,  .gdkKey = GDK_KEY_notsign                     },
  { .item = ITM_DEGREE                     ,  .gdkKey = GDK_KEY_degree                      },
  { .item = ITM_PLUS_MINUS                 ,  .gdkKey = GDK_KEY_plusminus                   },
  { .item = ITM_mu_b                       ,  .gdkKey = GDK_KEY_mu                          },
//  #define ITM_DOT                        849
  { .item = ITM_RIGHT_DOUBLE_ANGLE         ,  .gdkKey = GDK_KEY_guillemotright              },
  { .item = ITM_ONE_HALF                   ,  .gdkKey = GDK_KEY_onehalf                     },
  { .item = ITM_ONE_QUARTER                ,  .gdkKey = GDK_KEY_onequarter                  },
  { .item = ITM_ONE_HALF                   ,  .gdkKey = GDK_KEY_onehalf                     },
  { .item = ITM_INVERTED_QUESTION_MARK     ,  .gdkKey = GDK_KEY_questiondown                },
  { .item = ITM_ETH                        ,  .gdkKey = GDK_KEY_ETH                         },
  { .item = ITM_CROSS                      ,  .gdkKey = GDK_KEY_multiply                    },
  { .item = ITM_eth                        ,  .gdkKey = GDK_KEY_eth                         },
//  #define ITM_OBELUS                     857
  { .item = ITM_E_DOT                      ,  .gdkKey = GDK_KEY_Eabovedot                   },
  { .item = ITM_e_DOT                      ,  .gdkKey = GDK_KEY_eabovedot                   },
  { .item = ITM_E_CARON                    ,  .gdkKey = GDK_KEY_Ecaron                      },
  { .item = ITM_e_CARON                    ,  .gdkKey = GDK_KEY_ecaron                      },
  { .item = ITM_R_ACUTE                    ,  .gdkKey = GDK_KEY_Racute                      },
  { .item = ITM_R_CARON                    ,  .gdkKey = GDK_KEY_Rcaron                      },
  { .item = ITM_U_OGONEK                   ,  .gdkKey = GDK_KEY_Uogonek                     },
  { .item = ITM_u_OGONEK                   ,  .gdkKey = GDK_KEY_uogonek                     },
//  #define ITM_y_UNDER_ROOT               866
//  #define ITM_x_UNDER_ROOT               867
  { .item = ITM_SPACE_EM                   ,  .gdkKey = GDK_KEY_emspace                     },
  { .item = ITM_SPACE_3_PER_EM             ,  .gdkKey = GDK_KEY_em3space                    },
  { .item = ITM_SPACE_4_PER_EM             ,  .gdkKey = GDK_KEY_em4space                    },
//  #define ITM_SPACE_6_PER_EM             871
  { .item = ITM_SPACE_FIGURE               ,  .gdkKey = GDK_KEY_digitspace                  },
  { .item = ITM_SPACE_PUNCTUATION          ,  .gdkKey = GDK_KEY_punctspace                  },
  { .item = ITM_SPACE_HAIR                 ,  .gdkKey = GDK_KEY_hairspace                   },
  { .item = ITM_LEFT_SINGLE_QUOTE          ,  .gdkKey = GDK_KEY_leftsinglequotemark         },
  { .item = ITM_RIGHT_SINGLE_QUOTE         ,  .gdkKey = GDK_KEY_rightsinglequotemark        },
  { .item = ITM_SINGLE_LOW_QUOTE           ,  .gdkKey = GDK_KEY_singlelowquotemark          },
//  #define ITM_SINGLE_HIGH_QUOTE          878
  { .item = ITM_LEFT_DOUBLE_QUOTE          ,  .gdkKey = GDK_KEY_leftdoublequotemark         },
  { .item = ITM_RIGHT_DOUBLE_QUOTE         ,  .gdkKey = GDK_KEY_rightdoublequotemark        },
  { .item = ITM_DOUBLE_LOW_QUOTE           ,  .gdkKey = GDK_KEY_doublelowquotemark          },
//  #define ITM_DOUBLE_HIGH_QUOTE          882
  { .item = ITM_ELLIPSIS                   ,  .gdkKey = GDK_KEY_ellipsis                    },
//  #define ITM_BINARY_ONE                 884
  { .item = ITM_EURO                       ,  .gdkKey = GDK_KEY_EuroSign                    },
//  #define ITM_COMPLEX_C                  886
//  #define ITM_PLANCK                     887
//  #define ITM_PLANCK_2PI                 888
//  #define ITM_NATURAL_N                  889
//  #define ITM_RATIONAL_Q                 890
//  #define ITM_REAL_R                     891
  { .item = ITM_LEFT_ARROW                 ,  .gdkKey = GDK_KEY_leftarrow                   },
  { .item = ITM_UP_ARROW                   ,  .gdkKey = GDK_KEY_uparrow                     },
  { .item = ITM_RIGHT_ARROW                ,  .gdkKey = GDK_KEY_rightarrow                  },
  { .item = ITM_DOWN_ARROW                 ,  .gdkKey = GDK_KEY_downarrow                   },
//  #define ITM_SERIAL_IO                  896
//  #define ITM_RIGHT_SHORT_ARROW          897
//  #define ITM_LEFT_RIGHT_ARROWS          898
//  #define ITM_BST_SIGN                   899
//  #define ITM_SST_SIGN                   900
//  #define ITM_HAMBURGER                  901
//  #define ITM_UNDO_SIGN                  902
//  #define ITM_FOR_ALL                    903
//  #define ITM_COMPLEMENT                 904
  { .item = ITM_PARTIAL_DIFF               ,  .gdkKey = GDK_KEY_partialderivative           },
//  #define ITM_THERE_EXISTS               906
//  #define ITM_THERE_DOES_NOT_EXIST       907
  { .item = ITM_EMPTY_SET                  ,  .gdkKey = GDK_KEY_emptyset                    },
//  #define ITM_INCREMENT                  909
  { .item = ITM_NABLA                      ,  .gdkKey = GDK_KEY_nabla                       },
  { .item = ITM_ELEMENT_OF                 ,  .gdkKey = GDK_KEY_elementof                   },
  { .item = ITM_NOT_ELEMENT_OF             ,  .gdkKey = GDK_KEY_notelementof                },
  { .item = ITM_CONTAINS                   ,  .gdkKey = GDK_KEY_containsas                  },
//  #define ITM_DOES_NOT_CONTAIN           914
//  #define ITM_BINARY_ZERO                915
//  #define ITM_PRODUCT                    916
  { .item = ITM_MINUS_PLUS                 ,  .gdkKey = GDK_KEY_plusminus                   },
  { .item = ITM_RING                       ,  .gdkKey = GDK_KEY_jot                         },
  { .item = ITM_BULLET                     ,  .gdkKey = GDK_KEY_enfilledcircbullet          },
  { .item = ITM_SQUARE_ROOT                ,  .gdkKey = GDK_KEY_squareroot                  },
  { .item = ITM_CUBE_ROOT                  ,  .gdkKey = GDK_KEY_cuberoot                    },
//  #define ITM_xTH_ROOT                   922
//  #define ITM_PROPORTIONAL               923
  { .item = ITM_INFINITY                   ,  .gdkKey = GDK_KEY_infinity                    },
//  #define ITM_RIGHT_ANGLE                925
//  #define ITM_ANGLE_SIGN                 926
//  #define ITM_MEASURED_ANGLE             927
//  #define ITM_DIVIDES                    928
//  #define ITM_DOES_NOT_DIVIDE            929
//  #define ITM_PARALLEL_SIGN              930
//  #define ITM_NOT_PARALLEL               931
  { .item = ITM_AND                        ,  .gdkKey = GDK_KEY_logicaland                  },
  { .item = ITM_OR                         ,  .gdkKey = GDK_KEY_logicalor                   },
  { .item = ITM_INTERSECTION               ,  .gdkKey = GDK_KEY_intersection                },
  { .item = ITM_UNION                      ,  .gdkKey = GDK_KEY_union                       },
  { .item = ITM_INTEGRAL_SIGN              ,  .gdkKey = GDK_KEY_integral                    },
  { .item = ITM_DOUBLE_INTEGRAL            ,  .gdkKey = GDK_KEY_dintegral                   },
//  #define ITM_CONTOUR_INTEGRAL           938
//  #define ITM_SURFACE_INTEGRAL           939
//  #define ITM_RATIO                      940
  { .item = ITM_CHECK_MARK                 ,  .gdkKey = GDK_KEY_checkmark                   },
  { .item = ITM_ASYMPOTICALLY_EQUAL        ,  .gdkKey = GDK_KEY_similarequal                },
  { .item = ITM_ALMOST_EQUAL               ,  .gdkKey = GDK_KEY_approximate                 },
//  #define ITM_COLON_EQUALS               944
//  #define ITM_CORRESPONDS_TO             945
//  #define ITM_ESTIMATES                  946
  { .item = ITM_NOT_EQUAL                  ,  .gdkKey = GDK_KEY_notequal                    },
  { .item = ITM_IDENTICAL_TO               ,  .gdkKey = GDK_KEY_identical                   },
  { .item = ITM_LESS_EQUAL                 ,  .gdkKey = GDK_KEY_lessthanequal               },
  { .item = ITM_GREATER_EQUAL              ,  .gdkKey = GDK_KEY_greaterthanequal            },
//  #define ITM_MUCH_LESS                  951
//  #define ITM_MUCH_GREATER               952
//  #define ITM_SUN                        953
  { .item = ITM_DOWN_TACK                  ,  .gdkKey = GDK_KEY_downtack                    },
//  #define ITM_PERPENDICULAR              955
//  #define ITM_XOR                        956
//  #define ITM_NAND                       957
//  #define ITM_NOR                        958
//  #define ITM_WATCH                      959
//  #define ITM_HOURGLASS                  960
//  #define ITM_PRINTER                    961
//  #define ITM_MAT_TL                     962
//  #define ITM_MAT_ML                     963
//  #define ITM_MAT_BL                     964
//  #define ITM_MAT_TR                     965
//  #define ITM_MAT_MR                     966
//  #define ITM_MAT_BR                     967
//  #define ITM_OBLIQUE1                   968
//  #define ITM_OBLIQUE2                   969
//  #define ITM_OBLIQUE3                   970
//  #define ITM_OBLIQUE4                   971
//  #define ITM_CURSOR                     972
//  #define ITM_PERIOD34                   973
//  #define ITM_COMMA34                    974
//  #define ITM_BATTERY                    975
//  #define ITM_PGM_BEGIN                  976
//  #define ITM_USER_MODE                  977
//  #define ITM_UK                         978
//  #define ITM_US                         979
//  #define ITM_NEG_EXCLAMATION_MARK       980
//  #define ITM_ex                         981
//  #define ITM_Max                        982
//  #define ITM_Min                        983
//  #define ITM_Config                     984
//  #define ITM_Stack                      985
//  #define ITM_dddEL                      986
//  #define ITM_dddIJ                      987
//  #define ITM_0P                         988
//  #define ITM_1P                         989
//  #define ITM_EXPONENT                   990
//  #define ITM_HEX                        991
//  #define ITM_M_GOTO_ROW                 992
//  #define ITM_M_GOTO_COLUMN              993
//  #define ITM_SOLVE_VAR                  994
//  #define ITM_EQ_LEFT                    995
//  #define ITM_EQ_RIGHT                   996
//  #define ITM_PAIR_OF_PARENTHESES        997
//  #define ITM_VERTICAL_BAR               998
//  #define ITM_ALOG_SYMBOL                999
//  #define ITM_ROOT_SIGN                 1000
//  #define ITM_TIMER_SYMBOL              1001
//  #define ITM_Sfdx_VAR                  1002
//  #define ITM_SUP_PLUS                  1003
//  #define ITM_SUP_MINUS                 1004
//  #define ITM_SUP_MINUS_1               1005
//  #define ITM_SUP_INFINITY              1006
//  #define ITM_SUP_ASTERISK              1007
  { .item = ITM_SUP_0                     ,  .gdkKey = GDK_KEY_zerosuperior                 },
  { .item = ITM_SUP_1                     ,  .gdkKey = GDK_KEY_onesuperior                  },
  { .item = ITM_SUP_2                     ,  .gdkKey = GDK_KEY_twosuperior                  },
  { .item = ITM_SUP_3                     ,  .gdkKey = GDK_KEY_threesuperior                },
  { .item = ITM_SUP_4                     ,  .gdkKey = GDK_KEY_foursuperior                 },
  { .item = ITM_SUP_5                     ,  .gdkKey = GDK_KEY_fivesuperior                 },
  { .item = ITM_SUP_6                     ,  .gdkKey = GDK_KEY_sixsuperior                  },
  { .item = ITM_SUP_7                     ,  .gdkKey = GDK_KEY_sevensuperior                },
  { .item = ITM_SUP_8                     ,  .gdkKey = GDK_KEY_eightsuperior                },
  { .item = ITM_SUP_9                     ,  .gdkKey = GDK_KEY_ninesuperior                 },
//  #define ITM_SUP_A                     1018
//  #define ITM_SUP_B                     1019
//  #define ITM_SUP_C                     1020
//  #define ITM_SUP_D                     1021
//  #define ITM_SUP_E                     1022
//  #define ITM_SUP_F                     1023
//  #define ITM_SUP_G                     1024
//  #define ITM_SUP_H                     1025
//  #define ITM_SUP_I                     1026
//  #define ITM_SUP_J                     1027
//  #define ITM_SUP_K                     1028
//  #define ITM_SUP_L                     1029
//  #define ITM_SUP_M                     1030
//  #define ITM_SUP_N                     1031
//  #define ITM_SUP_O                     1032
//  #define ITM_SUP_P                     1033
//  #define ITM_SUP_Q                     1034
//  #define ITM_SUP_R                     1035
//  #define ITM_SUP_S                     1036
//  #define ITM_SUP_T                     1037
//  #define ITM_SUP_U                     1038
//  #define ITM_SUP_V                     1039
//  #define ITM_SUP_W                     1040
//  #define ITM_SUP_X                     1041
//  #define ITM_SUP_Y                     1042
//  #define ITM_SUP_Z                     1043
//  #define ITM_SUP_a                     1044
//  #define ITM_SUP_b                     1045
//  #define ITM_SUP_c                     1046
//  #define ITM_SUP_d                     1047
//  #define ITM_SUP_e                     1048
//  #define ITM_SUP_f                     1049
//  #define ITM_SUP_g                     1050
//  #define ITM_SUP_h                     1051
//  #define ITM_SUP_i                     1052
//  #define ITM_SUP_j                     1053
//  #define ITM_SUP_k                     1054
//  #define ITM_SUP_l                     1055
//  #define ITM_SUP_m                     1056
//  #define ITM_SUP_n                     1057
//  #define ITM_SUP_o                     1058
//  #define ITM_SUP_p                     1059
//  #define ITM_SUP_q                     1060
//  #define ITM_SUP_r                     1061
//  #define ITM_SUP_s                     1062
//  #define ITM_SUP_t                     1063
//  #define ITM_SUP_u                     1064
//  #define ITM_SUP_v                     1065
//  #define ITM_SUP_w                     1066
//  #define ITM_SUP_x                     1067
//  #define ITM_SUP_y                     1068
//  #define ITM_SUP_z                     1069
//  #define ITM_SUB_alpha                 1070
//  #define ITM_SUB_delta                 1071
//  #define ITM_SUB_mu                    1072
//  #define ITM_SUB_SUN                   1073
//  #define ITM_SUB_EARTH                 1074
//  #define ITM_SUB_PLUS                  1075
//  #define ITM_SUB_MINUS                 1076
//  #define ITM_SUB_INFINITY              1077
//  #define ITM_SUB_10                    1078
//  #define ITM_SUB_E_OUTLINE             1079
  { .item = ITM_SUB_0                     ,  .gdkKey = GDK_KEY_zerosubscript                },
  { .item = ITM_SUB_1                     ,  .gdkKey = GDK_KEY_onesubscript                 },
  { .item = ITM_SUB_2                     ,  .gdkKey = GDK_KEY_twosubscript                 },
  { .item = ITM_SUB_3                     ,  .gdkKey = GDK_KEY_threesubscript               },
  { .item = ITM_SUB_4                     ,  .gdkKey = GDK_KEY_foursubscript                },
  { .item = ITM_SUB_5                     ,  .gdkKey = GDK_KEY_fivesubscript                },
  { .item = ITM_SUB_6                     ,  .gdkKey = GDK_KEY_sixsubscript                 },
  { .item = ITM_SUB_7                     ,  .gdkKey = GDK_KEY_sevensubscript               },
  { .item = ITM_SUB_8                     ,  .gdkKey = GDK_KEY_eightsubscript               },
  { .item = ITM_SUB_9                     ,  .gdkKey = GDK_KEY_ninesubscript                },
//  #define ITM_SUB_A                     1090
//  #define ITM_SUB_B                     1091
//  #define ITM_SUB_C                     1092
//  #define ITM_SUB_D                     1093
//  #define ITM_SUB_E                     1094
//  #define ITM_SUB_F                     1095
//  #define ITM_SUB_G                     1096
//  #define ITM_SUB_H                     1097
//  #define ITM_SUB_I                     1098
//  #define ITM_SUB_J                     1099
//  #define ITM_SUB_K                     1100
//  #define ITM_SUB_L                     1101
//  #define ITM_SUB_M                     1102
//  #define ITM_SUB_N                     1103
//  #define ITM_SUB_O                     1104
//  #define ITM_SUB_P                     1105
//  #define ITM_SUB_Q                     1106
//  #define ITM_SUB_R                     1107
//  #define ITM_SUB_S                     1108
//  #define ITM_SUB_T                     1109
//  #define ITM_SUB_U                     1110
//  #define ITM_SUB_V                     1111
//  #define ITM_SUB_W                     1112
//  #define ITM_SUB_X                     1113
//  #define ITM_SUB_Y                     1114
//  #define ITM_SUB_Z                     1115
//  #define ITM_SUB_a                     1116
//  #define ITM_SUB_b                     1117
//  #define ITM_SUB_c                     1118
//  #define ITM_SUB_d                     1119
//  #define ITM_SUB_e                     1120
//  #define ITM_SUB_f                     1121
//  #define ITM_SUB_g                     1122
//  #define ITM_SUB_h                     1123
//  #define ITM_SUB_i                     1124
//  #define ITM_SUB_j                     1125
//  #define ITM_SUB_k                     1126
//  #define ITM_SUB_l                     1127
//  #define ITM_SUB_m                     1128
//  #define ITM_SUB_n                     1129
//  #define ITM_SUB_o                     1130
//  #define ITM_SUB_p                     1131
//  #define ITM_SUB_q                     1132
//  #define ITM_SUB_r                     1133
//  #define ITM_SUB_s                     1134
//  #define ITM_SUB_t                     1135
//  #define ITM_SUB_u                     1136
//  #define ITM_SUB_v                     1137
//  #define ITM_SUB_w                     1138
//  #define ITM_SUB_x                     1139
//  #define ITM_SUB_y                     1140
//  #define ITM_SUB_z                     1141
    {.item = 0                            ,  .gdkKey = 0                                    }
};

const deadKeysMap_t deadKeysMap[] = {
//    item           item_macron      item_acute      item_breve      item_grave      item_diaresis      item_tilde      item_circ       item_caron     item_ogonek    item_ring      item_cedilla   item_stroke    item_dot
    { ITM_A        , ITM_A_MACRON   , ITM_A_ACUTE   , ITM_A_BREVE   , ITM_A_GRAVE   , ITM_A_DIARESIS   , ITM_A_TILDE   , ITM_A_CIRC    , ITM_A        , ITM_A_OGONEK , ITM_A_RING   , ITM_A        , ITM_A        , ITM_A        },
    { ITM_C        , ITM_C          , ITM_C_ACUTE   , ITM_C         , ITM_C         , ITM_C            , ITM_C         , ITM_C         , ITM_C_CARON  , ITM_C        , ITM_C        , ITM_C_CEDILLA, ITM_C        , ITM_C        },
    { ITM_D        , ITM_D          , ITM_D         , ITM_D         , ITM_D         , ITM_D            , ITM_D         , ITM_D         , ITM_D_CARON  , ITM_D        , ITM_D        , ITM_D        , ITM_D_STROKE , ITM_D        },
    { ITM_E        , ITM_E_MACRON   , ITM_E_ACUTE   , ITM_E_BREVE   , ITM_E_GRAVE   , ITM_E_DIARESIS   , ITM_E         , ITM_E_CIRC    , ITM_E_CARON  , ITM_E_OGONEK , ITM_E        , ITM_E        , ITM_E        , ITM_E_DOT    },
    { ITM_G        , ITM_G          , ITM_G         , ITM_G_BREVE   , ITM_G         , ITM_G            , ITM_G         , ITM_G         , ITM_G        , ITM_G        , ITM_G        , ITM_G        , ITM_G        , ITM_G        },
    { ITM_I        , ITM_I_MACRON   , ITM_I_ACUTE   , ITM_I_BREVE   , ITM_I_GRAVE   , ITM_I_DIARESIS   , ITM_I         , ITM_I_CIRC    , ITM_I        , ITM_I_OGONEK , ITM_I        , ITM_I        , ITM_I        , ITM_I_DOT    },
    { ITM_L        , ITM_L          , ITM_L_ACUTE   , ITM_L         , ITM_L         , ITM_L            , ITM_L         , ITM_L         , ITM_L        , ITM_L        , ITM_L        , ITM_L        , ITM_L_STROKE , ITM_L        },
    { ITM_N        , ITM_N          , ITM_N_ACUTE   , ITM_N         , ITM_N         , ITM_N            , ITM_N_TILDE   , ITM_N         , ITM_N_CARON  , ITM_N        , ITM_N        , ITM_N        , ITM_N        , ITM_N        },
    { ITM_O        , ITM_O_MACRON   , ITM_O_ACUTE   , ITM_O_BREVE   , ITM_O_GRAVE   , ITM_O_DIARESIS   , ITM_O_TILDE   , ITM_O_CIRC    , ITM_O        , ITM_O        , ITM_O        , ITM_O        , ITM_O_STROKE , ITM_O        },
    { ITM_R        , ITM_R          , ITM_R_ACUTE   , ITM_R         , ITM_R         , ITM_R            , ITM_R         , ITM_R         , ITM_R_CARON  , ITM_R        , ITM_R        , ITM_R        , ITM_R        , ITM_R        },
    { ITM_S        , ITM_S          , ITM_S_ACUTE   , ITM_S         , ITM_S         , ITM_S            , ITM_S         , ITM_S         , ITM_S_CARON  , ITM_S        , ITM_S        , ITM_S_CEDILLA, ITM_S        , ITM_S        },
    { ITM_T        , ITM_T          , ITM_T         , ITM_T         , ITM_T         , ITM_T            , ITM_T         , ITM_T         , ITM_T_CARON  , ITM_T        , ITM_T        , ITM_T_CEDILLA, ITM_T        , ITM_T        },
    { ITM_U        , ITM_U_MACRON   , ITM_U_ACUTE   , ITM_U_BREVE   , ITM_U_GRAVE   , ITM_U_DIARESIS   , ITM_U_TILDE   , ITM_U_CIRC    , ITM_U        , ITM_U_OGONEK , ITM_U_RING   , ITM_U        , ITM_U        , ITM_U        },
    { ITM_W        , ITM_W          , ITM_W         , ITM_W         , ITM_W         , ITM_W            , ITM_W         , ITM_W_CIRC    , ITM_W        , ITM_W        , ITM_W        , ITM_W        , ITM_W        , ITM_W        },
    { ITM_Y        , ITM_Y          , ITM_Y_ACUTE   , ITM_Y         , ITM_Y         , ITM_Y_DIARESIS   , ITM_Y         , ITM_Y_CIRC    , ITM_Y        , ITM_Y        , ITM_Y        , ITM_Y        , ITM_Y        , ITM_Y        },
    { ITM_Z        , ITM_Z          , ITM_Z_ACUTE   , ITM_Z         , ITM_Z         , ITM_Z            , ITM_Z         , ITM_Z         , ITM_Z_CARON  , ITM_Z        , ITM_Z        , ITM_Z        , ITM_Z        , ITM_Z_DOT    },
    { ITM_a        , ITM_a_MACRON   , ITM_a_ACUTE   , ITM_a_BREVE   , ITM_a_GRAVE   , ITM_a_DIARESIS   , ITM_a_TILDE   , ITM_a_CIRC    , ITM_a        , ITM_a_OGONEK , ITM_a_RING   , ITM_a        , ITM_a        , ITM_a        },
    { ITM_c        , ITM_c          , ITM_c_ACUTE   , ITM_c         , ITM_c         , ITM_c            , ITM_c         , ITM_c         , ITM_c_CARON  , ITM_c        , ITM_c        , ITM_c_CEDILLA, ITM_c        , ITM_c        },
    { ITM_d        , ITM_d          , ITM_d         , ITM_d         , ITM_d         , ITM_d            , ITM_d         , ITM_d         , ITM_d        , ITM_d        , ITM_d        , ITM_d        , ITM_d_STROKE , ITM_d        },
    { ITM_e        , ITM_e_MACRON   , ITM_e_ACUTE   , ITM_e_BREVE   , ITM_e_GRAVE   , ITM_e_DIARESIS   , ITM_e         , ITM_e_CIRC    , ITM_e_CARON  , ITM_e_OGONEK , ITM_e        , ITM_e        , ITM_e        , ITM_e_DOT    },
    { ITM_g        , ITM_g          , ITM_g         , ITM_g_BREVE   , ITM_g         , ITM_g            , ITM_g         , ITM_g         , ITM_g        , ITM_g        , ITM_g        , ITM_g        , ITM_g        , ITM_g        },
    { ITM_h        , ITM_h          , ITM_h         , ITM_h         , ITM_h         , ITM_h            , ITM_h         , ITM_h         , ITM_h        , ITM_h        , ITM_h        , ITM_h        , ITM_h_STROKE , ITM_h        },
    { ITM_i        , ITM_i_MACRON   , ITM_i_ACUTE   , ITM_i_BREVE   , ITM_i_GRAVE   , ITM_i_DIARESIS   , ITM_i         , ITM_i_CIRC    , ITM_i        , ITM_i_OGONEK , ITM_i        , ITM_i        , ITM_i        , ITM_i_DOT    },
    { ITM_l        , ITM_l          , ITM_l_ACUTE   , ITM_l         , ITM_l         , ITM_l            , ITM_l         , ITM_l         , ITM_l        , ITM_l        , ITM_l        , ITM_l        , ITM_l_STROKE , ITM_l        },
    { ITM_n        , ITM_n          , ITM_n_ACUTE   , ITM_n         , ITM_n         , ITM_n            , ITM_n_TILDE   , ITM_n         , ITM_n_CARON  , ITM_n        , ITM_n        , ITM_n        , ITM_n        , ITM_n        },
    { ITM_o        , ITM_o_MACRON   , ITM_o_ACUTE   , ITM_o_BREVE   , ITM_o_GRAVE   , ITM_o_DIARESIS   , ITM_o_TILDE   , ITM_o_CIRC    , ITM_o        , ITM_o        , ITM_o        , ITM_o        , ITM_o_STROKE , ITM_o        },
    { ITM_r        , ITM_r          , ITM_r_ACUTE   , ITM_r         , ITM_r         , ITM_r            , ITM_r         , ITM_r         , ITM_r_CARON  , ITM_r        , ITM_r        , ITM_r        , ITM_r        , ITM_r        },
    { ITM_s        , ITM_s          , ITM_s_ACUTE   , ITM_s         , ITM_s         , ITM_s            , ITM_s         , ITM_s         , ITM_s_CARON  , ITM_s        , ITM_s        , ITM_s_CEDILLA, ITM_s        , ITM_s        },
    { ITM_t        , ITM_t          , ITM_t         , ITM_t         , ITM_t         , ITM_t            , ITM_t         , ITM_t         , ITM_t        , ITM_t        , ITM_t        , ITM_t_CEDILLA, ITM_t        , ITM_t        },
    { ITM_u        , ITM_u_MACRON   , ITM_u_ACUTE   , ITM_u_BREVE   , ITM_u_GRAVE   , ITM_u_DIARESIS   , ITM_u_TILDE   , ITM_u_CIRC    , ITM_u        , ITM_u_OGONEK , ITM_u_RING   , ITM_u        , ITM_u        , ITM_u        },
    { ITM_w        , ITM_w          , ITM_w         , ITM_w         , ITM_w         , ITM_w            , ITM_w         , ITM_w_CIRC    , ITM_w        , ITM_w        , ITM_w        , ITM_w        , ITM_w        , ITM_w        },
    { ITM_x        , ITM_x          , ITM_x         , ITM_x         , ITM_x         , ITM_x            , ITM_x         , ITM_x_CIRC    , ITM_x        , ITM_x        , ITM_x        , ITM_x        , ITM_x        , ITM_x        },
    { ITM_y        , ITM_y          , ITM_y_ACUTE   , ITM_y         , ITM_y         , ITM_y_DIARESIS   , ITM_y         , ITM_y_CIRC    , ITM_y        , ITM_y        , ITM_y        , ITM_y        , ITM_y        , ITM_y        },
    { ITM_z        , ITM_z          , ITM_z_ACUTE   , ITM_z         , ITM_z         , ITM_z            , ITM_z         , ITM_z         , ITM_z_CARON  , ITM_z        , ITM_z        , ITM_z        , ITM_z        , ITM_z_DOT    },
    { ITM_SPACE    , ITM_SPACE      , ITM_SPACE     , ITM_SPACE     , ITM_SPACE     , ITM_SPACE        , ITM_TILDE     , ITM_CIRCUMFLEX, ITM_SPACE    , ITM_SPACE    , ITM_RING     , ITM_SPACE    , ITM_SPACE    , ITM_DOT      },
    { 0            , 0              , 0             , 0             , 0             , 0                , 0             , 0             , 0            , 0            , 0            , 0            , 0            , 0            }
};

const int16_t greekMap[] = {
            ITM_ALPHA                      ,
            ITM_BETA                       ,
            ITM_GAMMA                      ,
            ITM_DELTA                      ,
            ITM_EPSILON                    ,
            ITM_PHI                        ,
            ITM_GAMMA                      ,
            ITM_CHI                        ,
            ITM_IOTA                       ,
            ITM_ETA                        ,
            ITM_KAPPA                      ,
            ITM_LAMBDA                     ,
            ITM_MU                         ,
            ITM_NU                         ,
            ITM_OMEGA                      ,
            ITM_PI                         ,
            ITM_OMICRON                    ,
            ITM_RHO                        ,
            ITM_SIGMA                      ,
            ITM_TAU                        ,
            ITM_THETA                      ,
            ITM_BETA                       ,
            ITM_PSI                        ,
            ITM_XI                         ,
            ITM_UPSILON                    ,
            ITM_ZETA                       ,
            ITM_alpha                      ,
            ITM_beta                       ,
            ITM_gamma                      ,
            ITM_delta                      ,
            ITM_epsilon                    ,
            ITM_phi                        ,
            ITM_gamma                      ,
            ITM_chi                        ,
            ITM_iota                       ,
            ITM_eta                        ,
            ITM_kappa                      ,
            ITM_lambda                     ,
            ITM_mu                         ,
            ITM_nu                         ,
            ITM_omega                      ,
            ITM_pi                         ,
            ITM_omicron                    ,
            ITM_rho                        ,
            ITM_sigma                      ,
            ITM_tau                        ,
            ITM_theta                      ,
            ITM_beta                       ,
            ITM_psi                        ,
            ITM_xi                         ,
            ITM_upsilon                    ,
            ITM_zeta
};

static int16_t _getGdkKeyItem (uint32_t gdkKey) {
  int16_t i=0;
  while(gdkKeyMap[i].item != 0) {
    if(gdkKeyMap[i].gdkKey == gdkKey) {
      break;
    }
    i++;
  }
  return gdkKeyMap[i].item;
}

static int16_t _getDeadKeyItem (int16_t item) {
  int16_t i=0;
  while(deadKeysMap[i].item != 0) {
    if(deadKeysMap[i].item == item) {
      switch(deadKey) {
        case GDK_KEY_dead_macron :
          return deadKeysMap[i].item_macron;

        case GDK_KEY_dead_acute :
          return deadKeysMap[i].item_acute;

        case GDK_KEY_dead_breve :
          return deadKeysMap[i].item_breve;

        case GDK_KEY_dead_grave :
          return deadKeysMap[i].item_grave;

        case GDK_KEY_dead_diaeresis :
          return deadKeysMap[i].item_diaresis;

        case GDK_KEY_dead_tilde :
          return deadKeysMap[i].item_tilde;

        case GDK_KEY_dead_circumflex:
          return deadKeysMap[i].item_circ;

        case GDK_KEY_dead_ogonek :
          return deadKeysMap[i].item_ogonek;

        case GDK_KEY_dead_abovering :
          return deadKeysMap[i].item_ring;

        case GDK_KEY_dead_cedilla :
          return deadKeysMap[i].item_cedilla;

        case GDK_KEY_dead_stroke :
          return deadKeysMap[i].item_ring;

        case GDK_KEY_dead_abovedot :
          return deadKeysMap[i].item_dot;
      }
    }
    i++;
  }
  return item;
}

void setAlphaCaseToCapsLockState() {
  if(gdk_keymap_get_caps_lock_state(gdk_keymap_get_for_display(gdk_display_get_default()))) {
    alphaCase = AC_UPPER;
  }
  else {
    alphaCase = AC_LOWER;
  }
  refreshStatusBar();
  lcd_refresh();
}

static keyCode_t _keyCodeFromGdkKey(uint32_t gdkKey, bool Alpha) {
  int16_t item;

  printf("**[DL]** _keyCodeFromGdkKey gdkKey %x capslock state %d\n", gdkKey, gdk_keymap_get_caps_lock_state(gdk_keymap_get_for_display(gdk_display_get_default())));

  if(Alpha) {
    setAlphaCaseToCapsLockState();
    switch(gdkKey) {
      case GDK_KEY_F1:
        return kcF1;

      case GDK_KEY_F2:
        return kcF2;

      case GDK_KEY_F3:
        return kcF3;

      case GDK_KEY_F4:
        return kcF4;

      case GDK_KEY_F5:
        return kcF5;

      case GDK_KEY_Page_Up:
        return kcShiftF;

      case GDK_KEY_Page_Down:
        return kcShiftG;

      case GDK_KEY_Return:
      case GDK_KEY_KP_Enter:
        return kcEnter;

      case GDK_KEY_Tab:
        shiftF = true;
        return kcSwap;

      case GDK_KEY_BackSpace:
        return kcBackspace;

      case GDK_KEY_Up:
        return kcUp;

      case GDK_KEY_Down:
        return kcDown;

      case GDK_KEY_Escape:
        return kcExit;

      //dead keys detection
      case GDK_KEY_dead_macron  :
      case GDK_KEY_dead_acute  :
      case GDK_KEY_dead_breve  :
      case GDK_KEY_dead_grave  :
      case GDK_KEY_dead_diaeresis :
      case GDK_KEY_dead_tilde :
      case GDK_KEY_dead_circumflex:
      case GDK_KEY_dead_ogonek  :
      case GDK_KEY_dead_abovering :
      case GDK_KEY_dead_cedilla :
      case GDK_KEY_dead_stroke :
      case GDK_KEY_dead_abovedot :
        deadKey = gdkKey;
        return kcNoKey;

      default:
        item = _getGdkKeyItem(gdkKey);
        if(item == ITM_PROD_SIGN) {
          item = (getSystemFlag(FLAG_MULTx) ? ITM_CROSS : ITM_DOT);
        }
        if(item != 0) {
          if(deadKey != 0) {
            item = _getDeadKeyItem(item);
            deadKey = 0;
          }
          if(ITM_A <= item && item <= ITM_z && shiftG) {
            shiftG = false;
            item = greekMap[item - ITM_A];
          }
          if((calcMode == cmPem) && !tamIsActive() && getSystemFlag(FLAG_ALPHA) && !catalog) {
            pemAlpha(item);
          }
          else {
            addItemToBuffer(item);
          }
          refreshScreen();
          lcd_refresh();
        }
        return kcNoKey;
    }
  }
  else {
    switch(gdkKey) {
      case GDK_KEY_F1:
        return kcF1;

      case GDK_KEY_F2:
        return kcF2;

      case GDK_KEY_F3:
        return kcF3;

      case GDK_KEY_F4:
        return kcF4;

      case GDK_KEY_F5:
        return kcF5;

      case GDK_KEY_F6:
        return kcF6;

      case GDK_KEY_C:
        return kcCC;

      case GDK_KEY_c:
        return kcChs;

      case GDK_KEY_E:
        return kcE;

      case GDK_KEY_e:
        return kcEToX;

      case GDK_KEY_F:
      case GDK_KEY_f:
        return kcShiftF;

      case GDK_KEY_G:
      case GDK_KEY_g:
        return kcShiftG;

      case GDK_KEY_I:
      case GDK_KEY_i:
        return kcInv;

      case GDK_KEY_L:
        return kcExp;

      case GDK_KEY_l:
        return kcLn;

      case GDK_KEY_Q:
      case GDK_KEY_q:
        return kcSqrt;

      case GDK_KEY_R:
      case GDK_KEY_r:
        return kcRcl;

      case GDK_KEY_S:
      case GDK_KEY_s:
        return kcSto;

      case GDK_KEY_T:
      case GDK_KEY_t:
        return kcTri;

      case GDK_KEY_X:
          #if defined (XPB)
            forceTamAlpha = true;
          #endif
          return(kcXeq);

      case GDK_KEY_Page_Down:
        return kcRdown;

      case GDK_KEY_Return:
      case GDK_KEY_KP_Enter:
        return kcEnter;

      case GDK_KEY_Tab:
        return kcSwap;

      case GDK_KEY_BackSpace:
        return kcBackspace;

      case GDK_KEY_slash:
      case GDK_KEY_KP_Divide:
        return kcDiv;

      case GDK_KEY_7:
      case GDK_KEY_KP_7:
        return kc7;

      case GDK_KEY_8:
      case GDK_KEY_KP_8:
        return kc8;

      case GDK_KEY_9:
      case GDK_KEY_KP_9:
        return kc9;

      case GDK_KEY_asterisk:
      case GDK_KEY_KP_Multiply:
        return kcMul;

      case GDK_KEY_4:
      case GDK_KEY_KP_4:
        return kc4;

      case GDK_KEY_5:
      case GDK_KEY_KP_5:
        return kc5;

      case GDK_KEY_6:
      case GDK_KEY_KP_6:
        return kc6;

      case GDK_KEY_Up:
        return kcUp;

      case GDK_KEY_minus:
      case GDK_KEY_KP_Subtract:
        return kcSub;

      case GDK_KEY_1:
      case GDK_KEY_KP_1:
        return kc1;

      case GDK_KEY_2:
      case GDK_KEY_KP_2:
        return kc2;

      case GDK_KEY_3:
      case GDK_KEY_KP_3:
        return kc3;

      case GDK_KEY_Down:
        return kcDown;

      case GDK_KEY_plus:
      case GDK_KEY_KP_Add:
        return kcAdd;

      case GDK_KEY_0:
      case GDK_KEY_KP_0:
        return kc0;

      case GDK_KEY_period:
        return kcDot;

      case GDK_KEY_comma:
      case GDK_KEY_KP_Decimal:
        return kcDot;

      case GDK_KEY_Control_L:
      case GDK_KEY_Control_R:
        return kcRun;

      case GDK_KEY_Escape:
        return kcExit;

      default:
        return kcNoKey;
    }
  }
}



static gboolean keyPressed(GtkWidget *w, GdkEventKey *event, gpointer data) {
  uint32_t gdkKey = event->keyval;
  bool alphaInput = (getSystemFlag(FLAG_ALPHA)  || (calcMode == cmAim || calcMode == cmEim || (catalog && catalog != CATALOG_MVAR)));
  if (!alphaInput) {
    switch(gdkKey) {
      case GDK_KEY_H:
      case GDK_KEY_h:
        copyScreenToClipboard();
        return FALSE;

      case GDK_KEY_x:
        copyRegisterXToClipboard();
        return FALSE;

      case GDK_KEY_z:
        copyStackRegistersToClipboard();
        return FALSE;

      case GDK_KEY_Z:
        copyAllRegistersToClipboard();
        return FALSE;
    }
  }
  keyCode_t kc = _keyCodeFromGdkKey(gdkKey,alphaInput);
  if(kc != kcNoKey) {
    btnClicked(kc);
    lcd_refresh();
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
      errorMoreInfo("error while loading CSS style sheet " CSSFILE);
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
      errorMoreInfo("error allocating %d x %d = %d bytes for screenData", screenStride * 4, SCREEN_HEIGHT, numBytes);
      exit(1);
    }

    g_signal_connect(screen, "draw", G_CALLBACK(drawScreen), NULL);

    for(int bezel=0; bezel<MAX_GUI_LAYOUTS; bezel++) {
      gtk_fixed_put(GTK_FIXED(grid), bezelImage[bezel], -999, -999);
    }
    for(int key=0; key<MAX_KEYS; key++) {
      for(int bezel=0; bezel<MAX_GUI_LAYOUTS; bezel++) {
        gtk_fixed_put(GTK_FIXED(grid), calcKeyboard[key].keyImage[bezel], -999, -999);
      }
    }
    gtk_fixed_put(GTK_FIXED(grid), calcKeyboard[10].keyImage[TAM_L_LAYOUT], -999, -999);

    #if (DEBUG_REGISTER_L == 1)
      lblRegisterL1 = gtk_label_new("");
      lblRegisterL2 = gtk_label_new("");
      gtk_widget_set_name(lblRegisterL1, "registerL");
      gtk_widget_set_name(lblRegisterL2, "registerL");
      gtk_fixed_put(GTK_FIXED(grid), lblRegisterL1, 5, 16);
      gtk_fixed_put(GTK_FIXED(grid), lblRegisterL2, 5, 31);
    #endif // (DEBUG_REGISTER_L == 1)

    #if (SHOW_MEMORY_STATUS == 1)
      lblMemoryStatus = gtk_label_new("");
      gtk_widget_set_name(lblMemoryStatus, "memoryStatus");
      gtk_fixed_put(GTK_FIXED(grid), lblMemoryStatus, 5, 1);
    #endif // (SHOW_MEMORY_STATUS == 1)


    for(int bezel=0; bezel<MAX_GUI_LAYOUTS; bezel++) {
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
    gtk_widget_set_tooltip_text(GTK_WIDGET(calcKeyboard[10].keyImage[TAM_L_LAYOUT]), "e");

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
      errorMoreInfo("error allocating %d x %d = %d bytes for screenData", screenStride * 4, SCREEN_HEIGHT, numBytes);
      exit(1);
    }

    g_signal_connect(screen, "draw", G_CALLBACK(drawScreen), NULL);

    gtk_widget_show_all(frmCalc);
  #endif //  (SCREEN_800X480 == 0)
}
