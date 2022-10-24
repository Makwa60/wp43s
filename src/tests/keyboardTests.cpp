// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

extern "C" {
  #include "ui/keyboard.h"
}
#include <gtest/gtest.h>
#include <stdint.h>

struct KeyCodeAndRowColumn {
  keyCode_t keyCode;
  uint8_t   rowColumn;

  friend std::ostream& operator<<(std::ostream& os, const KeyCodeAndRowColumn& st) {
    return os << "key: " << st.keyCode << "; row/column: " << (uint32_t)st.rowColumn;
  }
};

class KeyCodeTransformTest : public testing::TestWithParam<KeyCodeAndRowColumn> {};

TEST_P(KeyCodeTransformTest, KeyCodeMapsToRowColumn) {
  auto st = GetParam();
  auto transform = kbRowColumnFromKeyCode(st.keyCode);

  EXPECT_EQ(st.rowColumn, transform) << "Key code transform doesn't give expected row column";
}

TEST_P(KeyCodeTransformTest, KeyCodeTransformAndThenTransformBackGivesOriginal) {
  auto st = GetParam();
  auto transformAndReverse = kbKeyCodeFromRowColumn(kbRowColumnFromKeyCode(st.keyCode));

  EXPECT_EQ(st.keyCode, transformAndReverse) << "Reverse of transformation doesn't give the original value";
}

TEST_P(KeyCodeTransformTest, RowColumnTransformAndThenTransformBackGivesOriginal) {
  auto st = GetParam();
  auto transformAndReverse = kbRowColumnFromKeyCode(kbKeyCodeFromRowColumn(st.rowColumn));

  EXPECT_EQ(st.rowColumn, transformAndReverse) << "Reverse of transformation doesn't give the original value";
}

INSTANTIATE_TEST_SUITE_P(KeyCodes, KeyCodeTransformTest, ::testing::Values(
  KeyCodeAndRowColumn{kcF1,    11},
  KeyCodeAndRowColumn{kcExp,   22},
  KeyCodeAndRowColumn{kcRdown, 33},
  KeyCodeAndRowColumn{kcE,     44},
  KeyCodeAndRowColumn{kc8,     53},
  KeyCodeAndRowColumn{kc4,     62},
  KeyCodeAndRowColumn{kcSub,   71},
  KeyCodeAndRowColumn{kcExit,  85}
));
