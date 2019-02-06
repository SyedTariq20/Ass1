CC = gcc
CFLAGS = -Wall -Wextra

SRC = main.c
OBJS = main.o

PROJ = logParse

# directory for .o files
OBJDIR := obj
DOBJS := $(addprefix $(OBJDIR)/,$(OBJS))
# ---------------------------

all: $(PROJ)

$(PROJ): $(DOBJS)
	$(CC) $(CFLAGS) -o $@ $(DOBJS) $(LIBS)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR): $(SRC)
	mkdir -p $(OBJDIR)

.PHONY: clean debug rebuild

debud: CFLAGS += -g

debug: all

rebuild: clean all

clean:
	rm -rf $(OBJDIR)
	rm -f $(PROJ)
