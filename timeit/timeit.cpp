// timeit.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>


// timeit.cpp 
//
//#define _AFXDLL

// stolen from https://github.com/HPCE/hpce-2018-cw1/blob/master/src/mingw64-time.cpp
// Small changes, to convert from Mingw to Win32
// converted commandLine to unicode (LPWSTR), to match _tmain

//#define WIN32_LEAN_AND_MEAN
//
//#include <atlstr.h>

//#include "stdafx.h"
#include <windows.h>
//#include <minwinbase.h>

#include <string>
#include <math.h>

double toSeconds( const FILETIME &f)
{ 
    double x = f.dwLowDateTime + ldexp(f.dwHighDateTime , 32);
    return x * 1e-7;
}

//#include "pch.h"
//#include "framework.h"
//#include "timeit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object

//CWinApp theApp;

using namespace std;

//https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
int StringToWString(std::wstring& ws, const std::string& s)
{
    std::wstring wsTmp(s.begin(), s.end());

    ws = wsTmp;

    return 0;
}


int main(int argc, char* argv[])
{
    int nRetCode = 0;

    //HMODULE hModule = ::GetModuleHandle(nullptr);

    //if (hModule != nullptr)
    //{
    //    // initialize MFC and print and error on failure
    //    if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
    //    {
    //        // TODO: code your application's behavior here.
    //        wprintf(L"Fatal Error: MFC initialization failed\n");
    //        nRetCode = 1;
    //    }
    //    else
    //    {
    //        // TODO: code your application's behavior here.
    //    }
    //}
    //else
    //{
    //    // TODO: change error code to suit your needs
    //    wprintf(L"Fatal Error: GetModuleHandle failed\n");
    //    nRetCode = 1;
    //}

    /////////////////////////////

    if (argc < 2) {
        printf("Usage: timeit program [arg0 arg1 ...]\n");
        return 1;
    }

    std::string commandLine;
    std::wstring wCommandLine;
    commandLine = argv[1];
    for (int i = 2; i < argc; i++) {
        commandLine += " ";
        commandLine += argv[i];
    }
    StringToWString(wCommandLine, commandLine);


    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // turn that ascii commandline into unicode 
    /*CStringW cmdLineW(commandLine.c_str());
    LPCWSTR cmdLineWW = (LPCWSTR)cmdLineW;
    std::wstring cmdLineWWW(cmdLineWW);*/

    BOOL ret = CreateProcess(
        /*lpApplicationName*/ NULL,
        const_cast<LPWSTR> ( wCommandLine.c_str() ) , //const_cast<LPWSTR>(commandLine.c_str()),
        /*lpProcessAttributes*/ NULL,
        /*lpThreadAttributes*/ NULL,
        /*bInheritHandles*/ TRUE,
        /*dwCreationFlags*/ 0,
        /*lpEnvironment*/ 0,
        /*lpCurrentDirectory*/ NULL,
        /*lpStartupInfo*/ &si,
        /*lpProcessInformation*/ &pi
    );
    if (!ret) {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return 1;
    }

    HANDLE hProcess = pi.hProcess;

    WaitForSingleObject(hProcess, INFINITE);

    FILETIME CreationTime;
    FILETIME ExitTime;
    FILETIME KernelTime;
    FILETIME UserTime;

    ret = GetProcessTimes(
        hProcess,
        &CreationTime,
        &ExitTime,
        &KernelTime,
        &UserTime
    );
    if (!ret) {
        printf("GetProcessTimes failed (%d).\n", GetLastError());
        return 1;
    }

    double user = toSeconds(UserTime);
    double kernel = toSeconds(KernelTime);
    double real = toSeconds(ExitTime) - toSeconds(CreationTime);

    fprintf(stderr, "real %.3lf\n", real);
    fprintf(stderr, "user %.3lf\n", user);
    fprintf(stderr, "sys  %.3lf\n", kernel);

    CloseHandle(hProcess);

    return nRetCode;
}