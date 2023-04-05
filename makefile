CC = gcc
OBJS = chess.o chessSystemTestsExample.o game.o participance.o player.o tournament.o
EXEC = chess
CFLAGS = -std=c99 -Wall -pedantic-errors -Werror -DNDEBUG
LIBS = -L. -lmap

$(EXEC) : $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

chess.o: chessSystem.c chessSystem.h map.h tournament.h game.h player.h participance.h
	$(CC) $(CFLAGS) -c -o $@ $<
chessSystemTestsExample.o: tests/chessSystemTestsExample.c chessSystem.h test_utilities.h
	$(CC) $(CFLAGS) -c -o $@ $<
game.o: game.c chessSystem.h map.h tournament.h game.h player.h participance.h
participance.o: participance.c chessSystem.h map.h tournament.h game.h player.h participance.h
player.o: player.c chessSystem.h map.h tournament.h game.h player.h participance.h
tournament.o: tournament.c chessSystem.h map.h tournament.h game.h player.h participance.h

clean:
	rm -f $(OBJS) $(EXEC)