CC       = gcc
CFLAGS   = -Wall -g
LDFLAGS  =
LDLIBS   =

SRCDIR   = src
BUILDDIR = build
BINDIR   = bin

SRC      = $(wildcard $(SRCDIR)/*.c)
OBJ      = $(SRC:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
BIN      = $(SRC:$(SRCDIR)/%.c=$(BINDIR)/%)

.PHONY: all clean

all: $(BIN)

$(BIN): $(BINDIR)/% : $(BUILDDIR)/%.o
	mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(LDLIBS) $< -o $@

$(OBJ): $(BUILDDIR)/%.o : $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)
