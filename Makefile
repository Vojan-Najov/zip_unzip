
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
CFLAGS = -std=c99 -D_GNU_SOURCE -Wall -Wextra -Werror -I${INCLUDE_DIR} -I${ZLIB_DIR}
LIBS = -L${ZLIB_DIR} -lz

#SRC_DIR = ./src
#SRC_AUX_DIR = ./src/utils
#OBJ_DIR = ./obj
#OBJ_AUX_DIR = $(OBJ_DIR)/utils
#SRC_AUX = $(wildcard $(SRC_DIR)/utils/*.c)
#SRC_ZIP = ${SRC_DIR}/zip.c
#SRC_UNZIP = ${SRC_DIR}/unzip.c
#OBJ_AUX = $(addprefix $(OBJ_AUX_DIR)/, $(notdir $(SRC_AUX:.c=.o)))
#OBJ_ZIP = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_ZIP:.c=.o)))
#OBJ_UNZIP = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_UNZIP:.c=.o)))

echo:
	@echo ${ZIP_SRC}
	@echo ${ZIP_OBJ}

build: zlib-1.3 zip unzip

zip: ${ZIP_OBJ}
	${CC} -g $^ ${LIBS} -o $@

unzip: ${UNZIP_OBJ}
	${CC} -g $^ -o $@ ${LIBS}

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


#${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
#	${CC} ${CFLAGS} -c $< -o $@ 
#${OBJ_AUX_DIR}/%.o: ${SRC_AUX_DIR}/%.c | $(OBJ_AUX_DIR)
#	${CC} ${CFLAGS} -c $< -o $@ 
#${OBJ_DIR}:
#	@mkdir -p $@
#${OBJ_AUX_DIR}: ${OBJ_DIR}
#	@mkdir -p $@

.PHONY: build all test clean
