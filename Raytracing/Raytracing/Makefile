BIN = raytracer
SRC = \
	bitmap.c \
	color.c \
	geometry.c \
	list.c \
	math.c \
	raytrace.c \
	scene.c \
	util.c \
	main.c

CC	     = cc
CFLAGS   = -g -std=c99 -Wall -Wextra -Werror -O3
LFLAGS   = -lm


GDB    = gdb
RM     = rm -f

OBJS = $(SRC:%.c=%.o)

all: $(BIN)


$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@

# pull in dependencies for .o files
-include $(OBJS:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
	$(CC) -MM $(CFLAGS) $*.c > $*.d


debug: $(BIN)
	$(GDB) ./$(BIN)

run: $(BIN)
	./$(BIN)

clean:
	$(RM) $(BIN) $(OBJS) *.d

.PHONY: clean debug run 
