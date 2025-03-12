NAME = ft_traceroute

CC = cc

CFLAGS = -Wall -Werror -Wextra -g3

RM = rm

OBJ = $(SRC:.c=.o)

SRC =	srcs/main.c \
		srcs/utils.c
		
INCL =	includes/ft_traceroute.h

all: $(NAME)

bonus: $(NAME)_bonus

%.o: %.c 
	@$(CC) $(CFLAGS) -c $< -o $@
		
$(NAME): $(OBJ)
	@echo " \033[0;31mCompiling $(NAME)...\033[0m"
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo " \033[0;32mSuccess\033[0m"

clean:
	@$(RM) -f $(OBJ)
	@echo " \033[0;32mCleaning done!\033[0m"
	
fclean: clean
	@$(RM) -f $(NAME) 
	@$(RM) -f $(NAME)_bonus

re: fclean all

	
.PHONY: clean fclean
