#pragma once

#include <thunk/yy/yy.h>

#include <stdlib.h>

#include <windows.h>

// YY-Thunks 1.1.6
namespace mingw_thunk::internal
{
  // 尝试将一个中性语言匹配到实际区域
  static LPCWSTR __fastcall
  DownlevelNeutralToSpecificLocaleName(LPCWSTR szLocaleName)
  {
    struct NeutralToSpecific
    {
      LPCWSTR szNeutralLocale;
      LPCWSTR szSpecificLocale;
    };

    static constexpr NeutralToSpecific NeutralToSpecificMap[] = {
        {L"af", L"af-ZA"},           {L"am", L"am-ET"},
        {L"ar", L"ar-SA"},           {L"arn", L"arn-CL"},
        {L"as", L"as-IN"},           {L"az", L"az-Latn-AZ"},
        {L"az-Cyrl", L"az-Cyrl-AZ"}, {L"az-Latn", L"az-Latn-AZ"},
        {L"ba", L"ba-RU"},           {L"be", L"be-BY"},
        {L"bg", L"bg-BG"},           {L"bn", L"bn-IN"},
        {L"bo", L"bo-CN"},           {L"br", L"br-FR"},
        {L"bs-Cyrl", L"bs-Cyrl-BA"}, {L"bs-Latn", L"bs-Latn-BA"},
        {L"ca", L"ca-ES"},           {L"co", L"co-FR"},
        {L"cs", L"cs-CZ"},           {L"cy", L"cy-GB"},
        {L"da", L"da-DK"},           {L"de", L"de-DE"},
        {L"dsb", L"dsb-DE"},         {L"dv", L"dv-MV"},
        {L"el", L"el-GR"},           {L"en", L"en-US"},
        {L"es", L"es-ES"},           {L"et", L"et-EE"},
        {L"eu", L"eu-ES"},           {L"fa", L"fa-IR"},
        {L"fi", L"fi-FI"},           {L"fil", L"fil-PH"},
        {L"fo", L"fo-FO"},           {L"fr", L"fr-FR"},
        {L"fy", L"fy-NL"},           {L"ga", L"ga-IE"},
        {L"gd", L"gd-GB"},           {L"gl", L"gl-ES"},
        {L"gsw", L"gsw-FR"},         {L"gu", L"gu-IN"},
        {L"ha-Latn", L"ha-Latn-NG"}, {L"he", L"he-IL"},
        {L"hi", L"hi-IN"},           {L"hr", L"hr-HR"},
        {L"hsb", L"hsb-DE"},         {L"hu", L"hu-HU"},
        {L"hy", L"hy-AM"},           {L"id", L"id-ID"},
        {L"ig", L"ig-NG"},           {L"ii", L"ii-CN"},
        {L"is", L"is-IS"},           {L"it", L"it-IT"},
        {L"iu-Cans", L"iu-Cans-CA"}, {L"iu-Latn", L"iu-Latn-CA"},
        {L"ja", L"ja-JP"},           {L"ka", L"ka-GE"},
        {L"kk", L"kk-KZ"},           {L"kl", L"kl-GL"},
        {L"km", L"km-KH"},           {L"kn", L"kn-IN"},
        {L"ko", L"ko-KR"},           {L"kok", L"kok-IN"},
        {L"ky", L"ky-KG"},           {L"lb", L"lb-LU"},
        {L"lo", L"lo-LA"},           {L"lt", L"lt-LT"},
        {L"lv", L"lv-LV"},           {L"mi", L"mi-NZ"},
        {L"mk", L"mk-MK"},           {L"ml", L"ml-IN"},
        {L"mn", L"mn-MN"},           {L"mn-Cyrl", L"mn-MN"},
        {L"mn-Mong", L"mn-Mong-CN"}, {L"moh", L"moh-CA"},
        {L"mr", L"mr-IN"},           {L"ms", L"ms-MY"},
        {L"mt", L"mt-MT"},           {L"nb", L"nb-NO"},
        {L"ne", L"ne-NP"},           {L"nl", L"nl-NL"},
        {L"nn", L"nn-NO"},           {L"no", L"nb-NO"},
        {L"nso", L"nso-ZA"},         {L"oc", L"oc-FR"},
        {L"or", L"or-IN"},           {L"pa", L"pa-IN"},
        {L"pl", L"pl-PL"},           {L"prs", L"prs-AF"},
        {L"ps", L"ps-AF"},           {L"pt", L"pt-BR"},
        {L"qut", L"qut-GT"},         {L"quz", L"quz-BO"},
        {L"rm", L"rm-CH"},           {L"ro", L"ro-RO"},
        {L"ru", L"ru-RU"},           {L"rw", L"rw-RW"},
        {L"sa", L"sa-IN"},           {L"sah", L"sah-RU"},
        {L"se", L"se-NO"},           {L"si", L"si-LK"},
        {L"sk", L"sk-SK"},           {L"sl", L"sl-SI"},
        {L"sma", L"sma-SE"},         {L"smj", L"smj-SE"},
        {L"smn", L"smn-FI"},         {L"sms", L"sms-FI"},
        {L"sq", L"sq-AL"},           {L"sr", L"sr-Latn-RS"},
        {L"sr-Cyrl", L"sr-Cyrl-RS"}, {L"sr-Latn", L"sr-Latn-RS"},
        {L"sv", L"sv-SE"},           {L"sw", L"sw-KE"},
        {L"syr", L"syr-SY"},         {L"ta", L"ta-IN"},
        {L"te", L"te-IN"},           {L"tg-Cyrl", L"tg-Cyrl-TJ"},
        {L"th", L"th-TH"},           {L"tk", L"tk-TM"},
        {L"tn", L"tn-ZA"},           {L"tr", L"tr-TR"},
        {L"tt", L"tt-RU"},           {L"tzm-Latn", L"tzm-Latn-DZ"},
        {L"ug", L"ug-CN"},           {L"uk", L"uk-UA"},
        {L"ur", L"ur-PK"},           {L"uz", L"uz-Latn-UZ"},
        {L"uz-Cyrl", L"uz-Cyrl-UZ"}, {L"uz-Latn", L"uz-Latn-UZ"},
        {L"vi", L"vi-VN"},           {L"wo", L"wo-SN"},
        {L"xh", L"xh-ZA"},           {L"yo", L"yo-NG"},
        {L"zh", L"zh-CN"},           {L"zh-CHS", L"zh-CN"},
        {L"zh-CHT", L"zh-HK"},       {L"zh-Hans", L"zh-CN"},
        {L"zh-Hant", L"zh-HK"},      {L"zu", L"zu-ZA"},
    };

    int bottom = 0;
    int top = _countof(NeutralToSpecificMap) - 1;

    while (bottom <= top) {
      int middle = (bottom + top) / 2;
      int testIndex = StringCompareIgnoreCaseByAscii(
          szLocaleName,
          NeutralToSpecificMap[middle].szNeutralLocale,
          LOCALE_NAME_MAX_LENGTH);

      if (testIndex == 0)
        return NeutralToSpecificMap[middle].szSpecificLocale;

      if (testIndex < 0)
        top = middle - 1;
      else
        bottom = middle + 1;
    }

    // 找不到就直接返回本身
    return szLocaleName;
  }
} // namespace mingw_thunk::internal
