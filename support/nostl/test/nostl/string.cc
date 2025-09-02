#include <catch_amalgamated.hpp>

#include <string.h>

#include <nostl/string.h>

const char *short_string = "hello";
const size_t short_string_length = strlen(short_string);

const char *long_string = "hello, this is a real-really long string";
const size_t long_string_length = strlen(long_string);

TEST_CASE("string")
{
  REQUIRE(long_string_length > sizeof(nostl::string));

  SECTION("constructor")
  {
    nostl::string s1;
    REQUIRE(s1.empty());
    REQUIRE(s1.size() == 0);
    REQUIRE(s1[0] == 0);

    nostl::string s2(long_string);
    REQUIRE(s2.size() == long_string_length);
    for (size_t i = 0; i < long_string_length; i++)
      REQUIRE(s2[i] == long_string[i]);
    REQUIRE(s2[long_string_length] == 0);

    s2 = nostl::string(long_string, long_string_length);
    REQUIRE(s2.size() == long_string_length);
    for (size_t i = 0; i < long_string_length; i++)
      REQUIRE(s2[i] == long_string[i]);
    REQUIRE(s2[long_string_length] == 0);
  }

  SECTION("sso")
  {
    nostl::string s1(short_string);
    REQUIRE(s1.size() == short_string_length);
    for (size_t i = 0; i < short_string_length; i++)
      REQUIRE(s1[i] == short_string[i]);
    REQUIRE(s1[short_string_length] == 0);

    char *data = s1.data();
    char *b = reinterpret_cast<char *>(&s1);
    char *e = b + sizeof(s1);
    REQUIRE(data >= b);
    REQUIRE(data < e);
  }

  SECTION("push_back")
  {
    nostl::string s1(short_string);
    s1.push_back('!');
    REQUIRE(s1.size() == short_string_length + 1);
    REQUIRE(s1[short_string_length] == '!');
    REQUIRE(s1[short_string_length + 1] == 0);

    nostl::string s2(long_string);
    s2.push_back('!');
    REQUIRE(s2.size() == long_string_length + 1);
    REQUIRE(s2[long_string_length] == '!');
    REQUIRE(s2[long_string_length + 1] == 0);
  }

  SECTION("resize")
  {
    nostl::string s1(short_string);
    s1.resize(short_string_length + 1, '!');
    REQUIRE(s1.size() == short_string_length + 1);
    REQUIRE(s1[short_string_length] == '!');
    REQUIRE(s1[short_string_length + 1] == 0);

    s1.resize(short_string_length - 1, '!');
    REQUIRE(s1.size() == short_string_length - 1);
    REQUIRE(s1[short_string_length - 1] == 0);

    nostl::string s2(short_string);
    s2.resize(long_string_length, '!');
    REQUIRE(s2.size() == long_string_length);
    for (size_t i = short_string_length; i < long_string_length; ++i)
      REQUIRE(s2[i] == '!');
    REQUIRE(s2[long_string_length] == 0);

    nostl::string s3(long_string);
    s3.resize(long_string_length + 1, '!');
    REQUIRE(s3.size() == long_string_length + 1);
    REQUIRE(s3[long_string_length] == '!');
    REQUIRE(s3[long_string_length + 1] == 0);

    nostl::string s4(long_string);
    s4.resize(long_string_length * 2, '!');
    REQUIRE(s4.size() == long_string_length * 2);
    for (size_t i = long_string_length; i < long_string_length * 2; ++i)
      REQUIRE(s4[i] == '!');
    REQUIRE(s4[long_string_length * 2] == 0);
  }
}
