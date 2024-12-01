#include <stdio.h>
#include <stdint.h> 
#include <err.h>
#include <errno.h>

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

#define arraylen(x) (sizeof(x) / sizeof(*(x)))

int main(int argc, char *argv[]) {
  if(argc > 2) errx(1, "SYNOPSIS: base64enc [FILE]");

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
        err(1, "Error opening file '%s'", argv[1]);
    }
  }

  size_t line_character = 0;
  while (1) {
    uint8_t buf[3] = {0};
    /* read three BYTES (8bit*3 = 24bit)*/
    size_t howMany = fread(&buf, sizeof(uint8_t), arraylen(buf), fp);

    /* fread returns short item count on eof OR error */
    if (howMany < arraylen(buf)) {
      if (ferror(fp)) 
        err(1, "Error reading file");
      // Hit end of file without any new characters in buf, exit
      if (howMany == 0) break; 
    }
    
    // contains 4 * 8bit value 
    uint8_t t_idx[4] = {0};

    // 8bits * 3BYTES -> 6bits * 4BYTES
    // [12345678] [12345678] [12345678]
    // [123456] [781234] [567812] [345678]

    t_idx[0] = buf[0] >> 2; // [12345678] → [00123456]
    t_idx[1] = ((buf[0] & 0x03) << 4) | (buf[1] >> 4);
    // last 2 bits of the first byte [78] + first 4 bits of the second byte [1234]
    // 0x03 = 00000011
    // [12345678] → [00000078] << 4 = [07800000]
    // [12345678] → [00001234] 
    // combine with OR [07801234]

    t_idx[2] = ((buf[1] & 0x0f) << 2) | (buf[2] >> 6);
    // last 4 bits of the second byte [5678] + first 2 bits of the third byte [12]
    // 0x0f = 00001111
    // [00567812]

    t_idx[3] = buf[2] & 0x3f;
    // last 6 bits of the third byte 
    // 0x3f = 00111111
    // [00345678]

    char output[arraylen(t_idx)];
    for(size_t i = 0; i < arraylen(t_idx); ++i){
      if(i * 4 + 1 > howMany * 6) output[i] = '=';
      else output[i] = base64_chars[t_idx[i]];
    }
    
    line_character += fwrite(output, sizeof *output, arraylen(output), stdout);

    if(line_character == 76) {
        putchar('\n');
        line_character = 0;
    }

    /* reached end of file */
    if (howMany < arraylen(buf)) break;
  }

  if (line_character > 0) {
    putchar('\n');
  }
  /* free up the memory */
  fclose(fp);
  return 0;
}

