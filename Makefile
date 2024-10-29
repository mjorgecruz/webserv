

RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
END_COLOR = \033[0m
BOLD_GREEN = \033[1;32m
BOLD_YELLOW = \033[1;33m
END = \033[0m

NAME = 	webserv

CFLAGS = -Wall -Werror -Wextra -g

INCDIR:=srcs
ODIR:=obj

SRC := main.cpp  webserv.cpp HttpRequest.cpp HttpResponse.cpp Server.cpp Http.cpp FileChecks.cpp ServerConfig.cpp Location.cpp HttpConfig.cpp

OBJ := $(patsubst %.cpp, $(ODIR)/%.o,$(SRC))

CC = c++ -std=c++98

RM = rm -rf

TOTAL_FILES := $(words $(SRC))
COMPILED_FILES := $(shell if [ -d "$(ODIR)" ]; then find $(ODIR) -name "*.o" | wc -l; else echo 0; fi)

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LIBFLAGS)
	@printf "$(BOLD_GREEN)...calm champions: $$(echo "$(shell find $(ODIR) -name "*.o" | wc -l) $(TOTAL_FILES)" | awk '{printf "%.2f", $$1/$$2 * 100}')%%$(RES)\r"
	@printf "\n"
	@echo "${BOLD_GREEN}Compilation completed${END}"

$(ODIR):
	@mkdir -p $@
	
$(ODIR)/%.o: $(INCDIR)/%.cpp | $(ODIR)
	@mkdir -p $(dir $@) 
	@printf "$(BOLD_GREEN): $$(echo "$(shell find $(ODIR) -name "*.o" | wc -l) $(TOTAL_FILES)" | awk '{printf "%.2f", $$1/$$2 * 100}')%%$(RES)\r"
	@$(CC) $(CFLAGS) -c -o $@ $<

fclean: clean
	@$(RM) $(NAME)
	@echo "${RED} cpp is no more...${END}"

clean:
	@$(RM) $(OBJ)
	@$(RM) $(ODIR)

re: fclean all
