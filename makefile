
.PHONY: src build examples include test

all: run

setup:
	cmake -S . -B build

compile: setup
	$(MAKE) -s -C "build"

run: compile setup
	"./build/PolyLang"
	
test: compile
	"./build/PolyLangTester"

count:
	cloc src/ include/




