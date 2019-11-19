#pragma once

#include <windows.h>
#include <iostream>
#include <tchar.h>

#define OLS_DLL_NO_ERROR						0
#define OLS_DLL_UNSUPPORTED_PLATFORM			1
#define OLS_DLL_DRIVER_NOT_LOADED				2
#define OLS_DLL_DRIVER_NOT_FOUND				3
#define OLS_DLL_DRIVER_UNLOADED					4
#define OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK	5
#define OLS_DLL_UNKNOWN_ERROR					9

typedef unsigned long long QWORD;
typedef bool(*InitializeOls_T)();
typedef void(*DeinitializeOls_T)();
typedef DWORD(*GetDllStatus_T)();
typedef DWORD(*GetDllVersion_T)(BYTE* major, BYTE* minor, BYTE* revision, BYTE* release);
typedef DWORD(*GetDriverVersion_T)(BYTE* major, BYTE* minor, BYTE* revision, BYTE* release);
typedef void(*Wrmsr_T)(DWORD index, DWORD eax, DWORD edx);
typedef bool(*Rdmsr_T)(DWORD index, DWORD* eax, DWORD* edx);
typedef bool(*IsMsr_T)();