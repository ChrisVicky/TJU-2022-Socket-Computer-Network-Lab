SRC_DIR := src
OBJ_DIR := obj
# all src files
SRC := $(wildcard $(SRC_DIR)/*.c)
# all objects
OBJ := $(OBJ_DIR)/y.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/buffer.o $(OBJ_DIR)/response.o $(OBJ_DIR)/example.o $(OBJ_DIR)/logger.o
# all yacc, lex and parse objects
YLOOBJ := $(OBJ_DIR)/y.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/buffer.o $(OBJ_DIR)/logger.o  $(OBJ_DIR)/response.o 
# all binaries
BIN := example echo_server echo_client
# C compiler
CC  := gcc
# C PreProcessor Flag
CPPFLAGS := -Iinclude
# compiler flags
CFLAGS   := -g -Wall
# DEPS = parse.h y.tab.h

default: all
all : example echo_server echo_client 

example: $(OBJ)
	$(CC) $^ -o $@

tarit:
	tar -cvf webServerStartCodes-new.tar ./ 

test_example: example
	@for f in $(shell ls samples); do \
		echo "=====Test file" $$f "========="; \
		./example samples/$$f | grep Segmentation --color ; \
		echo "---------------------------------------\n"; \
	done	


$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l
	flex -o $@ $^

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/parser.y
	yacc -d $^
	mv y.tab.c $@
	mv y.tab.h $(SRC_DIR)/y.tab.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# response.o should be compiled after buffer.o
$(OBJ_DIR)/response.o: $(OBJ_DIR)/buffer.o

echo_server: $(YLOOBJ) $(OBJ_DIR)/echo_server.o 
	$(CC) -Werror $^ -o $@

echo_client: $(YLOOBJ) $(OBJ_DIR)/echo_client.o
	$(CC) -Werror $^ -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ) $(BIN) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.*
	$(RM) -r $(OBJ_DIR)
	$(RM) tags
	$(RM) webServerStartCodes-new.tar
