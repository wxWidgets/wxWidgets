#include <windows.h>
#include <stdio.h>

// All of the LCTYPES new to Windows Vista
LCTYPE winLocaleTypes[] =
{
  LOCALE_SNAME,                   // Tag identifying the locale
  LOCALE_SENGLISHDISPLAYNAME,     // English locale name (example: "German (Germany)"
  LOCALE_SENGLISHLANGUAGENAME,    // English language name
  LOCALE_SENGLISHCOUNTRYNAME,     // English country name
  LOCALE_SSCRIPTS,                // List of scripts
  LOCALE_SISO3166CTRYNAME2,
  LOCALE_SINTLSYMBOL,             // Currency symbol ISO
  LOCALE_IREADINGLAYOUT,          // Layout direction 0=LTR, 1=RTL, 2=TTB-RTL or LTR, 3=TTB-LTR
  LOCALE_ILANGUAGE,               // Numeric language code
  LOCALE_SNATIVEDISPLAYNAME,      // Native locale name (example: "Deutsch (Deutschland)"
  LOCALE_SNATIVELANGUAGENAME,     // Native language name
  LOCALE_SNATIVECOUNTRYNAME       // Native country name
};

// Strings so we can print out the LCTYPES
LPCWSTR winLocaleTypeNames[] =
{
  L"LOCALE_SNAME",                   // Tag identifying the locale
  L"LOCALE_SENGLISHDISPLAYNAME",     // English locale name (example: "German (Germany)"
  L"LOCALE_SENGLISHLANGUAGENAME",    // English language name
  L"LOCALE_SENGLISHCOUNTRYNAME",     // English country name
  L"LOCALE_SSCRIPTS",                // List of scripts
  L"LOCALE_SISO3166CTRYNAME2",
  L"LOCALE_SINTLSYMBOL",             // Currency symbol ISO
  L"LOCALE_IREADINGLAYOUT",          // Layout direction 0=LTR, 1=RTL, 2=TTB-RTL or LTR, 3=TTB-LTR
  L"LOCALE_ILANGUAGE",               // Numeric language code
  L"LOCALE_SNATIVEDISPLAYNAME",      // Native locale name (example: "Deutsch (Deutschland)"
  L"LOCALE_SNATIVELANGUAGENAME",     // Native language name
  L"LOCALE_SNATIVECOUNTRYNAME"       // Native country name
};

// Callback for EnumSystemLocalesEx()
#define BUFFER_SIZE 512
BOOL CALLBACK MyFuncLocaleEx(LPWSTR pStr, DWORD dwFlags, LPARAM lparam)
{
  if (*pStr == 0) return (TRUE);

  UNREFERENCED_PARAMETER(dwFlags);
  UNREFERENCED_PARAMETER(lparam);
  WCHAR wcBuffer[BUFFER_SIZE];
  char utf8Buffer[BUFFER_SIZE*4];

  int iResult;
  int i;

  // Loop through all specified LCTYPES and do GetLocaleInfoEx on them
  for (i = 0; i < sizeof(winLocaleTypes) / sizeof(winLocaleTypes[0]); i++)
  {
    // Get this LCTYPE result for this locale
    iResult = GetLocaleInfoEx(pStr, winLocaleTypes[i], wcBuffer, BUFFER_SIZE);

    // If it succeeds, print it out
    if (i != 0) printf("|");
    if (iResult > 0)
    {
      WideCharToMultiByte(CP_UTF8, 0, wcBuffer, -1, utf8Buffer, BUFFER_SIZE*4, NULL, NULL);
      printf(utf8Buffer);
    }
    else
    {
      //wprintf(L"  %s had error %d\n", NewTypeNames[i], GetLastError());
    }
  }
  printf("\n");

  return (TRUE);
}

int main(int argc, char* argv[])
{
  UNREFERENCED_PARAMETER(argc);
  // Enumerate the known locales and report on them
  // LOCALE_ALL = LOCALE_WINDOWS | LOCALE_SUPPLEMENTAL | LOCALE_ALTERNATE_SORTS | LOCALE_NEUTRALDATA
  EnumSystemLocalesEx( MyFuncLocaleEx, LOCALE_WINDOWS, (LPARAM)argv, NULL);
}
