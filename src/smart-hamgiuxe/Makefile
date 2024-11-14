# SPDX-FileCopyrightText: © 2022 Uri Shaked <uri@wokwi.com>
# SPDX-License-Identifier: MIT

SOURCES = chips/mq2.chip.c
TARGET  = dist/mq2.chip.wasm

.PHONY: all
all: $(TARGET) dist/mq2.chip.json

.PHONY: clean
clean:
		rm -rf dist

dist:
		mkdir -p dist

$(TARGET): dist $(SOURCES) chips/wokwi-api.h
	  clang --target=wasm32-unknown-wasi --sysroot "C:\Users\MSI BRAVO\OneDrive - VNU-HCMUS\Documents\PlatformIO\Projects\smart-hamgiuxe\lib\wasi-sdk-24.0-x86_64-windows\share\wasi-sysroot" -nostartfiles -Wl,--import-memory -Wl,--export-table -Wl,--no-entry -Werror -o $(TARGET) $(SOURCES)

dist/mq2.chip.json: dist mq2.chip.json
	  copy mq2.chip.json dist

.PHONY: test
test:
	  cd test && arduino-cli compile -e -b arduino:avr:uno blink
