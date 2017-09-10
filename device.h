#ifndef DEVICE_H
#define DEVICE_H

#include "buffer.h"
#include "typedefs.h"

class Device {
    Buffer *_device;
    size_t _pos;
    size_t _sz;
public:
    Device(Buffer *device) : _device(device), _pos(0), _sz(device->size()) {}
    Device(const Device &dev) = default;
    Device &operator = (const Device &) = default;

    size_t read(void *destination, uint64 len) {
        auto sub = _sz-_pos;
        size_t readlen = sub < len ? sub : len;
        memcpy(destination, _device->data() + _pos, readlen);
        _pos += readlen;
        return readlen;
    }
    inline size_t getPos() const { return _pos; }
    inline size_t getAvailableSize() const { return _device->size() - _pos; }
    inline Buffer *getDevice() const { return _device; }
    inline void *getCurrentPointerOnData() const { return _device->data() + _pos; }

    void setBuffer(Buffer *device) {
        _device = device;
        _pos = 0;
        _sz = _device->size();
    }

    inline void setPos(size_t pos) { _pos = pos; }

    inline void addPos(size_t offset) { _pos += offset; }

    inline void reset() { _pos = 0; }
};

#endif // DEVICE_H
