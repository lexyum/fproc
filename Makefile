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
OBJDIR := obj

LIBS =

# Options and flags for compiler SET FOR DEBUGGING
CFLAGS = -Og -ggdb -I$(INCLUDE) -Wall -Wextra -pedantic -std=gnu99

# Flags for linker
LDFLAGS = 

# Directory to install TARGET in
BINDIR = ./

#### End of configuration section ####

SRCS :=  $(addprefix $(SRCDIR)/,main.c fproc.c genetree.c treeops.c dsw.c)
OBJS :=  $(addprefix $(OBJDIR)/,main.o fproc.o genetree.o treeops.o dsw.o)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(addprefix $(OBJDIR)/,%.o):$(addprefix $(SRCDIR)/,%.c)
	$(CC) $(CFLAGS) -c $< -o $@

main.o: $(addprefix $(INCLUDE)/,fproc.h)
fproc.o: $(addprefix $(INCLUDE)/,genetree.h treeops.h dsw.h)
genetree.o: $(addprefix $(INCLUDE)/,genetree.h)
treeops.o: $(addprefix $(INCLUDE)/,genetree.h dsw.h)
dsw.o: $(addprefix $(INLUDE)/,genetree.h)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

# run with valgrind
.PHONY: memtest
memtest: fproc
	echo -e '#!/bin/bash\n' \
		'valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes' \
		'./fproc' > ./memtest; \
	chmod 755 memtest
