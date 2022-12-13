// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file charString.h
 */
#if !defined(CHARSTRING_H)
  #define CHARSTRING_H

  #include "typeDefinitions.h"
  #include <stdbool.h>
  #include <stdint.h>

  /**
   * Returns a pointer to the last glyph of a string
   *
   * \param[in] str
   * \return Pointer to the last glyph
   */
  int16_t  stringLastGlyph  (const char *str);

  /**
   * Returns a pointer to the glyph after pos a string
   *
   * \param[in] str
   * \param[in] pos Location after which search the next glyph
   * \return Pointer to the glyph after pos
   */
  int16_t  stringNextGlyph  (const char *str, int16_t pos);

  /**
   * Returns a string length in byte
   *
   * \param[in] str
   * \return
   */
  int32_t  stringByteLength (const char *str);

  /**
   * Returns a string length in glyphs
   *
   * \param[in] str
   * \return
   */
  int32_t  stringGlyphLength(const char *str);

  /**
   * Calculates a string width in pixel using a certain font
   *
   * \param[in] str String whose length is to calculate
   * \param[in] font Font
   * \param[in] withLeadingEmptyRows With the leading empty rows
   * \param[in] withEndingEmptyRows With the ending empty rows
   * \return Width in pixel of the string
   */
  int16_t  stringWidth      (const char *str, const font_t *font, bool withLeadingEmptyRows, bool withEndingEmptyRows);

  /**
   * Calculates the first character which does not fit to specified width using a certain font
   *
   * \param[in] str String
   * \param[in] font Font
   * \param[in] width Width of where to show the string
   * \param[in] withLeadingEmptyRows With the leading empty rows
   * \param[in] withEndingEmptyRows With the ending empty rows
   * \return Width in pixel of the string
   */
  char    *stringAfterPixels(const char *str, const font_t *font, int16_t width, bool withLeadingEmptyRows, bool withEndingEmptyRows);


  /**
   * Converts an unicode code point to utf8
   *
   * \param[in]  codePoint Unicode code point
   * \param[out] utf8 utf8 string
   */
  void     codePointToUtf8  (uint32_t codePoint, uint8_t *utf8);

  /**
   * Converts one utf8 char to an unicode code point
   *
   * \param[in]  utf8 utf8 string
   * \param[out] codePoint Unicode code point
   */
  uint32_t utf8ToCodePoint  (const uint8_t *utf8, uint32_t *codePoint);

  void     stringToUtf8     (const char *str, uint8_t *utf8);
  void     utf8ToString     (const uint8_t *utf8, char *str);
  void    *xcopy            (void *dest, const void *source, int n);
  #if defined(WIN32)
    char    *stpcpy         (char *dest, const char *source);
  #endif // WIN32
  #if !defined(DMCP_BUILD)
    void strReplace         (char *haystack, const char *needle, const char *newNeedle);
  #endif // !DMCP_BUILD

#endif // !CHARSTRING_H
