PROJECTNAME = FallingSand
OUTPUT_DIR = build

INCLUDE_DIRS = -Iinclude/SDL2
LIB_DIRS = -Llib

LIBS = -lmingw32 -lSDL2main -lSDL2

SRC = $(wildcard src/*.cpp)

default:
	g++ $(SRC) -o $(OUTPUT_DIR)/$(PROJECTNAME) $(INCLUDE_DIRS) $(LIB_DIRS) $(LIBS)