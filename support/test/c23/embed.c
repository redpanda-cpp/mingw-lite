#include <stdio.h>

const char message[] = {
#embed "message.txt"
    ,
    '\0',
};

int main() {
  printf("%s", message);
  return 0;
}
