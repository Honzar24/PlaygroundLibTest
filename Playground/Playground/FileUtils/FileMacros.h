#ifndef FILE_MACROS_H
#define FILE_MACROS_H

#ifdef _MSC_VER
#	ifndef my_fopen 
#		define my_fopen(a, b, c) fopen_s(a, b, c)	
#	endif
#else
#	ifndef my_fopen 
#		define my_fopen(a, b, c) (*a = fopen(b, c))
#	endif
#endif

#ifdef _MSC_VER
#	ifdef _WIN64
#		ifndef my_fseek 
#			define my_fseek(a, b, c) _fseeki64(a, b, c)	
#		endif
#		ifndef my_ftell
#			define my_ftell(a) _ftelli64(a)	
#		endif
#	else
#		ifndef my_fseek
#			define my_fseek(a, b, c) fseek(a, b, c)	
#		endif
#		ifndef my_ftell
#			define my_ftell(a) ftell(a)	
#		endif
#	endif
#else
#	ifdef _WIN64	
#		ifndef my_fseek 
#			define my_fseek(a, b, c) fseeko64(a, b, c)	
#		endif
#		ifndef my_ftell
#			define my_ftell(a) ftello64(a)	
#		endif
#	else
#		ifndef my_fseek
#			define my_fseek(a, b, c) fseek(a, b, c)	
#		endif
#		ifndef my_ftell
#			define my_ftell(a) ftell(a)	
#		endif
#	endif
#endif





#endif
