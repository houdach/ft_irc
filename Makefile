# **************************************************************************** #
#                                   ft_irc                                     #
# **************************************************************************** #

NAME        = ircserv

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++98 -Iinclude

SRC_DIR     = src
BONUS_DIR   = bonus/srcs
OBJ_DIR     = obj

SRC         = $(shell find $(SRC_DIR) -type f -name "*.cpp")
BONUS_SRC   = $(shell find $(BONUS_DIR) -type f -name "*.cpp")
ALL_SRC     = $(SRC) $(BONUS_SRC)

OBJ         = $(ALL_SRC:%.cpp=$(OBJ_DIR)/%.o)


all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "[OK] Built $(NAME)"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ_DIR)
	@echo "[CLEAN] Object files removed"

fclean: clean
	@rm -f $(NAME)
	@echo "[FCLEAN] Removed $(NAME)"

re: fclean all

.PHONY: all clean fclean re
