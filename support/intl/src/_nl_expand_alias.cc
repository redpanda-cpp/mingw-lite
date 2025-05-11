// workaround AC check for GNU libintl
extern "C" const char *_nl_expand_alias(const char *)
{
  return "";
}
