// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file solver/graph.c
 * Graphing module
 */
#if !defined(GRAPH_H)
  #define GRAPH_H

  #include "typeDefinitions.h"
  #include <stdint.h>

  extern char plotStatMx[8];

  #define EQ_SOLVE 0   //fnEqSolvGraph
  #define EQ_PLOT  1   //graph_eqn

  void    fnEqSolvGraph (uint16_t func);
  void    graph_eqn(uint16_t unusedButMandatoryParameter);
  int32_t drawMxN(void);
  void    fnClDrawMx(void);

#endif // !GRAPH_H
