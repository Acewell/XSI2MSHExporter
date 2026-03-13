/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/

#include "ZeroTypes.h"
#include "ZeroVirtualMem.h"


#define VIRTUAL_MEM_GRANULARITY 65536

ZeroVirtualMemory *ZeroVirtualMemory::root;


//void *Virtual_Memory_Alloc (
ZeroVirtualMemory::ZeroVirtualMemory (
	int Commited_Size,
	int Max_Size)
	:ZeroUnknown ()
{
   maxSize = 0;
   baseAddress = 0;
   next = NULL;

#if (PLATFORM == PLATFORM_XBOX)
   int Page_Size = 4096;
#else
   SYSTEM_INFO Sys_Info;
   GetSystemInfo(&Sys_Info);
   int Page_Size = Sys_Info.dwPageSize;
#endif
	
   Commited_Size &= -(int)Page_Size;
   Commited_Size += Page_Size;
	
   if (Max_Size < Commited_Size ) {
		Max_Size = Commited_Size;
	}
	
   maxSize = (Max_Size - 1) & (- VIRTUAL_MEM_GRANULARITY);
   maxSize += VIRTUAL_MEM_GRANULARITY;
	
   baseAddress = VirtualAlloc (NULL, maxSize, MEM_RESERVE |MEM_TOP_DOWN, PAGE_READWRITE);
   if (!baseAddress) {
		throw "Virtual_Mem_Buffer: Unable to commit Virtual Memory";
	}
	
   if (!VirtualAlloc (baseAddress, Commited_Size, MEM_COMMIT, PAGE_READWRITE)) {
      VirtualFree (baseAddress, 0, MEM_RELEASE);
      throw "Virtual_Mem_Buffer: Unable to commit Virtual Memory";
   }
	
   maxSize = maxSize;
   pageSize = Page_Size;
   next = root;
   root = this;
}

ZeroVirtualMemory::~ZeroVirtualMemory()
{
   ZeroVirtualMemory *Tmp;
   ZeroVirtualMemory *Prev;
	
   Prev = NULL;
   for (Tmp = ZeroVirtualMemory::root; Tmp; Prev = Tmp, Tmp= Tmp->next) {
      if (Tmp != this) {
			continue;
		}
      if (Prev) {
			Prev->next = Tmp->next;
		} else {
			root = Tmp->next;
		}
      VirtualFree (baseAddress, 0, MEM_RELEASE);
      break;
   }
}



int __cdecl ZeroVirtualMemory::ExecptionHandler (void *Except_Ptr)
{
   unsigned int Offending_Address;
   unsigned int Low_Address;
   unsigned int Hi_Address;
   unsigned int Spawn_Ptr;
   EXCEPTION_RECORD *Record;
   ZeroVirtualMemory  *Tmp;
   MEMORY_BASIC_INFORMATION Mem_Info;

   Record = ((EXCEPTION_POINTERS *)Except_Ptr)->ExceptionRecord;

   if (Record->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) {
		return 0;
	}

   Offending_Address = Record->ExceptionInformation [1];

   for (Tmp = ZeroVirtualMemory::root; Tmp; Tmp= Tmp->next) {
      Low_Address = (unsigned int ) Tmp->baseAddress;
      Hi_Address = Low_Address + Tmp->maxSize;
      if ( (Low_Address <= Offending_Address) && (Hi_Address > Offending_Address)) {
         VirtualQuery(Tmp->baseAddress, &Mem_Info, sizeof (MEMORY_BASIC_INFORMATION));
         Spawn_Ptr = (unsigned int)Mem_Info.BaseAddress + Mem_Info.RegionSize;
         if ( Spawn_Ptr >= (Hi_Address - Tmp->pageSize)) {
//            char Message [256];
//            sprintf (Message, "Virtual Access Violation Error cr2=%p", Spawn_Ptr);
//            MessageBox ( NULL, Message, "LIB3d Error", IDOK );
			 _ASSERTE (0);
            return 0;
         }

         VirtualAlloc ((void*)Spawn_Ptr, Tmp->pageSize * 64, MEM_COMMIT, PAGE_READWRITE);
         return -1;
      }
   }
   return 0;
}


int ZeroVirtualMemory::TotalMemoryUsed ()
{
   int Memsize;
   ZeroVirtualMemory *Tmp;
   MEMORY_BASIC_INFORMATION Mem_Info;

   Memsize = 0;
   for (Tmp = root; Tmp; Tmp= Tmp->next) {
      VirtualQuery(Tmp->baseAddress, &Mem_Info, sizeof (MEMORY_BASIC_INFORMATION));
      Memsize += Mem_Info.RegionSize;
   }
   return Memsize;
}


