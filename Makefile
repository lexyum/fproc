#!/usr/bin/make -f
#
# Makefile for fproc - FASTA file processor

SHELL = /bin/sh

#### Start of configuration section ####
TARGET = fproc
INSTALL = /usr/bin/install -c

CC = gcc

INCLUDE := include
SRCDIR := src

LIBS =

# Options and flags for compiler SET FOR DEBUGGING
CFLAGS = -Og -ggdb -I$(INCLUDE) -Wall -Wextra -pedantic -std=gnu99

# Flags for linker
LDFLAGS = 

# Directory to install TARGET in
BINDIR = ./

#### End of configuration section ####

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

# Target rule
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

-include $(DEP) # include C dependency files

# rule to generate dep files using C preprocessor
%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.o: %.c, %.d
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: 
clean:
	rm -f $(OBJ) $(DEP) 

.PHONY: cleanobj
cleanobj:
	rm -f $(OBJ)

.PHONY: cleandep
cleandep:
	rm -f $(DEP)
