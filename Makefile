CC = gcc
CFLAGS += -Wall #-Werror -Wextra -pedantic -g -DDEBUG
LIBS += -lm -lncurses -lmenu -lform -lpthread
ODIR = obj
EXE = hex

DEPS = screen.h menu_screen.h utils.h text_res.h \
	singleplayer_screen.h board.h disjoint_set.h \
	scoreboard.h multiplayer_screen.h server.h \
	client.h

_OBJ = main.o screen.o menu_screen.o utils.o\
	singleplayer_screen.o board.o disjoint_set.o scoreboard.o\
	multiplayer_screen.o server.o client.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

run: $(EXE)
	./$(EXE)

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
