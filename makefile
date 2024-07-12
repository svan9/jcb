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
	g++ -o $(EXE) $(ROOT)/$(ROOT_NAME).cpp -ggdb

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
	g++ -I./include -o $(BIN)/descryptor.o  -c $(ROOT)/jcb/descryptor.cpp		
	g++ -I./include -o $(BIN)/jit.o         -c $(ROOT)/jcb/jit.cpp				 	
	g++ -I./include -o $(BIN)/lexer.o       -c $(ROOT)/jcb/lexer.cpp 				
	g++ -I./include -o $(BIN)/nan.o         -c $(ROOT)/jcb/nan.cpp 					
	ar -r $(BIN)/libjcb.a $(BIN)/jit.o $(BIN)/descryptor.o $(BIN)/lexer.o   $(BIN)/nan.o 