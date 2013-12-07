CPPUTEST_HOME = ../../tdd-nrf51/CppUTest-3.3

SDK_PATH = ../../nrf51/

INCLUDE_PATHS += ../lib/
INCLUDE_PATHS += $(SDK_PATH)nrf51822/Include/
INCLUDE_PATHS += $(SDK_PATH)nrf51822/Include/gcc/
INCLUDE_PATHS += $(CPPUTEST_HOME)/include/

CPPFLAGS = -g $(addprefix -I, $(INCLUDE_PATHS))

CXXFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorNewMacros.h
CFLAGS += -include $(CPPUTEST_HOME)/include/CppUTest/MemoryLeakDetectorMallocMacros.h

TARGET = all_tests
SOURCES_APP = $(wildcard ../lib/*.c)
OBJECTS_APP = $(SOURCES_APP:.c=.o)

SOURCES_TESTS = $(wildcard *.cpp) 
OBJECTS_TESTS = $(SOURCES_TESTS:.cpp=.o) 

LDFLAGS = -L$(CPPUTEST_HOME)/lib/ -lCppUTest -lpthread -lstdc++

all: check

rebuild: clean all

check: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET) $(OBJECTS_APP) $(OBJECTS_TESTS)

$(TARGET): $(OBJECTS_APP) $(OBJECTS_TESTS)
	$(CC) $^ -o $@ $(LDFLAGS)

.PHONY: check


