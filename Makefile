CC := gcc
SRCDIR := src
BINDIR := bin
BUILDDIR := build
TARGET := game_of_life

SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f -name "*.$(SRCEXT)")
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g#-Wall
INC := -I include -I /usr/local/include/SDL2
LIB := -L /usr/local/lib -l SDL2 -l SDL2_ttf

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@mkdir -p $(BINDIR)
	@echo " $(CC) $(LIB) $^ -o $(BINDIR)/$(TARGET)"; $(CC) $(LIB) $^ -o $(BINDIR)/$(TARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(BINDIR)

.PHONY: clean
