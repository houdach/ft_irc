# **************************************************************************** #
#                                   ft_irc                                     #
# **************************************************************************** #

NAME        = ircserv

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -Iinclude

# Find all source files recursively under src/
SRC_DIR     = src
OBJ_DIR     = obj
BONUS_DIR   = ./bonus/srcs

SRC         = $(shell find $(SRC_DIR) -name '*.cpp')
BONUS_SRC   = $(shell find $(BONUS_DIR) -type f -name '*.cpp')
ALL_SRC     = $(SRC) $(BONUS_SRC)
OBJ         = $(ALL_SRC:.cpp=.o)

# Colors
GREEN   = \033[0;32m
RED     = \033[0;31m
YELLOW  = \033[0;33m
RESET   = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)[OK]$(RESET) Built $(NAME)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)[CLEAN]$(RESET) Object files removed"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED)[FCLEAN]$(RESET) Removed $(NAME)"

re: fclean all

.PHONY: all clean fclean re
