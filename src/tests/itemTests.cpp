// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

extern "C" {
  #include "items.h"
  #include "typeDefinitions.h"
  #include "wp43.h"
}
#include <gtest/gtest.h>

TEST(ItemTest, LastItemIsLastInIndexOfItems) {
  EXPECT_STREQ(indexOfItems[LAST_ITEM].itemSoftmenuName, "Last item");
}
