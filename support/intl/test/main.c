#include <stdio.h>

#include <libintl.h>

void part1();
void check_quote();
void part2();

int main()
{
  textdomain("test");

  part1();
  part2();
  check_quote();
}
