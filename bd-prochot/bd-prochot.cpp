#include "bd-prochot.h"

// This program can enable or disable the BD PROCHOT feature of Intel CPUs by writing to the MSR (model-specific register).
// The manual for the WinRing0 library can be found at https://openlibsys.org/manual/
// For some reason the method signatures don't match up, e.g. Rdmsr takes three parameters instead of two.
int main(int argc, char* argv[])
{
#ifdef _WIN64
	auto winRing0Dll = LoadLibrary(_T(".\\WinRing0x64.dll"));
#else
	auto winRing0Dll = LoadLibrary(_T(".\\WinRing0.dll"));
#endif

	if (!winRing0Dll) {
		auto error = GetLastError();
		std::cout << "LoadLibrary failed with error " << error << std::endl;
		if (error == 126) { // ERROR_MOD_NOT_FOUND
			std::cout << "Make sure the .dll file is in the same folder as the program" << std::endl;
		}

		return EXIT_FAILURE;
	}

	auto InitializeOls = (InitializeOls_T)GetProcAddress(winRing0Dll, "InitializeOls");
	auto DeinitializeOls = (DeinitializeOls_T)GetProcAddress(winRing0Dll, "DeinitializeOls");
	auto GetDllStatus = (GetDllStatus_T)GetProcAddress(winRing0Dll, "GetDllStatus");
	auto GetDllVersion = (GetDllVersion_T)GetProcAddress(winRing0Dll, "GetDllVersion");
	auto GetDriverVersion = (GetDriverVersion_T)GetProcAddress(winRing0Dll, "GetDriverVersion");
	auto Wrmsr = (Wrmsr_T)GetProcAddress(winRing0Dll, "Wrmsr");
	auto Rdmsr = (Rdmsr_T)GetProcAddress(winRing0Dll, "Rdmsr");
	auto IsMsr = (IsMsr_T)GetProcAddress(winRing0Dll, "IsMsr");

	if (!InitializeOls()) {
		std::cout << "InitializeOls failed. DllStatus: ";

		// OLS_DLL_DRIVER_NOT_LOADED happens when program is run as non-admin.
		// Since the manifest defines that admin privileges are required, this should not happen.
		auto dllStatus = GetDllStatus();
		switch (dllStatus) {
		case OLS_DLL_NO_ERROR:						std::cout << "OLS_DLL_NO_ERROR\n";						break;
		case OLS_DLL_UNSUPPORTED_PLATFORM:			std::cout << "OLS_DLL_UNSUPPORTED_PLATFORM\n";			break;
		case OLS_DLL_DRIVER_NOT_LOADED:				std::cout << "OLS_DLL_DRIVER_NOT_LOADED\n";				break;
		case OLS_DLL_DRIVER_NOT_FOUND:				std::cout << "OLS_DLL_DRIVER_NOT_FOUND\n";				break;
		case OLS_DLL_DRIVER_UNLOADED:				std::cout << "OLS_DLL_DRIVER_UNLOADED\n";				break;
		case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:	std::cout << "OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK\n";	break;
		case OLS_DLL_UNKNOWN_ERROR:					std::cout << "OLS_DLL_UNKNOWN_ERROR\n";					break;
		}

		return EXIT_FAILURE;
	}

	BYTE major, minor, revision, release;
	GetDllVersion(&major, &minor, &revision, &release);
	std::cout << "DLL Version: " 
		<< static_cast<int>(major) << "." 
		<< static_cast<int>(minor) << "." 
		<< static_cast<int>(revision) << "." 
		<< static_cast<int>(release) << std::endl;

	GetDriverVersion(&major, &minor, &revision, &release);
	std::cout << "Driver Version: "
		<< static_cast<int>(major) << "."
		<< static_cast<int>(minor) << "."
		<< static_cast<int>(revision) << "."
		<< static_cast<int>(release) << std::endl;

	if (!IsMsr()) {
		std::cout << "IsMsr failed.	Your CPU does not support changing the MSR" << std::endl;
		DeinitializeOls();
		return EXIT_FAILURE;
	}

	DWORD eax;
	DWORD edx;
	// Read the MSR at address 0x1FC. Higher-order bits go into edx, lower-order bits go into eax.
	if (!Rdmsr(0x1FC, &eax, &edx)) {
		std::cout << "Rdmsr failed" << std::endl;
		DeinitializeOls();
		return EXIT_FAILURE;
	}
	
	if (argc >= 2) {
		if (strcmp(argv[1], "enable") == 0) {
			// enable BD PROCHOT by setting the LSB to 1
			eax |= 0x01;
			std::cout << "Enabling BD PROCHOT" << std::endl;
			Wrmsr(0x1FC, eax, edx);
			std::cout << "Success" << std::endl;
		}
		else if (strcmp(argv[1], "disable") == 0)  {
			// disable BD PROCHOT by setting the LSB to 0
			eax &= 0xFFFFFFFE;
			std::cout << "Disabling BD PROCHOT" << std::endl;
			Wrmsr(0x1FC, eax, edx);
			std::cout << "Success" << std::endl;
		}
		else {
			std::cout << "Usage: Run without parameters to read the BD PROCHOT bit. Run with enable/disable paramters to enable/disable BD PROCHOT" << std::endl;
		}
	}
	else {
		std::cout << "BD PROCHOT bit is currently: " << (eax & 0x1) << std::endl;
	}
	
	DeinitializeOls();
	return EXIT_SUCCESS;
}
