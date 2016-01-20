#ifndef NGREST_RUNTIME_H
#define NGREST_RUNTIME_H

#include <string>

namespace ngrest {

class Runtime
{
public:
    static const std::string& getApplicationDirPath();
    static const std::string& getApplicationFilePath();
};

} // namespace ngrest

#endif // NGREST_RUNTIME_H
