#ifndef CALLBACKTYPES_H
#define CALLBACKTYPES_H

#include "buffer.h"
#include "socketfd.h"
#include <functional>

using CallbackRead = std::function<void(Buffer, SocketFD)>;
using CallbackLoop = std::function<void()>;

using CallbackConnected = std::function<void(SocketFD)>;
using CallbackDisconnect= std::function<void(SocketFD)>;

#endif // CALLBACKTYPES_H
