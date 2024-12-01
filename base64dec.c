#include <stdio.h>
#include <stdint.h> 
#include <errno.h>
#include <err.h>

static int base64_char_to_value(char c) {
    if ('A' <= c && c <= 'Z') return c - 'A';
    if ('a' <= c && c <= 'z') return c - 'a' + 26;
    if ('0' <= c && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return -1;
    return -2;
}

int main(int argc, char *argv[]) {
    if (argc > 2) err(errno = EINVAL, "SYNOPSIS: base64dec [FILE]");

    FILE *fp = stdin;
    if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror("Error: ");
            return -1;
        }
    }

    char input[4];
    uint8_t output[3];
    int input_pos = 0;
    int c;

    while ((c = fgetc(fp)) != EOF) {
        // Skip space and new line
        if (c == '\n' || c == '\r' || c == ' ') continue;

        input[input_pos++] = c;

        if (input_pos == 4) {
            int values[4];
            for (size_t i = 0; i < 4; ++i) {
                values[i] = base64_char_to_value(input[i]);
                if (values[i] == -2) {
                    fprintf(stderr, "Error: Invalid character '%c' in input\n", input[i]);
                    return -1;
                }
            }

            output[0] = (values[0] << 2) | (values[1] >> 4);
            output[1] = (values[1] << 4) | (values[2] >> 2);
            output[2] = (values[2] << 6) | values[3];

            size_t valid_bytes = 3;
            if (values[3] == -1) valid_bytes = 2;
            if (values[2] == -1) valid_bytes = 1;

            fwrite(output, sizeof(uint8_t), valid_bytes, stdout);
            input_pos = 0;
        }
    }

    if (input_pos > 0) {
        fprintf(stderr, "Error: Input length not a multiple of 4\n");
        return -1;
    }

    fclose(fp);
    return 0;
}
