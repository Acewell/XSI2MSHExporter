#include "ZeroTypes.h"

#if (PLATFORM == PLATFORM_X86_WIN32)
#include "ZeroCallStack.h"
#include "ZeroDebug.h"
#include "ZeroList.h"
#include <imagehlp.h>
#pragma comment (lib,"imagehlp.lib")

namespace ZeroCallStack
{
	HANDLE hProcess;
	bool bSymIsInit = false;

	typedef ZeroVisList<unsigned> AddressList;

	void Init(void)
	{
		if (bSymIsInit)
			return;

		// get the current process handle
		hProcess = GetCurrentProcess ();

		// set the load lines option
		SymSetOptions (SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

		// initialize the symbol library
		if (!SymInitialize(hProcess, NULL, TRUE))
		{
			ZeroWarn((0U, "Could not initialize symbol engine"));
			bSymIsInit = false;
		}
		else
		{
			bSymIsInit = true;
		}
	}

	void Done(void)
	{
		if (!bSymIsInit)
			return;

		// cleanup the symbol library
		SymCleanup(hProcess);
		bSymIsInit = false;
	}

	static unsigned ConvertAddress (unsigned aAddress, char aOutBuffer[])
	{
		// create an image symbol structure
		char szTemp[MAX_PATH + sizeof(IMAGEHLP_SYMBOL)];
		PIMAGEHLP_SYMBOL symbolInfo = (PIMAGEHLP_SYMBOL)&szTemp;
		ZeroMemory (symbolInfo, MAX_PATH + sizeof (IMAGEHLP_SYMBOL));
		symbolInfo->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL);
		symbolInfo->Address = aAddress;
		symbolInfo->MaxNameLength = MAX_PATH;

		// create a module info structure
		IMAGEHLP_MODULE moduleInfo;
		ZeroMemory (&moduleInfo, sizeof (IMAGEHLP_MODULE));
		moduleInfo.SizeOfStruct = sizeof (IMAGEHLP_MODULE);

		// start at the beginning of the output buffer
		char *bufferPos = aOutBuffer;

		// add the hexadecimal address
		bufferPos += sprintf (bufferPos, "0x%08X ", aAddress);

		// get the module info
		if (SymGetModuleInfo (hProcess, aAddress, &moduleInfo))
		{
			// strip off any path from the name
			char *moduleName = strrchr (moduleInfo.ImageName, '\\');
			if (moduleName)
				moduleName++;
			else
				moduleName = moduleInfo.ImageName;
			bufferPos += sprintf (bufferPos, "%s: ", moduleName);
		}
		else
		{
			bufferPos += sprintf (bufferPos, "<unknown module>: ");
		}

		// get the function info
		DWORD symbolDisp;
		if (SymGetSymFromAddr (hProcess, aAddress, &symbolDisp, symbolInfo))
		{
			if (0 == symbolDisp)
			{
				bufferPos += sprintf (bufferPos, "%s", symbolInfo->Name);
			}
			else
			{
				bufferPos += sprintf (bufferPos, "%s [+%d bytes]", symbolInfo->Name, symbolDisp);
			}

			// get the source line
			IMAGEHLP_LINE lineInfo;
			ZeroMemory (&lineInfo, sizeof (IMAGEHLP_LINE));
			lineInfo.SizeOfStruct = sizeof (IMAGEHLP_LINE);
			if (SymGetLineFromAddr (hProcess, aAddress, &symbolDisp, &lineInfo))
			{
				// append to the function info
				bufferPos += sprintf (bufferPos,
									 " %s (%d)",
									 lineInfo.FileName,
									 lineInfo.LineNumber);
				if (0 != symbolDisp)
				{
					bufferPos += sprintf (bufferPos,
										 " [+%d bytes]",
										 symbolDisp);
				}
			}
		}
		else
		{
			bufferPos += sprintf (bufferPos, "<unknown symbol>");
		}

		// add a newline
		bufferPos += sprintf (bufferPos, "\n");

		// return the number of characters added
		return (bufferPos - aOutBuffer);
	}

	void Trace(char aBuffer[], unsigned aSize, unsigned aLevels, unsigned aSkip)
	{
		if (!bSymIsInit)
		{
			Init();
			return;
		}

		// get the current thread
		HANDLE hThread = GetCurrentThread();

		// temporary list of addresses
		AddressList addressList;

		// get thread context information
		CONTEXT threadContext;
		threadContext.ContextFlags = CONTEXT_FULL;
		if (GetThreadContext (GetCurrentThread(), &threadContext))
		{
			// set up the stack frame data
			STACKFRAME stackFrame;
			ZeroMemory (&stackFrame, sizeof (STACKFRAME));
			stackFrame.AddrPC.Offset	= threadContext.Eip;
			stackFrame.AddrPC.Mode		= AddrModeFlat;
			stackFrame.AddrStack.Offset	= threadContext.Esp;
			stackFrame.AddrStack.Mode	= AddrModeFlat;
			stackFrame.AddrFrame.Offset	= threadContext.Ebp;
			stackFrame.AddrFrame.Mode	= AddrModeFlat;

			// get the first aLevels stack elements.
			for (unsigned i = 0; i < aSkip + aLevels; i++)
			{
				// perform a stack walk
				if (!StackWalk(IMAGE_FILE_MACHINE_I386,
					hProcess, hThread, &stackFrame, &threadContext,
					NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))
				{
					break;
				}

				// if not skipping...
				if (i >= aSkip)
				{
					// append the address
					addressList.Append(stackFrame.AddrPC.Offset);
				}
			}

			// convert addresses to names
			unsigned sizeLeft = aSize;
			unsigned symbolSize;

			char symbolName[MAX_PATH * 2];
			char *bufferPos = aBuffer;

			AddressList::Iterator loop(addressList);
			for (loop.Begin(); loop; loop++)
			{
				symbolSize = ConvertAddress(*loop, symbolName);
				if (sizeLeft < symbolSize)
				{
					break;
				}
				strcpy (bufferPos, symbolName);
				bufferPos += symbolSize;
				sizeLeft -= symbolSize;
			}
		}
	}
}

#endif
