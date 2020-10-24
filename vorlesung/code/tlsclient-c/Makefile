CC       = gcc
CFLAGS   = -Wall -g
LDFLAGS  =
LDLIBS   = -lssl -lcrypto

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SRC      = $(wildcard $(SRCDIR)/*.c)
OBJ      = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
BIN      = $(SRC:$(SRCDIR)/%.c=$(BINDIR)/%)

.PHONY: all
all: $(BIN)

$(BIN): $(BINDIR)/% : $(OBJDIR)/%.o
	$(CC) $(LDFLAGS) $(LDLIBS) $< -o $@

$(OBJ): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)
