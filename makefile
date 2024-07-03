ROOT := ./src
ROOT_NAME := cuba-jit
RELEASE_NAME := jcb

EXE := $(ROOT)/$(ROOT_NAME).exe


all: build

run: build
	./$(EXE)

build:
	gcc -o $(EXE) $(ROOT)/$(ROOT_NAME).c  

build-release:
	gcc -o ./release/$(RELEASE_NAME).exe $(ROOT)/$(ROOT_NAME).c -Isrc -O3 -Wall

build-release-nowarn-w:
	gcc -o ./release/$(RELEASE_NAME)-windows.exe $(ROOT)/$(ROOT_NAME).c -Isrc -O3

build-release-nowarn-u:
	gcc -o ./release/$(RELEASE_NAME)-ubuntu $(ROOT)/$(ROOT_NAME).c -Isrc -O3

build-j:
	gcc -ggdb -o $(EXEJIT) $(ROOT)/$(ROOT_NAME).c

