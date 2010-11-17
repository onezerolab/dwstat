include config.mk

DEBUG_DIR = $(ROOT)/build/$(OUT_DIR)/debug
RELEASE_DIR = $(ROOT)/build/$(OUT_DIR)/release
DEBUG_OUTPUT = $(DEBUG_DIR)/$(OUTPUT)
RELEASE_OUTPUT = $(RELEASE_DIR)/$(OUTPUT)
CLEAN_DEBUG = $(CLEAN) $(DEBUG_DIR)/*
CLEAN_REELASE = $(CLEAN) $(RELEASE_DIR)/*

INC_DIR = $(ROOT)/include
HEADERS = $(wildcard $(INC_DIR)/*.hxx)
INCS = $(INC_DIR)
INC_FLAGS = $(addprefix -I, $(INCS))

LIB_DIRS =
LIB_DIR_FLAGS = $(addprefix -L, $(LIB_DIRS))

LIB_FLAGS =  $(LIB_DIR_FLAGS) $(addprefix -l, $(LIBS))

SRC_DIR = $(ROOT)/source
SOURCES = $(wildcard $(SRC_DIR)/*.c)

DEPENDS = $(HEADERS) $(SOURCES)

DEBUG_COMMAND = $(COMPILER) $(DEBUG_FLAGS) $(INC_FLAGS) \
				-o $(DEBUG_OUTPUT) $(SOURCES) $(LIB_FLAGS)
RELEASE_COMMAND = $(COMPILER) $(RELEASE_FLAGS) $(INC_FLAGS) \
				-o $(RELEASE_OUTPUT) $(SOURCES) $(LIB_FLAGS)


all: release


debug: $(DEPENDS)
	$(DEBUG_COMMAND)


release: $(DEPENDS)
	$(RELEASE_COMMAND)
	$(STRIP) $(RELEASE_OUTPUT)
	
run: debug .run_debug


.run_debug: $(DEPENDS)
	$(DEBUG_OUTPUT)


clean:
	$(CLEAN_DEBUG)
	$(CLEAN_RELEASE)
