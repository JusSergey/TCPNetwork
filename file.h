#ifndef FILE_H
#define FILE_H

/* Structure Message Information
[type][data]
*/

enum class TypeFileMsg : char {
    CreateNewFile = 1,
    Append,
    GetNextPackage,
    Complete,
    RejectRecv
};

enum class TypeLongDataMsg : char {
    InfoOfData = 1,
    Append,
    GetNextPackage,
    Complete
};

#include <functional>
#include <string>
#include "net.h"

using CallbackAcceptRecvFile   = std::function<bool(const std::string &filename)>;
using CallbackFileSendComplete = std::function<void(const std::string &filename)>;
using CallbackFileSendAbort    = std::function<void(const std::string &filename)>;
using CallbackReadInfoOfFile   = std::function<void(const std::string &info)>;
using CallbackReadPartData     = std::function<void(Buffer &buffer)>;

#endif // FILE_H
