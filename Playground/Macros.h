#ifndef MY_MACROS_H

#define MY_MACROS_H


#ifndef SAFE_DELETE
#   define SAFE_DELETE(a) {if (a != nullptr) { delete   a; a = nullptr; }};
#endif

#ifndef SAFE_DELETE_ARRAY
#   define SAFE_DELETE_ARRAY(a) {if (a != nullptr) { delete[] a; a = nullptr; }};
#endif


#endif
