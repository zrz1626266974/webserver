APP_NAME = main

TOP_DIR = $(shell pwd)
MODULES = . socket

SOURCE_FILES = $(foreach module, $(MODULES), $(wildcard $(module)/*.cpp))
INCLUDES = $(foreach module, $(MODULES), -I$(module)/)

OBJS = $(SOURCE_FILES:.cpp=.o)

CC = g++
CXXFLAGS = -c -g $(INCLUDES) -std=c++11
LDFLAGS = 




all: $(OBJS)
	@echo "$(CC) -o main $^ $(LDFLAGS)"
	@$(CC) -o $(APP_NAME) $^ $(LDFLAGS)
	
$(OBJS): %.o: %.cpp
	@echo "$(CC) -o $@ $(CXXFLAGS) $<"
	@$(CC) -o $@ $(CXXFLAGS) $<
	
.PHONY: clean
clean:
	@echo $(SOURCE_FILES)
	@echo '=================='
	@echo $(OBJS)
	@echo '=================='
	@echo $(CXXFLAGS)
	@rm -rf $(OBJS) $(APP_NAME)
