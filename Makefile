# Makefile
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I.
LIBS = -lSDL2

# Diretórios
SRC_DIR = .
GEOMETRY_DIR = geometry
GRAPHICS_DIR = graphics
INPUT_DIR = input
POLYGONS_DIR = polygons

# Arquivos fonte com caminhos completos
SRCS = \
	$(SRC_DIR)/main.cpp \
	$(GEOMETRY_DIR)/geometry.cpp \
	$(GRAPHICS_DIR)/graphics.cpp \
	$(INPUT_DIR)/input.cpp \
	$(POLYGONS_DIR)/polygon.cpp

# Objetos (mesmo nome mas com .o)
OBJS = $(SRCS:.cpp=.o)
TARGET = projeto

# Alvo principal
all: $(TARGET)

# Regra para ligar os objetos
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

# Regra para compilar arquivos .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean