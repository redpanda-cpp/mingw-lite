#include "name_table.h"

namespace libiconv
{
  static const name_entry name_table[] = {

      /*****************
       * encodings.def *
       *****************/

      // ascii
      {"us-ascii", 20127},
      {"ascii", 20127},
      {"iso646-us", 20127},
      {"iso_646.irv:1991", 20127},
      {"iso-ir-6", 20127},
      {"ansi_x3.4-1968", 20127},
      {"ansi_x3.4-1986", 20127},
      {"cp367", 20127},
      {"ibm367", 20127},
      {"us", 20127},
      {"csascii", 20127},

      /* general multi-byte encodings */

      // utf8
      {"utf-8", 65001},

      // ucs2
      {"ucs-2", 1200},
      {"iso-10646-ucs-2", 1200},
      {"csunicode", 1200},

      // ucs2be
      {"ucs-2be", 1201},
      {"unicodebig", 1201},
      {"unicode-1-1", 1201},
      {"csunicode11", 1201},

      // ucs2le
      {"ucs-2le", 1200},
      {"unicodelittle", 1200},

      // ucs4
      {"ucs-4", 12000},
      {"iso-10646-ucs-4", 12000},
      {"csucs4", 12000},

      // ucs4be
      {"ucs-4be", 12001},

      // ucs4le
      {"ucs-4le", 12000},

      // utf16
      {"utf-16", 1200},

      // utf16be
      {"utf-16be", 1201},

      // utf16le
      {"utf-16le", 1200},

      // utf32
      {"utf-32", 12000},

      // utf32be
      {"utf-32be", 12001},

      // utf32le
      {"utf-32le", 12000},

      // utf7: not supported (stateful)

      // ucs2internal
      {"ucs-2-internal", 1200},

      // ucs2swapped
      {"ucs-2-swapped", 1201},

      // ucs4internal
      {"ucs-4-internal", 12000},

      // ucs4swapped
      {"ucs-4-swapped", 12001},

      // c99: not supported

      // java: not supported

      /* Standard 8-bit encodings */

      // iso8859_1
      {"iso-8859-1", 28591},
      {"iso_8859-1", 28591},
      {"iso_8859-1:1987", 28591},
      {"iso-ir-100", 28591},
      {"cp819", 28591},
      {"ibm819", 28591},
      {"latin1", 28591},
      {"l1", 28591},
      {"csisolatin1", 28591},
      {"iso8859-1", 28591},

      // iso8859_2
      {"iso-8859-2", 28592},
      {"iso_8859-2", 28592},
      {"iso_8859-2:1987", 28592},
      {"iso-ir-101", 28592},
      {"latin2", 28592},
      {"l2", 28592},
      {"csisolatin2", 28592},
      {"iso8859-2", 28592},

      // iso8859_3
      {"iso-8859-3", 28593},
      {"iso_8859-3", 28593},
      {"iso_8859-3:1988", 28593},
      {"iso-ir-109", 28593},
      {"latin3", 28593},
      {"l3", 28593},
      {"csisolatin3", 28593},
      {"iso8859-3", 28593},

      // iso8859_4
      {"iso-8859-4", 28594},
      {"iso_8859-4", 28594},
      {"iso_8859-4:1988", 28594},
      {"iso-ir-110", 28594},
      {"latin4", 28594},
      {"l4", 28594},
      {"csisolatin4", 28594},
      {"iso8859-4", 28594},

      // iso8859_5
      {"iso-8859-5", 28595},
      {"iso_8859-5", 28595},
      {"iso_8859-5:1988", 28595},
      {"iso-ir-144", 28595},
      {"cyrillic", 28595},
      {"csisolatincyrillic", 28595},
      {"iso8859-5", 28595},

      // iso8859_6
      {"iso-8859-6", 28596},
      {"iso_8859-6", 28596},
      {"iso_8859-6:1987", 28596},
      {"iso-ir-127", 28596},
      {"ecma-114", 28596},
      {"asmo-708", 28596},
      {"arabic", 28596},
      {"csisolatinarabic", 28596},
      {"iso8859-6", 28596},

      // iso8859_7
      {"iso-8859-7", 28597},
      {"iso_8859-7", 28597},
      {"iso_8859-7:1987", 28597},
      {"iso_8859-7:2003", 28597},
      {"iso-ir-126", 28597},
      {"ecma-118", 28597},
      {"elot_928", 28597},
      {"greek8", 28597},
      {"greek", 28597},
      {"csisolatingreek", 28597},
      {"iso8859-7", 28597},

      // iso8859_8
      {"iso-8859-8", 28598},
      {"iso_8859-8", 28598},
      {"iso_8859-8:1988", 28598},
      {"iso-ir-138", 28598},
      {"hebrew", 28598},
      {"csisolatinhebrew", 28598},
      {"iso8859-8", 28598},

      // iso8859_9
      {"iso-8859-9", 28599},
      {"iso_8859-9", 28599},
      {"iso_8859-9:1989", 28599},
      {"iso-ir-148", 28599},
      {"latin5", 28599},
      {"l5", 28599},
      {"csisolatin5", 28599},
      {"iso8859-9", 28599},

      // iso8859_10: not supported

      // iso8859_11: not supported

      // iso8859_13
      {"iso-8859-13", 28603},
      {"iso_8859-13", 28603},
      {"iso-ir-179", 28603},
      {"latin7", 28603},
      {"l7", 28603},
      {"iso8859-13", 28603},

      // iso8859_14: not supported

      // iso8859_15
      {"iso-8859-15", 28605},
      {"iso_8859-15", 28605},
      {"iso_8859-15:1998", 28605},
      {"iso-ir-203", 28605},
      {"latin-9", 28605},
      {"iso8859-15", 28605},

      // iso8859_16: not supported

      // koi8_r
      {"koi8-r", 20866},
      {"cskoi8r", 20866},

      // koi8_u
      {"koi8-u", 21866},

      // koi8_ru
      {"koi8-ru", 21866},

      /* Windows 8-bit encodings */

      // cp1250
      {"cp1250", 1250},
      {"windows-1250", 1250},
      {"ms-ee", 1250},

      // cp1251
      {"cp1251", 1251},
      {"windows-1251", 1251},
      {"ms-cyrl", 1251},

      // cp1252
      {"cp1252", 1252},
      {"windows-1252", 1252},
      {"ms-ansi", 1252},

      // cp1253
      {"cp1253", 1253},
      {"windows-1253", 1253},
      {"ms-greek", 1253},

      // cp1254
      {"cp1254", 1254},
      {"windows-1254", 1254},
      {"ms-turk", 1254},

      // cp1255
      {"cp1255", 1255},
      {"windows-1255", 1255},
      {"ms-hebr", 1255},

      // cp1256
      {"cp1256", 1256},
      {"windows-1256", 1256},
      {"ms-arab", 1256},

      // cp1257
      {"cp1257", 1257},
      {"windows-1257", 1257},
      {"winbaltrim", 1257},

      // cp1258
      {"cp1258", 1258},
      {"windows-1258", 1258},

      /* DOS 8-bit encodings */

      // cp850
      {"cp850", 850},
      {"ibm850", 850},
      {"850", 850},
      {"cspc850multilingual", 850},

      // cp862
      {"cp862", 862},
      {"ibm862", 862},
      {"862", 862},
      {"cspc862latinhebrew", 862},

      // cp866
      {"cp866", 866},
      {"ibm866", 866},
      {"866", 866},
      {"csibm866", 866},

      // cp1131: not supported

      /* Macintosh 8-bit encodings */

      // mac_roman
      {"macroman", 10000},
      {"macintosh", 10000},
      {"mac", 10000},
      {"csmacintosh", 10000},

      // mac_centraleurope
      {"maccentraleurope", 10029},

      // mac_iceland
      {"maciceland", 10079},

      // mac_croatian
      {"maccroatian", 10082},

      // mac_romania
      {"macromania", 10010},

      // mac_cyrillic
      {"maccyrillic", 10007},

      // mac_ukraine
      {"macukraine", 10017},

      // mac_greek
      {"macgreek", 10006},

      // mac_turkish
      {"macturkish", 10081},

      // mac_hebrew
      {"machebrew", 10005},

      // mac_arabic
      {"macarabic", 10004},

      // mac_thai
      {"macthai", 10021},

      /* Other platform specific 8-bit encodings */

      // hp_roman8: not supported
      // nextstep: not supported

      /* Regional 8-bit encodings used for a single language */

      // armscii_8: not supported
      // georgian_academy: not supported
      // georgian_ps: not supported
      // koi8_t: not supported
      // pt154: not supported
      // rk1048: not supported
      // mulelao: not supported
      // cp1133: not supported

      // tis620
      {"tis-620", 874},
      {"tis620", 874},
      {"tis620-0", 874},
      {"tis620.2529-1", 874},
      {"tis620.2533-0", 874},
      {"tis620.2533-1", 874},
      {"iso-ir-166", 874},

      // cp874
      {"cp874", 874},
      {"windows-874", 874},

      // viscii: not supported
      // tcvn: not supported

      /* CJK character sets (not documented) */

      // iso646_jp: not supported
      // jisx0201: not supported
      // jisx0208: not supported
      // jisx0212: not supported
      // iso646_cn: not supported
      // gb2312: not supported
      // isoir165: not supported
      // ksc5601: not supported

      /* CJK encodings */

      // euc_jp
      {"euc-jp", 51932},
      {"eucjp", 51932},
      {"extended_unix_code_packed_format_for_japanese", 51932},
      {"cseucpkdfmtjapanese", 51932},

      // sjis
      {"shift_jis", 932},
      {"shift-jis", 932},
      {"sjis", 932},
      {"ms_kanji", 932},
      {"csshiftjis", 932},

      // cp932
      {"cp932", 932},

      // iso2022_jp: not supported (stateful)
      // iso2022_jp1: not supported (stateful)
      // iso2022_jp2: not supported (stateful)
      // iso2022_jpms: not supported (stateful)

      // euc_cn
      {"euc-cn", 51936},
      {"euccn", 51936},
      {"gb2312", 51936},
      {"cn-gb", 51936},
      {"csgb2312", 51936},

      // ces_gbk
      {"gbk", 936},

      // cp936
      {"cp936", 936},
      {"ms936", 936},
      {"windows-936", 936},

      // gb18030_2005
      {"gb18030", 54936},
      {"gb18030:2005", 54936},

      // gb18030_2022
      {"gb18030:2022", 54936},

      // iso2022_cn: not supported (stateful)
      // iso2022_cn_ext: not supported (stateful)
      // hz: not supported (stateful)

      // euc_tw
      {"euc-tw", 51950},
      {"euctw", 51950},
      {"cseuctw", 51950},

      // ces_big5
      {"big5", 950},
      {"big-5", 950},
      {"big-five", 950},
      {"bigfive", 950},
      {"cn-big5", 950},
      {"csbig5", 950},

      // cp950
      {"cp950", 950},

      // big5hkscs1999: not supported (stateful)
      // big5hkscs2001: not supported (stateful)
      // big5hkscs2004: not supported (stateful)
      // big5hkscs2008: not supported (stateful)

      // euc_kr
      {"euc-kr", 51949},
      {"euckr", 51949},
      {"cseuckr", 51949},

      // cp949
      {"cp949", 949},
      {"uhc", 949},

      // johab
      {"johab", 1361},
      {"cp1361", 1361},

      // iso2022_kr: not supported (stateful)

      /*********************
       * encodings_dos.def *
       *********************/

      // cp437
      {"cp437", 437},
      {"ibm437", 437},
      {"437", 437},
      {"cspc8codepage437", 437},

      // cp737
      {"cp737", 737},

      // cp775
      {"cp775", 775},
      {"ibm775", 775},
      {"cspc775baltic", 775},

      // cp852
      {"cp852", 852},
      {"ibm852", 852},
      {"852", 852},
      {"cspcp852", 852},

      // cp853
      {"cp853", 853},

      // cp855
      {"cp855", 855},
      {"ibm855", 855},
      {"855", 855},
      {"csibm855", 855},

      // cp857
      {"cp857", 857},
      {"ibm857", 857},
      {"857", 857},
      {"csibm857", 857},

      // cp858
      {"cp858", 858},

      // cp860
      {"cp860", 860},
      {"ibm860", 860},
      {"860", 860},
      {"csibm860", 860},

      // cp861
      {"cp861", 861},
      {"ibm861", 861},
      {"861", 861},
      {"cp-is", 861},
      {"csibm861", 861},

      // cp863
      {"cp863", 863},
      {"ibm863", 863},
      {"863", 863},
      {"csibm863", 863},

      // cp864
      {"cp864", 864},
      {"ibm864", 864},
      {"csibm864", 864},

      // cp865
      {"cp865", 865},
      {"ibm865", 865},
      {"865", 865},
      {"csibm865", 865},

      // cp869
      {"cp869", 869},
      {"ibm869", 869},
      {"869", 869},
      {"cp-gr", 869},
      {"csibm869", 869},

      // cp1125
      {"cp1125", 1125},

      /***********************
       * encodings_local.def *
       ***********************/

      // local_char
      {"char", 65001},

      // local_wchar_t
      {"wchar_t", 1200},
  };

  const name_entry *const name_table_begin = name_table;
  const name_entry *const name_table_end =
      name_table + sizeof(name_table) / sizeof(name_entry);
} // namespace libiconv
