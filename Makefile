CFLAGS=-std=c11 -Wall -Werror
CC=/usr/bin/clang
OBJS = parser.o

parser: $(OBJS)

run: parser
	@./parser

clean:
	@rm -rf parser *.o
