//

#include "iniutil.h"
#include "nclog.h"

BOOL ReadIni(LPCWSTR pwszSection, LPCWSTR pwszKey, LPWSTR pwszValue, size_t cchValue, LPCWSTR pwszIniFile)
{
   // GetPrivateProfileString is unavailable on CE

   BOOL bSuccess = FALSE;
   FILE * fin;
   WCHAR wszLine[256];
   LPWSTR pwsz = NULL;

   if (!pwszIniFile ||
      !pwszSection ||
      !pwszKey ||
      !pwszValue ||
      !cchValue)
   {
	   // nclog(L"ReadIni: one or more parms are empty! Leaving now\n");
      goto leave;
   }

   // nclog(L"ReadIni: opening iniFile: '%s'\n", pwszIniFile);
   fin = _wfopen(pwszIniFile, L"r");
   if (fin)
   {
      while (fgetws(wszLine, sizeof(wszLine) * sizeof(WCHAR), fin)) // _tcsclen replaced by _tcsclen
      {
         // nclog(L"ReadIni: read line: '%s'\n", wszLine);
         if (!wcsstr(wszLine, pwszSection)) // Section
         {
            // nclog(L"ReadIni: Section search: continue...\n");
            continue;
         }

         while (fgetws(wszLine, sizeof(wszLine) * sizeof(WCHAR), fin)) // Key & Value
         {
            pwsz = wcschr(wszLine, L'=');
            if (!pwsz)
            {
               // nclog(L"ReadIni: '=' search: continue...\n");
               continue;
            }

            *pwsz = L'\0'; // Erase the '='

            if (0 != _wcsicmp(wszLine, pwszKey)) // Key
            {
               // nclog(L"ReadIni: key search: continue...\n");
               continue;
            }

            pwsz++; // Point to the value

            bSuccess = SUCCEEDED(StringCchCopyEx(
                                    pwszValue,
                                    cchValue,
                                    pwsz,
                                    &pwsz,
                                    NULL,
                                    0
                                    ));

            if (bSuccess)
            {
               if (*--pwsz == L'\n')
               {
                  *pwsz = L'\0';
               }
            }
		    // nclog(L"ReadIni: returning '%s'\n", pwszValue);

         }
      }

      fclose(fin);
   }

leave:
   return bSuccess;
}

BOOL ReadIniA(LPCWSTR pwszSection, LPCWSTR pwszKey, LPWSTR pwszValue, size_t cchValue, LPCWSTR pwszIniFile)
{
   // GetPrivateProfileString is unavailable on CE

   BOOL bSuccess = FALSE;
   FILE * fin;
   LPWSTR pwsz = NULL;

   // non-unicode
   char* szSection = new char[_tcslen(pwszSection)];
   wcstombs(szSection, pwszSection, _tcslen(pwszSection));

   char* szKey = new char[_tcslen(pwszKey)];
   wcstombs(szKey, pwszKey, _tcslen(pwszKey));

   char* szValue = new char[_tcslen(pwszValue)];
   wcstombs(szValue, pwszValue, _tcslen(pwszValue));

   char* szIniFile = new char[_tcslen(pwszIniFile)];
   wcstombs(szIniFile, pwszIniFile, _tcslen(pwszIniFile));

   char szLine[256];

   LPSTR psz = NULL;

   if (!pwszIniFile ||
      !pwszSection ||
      !pwszKey ||
      !pwszValue ||
      !cchValue)
   {
	  // nclog(L"ReadIni: one or more parms are empty! Leaving now\n");
      goto leave;
   }


   // nclog(L"ReadIni: opening iniFile: '%s'\n", pwszIniFile);
   //fin = _wfopen(pwszIniFile, L"r");
   fin = fopen(szIniFile, "r");
   if (fin)
   {
      //while (fgetws(wszLine, _tcsclen(wszLine), fin)) // _tcsclen replaced by _tcsclen
	  while (fgets(szLine, sizeof(szLine), fin)) // _tcsclen replaced by _tcsclen
      {
         // nclog("ReadIni: read line: '%s'\n", szLine);
         if (!strstr(szLine, szSection)) // Section
         {
            // nclog(L"ReadIni: Section search: continue...\n");
            continue;
         }

         while (fgets(szLine, strlen(szLine), fin)) // Key & Value
         {
			// nclog("ReadIni: fgets szLine = %s\n", szLine);
            psz = strchr(szLine, '=');
            if (!psz)
            {
               // nclog(L"ReadIni: '=' search: continue...\n");
               continue;
            }

            *psz = '\0'; // Erase the '='

            if (0 != _stricmp(szLine, szKey)) // Key
            {
               // nclog(L"ReadIni: key search: continue...\n");
               continue;
            }

            psz++; // Point to the value

			//copy result to szValue
			bSuccess = SUCCEEDED(StringCchCopyExA(
                                    szValue, //dest str (out)
                                    cchValue,  //dest size (in)
                                    psz,	   //pointer to source buffer (in)
                                    &psz,	   //pointer to end of dest (out)
                                    NULL,      //number of unused chars in dest (out)
                                    0		   //flags
                                    ));
			
			mbstowcs(pwszValue, szValue, strlen(szValue));
		    // nclog(L"ReadIni: returning '%s'\n", pwszValue);


/*
            bSuccess = SUCCEEDED(StringCchCopyEx(
                                    pwszValue,
                                    cchValue,
                                    pwsz,
                                    &pwsz,
                                    NULL,
                                    0
                                    ));
*/

            if (bSuccess)
            {
               if (*--psz == '\n')
               {
                  *psz = '\0';
               }
            }
         }
      }

      fclose(fin);
   }

leave:
   return bSuccess;
}


BOOL WriteIni(LPCWSTR pwszSection, LPCWSTR pwszKey, LPWSTR pwszValue, size_t cchValue, LPCWSTR pwszIniFile)
{
   // WritePrivateProfileString is unavailable on CE

   BOOL bSuccess = FALSE;
   BOOL bReplace;
   FILE * fin;
   fpos_t posSrc;
   fpos_t posDest;
   size_t cchLine;
   size_t cchNewLine;
   WCHAR wszLine[256];
   WCHAR wszNewLine[256];
   LPWSTR pwsz = NULL;
   DWORD dwAttrib;
   size_t cchKey = 0; //HGO

   if (!pwszIniFile ||
      !pwszSection ||
      !pwszKey ||
      (!pwszValue && cchValue) ||
      (cchValue >= _tcsclen(wszLine)))
   {
      goto leave;
   }

   dwAttrib = GetFileAttributes(pwszIniFile);
   if (dwAttrib == -1)
   {
      goto leave;
   }

   SetFileAttributes(pwszIniFile, dwAttrib & ~FILE_ATTRIBUTE_READONLY);

   fin = _wfopen(pwszIniFile, L"r+");
   if (fin)
   {
      bReplace = FALSE;
      cchNewLine = 0;

      while (fgetws(wszLine, _tcsclen(wszLine), fin))
      {
         if (!wcsstr(wszLine, pwszSection)) // Section
            continue;

         if (fgetpos(fin, &posSrc) || fgetpos(fin, &posDest))
         {
            goto closeFile;
         }

         while (fgetws(wszLine, _tcsclen(wszLine), fin)) // Key & Value
         {
            cchLine = wcslen(wszLine);

            pwsz = wcschr(wszLine, L'=');
            if (!pwsz)
            {
               goto nextLine;
            }

            *pwsz = L'\0'; // Erase the '='

            if (0 != _wcsicmp(wszLine, pwszKey)) // Key
            {
               *pwsz = L'='; // Add the '=' back
               goto nextLine;
            }

            *pwsz = L'='; // Add the '=' back
            pwsz++; // Point to the value

            // Clear Key & Value on NULL or empty string 
            if (!pwszValue || !cchValue)
            {
               bReplace = TRUE;
               cchLine = 0;
               goto nextLine;
            }

            // Do not overwritte more than the number of read characters.
            /* size_t */ cchKey = (pwsz - wszLine);
            if (SUCCEEDED(StringCchCopyNEx(
                                    wszNewLine,
                                    _tcsclen(wszNewLine),
                                    wszLine,
                                    cchKey,
                                    &pwsz,
                                    &cchNewLine,
                                    0
                                    )) &&
               SUCCEEDED(StringCchCopyNEx(
                                    pwsz,
                                    cchNewLine,
                                    pwszValue,
                                    min((cchLine - cchKey), cchValue),
                                    &pwsz,
                                    &cchNewLine,
                                    STRSAFE_IGNORE_NULLS
                                    )))
            {
               bReplace = TRUE;

               BOOL bAppendNewLine = FALSE;
               if (wszLine[cchLine-1] == L'\n')
               {
                  cchLine--;
                  bAppendNewLine = TRUE;
               }

               // See if there are remaining characters to copy
               if (cchValue > (cchLine - cchKey))
               {
                  if (bAppendNewLine)
                  {
                     cchLine++;
                  }

                  pwszValue += (cchLine - cchKey);
                  if (FAILED(StringCchCopyNEx(
                                       wszLine,
                                       _tcsclen(wszLine),
                                       pwszValue,
                                       (cchValue - (cchLine - cchKey)),
                                       &pwsz,
                                       &cchLine,
                                       0
                                       )) ||
                     (bAppendNewLine &&
                     FAILED(StringCchCopy(pwsz, cchLine, L"\n"))) ||
                     FAILED(StringCchLength(wszLine, _tcsclen(wszLine), &cchLine)))
                  {
                     bReplace = FALSE;
                  }
               }
               else
               {
                  if (bAppendNewLine &&
                     FAILED(StringCchCopy(pwsz, cchNewLine, L"\n")))
                  {
                    bReplace = FALSE;
                  }
                  cchLine = 0;
               }

               if (FAILED(StringCchLength(wszNewLine, _tcsclen(wszNewLine), &cchNewLine)))
               {
                  bReplace = FALSE;
               }
            }

nextLine:
            if (bReplace)
            {
               BOOL bEOF = (BOOL)feof(fin);

               // Save read offset, then restore write offset
               if (fgetpos(fin, &posSrc) || fsetpos(fin, &posDest))
               {
                  goto closeFile;
               }

               if (cchNewLine)
               {
                  bSuccess = (WEOF != fputws(wszNewLine, fin));
                  if (cchLine &&
                     SUCCEEDED(StringCchCopyN(
                                          wszNewLine,
                                          _tcsclen(wszLine),
                                          wszLine,
                                          cchLine
                                          )))
                  {
                     cchNewLine = cchLine;
                  }
                  else
                  {
                     cchNewLine = 0;
                  }
               }
               else if (cchLine)
               {
                  bSuccess = (WEOF != fputws(wszLine, fin));
                  cchLine = 0;
               }

               // Save write offset, then restore read offset
               if (fgetpos(fin, &posDest) ||
                  bEOF ||
                  fsetpos(fin, &posSrc))
               {
                  goto closeFile;
               }
            }
            else
            {
               // Just update read and write offsets
               if (fgetpos(fin, &posSrc) || fgetpos(fin, &posDest))
               {
                  goto closeFile;
               }
            }
         }
      }

closeFile:
      if (bReplace && !fsetpos(fin, &posDest))
      {
         if (cchNewLine)
         {
            bSuccess = (WEOF != fputws(wszNewLine, fin));
         }
         SetEndOfFile(_fileno(fin));
      }

      fclose(fin);
   }

   SetFileAttributes(pwszIniFile, dwAttrib);

leave:
   return bSuccess;
}


