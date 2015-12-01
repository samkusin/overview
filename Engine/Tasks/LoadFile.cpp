//
//  LoadFile.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/30/15.
//
//

#include "LoadFile.hpp"

namespace cinek {
    namespace ove {

const UUID LoadFile::kUUID = {
    0xd5, 0x8e, 0xa9, 0x05, 0xf7, 0x76, 0x4e, 0x6e,
    0xa6, 0xeb, 0x4b, 0x04, 0x3b, 0xc4, 0x2b, 0xba
};
        

LoadFile::LoadFile
(
    std::string name,
    EndCallback cb
) :
    Task(cb),
    _name(std::move(name)),
    _file(nullptr)
{

}

LoadFile::~LoadFile()
{
    close();
}
    
std::vector<uint8_t>  LoadFile::acquireBuffer()
{
    return std::move(_buffer);
}

void LoadFile::close()
{
    if (_file) {
        ckio_close(_file);
        _file = nullptr;
    }
}

void LoadFile::onBegin()
{
    _file = ckio_open(_name.c_str(), kCKIO_Async | kCKIO_ReadFlag);
    if (_file) {
        auto cnt = ckio_get_info(_file, nullptr);
        if (cnt) {
            _buffer.resize(cnt+1, 0);
            ckio_read(_file, _buffer.data(), cnt);
        }
    }
    else {
        fail();
    }
    
}

void LoadFile::onUpdate(uint32_t )
{
    if (_file) {
        ckio_status status = ckio_get_status(_file, nullptr);
        if (status == kCKIO_Success) {
            onFileLoaded();
        }
        else if (status != kCKIO_Pending) {
            fail();
        }
    }
}

void LoadFile::onCancel()
{
    if (_file) {
        ckio_cancel(_file);
    }
}

void LoadFile::onFileLoaded()
{
    end();
}
    
    }  /* namespace ove */
}  /* namespace cinek */