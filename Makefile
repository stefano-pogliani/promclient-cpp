.PHONY: build clean test
.DEFAULT_GOAL := build

#DEBUG_FLAGS ?=
DEBUG_FLAGS ?= -DDEBUG -ggdb

# Configuration variables.
COMPILE_FLAGS = -c -std=c++11 -Wall
GTEST_PATH = googletest/googletest
LINK_FLAGS =

INCLUDES = -Iinclude/
LIBS =


# Tests related variables and targers.
TEST_INCLUDES = $(INCLUDES) -I$(GTEST_PATH)/include
TEST_LIBS = $(LIBS) -lpthread
TEST_OPTS ?=

# Library objects to build.
SRC_OBJS = 
SRC_OBJS += src/internal/text_formatter.o
SRC_OBJS += src/internal/utils.o
SRC_OBJS += src/collector.o
SRC_OBJS += src/collector_registry.o
SRC_OBJS += src/counter.o
SRC_OBJS += src/exceptions.o
SRC_OBJS += src/metric.o

# Test objects to build.
TEST_OBJS =
TEST_OBJS += tests/internal/builder.o
TEST_OBJS += tests/internal/text_formatter.o
TEST_OBJS += tests/collector.o
TEST_OBJS += tests/collector_registry.o
TEST_OBJS += tests/counter.o


# Include files that provide extra features.
BUIILD_DEPS =
EXAMPLE_DEPS =
FEAT_CHECKS =
include features/*.makefile


# Compile C++ files to Objects.
%.o: %.cpp
	g++ $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(INCLUDES) $< -o $@

tests/%.o: tests/%.cpp
	g++ $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(TEST_INCLUDES) $< -o $@

out/gtest-all.o: out/ $(GTEST_PATH)/src/gtest-all.cc
	g++ $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(TEST_INCLUDES) \
		-I$(GTEST_PATH) $(GTEST_PATH)/src/gtest-all.cc -o $@

out/gtest_main.o: out/ $(GTEST_PATH)/src/gtest_main.cc
	g++ $(COMPILE_FLAGS) $(DEBUG_FLAGS) $(TEST_INCLUDES) \
		-I$(GTEST_PATH) $(GTEST_PATH)/src/gtest_main.cc -o $@


# Support targets.
out/:
	mkdir -p out/

out/libpromclient.a: $(SRC_OBJS)
	ar -rv out/libpromclient.a $^

out/tests: out/gtest-all.o out/gtest_main.o $(TEST_OBJS) $(SRC_OBJS)
	g++ $(LINK_FLAGS) $(TEST_LIBS) -o $@ $^


# Entry points.
build: out/ out/libpromclient.a $(BUIILD_DEPS)

clean:
	rm -rf out/
	rm -rf features/build
	find . -name '*.o' -print -delete
	find features/ -name '*.check' -print -delete

example: out/ $(EXAMPLE_DEPS)
	@echo 'All examples built!'

test: $(FEAT_CHECKS) out/tests
	out/tests $(TEST_OPTS)
