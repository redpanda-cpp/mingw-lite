#include <ctype.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "[diff] usage: %s <file1> <file2>\n", argv[0]);
    return 1;
  }

  FILE *f1 = fopen(argv[1], "rb");
  if (!f1) {
    fprintf(stderr, "[diff] error opening file: %s\n", argv[1]);
    return 1;
  }
  FILE *f2 = fopen(argv[2], "rb");
  if (!f2) {
    fprintf(stderr, "[diff] error opening file: %s\n", argv[2]);
    return 1;
  }

  int c1, c2;
  size_t pos = 0;
  while (1) {
    c1 = fgetc(f1);
    c2 = fgetc(f2);
    if (c1 == EOF && c2 == EOF)
      return 0;
    if (c1 == EOF) {
      fprintf(stderr, "[diff] EOF at %zu: %s\n", pos, argv[1]);
      return 1;
    }
    if (c2 == EOF) {
      fprintf(stderr, "[diff] EOF at %zu: %s\n", pos, argv[2]);
      return 1;
    }
    if (c1 != c2) {
      fprintf(stderr, "[diff] diff at %zu: ", pos);
      if (isalnum(c1))
        fprintf(stderr, "'%c' != ", c1);
      else
        fprintf(stderr, "'\\x%02x' != ", (unsigned char)c1);
      if (isalnum(c2))
        fprintf(stderr, "'%c'\n", c2);
      else
        fprintf(stderr, "'\\x%02x'\n", (unsigned char)c2);
      return 1;
    }
    pos++;
  }
  return 0;
}
