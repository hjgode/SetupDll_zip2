/*$T SetupDll_zip2.cpp GC 1.138 09/16/05 10:07:08 */

/* setup5.dll according to 751G users manual page 86 */
#pragma warning ( disable : 4068 4209 )

#include <windows.h>

#include "myWin.h"

#include <Tlhelp32.h>
#pragma comment (lib, "toolhelp.lib")
#include <winioctl.h>

#include <Pm.h>

//#ifndef DEBUG
//	#include "itc50.h"
//	#pragma comment (lib, "itc50.lib")
//#endif

#include "nclog.h"

#define TH32CS_SNAPNOHEAPS 0x40000000

#include "unzip.h"
#include "iniutil.h"
#include "registry.h"

#include "ce_setup.h"   // in the public SDK dir

//some globals
#define MYINIFILE L"SetupDll_zip2.ini"
TCHAR m_unzipBasedir[MAX_PATH];
TCHAR m_instlDir[MAX_PATH];
BOOL  m_doReboot = false;
TCHAR m_zipFilename[MAX_PATH];

#define MYREGENTRY L"Software\\SetupDll_ZIP2"
/*
	registry entries (all REG_SZ):
	unzipbasedir: "\"
	zipfilename: "install.zip"
	doreboot:"0"|"1"
*/

BOOL IsProcessRunning(TCHAR*);
int unzipFileWithPath(TCHAR*, TCHAR*);
void doReboot();
void readIni();
void unzipWithProgress(const TCHAR*, TCHAR* );

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
BOOL APIENTRY DllMain( HANDLE h, DWORD reason, LPVOID lpReserved )
{
	switch(reason){
	case DLL_PROCESS_ATTACH:
		nclog(L"DllMain: DLL_PROCESS_ATTACH\n");
		return TRUE;
	case DLL_THREAD_ATTACH:
		nclog(L"DllMain: DLL_THREAD_ATTACH\n");
		return TRUE;
	case DLL_THREAD_DETACH:
		nclog(L"DllMain: DLL_THREAD_DETACH\n");
		return TRUE;
	case DLL_PROCESS_DETACH:
		if(g_hInst!=NULL)
			g_hInst=NULL;
		nclog(L"DllMain: DLL_PROCESS_DETACH\n");
		return TRUE;
	default:
		nclog(L"DllMain: default\n");
		return TRUE;

	}
}   /* DllMain */


/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
codeINSTALL_INIT Install_Init( HWND hwndParent, BOOL fFirstCall, BOOL fPreviouslyInstalled, LPCTSTR pszInstallDir )
{
//	HINSTANCE hInst = GetModuleHandle(L"SetupDll_zip2.DLL");
	nclog(L"Install_Init started\n");
	g_hwndParent=hwndParent;

	nclog(L"Install_Init: instlDir: '%s' \nInstall_Init: codeINSTALL_INIT_CONTINUE\n", pszInstallDir);

    return codeINSTALL_INIT_CONTINUE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
codeINSTALL_EXIT Install_Exit
(
    HWND    hwndParent,
    LPCTSTR pszInstallDir,
    WORD    cFailedDirs,
    WORD    cFailedFiles,
    WORD    cFailedRegKeys,
    WORD    cFailedRegVals,
    WORD    cFailedShortcuts
)
{
	nclog(L"Install_Exit started\n");
	g_hwndParent=hwndParent;
//    TCHAR   srcfile[MAX_PATH];
//    TCHAR   dstfile[MAX_PATH];


	//Save the registry
	RegFlushKey(HKEY_LOCAL_MACHINE);

    if ( cFailedDirs || cFailedFiles || cFailedRegKeys || cFailedRegVals || cFailedShortcuts )
    {
		nclog(L"Install_Init codeINSTALL_EXIT_UNINSTALL\n");
        return codeINSTALL_EXIT_UNINSTALL;
    }

	if(pszInstallDir!=NULL)
		wsprintf(m_instlDir, pszInstallDir);
	else
		wsprintf(m_instlDir, L"\\Temp\\");

	readIni();

	showWindow(GetModuleHandle(NULL));

	TCHAR* zipFile = new TCHAR[MAX_PATH];

	//ensure m_instlDir ends with a backslash
	if(m_instlDir[wcslen(m_instlDir)]!='\\')
		wcscat(m_instlDir, L"\\");

	//create the full path and filename
	wsprintf(zipFile, L"%s%s", m_instlDir, m_zipFilename);
/*
	nclog(L"Install_Exit: Calling unzipFileWithPath with '%s' and '%s'\n", zipFile, m_unzipBasedir);
	unzipFileWithPath(zipFile, m_unzipBasedir);
*/
	nclog(L"Install_Exit: m_instlDir='%s'\n\tCalling unzipWithProgress with '%s' and '%s'\n",m_instlDir, zipFile, m_unzipBasedir);
	unzipWithProgress(zipFile, m_unzipBasedir);

	if(m_doReboot)
		doReboot();

	nclog(L"SetupDll: Install_Exit: codeINSTALL_EXIT_DONE\n");

	ShowWindow(FindWindow(NULL, L"Installation"), SW_SHOWNORMAL);
    return codeINSTALL_EXIT_DONE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
codeUNINSTALL_INIT Uninstall_Init( HWND hwndParent, LPCTSTR pszInstallDir )
{
	nclog(L"Uninstall_Init started\n");
	g_hwndParent=hwndParent;
    /* TODO: Perform the reverse of INSTALL_INIT here */
    return codeUNINSTALL_INIT_CONTINUE;
}

/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */
codeUNINSTALL_EXIT Uninstall_Exit( HWND hwndParent )
{
	nclog(L"Uninstall_Exit started\n");
	g_hwndParent=hwndParent;
    /* TODO: Perform the reverse of INSTALL_EXIT here */
    return codeUNINSTALL_EXIT_DONE;
}


/*
 =======================================================================================================================
 *
 =======================================================================================================================
 */

TCHAR* readRegBaseDir(){
	HKEY hKey;
	static TCHAR* retStr = new TCHAR[MAX_PATH];
	DWORD dwSize = MAX_PATH;
	TCHAR* lpData = new TCHAR[MAX_PATH];

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, MYREGENTRY, 0, 0, &hKey)==ERROR_SUCCESS){
		DWORD dwType = REG_SZ;
		if(RegQueryValueEx(hKey, L"unzipbasedir", NULL, &dwType, (BYTE*) &lpData, &dwSize)==ERROR_SUCCESS){
			wcsncpy(retStr, lpData, dwSize);
		}
		else{
			wsprintf(retStr, L"");
		}

		RegCloseKey(hKey);
	}
	else{
		wsprintf(retStr, L"");
	}
	return retStr;
}

TCHAR* getUnzipBaseDir(TCHAR* iniDir){
	nclog(L"getUnzipBaseDir: Trying to read ini file\n");

	static TCHAR* retStr = new TCHAR[MAX_PATH];

	TCHAR* iniFile;
	iniFile = new TCHAR[MAX_PATH];

	//add a backslash
	TCHAR lastchar = iniDir[_tcslen(iniDir)-1];
	if (lastchar!='\\' && lastchar!='/') 
		_tcscat(iniDir,_T("\\"));

	wsprintf(iniFile, L"%s%s", iniDir, MYINIFILE);
	
	nclog(L"getUnzipBaseDir: Trying to read %s\n", iniFile);

	//try to read basedir from the ini file
	BOOL bRes = ReadIni(L"unzip", L"basedir", retStr, MAX_PATH, iniFile);

	if(bRes){
		nclog(L"getUnzipBaseDir: ReadIni OK, value is %s\n", retStr);
		return retStr;
	}
	else{
		nclog(L"getUnzipBaseDir: ReadIni failed\n");
		//try to read from registry
		TCHAR* retStr1 = readRegBaseDir();
		if(wcslen(retStr1)>0){
			wsprintf(retStr,retStr1);
			return retStr;
		}
		else
			return L"";
	}
}


/*$DOCBEGIN$
 =======================================================================================================================
 *    £
 *    int unzipFileWithPath(TCHAR* zipFile, TCHAR* instDir); £
 *    * Description: unzip the zip file zipFile to instDir £
 *    * Arguments: zipFile: full name of zip file. £
 *    for example, /temp/myfile.zip. £
 *    instDir: the dir to unpack the zip file. £
 *    * Returns: positive numbers of files unpacked. £
 *    negative values mean error. £
 *    $DOCEND$ £
 *
 =======================================================================================================================
 */
int unzipFileWithPath(TCHAR* zipFile, TCHAR* baseDir){
	HZIP hz = OpenZip(zipFile, NULL);
	nclog(L"Trying to open '%s'\n", zipFile);
	unsigned long uRes=0;
	TCHAR* zipDir = new TCHAR[MAX_PATH];

	/*
	wsprintf(zipDir, getUnzipBaseDir(instDir));
	if(wcslen(zipDir)==0){
		nclog(L"getUnzipBaseDir returned empty string\n");
		wsprintf(zipDir, L"\\");
	}
	*/

	wsprintf(zipDir, baseDir);
	if(hz != NULL){
		uRes = SetUnzipBaseDir(hz, zipDir);
		nclog(L"SetUnzipBaseDir to '%s' returned %u\n", zipDir, uRes);

		nclog(L"OpenZip OK. Getting zip entries\n");

		ZIPENTRY ze; 
		uRes = GetZipItem(hz,-1,&ze);
		if (uRes == ZR_OK){
			nclog(L"Found %i zip entries\n", ze.index);

			int numitems=ze.index;
			for (int i=0; i<numitems; i++)
			{ 
				nclog(L"Trying to get zip entry #%i\n", i);
				
				uRes = GetZipItem(hz,i,&ze);
				if(uRes==ZR_OK){

					nclog(L"\tGot name of zip entry '%s'\n", ze.name);
					uRes = UnzipItem(hz,i,ze.name);
					if( uRes == ZR_OK){
						nclog(L"\tUnzip OK\n");
					}
					else{
						//possibly zero size file
						if(ze.unc_size==0)
							nclog(L"\t0 BYTE file Warning\n");
						if(ze.unc_size==0 && uRes==ZR_FLATE){
							//file should be OK
							nclog(L"\tfile should be OK\n");
						}
						else{
							nclog(L"\tUnzip failed! uRes=%u\n", uRes);
							TCHAR errMsg[256];
							FormatZipMessageU(uRes, errMsg, 256);
							nclog(L"\tLast error message: %s\n", errMsg);
						}
					}
				}
				else
					nclog(L"\tGetZipItem for %i failed! uRes=%u\n", i, uRes);
			}
			CloseZip(hz);
			return numitems;
		}
		else{
			nclog(L"\tGetZipItem failed! uRes=%u\n", uRes);
			return -2;
		}
	}
	else{
		nclog(L"OpenZip failed\n");
		return -1;
	}
}

/*$DOCBEGIN$
 =======================================================================================================================
 *    £
 *    BOOL IsProcessRunning( TCHAR * pname ); £
 *    * Description: Get process table snapshot, look for pname running. £
 *    * Arguments: pname - pointer to name of program to look for. £
 *    for example, app.exe. £
 *    * Returns: TRUE - process is running. £
 *    FALSE - process is not running. £
 *    $DOCEND$ £
 *
 =======================================================================================================================
 */

BOOL IsProcessRunning( TCHAR *pname )
{
    HANDLE          hProcList;
    PROCESSENTRY32  peProcess;
    DWORD           thDeviceProcessID;
    TCHAR           lpname[MAX_PATH];
    if ( !pname || !*pname )
    {
        return FALSE;
    }

    _tcscpy( lpname, pname );
    _tcslwr( lpname );
    hProcList = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0 );
    if ( hProcList == INVALID_HANDLE_VALUE )
    {
        return FALSE;
    }       /* end if */

    memset( &peProcess, 0, sizeof(peProcess) );
    peProcess.dwSize = sizeof( peProcess );
    if ( !Process32First( hProcList, &peProcess ) )
    {
        CloseToolhelp32Snapshot( hProcList );
        return FALSE;
    }       /* end if */

    thDeviceProcessID = 0;
    do
    {
        _tcslwr( peProcess.szExeFile );
        if ( _tcsstr( peProcess.szExeFile, lpname ) )
        {
            thDeviceProcessID = peProcess.th32ProcessID;
            break;
        }   /* end if */
    }
    while ( Process32Next( hProcList, &peProcess ) );
    if ( (GetLastError() == ERROR_NO_MORE_FILES) && (thDeviceProcessID == 0) )
    {
        CloseToolhelp32Snapshot( hProcList );
        return FALSE;
    }       /* end if */

    CloseToolhelp32Snapshot( hProcList );
    return TRUE;
}           /* IsProcessRunning */

void ITCWarmBoot(){
	SetSystemPowerState(NULL, POWER_STATE_RESET, 0);
}

void doReboot(){
    HANDLE  h;
    if ( IsProcessRunning( L"autocab.exe" ) )
    {
        h = CreateFile
            (
                L"\\Windows\\__resetmeplease__.txt",
                (GENERIC_READ | GENERIC_WRITE),
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_HIDDEN,
                NULL
            );
        if ( h != INVALID_HANDLE_VALUE )
        {
            CloseHandle( h );
        }
        else
        {
            /* Couldn’t create the file. If it failed because the file already exists, it is not fatal.*/
            /* Otherwise, notify user of the inability to reset the device and they will have to*/
            /* perform it manually after all of the installations are complete. */
        }   /* end if */
    }
    else
    {
		MessageBeep(MB_ICONEXCLAMATION);
		Sleep(1000);
#ifndef DEBUG
		ITCWarmBoot();
#endif
    }       /* end if */

}

// this needs the global var m_instlDir (with a trailing \) to be set!
void readIni(){
	nclog(L"readIni ENTRY: instlDir='%s', basedir='%s', zipname='%s', reboot=%i\n", m_instlDir, m_unzipBasedir, m_zipFilename, m_doReboot);

	TCHAR tempStr[MAX_PATH];

	//first try the ini file
	TCHAR* iniFile;
	iniFile = new TCHAR[MAX_PATH];

	//add a backslash
	TCHAR lastchar = m_instlDir[_tcslen(m_instlDir)-1];
	if (lastchar!='\\' && lastchar!='/') 
		_tcscat(m_instlDir,_T("\\"));

	wsprintf(iniFile, L"%s%s", m_instlDir, MYINIFILE);
	//try to read basedir from the ini file
	BOOL bRes = ReadIni(L"unzip", L"basedir", tempStr, MAX_PATH, iniFile);
	if(bRes){
		//basedir
		wsprintf(m_unzipBasedir, tempStr);

		//reboot
		bRes=ReadIni(L"unzip", L"reboot", tempStr, MAX_PATH, iniFile);
		if(bRes){
			if(wcscmp(tempStr, L"1")==0)
				m_doReboot=TRUE;
			else
				m_doReboot=FALSE;
		}
		else
			m_doReboot=FALSE;
		//zip file name
		bRes=ReadIni(L"unzip", L"zipfile", tempStr, MAX_PATH, iniFile);
		if(bRes)
			wsprintf(m_zipFilename, tempStr);
		else
			wsprintf(m_zipFilename, L"install.zip"); 
	}
	else{
		//now try the registry
		if(OpenKey(MYREGENTRY)==ERROR_SUCCESS){
			if(RegReadStr(L"unzipbasedir", tempStr)==0)
				wsprintf(m_unzipBasedir, tempStr);
			else
				wsprintf(m_unzipBasedir, L"");

			//add a backslash
			TCHAR lastchar = m_unzipBasedir[_tcslen(m_unzipBasedir)-1];
			if (lastchar!='\\' && lastchar!='/') 
				_tcscat(m_unzipBasedir,_T("\\"));


			if (RegReadStr(L"zipfilename", tempStr)==0)
				wsprintf(m_zipFilename, tempStr);
			else
				wsprintf(m_zipFilename, L"install.zip");
			
			if (RegReadStr(L"doreboot", tempStr)==0){
				if(wcscmp(tempStr, L"1")==0)
					m_doReboot=TRUE;
				else
					m_doReboot=FALSE;
			}
			else
				m_doReboot=FALSE;
			CloseKey();
		}
	}
	nclog(L"readIni EXIT: instlDir='%s', basedir='%s', zipname='%s', reboot=%i\n", m_instlDir, m_unzipBasedir, m_zipFilename, m_doReboot);
}

//==========================================================================================
// ExistFile implementation
//==========================================================================================
int ExistFile(TCHAR* filename)
{
	HANDLE hFile;

	hFile = CreateFile (filename,   // Open MYFILE.TXT
					  GENERIC_READ,           // Open for reading
					  FILE_SHARE_READ,        // Share for reading
					  NULL,                   // No security
					  OPEN_EXISTING,          // Existing file only
					  FILE_ATTRIBUTE_NORMAL,  // Normal file
					  NULL);                  // No template file

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Your error-handling code goes here.
		return FALSE;
	}
	else
	{
		CloseHandle(hFile);
		return TRUE;
	}
}

/************************************************************************

*************************************************************************/
BOOL DelFile(LPCTSTR pFile)
{
	int iRet=0;
	nclog(L"DelFile...'%s'\n", pFile);
	DWORD dwAttribs = GetFileAttributes(pFile);
	nclog(L"GetFileAttributes = 0x%0x\n", dwAttribs);
	// 0x845
	if (((dwAttribs & FILE_ATTRIBUTE_INROM) == FILE_ATTRIBUTE_INROM) && (dwAttribs!=0xFFFFffff)){
		nclog(L"WARNING: file marked with IN_ROM\n");
		nclog(L"creating dummy file\n");
		HANDLE hDummy = CreateFile(L"\\dummy.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if(hDummy != INVALID_HANDLE_VALUE){
			DWORD dwWrite;
			WriteFile(hDummy, "dummy", 5, &dwWrite, 0);
			CloseHandle(hDummy);
			if (CopyFile(L"\\dummy.txt", pFile, false) != 0)
			{
				nclog(L"CopyFile SUCCESS.\n");
				DeleteFile(L"\\dummy.txt");
				return TRUE;
			}
			else
			{
				nclog(L"CopyFile FAILED. LastError=0x%0x\n", GetLastError());
			}
		}
		else
			nclog(L"creating dummy file FAILED. LastError=0x%0x\n", GetLastError());

	}

	nclog(L"...DelFile\n");
	return FALSE;

	if (dwAttribs & FILE_ATTRIBUTE_READONLY)
	{
		nclog(L"SetFileAttributes...\n");
		iRet = SetFileAttributes(pFile, FILE_ATTRIBUTE_NORMAL);
		if(iRet==0){ //error
			nclog(L"SetFileAttributes failed: lasterror=0x%0x\n", GetLastError());
		}
		else{
			nclog(L"SetFileAttributes SUCCESS: returned 0x%0x\n", iRet);
		}
	}
	iRet = DeleteFile(pFile);
	if(iRet==0){ //error
		nclog(L"DeleteFile failed: lasterror=0x%0x\n", GetLastError());
	}
	else
		nclog(L"DeleteFile SUCCESS: returned 0x%0x\n", iRet);
	nclog(L"...DelFile\n");
	return (iRet);
}

void EnsureDirectory1(const TCHAR *rootdir, const TCHAR *dir)
{ if (rootdir!=0 && GetFileAttributes(rootdir)==0xFFFFFFFF) CreateDirectory(rootdir,0);
  if (*dir==0) return;
  const TCHAR *lastslash=dir, *c=lastslash;
  while (*c!=0) {if (*c=='/' || *c=='\\') lastslash=c; c++;}
  const TCHAR *name=lastslash;
  if (lastslash!=dir)
  { TCHAR tmp[MAX_PATH]; memcpy(tmp,dir,sizeof(TCHAR)*(lastslash-dir));
    tmp[lastslash-dir]=0;
    EnsureDirectory1(rootdir,tmp);
    name++;
  }
  TCHAR cd[MAX_PATH]; *cd=0; if (rootdir!=0) _tcscpy(cd,rootdir); _tcscat(cd,dir);
  if (GetFileAttributes(cd)==0xFFFFFFFF) CreateDirectory(cd,NULL);
}

char *replace_char_in_string(char *string, char to_search, char to_replace_with)
{
    char *buf;
    int i=0,j=0;
    char temp[1024];

    while(string[i] !='\0')
    {
		if(string[i] == to_search )
		{
			temp[j] = to_replace_with;
			i++;
			j++;
		}
		else
			temp[j++] = string[i++];
    }
    temp[j] = '\0';
    buf = (char *) malloc(strlen(temp) + 1);
    strcpy(buf,temp);
    // printf("%s\n",buf);
    return buf;
}


BOOL abort_p=false;
void unzipWithProgress(const TCHAR *zipfn, TCHAR* baseDir)
{ 
	nclog(L"SetupDll unzipWithProgress version: 1.3\n");

	nclog(L"unzipWithProgress started\n");
	HZIP hz = OpenZip(zipfn,0);
	if(hz==0){
		nclog(L"OpenZip for '%s' failed.\n", zipfn);
	}

	unsigned long uRes = SetUnzipBaseDir(hz, baseDir);
	nclog(L"SetUnzipBaseDir to '%s' returned %u\n", baseDir, uRes);

	nclog(L"OpenZip OK. Getting zip entries\n");

	ZIPENTRY ze; 
	uRes = GetZipItem(hz,-1,&ze); 
	if (uRes == ZR_OK){
		nclog(L"Found %i zip entries\n", ze.index);

		int numentries=ze.index;
		// first we retrieve the total size of all zip items
		DWORD tot=0; 
		int i; //counter var
		for (i=0; i<numentries; i++) {
			GetZipItem(hz,i,&ze); 
			tot+=ze.unc_size;
		}
		nclog(L"Total size=%l\n", tot);

		iProgressMax=tot;
		SendMessage(FindWindow(L"myWin", NULL), WM_UPDATE_TEXT, 0, 0);

		//
		DWORD countall=0; // this is our progress so far
		for (i=0; i<numentries && !abort_p; i++){ 

			nclog(L"Trying to get zip entry #%i\n", i);
			uRes = GetZipItem(hz,i,&ze);

			if(uRes==ZR_OK){
				//TCHAR* szMsg = new TCHAR[MAX_PATH];
				// We'll unzip each file bit by bit, to a file on disk
				TCHAR fn[1024];
				TCHAR szMsg[MAX_PATH+1];
				DWORD dwRes=0;
				wsprintf(fn, L"\\z\\%s", ze.name);
				wsprintf(szMsg, L"%s", ze.name);
				nclog(L"\tSendMessage '%s'\n", szMsg);

				wsprintf(szPuffer, szMsg);
				SendMessage(FindWindow(L"myWin", NULL), WM_UPDATE_TEXT, 0, 0);
				nclog(L"\tGot name of zip entry '%s'\n", ze.name);

				//NEW: to ensure we can overwrite IN_ROM files, we will use a temporary file and copy that later
/*				TCHAR tempFilename[MAX_PATH];
				wsprintf(tempFilename, L"\\Temp\\%s", ze.name);

				TCHAR pathOnly[MAX_PATH]; TCHAR filenameOnly[MAX_PATH];
				stripfilenameandpath(tempFilename, pathOnly, filenameOnly);
				createDirs(L"\\Temp", pathOnly);

				TCHAR finalFilename[MAX_PATH];
				wsprintf(finalFilename, L"%s%s", baseDir, ze.name);
*/
				TCHAR dst[MAX_PATH]; 
				wsprintf(dst, L"%s%s", baseDir, ze.name);

				//make unique backslash and replace / by backslash
				char dstA[MAX_PATH];
				char *dstTemp=new char[MAX_PATH];
				wcstombs(dstA, dst, MAX_PATH);
				
				dstTemp = replace_char_in_string(dstA, '/', '\\');
				mbstowcs(dst, dstTemp, MAX_PATH);

				nclog(L"\t********** dst = '%s'\n", dst);

				const TCHAR *ufn = (const TCHAR*)dst;
				const TCHAR *name=ufn; 
				const TCHAR *c=name; 
				while (*c!=0) {
					if (*c=='/' || *c=='\\') name=c+1; 
					c++;
				}
				
				TCHAR dir[MAX_PATH]; 
				_tcscpy(dir,ufn); 
				if (name==ufn) 
					*dir=0; 
				else 
					dir[name-ufn]=0;

				TCHAR fName[MAX_PATH]; 				
				bool isabsolute = (dir[0]=='/' || dir[0]=='\\' || (dir[0]!=0 && dir[1]==':'));
				
				if (isabsolute) {
					wsprintf(fName,_T("%s%s"),dir,name); 
					EnsureDirectory1(0,dir);
				}
				//else {wsprintf(fName,_T("%s%s%s"),rootdir,dir,name); EnsureDirectory1(rootdir,dir);}

				nclog(L"\t********** CreateFile for '%s'\n", fName);
				HANDLE hf = CreateFile(fName,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				//use a temp file
				//HANDLE hf = CreateFile(tempFilename,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
				char buf[16384]; // Each chunk will be 16k big. After each chunk, we show progress
				DWORD countfile=0;
				for (ZRESULT zr=ZR_MORE; zr==ZR_MORE && !abort_p; ) // nb. the global "abort_p" flag is set by the user clicking the Cancel button.
				{ 
					zr=UnzipItem(hz,i,buf,16384);
					nclog(L"calling UnzipItem returned %i\n", zr);
					unsigned long bufsize=16384; 
					if (zr==ZR_OK) 
						bufsize=ze.unc_size-countfile; // bufsize is how much we got this time
					DWORD writ; 
					WriteFile(hf,buf,bufsize,&writ,0); 
					countfile+=bufsize; // countfile counts how much of this file we've unzipped so far
					countall+=bufsize; // countall counts how much total we've unzipped so far
					// Now show progress, and let Windows catch up...
					int iPos = (int)(100.0*((double)countall)/((double)tot));
					
//					nclog(".");
//					wcscat(szPuffer, L".\0");
					iProgressPos=iPos;
					SendMessage(FindWindow(L"myWin", NULL), WM_UPDATE_TEXT, 0, 0);
					
					//PumpMessages();
				}
				if(ze.unc_size==0){
					nclog("L...touching 0 bytes file\n");
					DWORD writ=0;
					WriteFile(hf,buf,0,&writ,0); //
				}
				CloseHandle(hf);

				//now copy the temp file on top of the target file
/*				if (CopyFile(tempFilename, finalFilename, FALSE))
					nclog(L"Copying tempfile '%s' to target '%s' OK\n", tempFilename, finalFilename);
				else
					nclog(L"Copying tempfile '%s' to target '%s' FAILED with 0x%0x\n", tempFilename, finalFilename, GetLastError());

				DeleteFile(tempFilename);
*/
				if (abort_p) 
					DeleteFile(fn); //finalFilename);
			}
			else
				nclog(L"\tGetZipItem failed \n");

			nclog(L"\n");
		} //for
		CloseZip(hz);
	}
	SendMessage(FindWindow(L"myWin", NULL), WM_QUIT, 0, 0);
}
