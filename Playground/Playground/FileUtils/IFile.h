#ifndef IFILE_WRAPPER_H
#define IFILE_WRAPPER_H


#include <cstdio>

struct IFile
{
	virtual ~IFile() = default;

	virtual size_t GetSize() const = 0;
	virtual size_t Read(void* buffer, size_t elementSize, size_t elementCount) = 0;
	virtual void Seek(long  offset, int origin) = 0;

	virtual void Flush() = 0;
	virtual size_t Write(const void* buffer, size_t elementSize, size_t elementCount) = 0;

	virtual void Close() = 0;
	virtual void* GetRawFilePtr() = 0;


	size_t ReadAll(void** buffer);
};


#endif

