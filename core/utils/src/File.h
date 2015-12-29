#ifndef NGREST_UTILS_FILE_H
#define NGREST_UTILS_FILE_H

#include <list>
#include <string>
#include <stdint.h>
#include "ngrestutilsexport.h"

#ifdef _MSC_VER
#define NGREST_PATH_SEPARATOR "\\"
#else
#define NGREST_PATH_SEPARATOR "/"
#endif

namespace ngrest {

typedef std::list<std::string> StringList; //!< list of strings

//! file finder
class NGREST_UTILS_EXPORT File
{
public:
    //! file attributes
    enum Attribute
    {
        AttributeNone = 0,          //!< not a file nor directory
        AttributeDirectory = 1,     //!< directory
        AttributeRegularFile = 2,   //!< regular file
        AttributeOtherFile = 4,     //!< not a regular file (symlink, pipe, socket, etc...)
        AttributeAnyFile =          //!< any file (regular, symlink, pipe, socket, etc...)
        AttributeRegularFile | AttributeOtherFile,
        AttributeAny =              //!< any file or directory
        AttributeAnyFile | AttributeDirectory
    };

public:
    //! constructor
    /*! \param  path - path to the file or directory
      */
    File(const std::string& path);

    //! find files/directories by name
    /*! \param  list - resulting list of files/directories
        \param  mask - file mask (shell pattern)
        \param  attrs - match by attributes
        */
    void list(StringList& list, const std::string& mask = "*", int attrs = AttributeAny);

    //! get file/directory attributes
    /*! if file does not exists returns AttributeNone
        \return attributes
      */
    int getAttributes();

    //! test whether the file or directory exists
    /*! \return true if the file or directory exists
      */
    bool isExists();

    //! tests whether path is a directory
    /*! \return true if path is a directory
      */
    bool isDirectory();

    //! tests whether path is a regular file
    /*! \return true if path is a regular file
      */
    bool isRegularFile();

    //! tests whether path is not a regular file (symlink, pipe, socket, etc...)
    /*! \return true if path is not a regular file
      */
    bool isSystemFile();

    //! tests whether path is any file
    /*! \return true if path is any file
      */
    bool isFile();

    //! get file's last modification time
    /*! \return unix time
      */
    int64_t getTime();

    //! create the directory
    /*! \return true if the directory was created
      */
    bool mkdir();

    //! create the directory, including parent directories
    /*! \return true if the directory was created
      */
    bool mkdirs();

    //! is file name = "." or ".."
    /*! \param  name - filename
        \return true if file name = "." or ".."
        */
    static bool isDots(const char* name);

    //! is file name = "." or ".."
    /*! \param  name - filename
        \return true if file name = "." or ".."
        */
    static bool isDots(const std::string& name);

private:
    std::string path; //!< path to the file or directory
};
}

#endif // NGREST_UTILS_FILE_H

