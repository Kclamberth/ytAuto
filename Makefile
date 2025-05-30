CC       := gcc
CFLAGS   := -std=c17 -Wall -Wextra -pedantic -O2 -D_GNU_SOURCE -Iinclude -MMD -MP
LDFLAGS  :=
BIN      := ytAuto

SRC      := $(wildcard src/*.c)
OBJ      := $(patsubst src/%.c, build/%.o, $(SRC))
DEP      := $(OBJ:.o=.d)

$(BIN): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	@mkdir -p $@

.PHONY: clean
clean:
	rm -rf build $(BIN)

-include $(DEP)

