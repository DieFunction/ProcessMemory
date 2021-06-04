#include <Windows.h>
#include <TlHelp32.h>

class ProcessMemory{
	DWORD procId; 
	HANDLE procHandle;
	public:
		ProcessMemory(char* procName){
			HANDLE pidHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			PROCESSENTRY32 procEntry;
			procEntry.dwSize = sizeof(procEntry);
			do{
				if (!strcmp(procEntry.szExeFile, procName)){
					procId = procEntry.th32ProcessID;
					CloseHandle(pidHandle);
					procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procId);
					return;
				}
			}while(Process32Next(pidHandle, &procEntry));
			CloseHandle(pidHandle);
			exit(0);
		}
		
		template <class data>
		data read(DWORD address){                         
			data value;
			ReadProcessMemory(procHandle, (LPVOID)address, &value, sizeof(data), NULL);
			return value;
		}
		
		template <class data>
		data read(DWORD address, DWORD offsets[], int offsetsSize){
			address = read<DWORD>(address);
			for(int i = 0; i < offsetsSize-1; i++){
				address = read<DWORD>(address + offsets[i]);
			}
			return read<data>(address + offsets[offsetsSize-1]);
		}
	
		template <class data>
		void write(DWORD address, data value){  
			WriteProcessMemory(procHandle, (LPVOID)address, &value, sizeof(data), NULL);
			return;
		}
		
		template <class data>
		void write(DWORD address, DWORD offsets[], int offsetsSize, data value){
			address = read<DWORD>(address);
			for(int i = 0; i < offsetsSize-1; i++){
				address = read<DWORD>(address + offsets[i]);
			}
			write<data>(address + offsets[offsetsSize-1], value);
		}
		
		DWORD getModule(char* moduleName){
			HANDLE moduleHandle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
			if(moduleHandle != INVALID_HANDLE_VALUE){
				MODULEENTRY32 moduleEntry;
				moduleEntry.dwSize = sizeof(moduleEntry);
				if(Module32First(moduleHandle, &moduleEntry)){
					do{
						if(!strcmp((char*)moduleEntry.szModule, moduleName)){
							CloseHandle(moduleHandle);
							return (DWORD_PTR)moduleEntry.modBaseAddr;
						}
					}while(Module32Next(moduleHandle, &moduleEntry));
				}
			}
			CloseHandle(moduleHandle);
		    return 0;
		}		
};
