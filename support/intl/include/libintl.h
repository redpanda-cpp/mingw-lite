#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  char *bindtextdomain(const char *domainname, const char *dirname);
  char *textdomain(const char *domainname);

  char *gettext(const char *msgid);
  char *dgettext(const char *domainname, const char *msgid);
  char *dcgettext(const char *domainname, const char *msgid, int category);

  char *
  ngettext(const char *msgid, const char *msgid_plural, unsigned long int n);
  char *dngettext(const char *domainname,
                  const char *msgid,
                  const char *msgid_plural,
                  unsigned long int n);
  char *dcngettext(const char *domainname,
                   const char *msgid,
                   const char *msgid_plural,
                   unsigned long int n,
                   int category);

#ifdef __cplusplus
}
#endif
