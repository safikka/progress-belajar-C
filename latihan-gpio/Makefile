#
#	Makefile Info	: SHIKI_MAKE_FORMAT
#	ver         	: 1.01.19.12.19.16
#	author      	: Jaya Wikrama, S.T.
#	e-mail      	: jayawikrama89@gmail.com
#	Copyright (c) 2019 HANA,. Jaya Wikrama
#
#	1. Power full Makefile with separated object and executable file
#	2. Support for separated library directory
#	3. Auto generate for all structure directory
#	4. Save record for all history program
#
#	if you are using git, dont forget to run make init to ignore
#	BUILD_DIRECTORY and OBJ_DIRECTORY
#

PROJECT_DIRECTORY = $(shell pwd)

IFLAGS = -I$(PROJECT_DIRECTORY)/src

LIB_PKG_CONFIG = `pkg-config --cflags --libs gtk+-3.0 glib-2.0`

# Install dependency libpq5 
# sudo apt install libpq5=12.7-0ubuntu0.20.04.1
# lalu install libpq-dev
INCLUDE = $(LIB_PKG_CONFIG) \
          -lbcm2835

SOURCE   = 	src/main.c

TARGET             = blink
CC                 = gcc
ACU_CROSS_PLATFORM =
WARN_OPTION        = -Wall
BUILD_DIRECTORY    = build
OBJ_DIRECTORY      = obj
OBJECTS            = $(patsubst %.c,$(OBJ_DIRECTORY)/%.o,$(SOURCE))

TIME_CREATED   = `date +%y.%m.%d_%H.%M.%S`
GIT_IGNORE_CMD = `cat .gitignore | grep -v $(OBJ_DIRECTORY) | grep -v $(BUILD_DIRECTORY)`

vpath $(TARGET) $(BUILD_DIRECTORY)
vpath %.o $(OBJ_DIRECTORY)

$(TARGET):$(OBJECTS)
	@echo
	@echo "  \033[1;33mCreating executable file : $@\033[0m"
	$(CC) $(WARN_OPTION) $(OBJECTS) -o $(BUILD_DIRECTORY)/$@ $(INCLUDE) $(IFLAGS)
	@cp $(BUILD_DIRECTORY)/$@ .
	@echo "\033[1;33m-------------------------FINISH-------------------------- \033[0m"

$(OBJ_DIRECTORY)/%.o:%.c
	@echo
	@echo "  \033[1;32mCompiling: $<\033[0m"
	$(call init_proc);
	$(CC) $(WARN_OPTION) -c $< -o $@ $(INCLUDE) $(IFLAGS)

init:
	$(call init_proc);
	@echo "$(GIT_IGNORE_CMD)" > .gitignore
	@echo "$(OBJ_DIRECTORY)/" >> .gitignore
	@echo "$(BUILD_DIRECTORY)/" >> .gitignore

clean:
	@rm -fv `find . -type f -name '*.o'`
	@rm -fv ./$(BUILD_DIRECTORY)/$(TARGET)

define init_proc
	@mkdir -p $(OBJ_DIRECTORY)
	@mkdir -p $(BUILD_DIRECTORY)
	@find . -type f -name '*.c' -printf '%h\n' |sort -u | grep -v '$(BUILD_DIRECTORY)' | grep -v '$(OBJ_DIRECTORY)' > dir.struct
	@cd $(OBJ_DIRECTORY) && xargs mkdir -p < ../dir.struct
endef
