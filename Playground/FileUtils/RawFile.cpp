#include "./RawFile.h"

#include <cstdlib>
#include <stdarg.h>
#include <string.h>


#include "./FileMacros.h"

//==========================================================================

RawFile::RawFile(const char* path, const char* mode) :
	size(0),
	fp(nullptr)
{
	my_fopen(&fp, path, mode);
}

RawFile::RawFile(FILE* fp, size_t size) :
	size(size),
	fp(fp)
{
}


RawFile::~RawFile()
{
	this->Close();
}


bool RawFile::IsOpened() const
{
	return (this->fp != nullptr);
}

size_t RawFile::GetSize() const
{
	if (this->size == 0)
	{
		my_fseek(fp, 0L, SEEK_END);
		this->size = static_cast<size_t>(my_ftell(fp));
		my_fseek(fp, 0L, SEEK_SET);
	}

	return this->size;
}

size_t RawFile::Read(void* buffer, size_t elementSize, size_t elementCount)
{
	return fread(buffer, elementSize, elementCount, fp);
}

void RawFile::Seek(long  offset, int origin)
{
	my_fseek(fp, offset, origin);
}

void RawFile::Flush()
{
	fflush(fp);
}

size_t RawFile::Write(const void* buffer, size_t elementSize, size_t elementCount)
{
	return fwrite(buffer, elementSize, elementCount, fp);
}

void* RawFile::GetRawFilePtr()
{
	return this->fp;
}

void RawFile::Close()
{
	if (fp)
	{
		fclose(fp);
		fp = nullptr;
	}
}
