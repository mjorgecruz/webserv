

RED = \033[31m
GREEN = \033[32m
YELLOW = \033[33m
END_COLOR = \033[0m
BOLD_GREEN = \033[1;32m
BOLD_YELLOW = \033[1;33m
END = \033[0m

NAME = 	webserv

DEFAULT_INDEX = index.html
DEFAULT_DIR = $(HOME)/html
DEFAULT_CONF = default.conf
ERRORS = 204 301 403 404 409 500 502 503 504

CFLAGS = -Wall -Werror -Wextra -g

INCDIR:=srcs
ODIR:=obj

SRC := main.cpp  webserv.cpp HttpRequest.cpp HttpResponse.cpp Server.cpp  Socket.cpp Http.cpp FileChecks.cpp Location.cpp

OBJ := $(patsubst %.cpp, $(ODIR)/%.o,$(SRC))

CC = c++ -std=c++98

RM = rm -rf

TOTAL_FILES := $(words $(SRC))
COMPILED_FILES := $(shell if [ -d "$(ODIR)" ]; then find $(ODIR) -name "*.o" | wc -l; else echo 0; fi)

all: $(NAME)

$(NAME): $(OBJ) defaults 204 301 
	@mkdir -p $(dir $@) 
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

defaults:
	@mkdir -p $(DEFAULT_DIR)
	@echo "This is a test page" > $(DEFAULT_DIR)/index.html
	@echo "Server { " >> $(DEFAULT_DIR)/default.conf
	@echo "	listen 127.0.0.1:8080; " >> $(DEFAULT_DIR)/default.conf
	@echo "	server_name example.com;" >> $(DEFAULT_DIR)/default.conf
	@echo "	allow_methods GET POST DELETE;" >> $(DEFAULT_DIR)/default.conf
	@echo "	index index.html;" >> $(DEFAULT_DIR)/default.conf
	@echo "	location / {" >> $(DEFAULT_DIR)/default.conf
	@echo "		index index.html" >> $(DEFAULT_DIR)/default.conf
	@echo "	}" >> $(DEFAULT_DIR)/default.conf
	@echo "} \n" >> $(DEFAULT_DIR)/default.conf
	
	
204: defaults
	@echo "<!DOCTYPE html>" > $(DEFAULT_DIR)/204.html
	@echo "<html lang=\"en\">" >> $(DEFAULT_DIR)/204.html
	@echo "<head>" >> $(DEFAULT_DIR)/204.html
	@echo "	<meta charset=\"UTF-8\">" >> $(DEFAULT_DIR)/204.html
	@echo "	<title>No Content</title>" >> $(DEFAULT_DIR)/204.html
	@echo "</head>" >> $(DEFAULT_DIR)/204.html
	@echo "<body>" >> $(DEFAULT_DIR)/204.html
	@echo "	<h1>204 No Content</h1>" >> $(DEFAULT_DIR)/204.html
	@echo "	<p>The server successfully processed the request, but is not returning any content.</p>" >> $(DEFAULT_DIR)/204.html
	@echo "</body>" >> $(DEFAULT_DIR)/204.html
	@echo "</html>" >> $(DEFAULT_DIR)/204.html

301: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/301.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/301.html
	@echo "<head>" >> $(HOME)/html/301.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/301.html
	@echo "	<title>Moved Permanently</title>" >> $(HOME)/html/301.html
	@echo "</head>" >> $(HOME)/html/301.html
	@echo "<body>" >> $(HOME)/html/301.html
	@echo "	<h1>301 Moved Permanently</h1>" >> $(HOME)/html/301.html
	@echo "	<p>The requested resource has been permanently moved to a new location.</p>" >> $(HOME)/html/301.html
	@echo "</body>" >> $(HOME)/html/301.html
	@echo "</html>" >> $(HOME)/html/301.html

403: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/403.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/403.html
	@echo "<head>" >> $(HOME)/html/403.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/403.html
	@echo "	<title>Forbidden</title>" >> $(HOME)/html/403.html
	@echo "</head>" >> $(HOME)/html/403.html
	@echo "<body>" >> $(HOME)/html/403.html
	@echo "	<h1>403 Forbidden</h1>" >> $(HOME)/html/403.html
	@echo "	<p>You do not have permission to access this resource.</p>" >> $(HOME)/html/403.html
	@echo "</body>" >> $(HOME)/html/403.html
	@echo "</html>" >> $(HOME)/html/403.html

404: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/404.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/404.html
	@echo "<head>" >> $(HOME)/html/404.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/404.html
	@echo "	<title>Not Found</title>" >> $(HOME)/html/404.html
	@echo "</head>" >> $(HOME)/html/404.html
	@echo "<body>" >> $(HOME)/html/404.html
	@echo "	<h1>404 Not Found</h1>" >> $(HOME)/html/404.html
	@echo "	<p>The requested resource could not be found on this server.</p>" >> $(HOME)/html/404.html
	@echo "</body>" >> $(HOME)/html/404.html
	@echo "</html>" >> $(HOME)/html/404.html

409: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/409.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/409.html
	@echo "<head>" >> $(HOME)/html/409.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/409.html
	@echo "	<title>Conflict</title>" >> $(HOME)/html/409.html
	@echo "</head>" >> $(HOME)/html/409.html
	@echo "<body>" >> $(HOME)/html/409.html
	@echo "	<h1>409 Conflict</h1>" >> $(HOME)/html/409.html
	@echo "	<p>The request could not be completed due to a conflict with the current state of the resource.</p>" >> $(HOME)/html/409.html
	@echo "</body>" >> $(HOME)/html/409.html
	@echo "</html>" >> $(HOME)/html/409.html

500: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/500.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/500.html
	@echo "<head>" >> $(HOME)/html/500.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/500.html
	@echo "	<title>Internal Server Error</title>" >> $(HOME)/html/500.html
	@echo "</head>" >> $(HOME)/html/500.html
	@echo "<body>" >> $(HOME)/html/500.html
	@echo "	<h1>500 Internal Server Error</h1>" >> $(HOME)/html/500.html
	@echo "	<p>An unexpected error occurred on the server.</p>" >> $(HOME)/html/500.html
	@echo "</body>" >> $(HOME)/html/500.html
	@echo "</html>" >> $(HOME)/html/500.html

502: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/502.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/502.html
	@echo "<head>" >> $(HOME)/html/502.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/502.html
	@echo "	<title>Bad Gateway</title>" >> $(HOME)/html/502.html
	@echo "</head>" >> $(HOME)/html/502.html
	@echo "<body>" >> $(HOME)/html/502.html
	@echo "	<h1>502 Bad Gateway</h1>" >> $(HOME)/html/502.html
	@echo "	<p>The server received an invalid response from the upstream server.</p>" >> $(HOME)/html/502.html
	@echo "</body>" >> $(HOME)/html/502.html
	@echo "</html>" >> $(HOME)/html/502.html

503: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/503.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/503.html
	@echo "<head>" >> $(HOME)/html/503.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/503.html
	@echo "	<title>Service Unavailable</title>" >> $(HOME)/html/503.html
	@echo "</head>" >> $(HOME)/html/503.html
	@echo "<body>" >> $(HOME)/html/503.html
	@echo "	<h1>503 Service Unavailable</h1>" >> $(HOME)/html/503.html
	@echo "	<p>The server is currently unable to handle the request due to temporary overloading or maintenance.</p>" >> $(HOME)/html/503.html
	@echo "</body>" >> $(HOME)/html/503.html
	@echo "</html>" >> $(HOME)/html/503.html

504: defaults
	@echo "<!DOCTYPE html>" > $(HOME)/html/504.html
	@echo "<html lang=\"en\">" >> $(HOME)/html/504.html
	@echo "<head>" >> $(HOME)/html/504.html
	@echo "	<meta charset=\"UTF-8\">" >> $(HOME)/html/504.html
	@echo "	<title>Gateway Timeout</title>" >> $(HOME)/html/504.html
	@echo "</head>" >> $(HOME)/html/504.html
	@echo "<body>" >> $(HOME)/html/504.html
	@echo "	<h1>504 Gateway Timeout</h1>" >> $(HOME)/html/504.html
	@echo "	<p>The server did not receive a timely response from the upstream server.</p>" >> $(HOME)/html/504.html
	@echo "</body>" >> $(HOME)/html/504.html
	@echo "</html>" >> $(HOME)/html/504.html

re: fclean all
