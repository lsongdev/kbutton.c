CC ?= cc
CXX ?= c++

BUILD := build
CFLAGS := -std=c11 -Wall -Wextra -Werror -pedantic -Isrc
CXXFLAGS := -std=c++11 -Wall -Wextra -Werror -pedantic -Isrc -Itests/arduino

.PHONY: all test clean

all: test

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/kbutton.o: src/kbutton.c src/kbutton.h | $(BUILD)
	$(CC) $(CFLAGS) -c src/kbutton.c -o $@

$(BUILD)/test_kbutton: tests/test_kbutton.c $(BUILD)/kbutton.o
	$(CC) $(CFLAGS) tests/test_kbutton.c $(BUILD)/kbutton.o -o $@

$(BUILD)/test_arduino: tests/test_arduino.cpp src/kbutton.hpp $(BUILD)/kbutton.o
	$(CXX) $(CXXFLAGS) tests/test_arduino.cpp $(BUILD)/kbutton.o -o $@

test: $(BUILD)/test_kbutton $(BUILD)/test_arduino
	./$(BUILD)/test_kbutton
	./$(BUILD)/test_arduino

clean:
	rm -rf $(BUILD)
