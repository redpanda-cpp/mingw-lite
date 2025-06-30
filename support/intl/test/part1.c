#include <stdbool.h>
#include <stdio.h>

#include <libintl.h>

static const char *open_quote = NULL;
static const char *close_quote = NULL;
static bool quote_initialized = false;

static void init_quote()
{
  if (!quote_initialized) {
    open_quote = gettext("`");
    close_quote = gettext("'");
    quote_initialized = true;
  }
}

void part1()
{
  textdomain("part1");
  init_quote();
  puts(gettext("Part 1 Message 1"));
  puts(gettext("Part 1 Message 2"));
  puts(gettext("Part 1 Message 3"));
  puts(gettext("Part 1 Message 4"));
  puts(gettext("Part 1 Message 5"));
  puts(gettext("Part 1 Message 6"));
  puts(gettext("Part 1 Message 7"));
  puts(gettext("Part 1 Message 8"));
}

void check_quote()
{
  printf("%s%s%s\n", open_quote, "Hello, quote!", close_quote);
}
