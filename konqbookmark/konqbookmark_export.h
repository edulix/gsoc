#ifndef KONQBOOKMARK_EXPORT_H
#define KONQBOOKMARK_EXPORT_H
 
// needed for KDE_EXPORT and KDE_IMPORT macros
#include <kdemacros.h>
 
#ifndef KONQBOOKMARK_EXPORT
# if defined(MAKE_KONQBOOKMARK_LIB)
   // We are building this library
#  define KONQBOOKMARK_EXPORT KDE_EXPORT
# else
   // We are using this library
#  define KONQBOOKMARK_EXPORT KDE_IMPORT
# endif
#endif
 
# ifndef KONQBOOKMARK_EXPORT_DEPRECATED
#  define KONQBOOKMARK_EXPORT_DEPRECATED KDE_DEPRECATED KONQBOOKMARK_EXPORT
# endif
 
#endif