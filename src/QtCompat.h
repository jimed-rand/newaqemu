#ifndef QT_COMPAT_H
#define QT_COMPAT_H

#include <QtGlobal>
#include "RegExpCompat.h"

#include "RegExpCompat.h"

#define QRegExp RegExpCompat

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#  define QRegExpValidator RegExpCompatValidator
#  ifndef QStringSkipEmptyParts
#    define QStringSkipEmptyParts Qt::SkipEmptyParts
#  endif
#else
#  ifndef QStringSkipEmptyParts
#    define QStringSkipEmptyParts QString::SkipEmptyParts
#  endif
#endif

#endif
