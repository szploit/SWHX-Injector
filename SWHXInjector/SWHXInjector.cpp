#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <vector>

DWORD pid;
std::vector<DWORD> ids;

DWORD GetPID(const wchar_t* name)
{
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe{ pe.dwSize = sizeof(pe) };

	if (Process32First(hsnap, &pe))
	{
		do
		{
			if (!_wcsicmp(pe.szExeFile, name))
			{
				pid = pe.th32ProcessID;
				break;
			}
		} while (Process32Next(hsnap, &pe));
	}

	CloseHandle(hsnap);
	return pid;
}

std::vector<DWORD> GetThreads()
{
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	THREADENTRY32 te{ te.dwSize = sizeof(te) };

	if (Thread32First(hsnap, &te))
	{
		do
		{
			if (te.th32OwnerProcessID == pid)
			{
				ids.push_back(te.th32ThreadID);
			}
		} while (Thread32Next(hsnap, &te));
	}

	CloseHandle(hsnap);
	return ids;
}

int main()
{
	DWORD pid = GetPID(L"Idle Slayer.exe");
	std::vector<DWORD> threads = GetThreads();
	HMODULE dll = LoadLibraryEx(L"exploit.dll", NULL, DONT_RESOLVE_DLL_REFERENCES);

	std::cout << "Process ID: " << pid << "\n";

	if (dll == NULL)
	{
		std::cout << "Couldn't find the dll\n";
		return 0;
	}

	HOOKPROC hook = reinterpret_cast<HOOKPROC>(GetProcAddress(dll, "NextHook"));

	if (hook == NULL)
	{
		std::cout << "Failed the function wasn't found\n";
		return 0;
	}

	std::vector<HHOOK> hooks;
	for (DWORD tid : threads)
	{
		HHOOK h = SetWindowsHookEx(WH_GETMESSAGE, hook, dll, tid);
		PostThreadMessage(tid, WM_NULL, NULL, NULL);
		if (h) hooks.push_back(h);
	}

	std::cout << "Set the hook with SetWindowHookEx\n";
	std::cin.get();

	for (HHOOK h : hooks)
		UnhookWindowsHookEx(h);
}
