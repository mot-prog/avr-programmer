CC      = gcc
CFLAGS = -Wall -Wextra -MMD -MP 
LDFLAGS = -lusb-1.0
SRCS = $(wildcard *.c)
OBJS = $(addprefix build/, $(SRCS:.c=.o))
TARGET = build/prog

all: $(TARGET) | build
	$(MAKE) -C fonctions_test
	
$(TARGET): $(OBJS) | build
	$(CC) $(OBJS) $(LDFLAGS) -o $@ 

build/%.o: %.c | build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
	$(MAKE) -C fonctions_test clean
	
build:
	mkdir -p build

-include $(wildcard build/*.d)

.PHONY: clean