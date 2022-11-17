#include <stdio.h>
#include <stdint.h> /* for uint8_t */
#include <string.h> /* for strcmp */
#include <unistd.h> /* for size_t */
#include <err.h>
#include <errno.h>

#define arraylen(x) (sizeof x / sizeof *x)

#ifndef UINT8_MAX
#error "No support for uint8_t"
#endif

static char const base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

int main(int argc, char *argv[]) {
  if(argc > 2) err(errno=EINVAL, "SYNOPSIS: base64enc [FILE]");

  /*
   * If `FILE` is omitted, encode from stdin;
   * if `FILE` is "-", also encode from stdin;
   * if `FILE` is the name of a file, encode it.
   */
  FILE *fp = stdin;
  if(argc == 2) {
    /* Open file for reading */
    fp = fopen(argv[1], "r");
    if(fp == NULL) {
        perror("Error: ");
        return(-1);
    }
  }

  size_t line_character = 0;
  while (1) {
    uint8_t buf[3] = {0};
    /* read three BYTES (8bit*3 = 24bit)*/
    size_t howMany = fread(&buf, sizeof(uint8_t), arraylen(buf), fp);

    // Printing data to check validity
    // printf("Data read from file: %s \n", buf);
    // printf("Elements read: %d\n", howMany);

    /* fread returns short item count on eof OR error if this number differs from the nmemb param */
    if (howMany < arraylen(buf)) {
      if (ferror(stdin)) {
          err(errno, "fread");
      }
      /* note, we can now assume that feof(stdin) is true without explicitly checking */
      if (howMany == 0) break; // Hit end of file without any new characters in buf, exit
    }
    
    // contains index for base64_chars table
    uint8_t t_idx[4] = {0};

    // 8bits*3BYTES -> 6bits*4BYTES -> 8bits*4BYTES with two 0's at front
    //  [00000011] [11112222] [22333333]
    //  [00000000] [00111111] [00222222] [00333333]
    t_idx[0] = buf[0] >> 2;
    // 0x03 = 00000011
    t_idx[1] = ((buf[0] & 0x03) << 4) | (buf[1] >> 4);
    // 0x0f = 00001111
    t_idx[2] = ((buf[1] & 0x0f) << 2) | (buf[2] >> 6);
    // 0x3f = 00111111
    t_idx[3] = buf[2] & 0x3f;

    char output[arraylen(t_idx)];
    // Stores the converted chars into output according to the Base64 conversion table
    for(size_t i = 0; i < arraylen(t_idx); ++i){
      if(i * 4 + 1 > howMany * 6) output[i] = '=';
      else output[i] = base64_chars[t_idx[i]];
    }
    
    /* Write the output bytes to stdout */
    line_character += fwrite(output, sizeof *output, arraylen(output), stdout);

    /* Encoded lines wrap every 76 characters */
    if(line_character % 76 == 0){
      putchar('\n');
    }

    /* reached end of file */
    if (howMany < arraylen(buf)) break;
  }

  /* free up the memory */
  fclose(fp);
}

