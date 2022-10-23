// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

extern "C" {
  #include "defines.h"
  #include "items.h"
  #include "softmenuCatalogs.h"
  #include "sort.h"
  #include "wp43.h"
}
#include <gtest/gtest.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct Catalog {
  const int16_t *contents;
  uint16_t       id;
  const char    *name;

  friend std::ostream& operator<<(std::ostream& os, const Catalog& catalog) {
    return os << catalog.name;
  }
};

class CatalogSortTest : public testing::TestWithParam<Catalog> {};

TEST_P(CatalogSortTest, IsSorted) {
  auto catalog = GetParam();
  auto nbElements = 0;

  for(auto i = 0; softmenu[i].menuItem; i++) {
    if(softmenu[i].menuItem == -catalog.id) {
      nbElements = softmenu[i].numItems;
      break;
    }
  }

  ASSERT_NE(nbElements, 0) << "Menu not found in structure softmenu";

  auto getItemName = [&] (int32_t i) {
    return indexOfItems[abs(catalog.contents[i])].itemCatalogName;
  };

  for(auto i = 1; i < nbElements; i++) {
    EXPECT_LT(compareString(getItemName(i - 1), getItemName(i), CMP_EXTENSIVE), 0)
      << "Element " << i - 1 << " (item " << catalog.contents[i - 1] << ") should be after element " << i << " (item " << catalog.contents[i] << ")";
  }
}

INSTANTIATE_TEST_SUITE_P(CatalogsAreSorted, CatalogSortTest, ::testing::Values(
  Catalog{menu_FCNS,       MNU_FCNS,      "FCNS"},
  Catalog{menu_CONST,      MNU_CONST,     "CONST"},
  Catalog{menu_SYSFL,      MNU_SYSFL,     "SYS.FL"},
  Catalog{menu_alpha_INTL, MNU_ALPHAINTL, "alphaINTL"},
  Catalog{menu_alpha_intl, MNU_ALPHAintl, "alphaIntl"}
));
