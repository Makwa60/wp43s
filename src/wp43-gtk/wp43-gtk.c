// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "apps/timerApp.h"
#include "core/memory.h"
#include "flags.h"
#include "gtkGui.h"
#include "items.h"
#include "longIntegerType.h"
#include "saveRestoreCalcState.h"
#include "timer.h"
#include "ui/keyboard.h"
#include "ui/screen.h"
#include <stdbool.h>

#include "wp43.h"

bool                calcLandscape;
bool                calcAutoLandscapePortrait;
GtkWidget           *screen;
GtkWidget           *frmCalc;
int16_t             screenStride;
int16_t             debugWindow;
uint32_t            *screenData;
bool                screenChange;
char                debugString[10000];
#if (DEBUG_REGISTER_L == 1)
  GtkWidget         *lblRegisterL1;
  GtkWidget         *lblRegisterL2;
#endif // (DEBUG_REGISTER_L == 1)
#if (SHOW_MEMORY_STATUS == 1)
  GtkWidget         *lblMemoryStatus;
#endif // (SHOW_MEMORY_STATUS == 1)

#if defined(EXPORT_ITEMS)
  int sortItems(void const *a, void const *b) {
    return compareString(a, b, CMP_EXTENSIVE);
  }
#endif // EXPORT_ITEMS

/**
 * Refreshes calc's screen.
 * This function is called every SCREEN_REFRESH_PERIOD ms by a GTK timer.
 *
 * \param[in] unusedData Not used
 * \return What will happen next?
 *   - true  = timer will call this function again
 *   - false = timer stops calling this function
 */
gboolean refreshLcdCallback(gpointer unusedData) {
  refreshLcd();
  return TRUE;
}

/********************************************//**
 * \brief Refreshes timer. This function is
 * called every 5 ms by a GTK timer.
 *
 * \param[in] data gpointer Not used
 * \return gboolean         What will happen next?
 *                          * true  = timer will call this function again
 *                          * false = timer stops calling this function
 ***********************************************/
gboolean timerRefreshCallback(gpointer unusedData) {
  timerRefresh();
  return TRUE;
}

int main(int argc, char* argv[]) {
  #if defined(__APPLE__)
    // we take the directory where the application is as the root for this application.
    // in argv[0] is the application itself. We strip the name of the app by searching for the last '/':
    if(argc>=1) {
      char *curdir = malloc(1000);
      // find last /:
      char *s = strrchr(argv[0], '/');
      if(s != 0) {
        // take the directory before the appname:
        strncpy(curdir, argv[0], s-argv[0]);
        chdir(curdir);
        free(curdir);
      }
    }
  #endif // __APPLE__

  wp43MemInBlocks = 0;
  gmpMemInBytes = 0;
  mp_set_memory_functions(allocGmp, reallocGmp, freeGmp);

  calcLandscape             = false;
  calcAutoLandscapePortrait = true;

  for(int arg=1; arg<argc; arg++) {
    if(strcmp(argv[arg], "--landscape") == 0) {
      calcLandscape             = true;
      calcAutoLandscapePortrait = false;
    }

    if(strcmp(argv[arg], "--portrait") == 0) {
      calcLandscape             = false;
      calcAutoLandscapePortrait = false;
    }

    if(strcmp(argv[arg], "--auto") == 0) {
      calcLandscape             = false;
      calcAutoLandscapePortrait = true;
    }
  }

  if(strcmp(indexOfItems[LAST_ITEM].itemSoftmenuName, "Last item") != 0) {
    printf("The last item of indexOfItems[] is not \"Last item\"\n");
    exit(1);
  }

  #if defined(EXPORT_ITEMS)
    char name[LAST_ITEM][16], nameUtf8[25];
    int cat, nbrItems = 0;
    for(int i=1; i<LAST_ITEM; i++) {
      cat = indexOfItems[i].status & CAT_STATUS;
      if(cat == CAT_FNCT || cat == CAT_CNST || cat == CAT_SYFL || cat == CAT_RVAR) {
        strncpy(name[nbrItems++], indexOfItems[i].itemCatalogName, 16);
      }
    }
    qsort(name, nbrItems, 16, sortItems);
    printf("To be meaningfull, the list below must\n");
    printf("be displayed with the WP43_StandardFont!\n");
    for(int i=0; i<nbrItems; i++) {
      stringToUtf8(name[i], (uint8_t *)nameUtf8);
      printf("%s\n", nameUtf8);
    }
    exit(0);
  #endif // EXPORT_ITEMS

  gtk_init(&argc, &argv);
  setupUI();

  restoreCalc();
  //ramDump();
  //refreshScreen();

  gdk_threads_add_timeout(SCREEN_REFRESH_PERIOD, refreshLcdCallback, NULL); // refreshLcd is called every SCREEN_REFRESH_PERIOD ms

  timerReset();
  timerConfig(tidTimerAppRedraw,           cbTimerAppRedraw,           NOPARAM);
  timerConfig(tidTimerAppDetectWrapAround, cbTimerAppDetectWrapAround, NOPARAM);
  //timerConfig(tidShowNop,                  execNOPTimeout,             NOPARAM);
  gdk_threads_add_timeout(5, timerRefreshCallback, NULL);

  if(getSystemFlag(FLAG_AUTXEQ)) {
    clearSystemFlag(FLAG_AUTXEQ);
    if(programRunStop != PGM_RUNNING) {
      screenUpdatingMode = SCRUPD_AUTO;
      runFunction(ITM_RS);
    }
    refreshScreen();
  }

  gtk_main();

  return 0;
}
