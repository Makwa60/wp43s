# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: Copyright The WP43 Authors

.PHONY: all clean sim test dmcp docs testPgms dist_windows dist_macos dist_dmcp

all: sim

EXE =
ifeq ($(OS),Windows_NT)
  EXE = .exe
endif

clean:
	rm -f wp43$(EXE)
	rm -rf wp43-windows* wp43-macos* wp43-dmcp*
	rm -rf build build.sim build.dmcp build.dmcp-power build.rel

build.sim:
	meson setup build.sim --buildtype=debug -DRASPBERRY=`tools/onARaspberry` -Db_coverage=true

build.rel:
	meson setup build.rel --buildtype=release -DCI_COMMIT_TAG=$(CI_COMMIT_TAG)

build.dmcp:
	meson setup build.dmcp --cross-file=src/wp43-dmcp/cross_stm32l4_gcc.build -DCI_COMMIT_TAG=$(CI_COMMIT_TAG)

build.dmcp-power:
	meson setup build.dmcp-power --cross-file=src/wp43-dmcp/cross_stm32l4_gcc.build -DCI_COMMIT_TAG=$(CI_COMMIT_TAG) -DDEBUG_POWER=true

testPgms: build.sim
	cd build.sim && ninja testPgms
	mkdir -p res/dmcp
	cp build.sim/src/generateTestPgms/testPgms.bin res/dmcp/

sim: build.sim testPgms
	cd build.sim && ninja sim
	cp build.sim/src/generateCatalogs/softmenuCatalogs.h src/generated/
	cp build.sim/src/generateConstants/constantPointers.* src/generated/
	cp build.sim/src/ttf2RasterFonts/rasterFontsData.c src/generated/

release: build.rel testPgms
	cd build.rel && ninja sim
	cp build.rel/src/wp43-gtk/wp43$(EXE) ./
	cp build.rel/src/generateCatalogs/softmenuCatalogs.h src/generated/
	cp build.rel/src/generateConstants/constantPointers.* src/generated/
	cp build.rel/src/ttf2RasterFonts/rasterFontsData.c src/generated/

test: build.sim
	cd build.sim && ninja test

coverage: build.sim
	cd build.sim && ninja coverage-xml
	cd build.sim && ninja coverage-text && cat meson-logs/coverage.txt

dmcp: build.dmcp
	cd build.dmcp && ninja dmcp

dmcp-power: build.dmcp-power
	cd build.dmcp-power && ninja dmcp

docs: build.sim
	cd build.sim && ninja docs

build.rel/wiki: build.rel
	git clone https://gitlab.com/wpcalculators/wp43.wiki.git build.rel/wiki

build.dmcp/keymap: build.dmcp
	cd build.dmcp && ninja keymap

ifeq ($(CI_COMMIT_TAG),)
  WIN_DIST_DIR = wp43-windows
  MAC_DIST_DIR = wp43-macos
  DM_DIST_DIR = wp43-dmcp
else
  WIN_DIST_DIR = wp43-windows-$(CI_COMMIT_TAG)
  MAC_DIST_DIR = wp43-macos-$(CI_COMMIT_TAG)
  DM_DIST_DIR = wp43-dmcp-$(CI_COMMIT_TAG)
endif

dist_windows: testPgms build.rel/wiki
	cd build.rel && ninja sim
	mkdir -p $(WIN_DIST_DIR)/res/artwork $(WIN_DIST_DIR)/res/dmcp $(WIN_DIST_DIR)/res/tone
	cp build.rel/src/wp43-gtk/wp43.exe $(WIN_DIST_DIR)/
	cp res/artwork/*.cfg $(WIN_DIST_DIR)/res/artwork/
	cp -r res/artwork/standardSkin $(WIN_DIST_DIR)/res/artwork/
	cp res/tone/*.wav $(WIN_DIST_DIR)/res/tone/
	cp res/dmcp/testPgms.bin $(WIN_DIST_DIR)/res/dmcp/
	cp res/wp43_pre.css $(WIN_DIST_DIR)/res/
	cp res/fonts/WP43_StandardFont.ttf $(WIN_DIST_DIR)/
	cp build.rel/wiki/Installation-on-Windows.md $(WIN_DIST_DIR)/readme.txt
	zip -r wp43-windows.zip $(WIN_DIST_DIR)
	rm -rf $(WIN_DIST_DIR)

dist_macos: testPgms build.rel
	cd build.rel && ninja sim
	mkdir -p $(MAC_DIST_DIR)/res/artwork $(MAC_DIST_DIR)/res/dmcp
	cp build.rel/src/wp43-gtk/wp43 $(MAC_DIST_DIR)/
	cp res/artwork/*.cfg $(MAC_DIST_DIR)/res/artwork/
	cp -r res/artwork/standardSkin $(MAC_DIST_DIR)/res/artwork/
	cp res/dmcp/testPgms.bin $(MAC_DIST_DIR)/res/dmcp/
	cp res/wp43_pre.css $(MAC_DIST_DIR)/res/
	cp res/fonts/WP43_StandardFont.ttf $(MAC_DIST_DIR)/
	zip -r wp43-macos.zip $(MAC_DIST_DIR)
	rm -rf $(MAC_DIST_DIR)

dist_dmcp: dmcp testPgms build.dmcp/keymap build.rel/wiki
	mkdir -p $(DM_DIST_DIR)
	cp build.dmcp/src/wp43-dmcp/WP43.pgm build.dmcp/src/wp43-dmcp/WP43_qspi.bin $(DM_DIST_DIR)
	cp -r res/offimg $(DM_DIST_DIR)
	cp res/dmcp/testPgms.bin $(DM_DIST_DIR)
	cp build.dmcp/keymap.bin $(DM_DIST_DIR)
	cp dep/DMCP_SDK/keymap_utils/keymaps/empty/keymap.bin $(DM_DIST_DIR)/original_DM42_keymap.bin
	cp res/artwork/WP43_layout.svg $(DM_DIST_DIR)/WP43_layout.svg
	cp build.rel/wiki/Installation-on-a-DM42.md $(DM_DIST_DIR)/readme.txt
	zip -r wp43-dmcp.zip $(DM_DIST_DIR)
	rm -rf $(DM_DIST_DIR)
