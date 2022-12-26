CFLAGS = -Wall -Wextra -std=c11 -g
CPPFLAGS = -I.
EXEC = reversi

.PHONY : all clean help

all : $(EXEC)

$(EXEC) : reversi.o board.o player.o
	$(CC) $(CFLAGS) -o $@ $^

reversi.o : reversi.c reversi.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

board.o : board.c ../include/board.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<	

player.o : player.c ../include/player.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<	

clean : 
	rm -rf *.o $(EXEC)	

help :
	@echo 'Makefile usage :'
	@echo ' . all:     Run the whole build of reversi;'
	@echo ' . reversi: Build the executable file from reversi.c;'
	@echo ' . clean:   Remove all files produced by the compilation;'
	@echo ' . help:    Display the main target of this Makefile with a short description.'	