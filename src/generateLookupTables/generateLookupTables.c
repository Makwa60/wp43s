// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

#include "defines.h"
#include "realType.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

realContext_t ctxtReal34, ctxtReal75;

char externalDeclarations[1000000], externalDeclarations34[1000000]; // .h file
char realArray[1000000], real34Array[1000000], realPointerDeclarations[1000000], real34PointerDeclarations[1000000]; // .c file
FILE *constantsC, *constantsH;
int  idx, idx34, c;


void *xcopy(void *dest, const void *source, int n) {
  char       *pDest   = (char *)dest;
  const char *pSource = (char *)source;

  if(pSource > pDest) {
    while(n--) {
      *pDest++ = *pSource++;
    }
  }
  else {
    if(pSource < pDest) {
      while(n--) {
        pDest[n] = pSource[n];
      }
    }
  }

  return dest;
}

void generateConstantArray(const char *name, const real_t *value) {
  idx += sizeof(real_t);

  strcat(realArray, "  ");
  for(uint32_t i=0; i<sizeof(real_t); i++) {
    sprintf(realArray + strlen(realArray), "0x%02x,", *(((uint8_t *)(value)) + i));
  }

  strcat(realArray, "  // ");
  strcat(realArray, name);
  sprintf(realArray + strlen(realArray), "%03d: ", c);
  realToString(value, realArray + strlen(realArray));
  strcat(realArray, "\n");
}

void generateConstantArray34(const char *name, const real34_t *value) {
  idx34 += sizeof(real34_t);

  strcat(real34Array, "  ");
  for(uint32_t i=0; i<sizeof(real34_t); i++) {
    sprintf(real34Array + strlen(real34Array), "0x%02x,", *(((uint8_t *)(value)) + i));
  }

  strcat(real34Array, "  // ");
  strcat(real34Array, name);
  sprintf(real34Array + strlen(real34Array), "%03d: ", c);
  real34ToString(value, real34Array + strlen(real34Array));
  strcat(real34Array, "\n");
}

void generateArrayDef(const char *name) {
  //sprintf(realPointerDeclarations + strlen(realPointerDeclarations),
  //  "TO_QSPI const real_t * const %s = (real_t *)(lookupTableRaw + %5d);\n", name, idx);
  //sprintf(externalDeclarations + strlen(externalDeclarations),
  //  "  extern const real_t * const %s;\n", name);
  sprintf(real34PointerDeclarations + strlen(real34PointerDeclarations),
    "TO_QSPI const real34_t * const %s%s34 = (real34_t *)(lookupTable34Raw + %5d);\n", name, isdigit(name[strlen(name) - 1]) ? "_" : "", idx34);
  sprintf(externalDeclarations34 + strlen(externalDeclarations34),
    "  extern const real34_t * const %s%s34;\n", name, isdigit(name[strlen(name) - 1]) ? "_" : "");
}
void generateLookupTable(void) {
  real_t value, value2;
  real34_t value34;
  idx = 0; idx34 = 0;

  /*generateArrayDef("reciprocalsOfFactorial");
  memset(&value, 0, sizeof(real_t));
  int32ToReal(1, &value);
  for(c = 0; c < 200; ++c) {
    generateConstantArray("1/x! ", &value);
    realToReal34(&value, &value34);
    generateConstantArray34("1/x! ", &value34);
    int32ToReal(c + 1, &value2);
    realDivide(&value, &value2, &value, &ctxtReal75);
  }*/

  generateArrayDef("maclaurinCoeffSin");
  memset(&value, 0, sizeof(real_t));
  int32ToReal(1, &value);
  for(c = 0; c < 100; ++c) {
    //generateConstantArray("sin ", &value);
    realToReal34(&value, &value34);
    generateConstantArray34("sin ", &value34);
    int32ToReal(1, &value);
    int32ToReal(c * 2 + 2, &value2);
    realDivide(&value, &value2, &value, &ctxtReal75);
    int32ToReal(c * 2 + 3, &value2);
    realDivide(&value, &value2, &value, &ctxtReal75);
    if((c + 1) % 2 == 1) {
      realChangeSign(&value);
    }
  }

  generateArrayDef("maclaurinCoeffCos");
  memset(&value, 0, sizeof(real_t));
  int32ToReal(1, &value);
  for(c = 0; c < 100; ++c) {
    //generateConstantArray("cos ", &value);
    realToReal34(&value, &value34);
    generateConstantArray34("cos ", &value34);
    int32ToReal(1, &value);
    int32ToReal(c * 2 + 1, &value2);
    realDivide(&value, &value2, &value, &ctxtReal75);
    int32ToReal(c * 2 + 2, &value2);
    realDivide(&value, &value2, &value, &ctxtReal75);
    if((c + 1) % 2 == 1) {
      realChangeSign(&value);
    }
  }

  generateArrayDef("maclaurinCoeffAtan");
  memset(&value, 0, sizeof(real_t));
  int32ToReal(1, &value);
  for(c = 0; c < 100; ++c) {
    //generateConstantArray("arctan ", &value);
    realToReal34(&value, &value34);
    generateConstantArray34("arctan ", &value34);
    int32ToReal(1, &value);
    int32ToReal(c * 2 + 3, &value2);
    realDivide(&value, &value2, &value, &ctxtReal75);
    if((c + 1) % 2 == 1) {
      realChangeSign(&value);
    }
  }
}

int main(int argc, char* argv[]) {
  if(argc < 3) {
    printf("Usage: generateConstants <c file> <h file>\n");
    return 1;
  }

  decContextDefault(&ctxtReal34, DEC_INIT_DECQUAD);
  decContextDefault(&ctxtReal75, DEC_INIT_DECQUAD);
  ctxtReal34.digits = 34;
  ctxtReal34.traps  = 0;
  ctxtReal75.digits = 75;
  ctxtReal75.traps  = 0;

  externalDeclarations[0]      = 0;
  externalDeclarations34[0]    = 0;
  realArray[0]                 = 0;
  real34Array[0]               = 0;
  realPointerDeclarations[0]   = 0;
  real34PointerDeclarations[0] = 0;

  generateLookupTable();


  constantsH = fopen(argv[2], "wb");
  if(constantsH == NULL) {
    fprintf(stderr, "Cannot create file %s\n", argv[2]);
    exit(1);
  }

  fprintf(constantsH, "// SPDX-License-Identifier: GPL-3.0-only\n");
  fprintf(constantsH, "// SPDX-FileCopyrightText: Copyright The WP43 Authors\n\n");

  fprintf(constantsH, "/***************************************************************************************************\n");
  fprintf(constantsH, " * Do not edit this file manually! It's automagically generated by the program generateLookupTable *\n");
  fprintf(constantsH, " ***************************************************************************************************/\n\n");

  fprintf(constantsH, "/**\n");
  fprintf(constantsH, " * \\file lookupTables.h\n");
  fprintf(constantsH, " */\n");

  fprintf(constantsH, "#if !defined(LOOKUPTABLES_H)\n");
  fprintf(constantsH, "  #define LOOKUPTABLES_H\n\n");

  fprintf(constantsH, "  #include \"realType.h\"\n");
  fprintf(constantsH, "  #include <stdint.h>\n\n");

  //fprintf(constantsH, "%s", externalDeclarations);
  //fprintf(constantsH, "\n");

  fprintf(constantsH, "%s", externalDeclarations34);

  fprintf(constantsH, "#endif // !LOOKUPTABLES_H\n");

  fclose(constantsH);



  constantsC = fopen(argv[1], "wb");
  if(constantsC == NULL) {
    fprintf(stderr, "Cannot create file %s\n", argv[1]);
    exit(1);
  }

  fprintf(constantsC, "// SPDX-License-Identifier: GPL-3.0-only\n");
  fprintf(constantsC, "// SPDX-FileCopyrightText: Copyright The WP43 Authors\n\n");

  fprintf(constantsC, "/***************************************************************************************************\n");
  fprintf(constantsC, " * Do not edit this file manually! It's automagically generated by the program generateLookupTable *\n");
  fprintf(constantsC, " ***************************************************************************************************/\n\n");

  fprintf(constantsC, "#include \"lookupTables.h\"\n\n");
  fprintf(constantsC, "#include \"wp43.h\"\n\n");

  //fprintf(constantsC, "TO_QSPI static const uint8_t lookupTableRaw[] = {\n");
  //fprintf(constantsC, "%s", realArray);
  //fprintf(constantsC, "};\n");
  fprintf(constantsC, "\n");
  fprintf(constantsC, "TO_QSPI static const uint8_t lookupTable34Raw[] = {\n");
  fprintf(constantsC, "%s", real34Array);
  fprintf(constantsC, "};\n");
  fprintf(constantsC, "\n");
  fprintf(constantsC, "%s", realPointerDeclarations);
  fprintf(constantsC, "\n");
  fprintf(constantsC, "%s", real34PointerDeclarations);

  fclose(constantsC);

  return 0;
}
