SHELL := /bin/bash

SOURCE_FILES := $(shell find ../modules -type f -name "gin*.h" -or -name "gin*.dox" | sed 's/ /\\ /g')

.PHONEY: clean

doc/index.html: build/gin_modules.dox Doxyfile
	doxygen

build/gin_modules.dox: process_source_files.py $(SOURCE_FILES)
	python $< ../modules build

clean:
	rm -rf build doc

