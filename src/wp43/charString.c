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

/********************************************//**
 * \file charString.c
 ***********************************************/

#include "charString.h"

#include "apps/bugScreen.h"
#include "error.h"
#include "fonts.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "wp43.h"



int16_t stringWidth(const char *str, const font_t *font, bool withLeadingEmptyRows, bool withEndingEmptyRows) {
  int16_t ch, numPixels, charCode, glyphId;
  const glyph_t *glyph;
  bool  firstChar;

  glyph = NULL;
  firstChar = true;
  numPixels = 0;
  ch = 0;
  while(str[ch] != 0) {
    charCode = (uint8_t)str[ch++];
    if(charCode & 0x80) { // MSB set
      charCode = (charCode<<8) | (uint8_t)str[ch++];
    }

    glyphId = findGlyph(font, charCode);
    if(glyphId >= 0) {
      glyph = (font->glyphs) + glyphId;
    }
    else if(glyphId == -1) {
      generateNotFoundGlyph(-1, charCode);
      glyph = &glyphNotFound;
    }
    else if(glyphId == -2) {
      generateNotFoundGlyph(-2, charCode);
      glyph = &glyphNotFound;
    }
    else {
      glyph = NULL;
    }

    if(glyph == NULL) {
      #if defined(GENERATE_CATALOGS)
        printf("\n---------------------------------------------------------------------------\n"
                 "In function stringWidth: %d is an unexpected value returned by findGlyph!"
               "/n---------------------------------------------------------------------------\n", glyphId);
      #else // !GENERATE_CATALOGS
        sprintf(errorMessage, "In function stringWidth: %d is an unexpected value returned by findGlyph!", glyphId);
        bugScreen(errorMessage);
      #endif // GENERATE_CATALOGS
      return 0;
    }

    numPixels += glyph->colsGlyph + glyph->colsAfterGlyph;
    if(firstChar) {
      firstChar = false;
      if(withLeadingEmptyRows) {
        numPixels += glyph->colsBeforeGlyph;
      }
    }
    else {
      numPixels += glyph->colsBeforeGlyph;
    }
  }

  if(glyph != NULL && withEndingEmptyRows == false) {
    numPixels -= glyph->colsAfterGlyph;
  }
  return numPixels;
}



int16_t stringNextGlyph(const char *str, int16_t pos) {
  int16_t lg;

  lg = stringByteLength(str);
  if(pos >= lg) {
    return lg;
  }

  pos += (str[pos] & 0x80) ? 2 : 1;

  if(pos >= lg) {
    return lg;
  }
  else {
   return pos;
  }
}



int16_t stringLastGlyph(const char *str) {
  int16_t lastGlyph;

  if(str == NULL) {
    lastGlyph = -1;
  }
  else {
    int16_t lg = stringByteLength(str), next = 0;
    for(lastGlyph=0;;) {
      if(lastGlyph >= lg) {
        next = lg;
      }
      else {
        next += (str[lastGlyph] & 0x80) ? 2 : 1;

        if(next > lg) {
          next = lg;
        }
      }

      if(next == lg) {
        break;
      }

      lastGlyph = next;
    }
  }
  return lastGlyph;
}



int32_t stringByteLength(const char *str) {
  int32_t len = 0;

  while(*str != 0) {
    if(*str & 0x80) {
      str += 2;
      len += 2;
    }
    else {
      str++;
      len++;
    }
  }
  return len;
}



int32_t stringGlyphLength(const char *str) {
  int32_t len = 0;

  while(*str != 0) {
    if(*str & 0x80) {
      str += 2;
      len++;
    }
    else {
      str++;
      len++;
    }
  }
  return len;
}



void codePointToUtf8(uint32_t codePoint, uint8_t *utf8) { // WP43 supports only unicode code points from 0x0000 to 0x7FFF
  if(codePoint <= 0x00007F) {
    utf8[0] = codePoint;
    utf8[1] = 0;
    utf8[2] = 0;
    utf8[3] = 0;
    utf8[4] = 0;
  }

  else if(codePoint <= 0x0007FF) {
    utf8[0] = 0xC0 | (codePoint >> 6);
    utf8[1] = 0x80 | (codePoint &  0x3F);
    utf8[2] = 0;
    utf8[3] = 0;
    utf8[4] = 0;
  }

  else /*if(codePoint <= 0x00FFFF)*/ {
    utf8[0] = 0xE0 |  (codePoint >> 12);
    utf8[1] = 0x80 | ((codePoint >>  6) & 0x3F);
    utf8[2] = 0x80 | ((codePoint      ) & 0x3F);
    utf8[3] = 0;
    utf8[4] = 0;
  }

  /*else if(codePoint <= 0x1FFFFF) {
    utf8[0] = 0xF0 |  (codePoint >> 18);
    utf8[1] = 0x80 | ((codePoint >> 12) & 0x3F);
    utf8[2] = 0x80 | ((codePoint >>  6) & 0x3F);
    utf8[3] = 0x80 | ((codePoint      ) & 0x3F);
    utf8[4] = 0;
  }

  else if(codePoint <= 0x3FFFFFF) {
    utf8[0] = 0xF8 |  (codePoint >> 24);
    utf8[1] = 0x80 | ((codePoint >> 18) & 0x3F);
    utf8[2] = 0x80 | ((codePoint >> 12) & 0x3F);
    utf8[3] = 0x80 | ((codePoint >>  6) & 0x3F);
    utf8[4] = 0x80 | ((codePoint      ) & 0x3F);
    utf8[5] = 0;
  }

  else if(codePoint <= 0x7FFFFFFF) {
    utf8[0] = 0xFC |  (codePoint >> 30);
    utf8[1] = 0x80 | ((codePoint >> 24) & 0x3F);
    utf8[2] = 0x80 | ((codePoint >> 18) & 0x3F);
    utf8[3] = 0x80 | ((codePoint >> 12) & 0x3F);
    utf8[4] = 0x80 | ((codePoint >>  6) & 0x3F);
    utf8[5] = 0x80 | ((codePoint      ) & 0x3F);
    utf8[6] = 0;
  }*/
}



uint32_t utf8ToCodePoint(const uint8_t *utf8, uint32_t *codePoint) { // WP43 supports only unicode code points from 0x0000 to 0x7FFF
  if((*utf8 & 0x80) == 0) {
    *codePoint = *utf8;
    return 1;
  }

  else if((*utf8 & 0xE0) == 0xC0) {
    *codePoint =  (*utf8       & 0x1F) << 6;
    *codePoint |= (*(utf8 + 1) & 0x3F);
    return 2;
  }

  else /*if((*utf8 & 0xF0) == 0xE0)*/ {
    *codePoint =  (*utf8       & 0x0F) << 12;
    *codePoint |= (*(utf8 + 1) & 0x3F) <<  6;
    *codePoint |= (*(utf8 + 2) & 0x3F);
    return 3;
  }

  /*else if((*utf8 & 0xF8) == 0xF0) {
    *codePoint =  (*utf8       & 0x07) << 18;
    *codePoint |= (*(utf8 + 1) & 0x3F) << 12;
    *codePoint |= (*(utf8 + 2) & 0x3F) <<  6;
    *codePoint |= (*(utf8 + 3) & 0x3F);
    return 4;
  }

  else if((*utf8 & 0xFC) == 0xF8) {
    *codePoint =  (*utf8       & 0x03) << 24;
    *codePoint |= (*(utf8 + 1) & 0x3F) << 18;
    *codePoint |= (*(utf8 + 2) & 0x3F) << 12;
    *codePoint |= (*(utf8 + 3) & 0x3F) <<  6;
    *codePoint |= (*(utf8 + 4) & 0x3F);
    return 5;
  }

  else if((*utf8 & 0xFE) == 0xFC) {
    *codePoint =  (*utf8       & 0x01) << 30;
    *codePoint |= (*(utf8 + 1) & 0x3F) << 24;
    *codePoint |= (*(utf8 + 2) & 0x3F) << 18;
    *codePoint |= (*(utf8 + 3) & 0x3F) << 12;
    *codePoint |= (*(utf8 + 4) & 0x3F) <<  6;
    *codePoint |= (*(utf8 + 5) & 0x3F);
    return 6;
  }*/

  return 0;
}


void stringToUtf8(const char *str, uint8_t *utf8) {
  int16_t len;

  len = stringGlyphLength(str);

  if(len == 0) {
    *utf8 = 0;
    return;
  }

  for(int16_t i=0; i<len; i++) {
    if(*str & 0x80) {
      codePointToUtf8(((uint8_t)*str & 0x7F) * 256u + (uint8_t)str[1], utf8);
      str += 2;
      while(*utf8) {
        utf8++;
      }
    }
    else {
      *utf8 = *str;
      utf8++;
      str++;
      *utf8 = 0;
    }
  }
}


void utf8ToString(const uint8_t *utf8, char *str) {
  uint32_t codePoint;

  while(*utf8) {
    utf8 += utf8ToCodePoint(utf8, &codePoint);
    if(codePoint < 0x0080) {
      *(str++) = codePoint;
    }
    else {
      codePoint |= 0x8000;
      *(str++) = codePoint >> 8;
      *(str++) = codePoint & 0x00FF;
    }
  }
  *str = 0;
}


void *xcopy(void *dest, const void *source, int n) {
  char       *pDest   = (char *)dest;
  const char *pSource = (char *)source;

  if(pSource > pDest) {
    while(n--) {
      *pDest++ = *pSource++;
    }
  }
  else if(pSource < pDest) {
    while(n--) {
      pDest[n] = pSource[n];
    }
  }

  return dest;
}


#if defined(WIN32)
  char *stpcpy(char *dest, const char *source) {
    const size_t l = strlen(source);
    return (char *)memcpy(dest, source, l + 1) + l;
  }
#endif // WIN32


#if !defined(DMCP_BUILD)
void strReplace(char *haystack, const char *needle, const char *newNeedle) {
  ////////////////////////////////////////////////////////
  // There MUST be enough memory allocated to *haystack //
  // when strlen(newNeedle) > strlen(needle)            //
  ////////////////////////////////////////////////////////
  char *str, *needleLocation;
  int  needleLg;

  while(strstr(haystack, needle) != NULL) {
    needleLg = strlen(needle);
    needleLocation = strstr(haystack, needle);
    str = malloc(strlen(needleLocation + needleLg) + 1);
    #if defined(PC_BUILD) && !defined(GENERATE_CATALOGS)
      if(str == NULL) {
        moreInfoOnError("In function strReplace:", "error allocating memory for str!", NULL, NULL);
        exit(1);
      }
    #endif // PC_BUILD && !GENERATE_CATALOGS

    strcpy(str, needleLocation + needleLg);
    *strstr(haystack, needle) = 0;
    strcat(haystack, newNeedle);
    strcat(haystack, str);
    free(str);
  }
}
#endif // !DMCP_BUILD
