NAME        := Buffer
CXX         := g++
CXXFLAGS    := -std=c++2a
CXXVERSION	:= 11

SRC_DIR		:= .
LIBS        := -I.

SRCS        := ./main.cpp
OBJS        := $(SRCS:%.cpp=%.o)

.PHONY: all

$(NAME): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

$(OBJS): $(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX)-$(CXXVERSION) $(CXXFLAGS) -c -o $@ $< $(LIBS)