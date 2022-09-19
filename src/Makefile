CC 						= gcc -std=c11
OS 						= $(shell uname)
# flags #
CFLAG 					= -Wall -Wextra -Werror
GCOV 					= --coverage
LIBS 					= $(shell pkg-config --libs --cflags check) -lm
# libraries #
ifeq ($(OS), Linux)
	OPEN 				= xdg-open
else
	OPEN 				= open
endif
# path and files #
LIB_NAME 				= decimal.a
T_NAME					= decimal_test
T_SOURCE 				= $(T_NAME).c
T_HEADER 				= $(T_NAME).h
T_OBJECT 				= $(T_SOURCE:%.c=%.o)

SOURCES 				= $(filter-out $(T_SOURCE), $(wildcard *.c))
HEADERS 				= $(filter-out $(T_HEADER), $(wildcard *.h))
OBJECTS 				= $(SOURCES:%.c=%.o)

# targets #
all: test

$(LIB_NAME): $(OBJECTS) $(HEADERS)
	@ar -rcs $(LIB_NAME) $?
	@ranlib $@
	@rm -rf *.o
	@echo "Successfully. The library was created."

# targets for tests #
test: $(LIB_NAME)
	@echo " "
	@$(CC) $(T_SOURCE) -o $(T_NAME) $(LIBS) $(LIB_NAME)
	@./$(T_NAME)

gcov_report:
	@$(CC) $(GCOV) $(T_SOURCE) $(SOURCES) $(LIBS) $(LIB_NAME) -o coverage
	@./coverage
	@lcov --no-external -o report.info -c -q -d .
	@genhtml --legend --title "decimal" report.info -o report
	@rm -rf *.gcda *.gcno *.gz *.out *.info coverage

# assembly #
clean:
	@rm -rf *.o *.a *.gcno *.gcda *.info *.out coverage report $(T_NAME)
	@echo "Successfully. Everything was deleted."

rebuild: clean all

open_report:
	$(OPEN) report/index.html

# for debug #
check: cppcheck

cppcheck:
	@echo "------------  CPPCheck  -------------"
	@cppcheck --enable=warning --suppress=missingIncludeSystem *.c *.h

memory_check:
ifeq ($(OS), Linux)
	@echo "------------  Valgrind  -------------"
	@CK_FORK=no valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(T_NAME)
else
	@echo "-------------  Leaks  ---------------"
	@CK_FORK=no leaks --atExit -- ./$(T_NAME)
endif
