
SRC_DIR = ./src
SRC_AUX_DIR = ./src/utils

OBJ_DIR = ./obj
OBJ_AUX_DIR = $(OBJ_DIR)/utils

INCLUDE_DIR = ./include

SRC_AUX = $(wildcard $(SRC_DIR)/utils/*.c)
SRC_ZIP = ${SRC_DIR}/zip.c
SRC_UNZIP = ${SRC_DIR}/unzip.c

OBJ_AUX = $(addprefix $(OBJ_AUX_DIR)/, $(notdir $(SRC_AUX:.c=.o)))
OBJ_ZIP = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_ZIP:.c=.o)))
OBJ_UNZIP = $(addprefix $(OBJ_DIR)/, $(notdir $(SRC_UNZIP:.c=.o)))

CC = gcc
CFLAGS = -I${INCLUDE_DIR}

echo:
	@echo ${SRC_AUX}
	@echo ${OBJ_AUX}
	@echo ${SRC_ZIP}
	@echo ${OBJ_ZIP}

build: zip unzip

zip: ${OBJ_AUX} ${OBJ_ZIP}
	${CC} -g $^ -o $@

unzip:
	${CC} -g $^ -o $@

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	${CC} ${CFLAGS} -c $< -o $@ 

${OBJ_AUX_DIR}/%.o: ${SRC_AUX_DIR}/%.c | $(OBJ_AUX_DIR)
	${CC} ${CFLAGS} -c $< -o $@ 

${OBJ_DIR}:
	@mkdir -p $@

${OBJ_AUX_DIR}: ${OBJ_DIR}
	@mkdir -p $@

clean:
	rm -rf ${OBJ_DIR}
	rm zip
