#ifndef RTags_h
#define RTags_h

#include <typeinfo>
#include <ByteArray.h>
#include <Path.h>
#include <Log.h>
#include <memory>
#include <stdio.h>
#include <assert.h>
#include <getopt.h>
#include "Location.h"

namespace RTags
{
enum ReferenceType {
    NormalReference,
    MemberFunction,
    GlobalFunction
};

enum UnitType {
    CompileC,
    CompileCPlusPlus,
    PchC,
    PchCPlusPlus
};

static inline Path rtagsDir() {
    char buf[128];
    int w = snprintf(buf, sizeof(buf), "%s/.rtags/", getenv("HOME"));
    return Path(buf, w);
}

enum KeyFlag {
    NoFlag = 0x0,
    Padded = 0x1,
    ShowContext = 0x2,
    ShowLineNumbers = 0x4
};

static inline int digits(int len)
{
    int ret = 1;
    while (len >= 10) {
        len /= 10;
        ++ret;
    }
    return ret;
}

ByteArray shortOptions(const option *longOptions);
int readLine(FILE *f, char *buf = 0, int max = -1);
bool removeDirectory(const char *path);
int canonicalizePath(char *path, int len);
ByteArray unescape(ByteArray command);

template <typename T> class Ptr : public std::tr1::shared_ptr<T>
{
public:
    Ptr(T *t = 0)
        : std::tr1::shared_ptr<T>(t)
    {}
    operator T*() const { return std::tr1::shared_ptr<T>::get(); }
};
bool startProcess(const Path &dotexe, const List<ByteArray> &dollarArgs);

void findApplicationDirPath(const char *argv0);
Path applicationDirPath();
}

class FileInformation;
class CursorInfo;
typedef Map<Location, CursorInfo> SymbolMap;
typedef Map<Location, std::pair<Location, RTags::ReferenceType> > ReferenceMap;
typedef Map<ByteArray, Set<Location> > SymbolNameMap;
typedef Map<uint32_t, Set<uint32_t> > DependencyMap;
typedef std::pair<ByteArray, time_t> WatchedPair;
typedef Map<ByteArray, Location> PchUSRMap;
typedef Map<Path, Set<WatchedPair> > WatchedMap;
typedef Map<uint32_t, FileInformation> InformationMap;

#define eintrwrap(VAR, BLOCK)                  \
    do {                                       \
        VAR = BLOCK;                           \
    } while (VAR == -1 && errno == EINTR)

class CursorInfo;
class CXStringScope
{
public:
    CXStringScope(CXString str)
        : string(str)
    {
    }

    ~CXStringScope()
    {
        clang_disposeString(string);
    }
    CXString string;
};

static inline bool match(uint32_t fileId, const Location &loc)
{
    return loc.fileId() == fileId;
}

static inline bool match(const Set<uint32_t> &fileIds, const Location &loc)
{
    return fileIds.contains(loc.fileId());
}


#endif
