BUILD_DIR?=build
CC?=gcc
CXX?=g++
CFLAGS?=-ggdb
CXXFLAGS?=-g

SOURCES_C=$(wildcard *.c)
OBJECTS_C=$(patsubst %.c, $(BUILD_DIR)/tools/isrgen/%.o, $(SOURCES_C))

SOURCES_CXX=$(wildcard *.cpp)
OBJECTS_CXX=$(patsubst %.cpp, $(BUILD_DIR)/tools/isrgen/%.o, $(SOURCES_CXX))

.PHONY: all isrgen clean always

all: isrgen

isrgen: $(BUILD_DIR)/tools/gen.out

$(BUILD_DIR)/tools/gen.out: $(OBJECTS_ASM) $(OBJECTS_CXX)
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS_CXX)

$(BUILD_DIR)/tools/isrgen/%.o: %.cpp always
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

always:
	@mkdir -p $(BUILD_DIR)/tools/isrgen

clean:
	@rm -f $(BUILD_DIR)/tools/gen.out
	@rm -rf $(BUILD_DIR)/tools/isrgen