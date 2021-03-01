#ifndef RAW_FILE_WRAPPER_H
#define RAW_FILE_WRAPPER_H


#include "./IFile.h"

struct RawFile : public IFile
{
	RawFile(const char* path, const char* mode = "rb");
	RawFile(FILE* fp, size_t size = 0);
	virtual ~RawFile();

	bool IsOpened() const;


	size_t GetSize() const override;
	size_t Read(void* buffer, size_t elementSize, size_t elementCount) override;
	void Seek(long  offset, int origin) override;

	void Flush() override;
	size_t Write(const void* buffer, size_t elementSize, size_t elementCount) override;

	void Close() override;
	void* GetRawFilePtr() override;

protected:
	mutable size_t size;
	FILE* fp;
};



#endif

