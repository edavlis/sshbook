SRC = src/main.c src/menu.c src/paths.c src/term.c

NAME = sshbook

RM = rm -f

DESTDIR = /usr/local/bin/

CC = gcc

OBJS=	$(SRC:.c=.o)

CFLAGS = -Wall -Wextra -pedantic -g -I include


all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -g -o $(NAME) $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(OBJS)

fclean:
	$(RM) $(OBJS)
	$(RM) $(NAME)

install: $(NAME)
	install -m 755 $(NAME) $(DESTDIR)/$(NAME)

uninstall: $(NAME)
	$(RM) $(DESTDIR)$(NAME)

.PHONY: all clean install uninstall
