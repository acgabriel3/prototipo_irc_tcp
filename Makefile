EXE = exec
OBJ_DIR = bin
SRC_DIR = src
# Note que essa pasta Dev_libs deve existir no seu computador e possuir a biblioteca externa que vc usará
INC_DIR = 
LIB_DIR = 
SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
CXX = g++
CPPFLAGS = -std=c++11
CFLAGS = -g -Wall -pedantic
INC_PATH = -I$(INC_DIR)
LIB_PATH = -L$(LIB_DIR)
LINK_FLAGS = 

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $^ $(INC_PATH) $(LIB_PATH) $(CPPFLAGS) $(CFLAGS) $(LINK_FLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< $(INC_PATH) $(LIB_PATH) $(CPPFLAGS) $(CFLAGS) $(LINK_FLAGS) -o $@

clean:
	$(RM) *~ $(OBJ)
