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
	gcc -o ./release/$(RELEASE_NAME).exe $(ROOT)/$(ROOT_NAME).c -O3 -Wall

build-j:
	gcc -ggdb -o $(EXEJIT) $(ROOT)/$(ROOT_NAME).c

