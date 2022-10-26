

all: run

setup:
	cmake -S . -B build

compile: setup
	$(MAKE) -s -C "build"

run: compile setup
	"./build/PolyLang"
	





