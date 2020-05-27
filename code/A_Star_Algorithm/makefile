SRCS := $(shell find *.cpp)
TARGET_EXEC ?= run.out
FLAGS ?= -g
CC = g++

compile:
	$(CC) $(SRCS) -o $(TARGET_EXEC) $(FLAGS)
clean:
	$(RM) -rf $(TARGET_EXEC)