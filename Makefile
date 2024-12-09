# Source code must compile without errors against the c99 standard
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99

all: base64enc base64dec

base64enc:
	@$(CC) $(CFLAGS) base64enc.c -o $@

base64dec:
	@$(CC) $(CFLAGS) base64dec.c -o $@

clean:
	@rm -f base64enc base64dec

re: clean all