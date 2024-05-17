# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/05/16 18:24:38 by eseferi           #+#    #+#              #
#    Updated: 2024/05/16 19:59:06 by eseferi          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME =  webserv
COMP = c++
SRC = $(shell find src -name "*.cpp")
OBJ = $(SRC:src/%.cpp=obj/%.o)
INCL = $(shell find inc -type d)
FLAGS = -Wall -Wextra -Werror  $(addprefix -I,$(INCL)) -Wshadow  -Wno-shadow -g -std=c++98
GREEN = \033[32m
RED = \033[31m
YELLOW = \033[33m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
		@$(COMP) $(FLAGS) $^ -o $@
		@echo "$(GREEN)Compiled$(RESET)"

obj/%.o: src/%.cpp
		@mkdir -p $(@D)
		@$(COMP) $(FLAGS) -c $< -o $@

clean:
		@if [ -d obj ]; then \
			rm -rf obj; \
			echo "$(RED)Object files cleaned$(RESET)"; \
		else \
			echo "$(YELLOW)No Object file to clean$(RESET)"; \
		fi

fclean: clean
		@if [ -f $(NAME) ]; then \
			rm -rf $(NAME); \
			echo "$(RED)Executable file cleaned$(RESET)"; \
		else \
			echo "$(YELLOW)No executable file to clean$(RESET)"; \
		fi

debug: 
	$(MAKE) FLAGS="$(FLAGS) -DDEBUG=1" all

re: fclean all

.PHONY: all clean fclean re%     