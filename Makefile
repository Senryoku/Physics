CXX = g++
OBJ = obj/
SRC = src/
BIN = bin/
TESTS = tests/
POINTC = $(wildcard $(SRC)*/*.c) $(wildcard $(SRC)*.c) 
POINTCPP = $(wildcard $(SRC)*/*.cpp) $(wildcard $(SRC)*.cpp) 
POINTOP := $(POINTC:.c=.o) $(POINTCPP:.cpp=.o)
POINTO = $(patsubst src/%,$(OBJ)%,$(POINTOP)) #$(POINTOP:src=obj)

OPT := -Wall -pedantic -Wno-long-long -O2 -g -I "$(SRC)"


ifeq ($(SHELL), sh.exe) 
OS := Win
else
OS := $(shell uname)
endif

ifeq ($(OS), Linux)
RM = rm
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lGL
endif
ifeq ($(OS), Darwin)
RM = rm
LIBS := -framework sfml-system -framework sfml-window -framework sfml-graphics -framework OpenGL
endif
ifeq ($(OS), Win)
RM = del
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lopengl32
endif

all : run

dirs : 
ifeq ($(OS), Darwin)
	@./configure.sh bin obj src
endif
ifeq ($(OS), Linux)
	@./configure.sh bin obj src
endif

.PHONY : dirs

windirs :
	mkdir bin
	mkdir obj
.PHONY : windirs

$(OBJ)%.o : $(SRC)%.cpp
	@echo "Compilation for $^"
	@$(CXX) $(OPT) $^ -c -o $@
	

run : dirs test
ifeq ($(OS), Win)
	$(BIN)test.exe
else
	./$(BIN)test
endif
.PHONY : run

test : $(POINTO) $(OBJ)PhysicsMain.o
	@echo "Linking $@"
	@$(CXX) $(OPT) $^ -o $(BIN)$@ $(LIBS)

valgrind : all
	valgrind --leak-check=full --tool=memcheck ./$(BIN)test
.PHONY : valgrind


clean:
	@$(RM) -vf $(OBJ)*.o $(OBJ)*/*.o $(BIN)*
.PHONY : clean


info:
	@echo "Lignes\tMots\tOctets"
	@cd src ; \
	find -E . -regex "(.+)\.(h|cpp|c|hpp)" -print0 | xargs -0 cat | wc ; \
	cd ..
.PHONY : info
#find -E src/ -regex "(.+)\.(h|cpp|c|hpp)" -exec cat {} \; | wc -l | (read A ; echo "Lignes dans le projet: $A")
#find -E src/ -regex "(.+)\.(h|cpp|c|hpp)" -exec cat {} \; | wc | (read L W B ; echo "Le projet a $L lignes, $W mots et pèse $B octets")
