#include <stdio.h>

int main() {
#pragma omp parallel num_threads(4)
  {
    printf("Hello, world!\n");
  }
}
