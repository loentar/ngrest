#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <fnmatch.h>
#include <dirent.h>
#endif
#include "Exception.h"
#include "File.h"

#ifdef WIN32
#define stat _stat
#endif

namespace ngrest {

File::File(const std::string& path):
    path(path)
{
}

void File::list(StringList& list, const std::string& mask /*= "*" */,
                int attrs /*= AttributeAnyFile | AttributeDirectory */)
{
    list.clear();
#ifdef WIN32
    _finddata_t searchData;         // search record
    intptr_t file = _findfirst((path + "\\" + mask).c_str(), &searchData);

    if (file != -1) {
        do {
            if (!isDots(searchData.name) &&
                    (((attrs & AttributeDirectory) != 0)
                     && (searchData.attrib & _A_SUBDIR) != 0) ||
                    ((attrs & AttributeAnyFile) != 0))
                list.push_back(searchData.name);
        } while (!_findnext(file, &searchData));

        _findclose(file);
    }
#else
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct stat fileStat;
        dirent* fileDirent = nullptr;
        unsigned maskMode =
                ((attrs & AttributeDirectory) ? S_IFDIR : 0) |
                ((attrs & AttributeRegularFile) ? S_IFREG : 0) |
                ((attrs & AttributeOtherFile) ? (S_IFMT & (~(S_IFREG | S_IFDIR))) : 0);

        while ((fileDirent = readdir(dir))) {
            if (!isDots(fileDirent->d_name) &&
                    !fnmatch(mask.c_str(), fileDirent->d_name, 0) &&
                    !lstat(((path + "/") + fileDirent->d_name).c_str(), &fileStat) &&
                    (fileStat.st_mode & maskMode) == (fileStat.st_mode & S_IFMT))
                list.push_back(fileDirent->d_name);
        }

        closedir(dir);
    }
#endif
}

int File::getAttributes()
{
    int result = AttributeNone;

    struct stat fileStat;
    if (!stat(path.c_str(), &fileStat)) {
        if (fileStat.st_mode & S_IFDIR) {
            result |= AttributeDirectory;
        } else if (fileStat.st_mode & S_IFREG) {
            result |= AttributeRegularFile;
        } else {
            result |= AttributeOtherFile;
        }
    }
    return result;
}

bool File::isExists()
{
    return getAttributes() != AttributeNone;
}

bool File::isDirectory()
{
    return (getAttributes() & AttributeDirectory) != 0;
}

bool File::isRegularFile()
{
    return (getAttributes() & AttributeRegularFile) != 0;
}

bool File::isSystemFile()
{
    return (getAttributes() & AttributeOtherFile) != 0;
}

bool File::isFile()
{
    return (getAttributes() & AttributeAnyFile) != 0;
}

int64_t File::getTime()
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat))
        return 0;

    return static_cast<int64_t>(fileStat.st_mtime);
}

bool File::mkdir()
{
#ifndef WIN32
    int res = ::mkdir(path.c_str(), 0755);
#else
    int res = _mkdir(path.c_str());
#endif
    NGREST_ASSERT(res != -1 || errno == EEXIST, "Failed to create dir ["
                  + path + "]: " + strerror(errno));

    return !res;
}

bool File::mkdirs()
{
    int res = -1;
    for (std::string::size_type pos = 0; pos != std::string::npos;) {
        pos = path.find_first_of("/\\", pos + 1);
        const std::string& currDir = path.substr(0, pos);
        if (!currDir.empty()) {
#ifndef WIN32
            res = ::mkdir(currDir.c_str(), 0755);
#else
            res =  _mkdir(currDir.c_str());
#endif
            NGREST_ASSERT(res != -1 || errno == EEXIST, "Failed to create dir ["
                          + currDir + "]: " + strerror(errno));
        }
    }

    return !res;
}

bool File::isDots(const char* name)
{
    return (name == nullptr || (name[0] == '.' && (name[1] == '\0' ||
            (name[1] == '.' && name[2] == '\0'))));
}

bool File::isDots(const std::string& name)
{
    return isDots(name.c_str());
}

}
