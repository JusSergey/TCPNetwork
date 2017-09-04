#ifndef FILE_H
#define FILE_H

/* Structure Message Information
[type][data]
*/

enum TypeFileMsg : char {
    CreateFileName = 1,
    Append,
    GetNextPackage,
    Complete
};

#include <functional>
#include <string>

using CallbackFileSendComplete = std::function<void(const std::string &filename)>;

#endif // FILE_H
