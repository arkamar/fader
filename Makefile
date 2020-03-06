CFLAGS ?= -O2 -pipe
CFLAGS += -Wall -pedantic
CFLAGS += -Werror=implicit-function-declaration
CFLAGS += -Wimplicit-fallthrough

BIN = faderd fader

.PHONY: all
all: $(BIN)

faderd: ipc.o | arg.h ipc.h
fader: ipc.o | ipc.h

ipc.o: ipc.h

.PHONY: clean
clean:
	$(RM) $(BIN) *.o
