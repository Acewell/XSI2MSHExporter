/****************************************************************************
*
*  Visual C++ 6.0 created by: Julio Jerez
*
****************************************************************************/
#include "ZeroTypes.h"
#include "ZeroList.h"
#include "ZeroStack.h"
#include "ZeroVirtualMem.h"

#if (PLATFORM == PLATFORM_X86_WIN32)
//#define USE_VIRTUAL_MEMORY
#endif

namespace InternalStack
{
#ifdef USE_VIRTUAL_MEMORY

	#pragma warning (push)
	#pragma warning (disable : 4291)

	const int MAX_VIRTUAL_STACK_SIZE  = 64 * 1024 * 1024;

	const int INIT_COMMIT = 4096;

	class VirtualStackBase 
	{
		friend void __cdecl SetMaxStackSize (int Size, int Commit);
	
		class ZeroStackBuffer: public ZeroVirtualMemory
		{
			ZeroUnknown *CreateClone(bool isSim) 
			{
				_ASSERT(!isSim);
				return NULL;
			}
	
	
			public:
			ZeroStackBuffer	(int Size, int Commit = INIT_COMMIT)
				:ZeroVirtualMemory (Commit, Size + 65536)
			{
			}
	
			~ZeroStackBuffer()
			{
			}
	
			void* __cdecl operator new (unsigned int size_t, void *Ptr) 
			{
				return Ptr;
			}
	
			void __cdecl operator delete (void *Ptr) 
			{
			}
		};
	
	#ifdef _DEBUG
		int accSize;
		int peakSize;
	#endif
	
		int totalSize;
		char BufferMem[sizeof (ZeroStackBuffer)];
	
		ZeroVirtualMemory *virtualPool;
		struct CHUNK {
			int Size;
		};
		CHUNK *Start;
		CHUNK *Head;
	
		public:
		VirtualStackBase () 
		{
			Init (MAX_VIRTUAL_STACK_SIZE, INIT_COMMIT);
		}
	
		void Init (int Size, int Commit) 
		{
			virtualPool = new (BufferMem) ZeroStackBuffer (Size, Commit);
	
			Start = (CHUNK*)virtualPool->GetBuffer();
			Start->Size = 0;
	
			totalSize = Size;
	#ifdef _DEBUG
			accSize = 0;
			peakSize = 0;
	#endif
	
			Head = Start;
		}
	
		~VirtualStackBase ()
		{
			virtualPool->Release();
		}
	
		void *Push (int Size)
		{
			unsigned Address;
			unsigned Bytes;
			char *Ptr;
			char *Buffer;
	
			Address = ((unsigned) &Head[1] + 15) & ((unsigned)-16); 
			Buffer =	(char*) Address;
	
			Ptr = Buffer + Size;
			Bytes = (unsigned) Ptr - (unsigned) Head;
			Head = (CHUNK*) Ptr;
	
	#ifdef _DEBUG
			accSize += Bytes;
			_ASSERTE (accSize < totalSize);
	#endif
	
			__try {
				Head->Size = Bytes;
			} __except (ZeroVirtualMemory::ExecptionHandler (_exception_info ())) {
			}
	
			return Buffer;
		}
	
		void Pop () 
		{
			char *Ptr;
			char *Buffer;
	
			_ASSERTE (Start <= Head);
	
	#ifdef _DEBUG
			accSize -= Head->Size;
			_ASSERTE (accSize >= 0);
	#endif
	
			Buffer = (char*) Head; 
	
			Ptr = Buffer - Head->Size;
			Head = (CHUNK*) Ptr;
		}
	};
	#pragma warning (pop)

	
	static VirtualStackBase& GetBase()
	{
		ZeroStartup::InitEngine();
		static VirtualStackBase virtualStack;
		return virtualStack;
	}

	void Startup()
	{
		GetBase();
	}

	
	void SetMaxStackSize (int Size, int Commit)
	{
		if (Size < MAX_VIRTUAL_STACK_SIZE) {
			return;
		}
		if (GetBase().virtualPool) {
		 	GetBase().virtualPool->Release();
		}
		GetBase().Init (Size, Commit);
	}
	
	StackPool::StackPool (int Size)
	{
		Ptr = GetBase().Push(Size);
	}
	
	StackPool::~StackPool ()
	{
		GetBase().Pop();
	}
#else 

#ifdef USE_NEW_MEMORY_MGR
  // Don't use memory here, just allocate temp ram at runtime - NM 7/10/02
  const int MAX_VIRTUAL_STACK_SIZE = 4096;
#else 

  #if (PLATFORM == PLATFORM_X86_WIN32)
	  const int MAX_VIRTUAL_STACK_SIZE  = 1024 * 1024 * 32;
  #elif (PLATFORM == PLATFORM_XBOX)
  	const int MAX_VIRTUAL_STACK_SIZE  = 1024 * 1024 * 2;
  #elif (PLATFORM == PLATFORM_PS2)
	  // Ken says "same as GC". but Salah and I say "a bit larger" (was
  	// 4MB) - NM 6/5/02
	  //
	  // 256Kb is too small to load Geonosis1/geo_bldg_lpad.msh (2350
	  // verts in one segment) - NM 6/12/02
  	const int MAX_VIRTUAL_STACK_SIZE  = 1024 * 320;
  #elif (PLATFORM == PLATFORM_GAMECUBE)
	  const int MAX_VIRTUAL_STACK_SIZE  = 1024 * 196;
  #endif

#endif

	static char stackPool[MAX_VIRTUAL_STACK_SIZE];


	class VirtualStackBase: public ZeroVisList <int> 
	{
		friend void __cdecl SetMaxStackSize (int Size, int Commit);
	
		int totalSize;
		char *basePtr;
		char *leadingPtr;
	
	
		public:
		VirtualStackBase () 
			:ZeroVisList <int> () 
		{
			unsigned val;

			totalSize = 0;
			val = (unsigned)stackPool;
			basePtr = (char*)((val + 15) & -16);
			leadingPtr = basePtr;
		}
	
		~VirtualStackBase ()
		{
		}
	
		void *Push (int size)
		{
#ifdef USE_NEW_MEMORY_MGR
			printf("Shouldn't be here-- zerostack.cpp line %d\n",__LINE__);
			return NULL;
#else
			void* retPtr;

			size = (size + 15) & -16;

			retPtr = leadingPtr;
			leadingPtr += size;

#ifdef _DEBUG
			totalSize += size;
			_ASSERTE (totalSize <= MAX_VIRTUAL_STACK_SIZE);
#endif
			Append (size);
			return retPtr;
#endif
		}
	
		void Pop () 
		{
#ifdef USE_NEW_MEMORY_MGR
			printf("Shouldn't be here-- zerostack.cpp line %d\n",__LINE__);
#else
			unsigned size;
			ZeroListNode *node;

			node = GetLast();
			_ASSERTE (node);

			size = node->info;
			Remove (node);

#ifdef _DEBUG
			totalSize -= size;
			_ASSERTE (totalSize >= 0);
#endif
			leadingPtr -= size;

#endif
		}
	};
	
	static VirtualStackBase& GetBase()
	{
		static VirtualStackBase virtualStack;
		return virtualStack;
	}

	void Startup()
	{
		GetBase();
	}

	
	void SetMaxStackSize (int Size, int Commit)
	{
	}
	
	StackPool::StackPool (int Size)
	{
#ifdef USE_NEW_MEMORY_MGR
		Ptr=ZEROMALLOC(Size,1,false);
		/*printf("ZeroStack reserved %d bytes at %08X line %d\n",Size,Ptr,__LINE__);
		unsigned int iPtr=(unsigned int)Ptr;
		if(iPtr == 0x283ABE0)
		printf("Suspicious allocation. Watch me!");*/
#else
		Ptr = GetBase().Push(Size);
#endif
	}
	
	StackPool::~StackPool ()
	{
#ifdef USE_NEW_MEMORY_MGR
		//		printf("Freeing from ZeroStack.cpp line %d. Ptr=%08X\n",__LINE__,Ptr);
		ZeroFree(Ptr);
		Ptr = NULL;
#else
		GetBase().Pop();
#endif
	}

#endif
}

void __cdecl SetMaxStackSize (int Size, int Commit)
{
	InternalStack::SetMaxStackSize (Size, Commit);
}
