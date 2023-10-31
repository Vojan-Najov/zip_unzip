
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include
ZLIB_DIR = zlib-1.3

ZIP_SRC_DIR = $(SRC_DIR)/zip
ZIP_OBJ_DIR = $(OBJ_DIR)/zip
ZIP_SRC = $(wildcard $(ZIP_SRC_DIR)/*.c)
ZIP_OBJ = $(addprefix $(ZIP_OBJ_DIR)/, $(notdir $(ZIP_SRC:.c=.o)))

UNZIP_SRC_DIR = $(SRC_DIR)/unzip
UNZIP_OBJ_DIR = $(OBJ_DIR)/unzip
UNZIP_SRC = $(wildcard $(UNZIP_SRC_DIR)/*.c)
UNZIP_OBJ = $(addprefix $(UNZIP_OBJ_DIR)/, $(notdir $(UNZIP_SRC:.c=.o)))

CC = gcc
CFLAGS = -std=c99 -D_GNU_SOURCE -Wpedantic \
					-Wall -Wextra -Werror -I${INCLUDE_DIR} -I${ZLIB_DIR}
LIBS = -L${ZLIB_DIR} -lz

all: build test

build: zlib-1.3 zip unzip

zip: ${ZIP_OBJ}
	${CC} -g $^ ${LIBS} -o $@

unzip: ${UNZIP_OBJ}
	${CC} -g $^ -o $@ ${LIBS}

test: build tests/test.sh
	cd tests && bash test.sh

fuzz: build tests/fuzz.sh
	cd tests && bash fuzz.sh

memory_test: zlib-1.3
	$(CC) -fsanitize=address -fsanitize=leak -fsanitize=undefined \
		-I$(INCLUDE_DIR) -Izlib-1.3 $(ZIP_SRC) -L zlib-1.3 -lz -o zip
	$(CC) -fsanitize=address -fsanitize=leak -fsanitize=undefined \
		-I$(INCLUDE_DIR) -Izlib-1.3 $(UNZIP_SRC) -L zlib-1.3 -lz -o unzip
	cd tests && bash test.sh && bash fuzz.sh

$(ZIP_OBJ_DIR)/%.o: $(ZIP_SRC_DIR)/%.c | $(ZIP_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(UNZIP_OBJ_DIR)/%.o: $(UNZIP_SRC_DIR)/%.c | $(UNZIP_OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ZIP_OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(ZIP_OBJ_DIR)

$(UNZIP_OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(UNZIP_OBJ_DIR)

zlib-1.3: zlib-1.3.tar.gz
	tar -xvf $<
	cd $@ && ./configure && make

clean:
	rm -rf zlib-1.3
	rm -rf ${OBJ_DIR}
	rm -f zip
	rm -f unzip
	rm -rf tests/samples

.PHONY: build all test memory_test clean
