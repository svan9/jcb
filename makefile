ROOT          := ./src
BIN           := ./bin
ROOT_NAME     := cuba-jit
RELEASE_NAME  := jcb

EXE := $(ROOT)/$(ROOT_NAME).exe

all: build

run: build
	./$(EXE) --test

run-super: build-cp
	./$(EXE) --test

build:
	g++ -o $(EXE) $(ROOT)/$(ROOT_NAME).cpp

build-cp:
	g++ -o $(EXE) $(ROOT)/$(ROOT_NAME).cpp -Wall

build-release:
	g++ -o ./release/$(RELEASE_NAME).exe $(ROOT)/$(ROOT_NAME).cpp -Isrc -O3 -Wall

build-release-nowarn-w:
	g++ -o ./release/$(RELEASE_NAME)-windows.exe $(ROOT)/$(ROOT_NAME).cpp -Isrc -O3

build-release-nowarn-u:
	g++ -o ./release/$(RELEASE_NAME)-ubuntu $(ROOT)/$(ROOT_NAME).cpp -Isrc -O3

build-j:
	g++ -ggdb -o $(EXE) $(ROOT)/$(ROOT_NAME).cpp

build-lib:
	g++ -o $(BIN)/descryptor.o  -c $(ROOT)/descryptor.cpp
	g++ -o $(BIN)/jit.o         -c $(ROOT)/jit.cpp
	g++ -o $(BIN)/lexer.o       -c $(ROOT)/lexer.cpp
	g++ -o $(BIN)/nan.o         -c $(ROOT)/nan.h
	ar  -r $(BIN)/jcb.a $(BIN)/descryptor.o $(BIN)/jit.o $(BIN)/lexer.o $(BIN)/nan.o 