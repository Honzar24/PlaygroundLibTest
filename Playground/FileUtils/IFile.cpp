#include "./IFile.h"

#include <cstdlib>
#include <stdarg.h>
#include <string.h>

size_t IFile::ReadAll(void** buffer)
{
	size_t fs = this->GetSize();
	*buffer = malloc(fs);

	return this->Read(*buffer, sizeof(char), fs);
}
