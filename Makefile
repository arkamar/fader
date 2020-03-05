CFLAGS ?= -O2 -pipe
CFLAGS += -Wall -pedantic
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Wimplicit-fallthrough

BIN = faderd

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN) *.o
