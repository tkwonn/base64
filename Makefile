# Source code must compile without errors against the c99 standard
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99

all: base64enc base64dec

base64enc:
	@echo "Compiling and Linking $@"
	@$(CC) $(CFLAGS) base64enc.c -o $@
	@echo "Done!"

base64dec:
	@echo "Compiling and Linking $@"
	@$(CC) $(CFLAGS) base64dec.c -o $@
	@echo "Done!"

clean:
	@echo "Cleaning all generated files"
	@rm -f base64enc base64dec
	@echo "Done!"

re: clean all