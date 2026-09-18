NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread -Iincludes

SRCS = src/main.c \
       src/parser.c \
       src/utils.c \
       src/init.c \
       src/scheduler.c \
       src/scheduler_utils.c \
       src/scheduler_thread.c \
       src/coder_actions.c \
       src/coder_actions_utils.c \
       src/monitor.c \
       src/simulation.c

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

valgrind: $(NAME)
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) $(ARGS)

.PHONY: all clean fclean re