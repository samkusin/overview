//
//  AssetLoader.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/24/15.
//
//

#include "AssetLoader.hpp"
#include <cstring>

namespace cinek {
    namespace ove {

AssetLoader::AssetLoader() :
    _activeCKIO(NULL),
    _activeResult(Result::kIdle)
{
}

AssetLoader::~AssetLoader()
{
    if (_activeCKIO) {
        ckio_close(_activeCKIO);
        _activeCKIO = NULL;
    }
}
    
    
AssetLoader::Result AssetLoader::load
(
    const char* pathname,
    std::function<void(Result, unique_ptr<Asset>)> fn
)
{
    if (_activeCKIO) {
        return _activeResult;
    }
    _activeResult = Result::kIdle;
    _activeCKIO = ckio_open(pathname, kCKIO_ReadFlag | kCKIO_Async);
    if (!_activeCKIO) {
        return Result::kFailed;
    }
    
    Result result;
    size_t sz = ckio_get_info(_activeCKIO, NULL);
    if (sz) {
        Asset::Type type = Asset::Type::kNone;
        size_t slen = strlen(pathname);
        do {
            --slen;
            if (pathname[slen] == '.')
                break;
        }
        while (slen > 0);
        const char* ext = pathname + slen;
        if (!strcasecmp(ext, ".json")) {
            type = Asset::Type::kJSONText;
        }
        else if (!strcasecmp(ext, ".png") ||
                 !strcasecmp(ext, ".tga") ||
                 !strcasecmp(ext, ".jpg") ||
                 !strcasecmp(ext, ".jpeg") ||
                 !strcasecmp(ext, ".bmp")) {
            type = Asset::Type::kImage;
        }
        else {
            type = Asset::Type::kBinary;
        }
        
        _activeAsset = allocate_unique<Asset>(type, (uint32_t)sz);
        if (_activeAsset && *_activeAsset) {
            //  begin read request - during update we'll trigger callback
            ckio_read(_activeCKIO, _activeAsset->data(), sz);
            _callback = std::move(fn);
            result = _activeResult = Result::kStarted;
        }
        else {
            result = Result::kOutOfMemory;
            _activeAsset = nullptr;
        }
    }
    else {
        result = Result::kFailed;
    }
    
    if (result != Result::kStarted) {
        ckio_close(_activeCKIO);
        _activeCKIO = NULL;
    }
    return result;
}

AssetLoader::Result AssetLoader::status() const
{
    return _activeResult;
}

void AssetLoader::update()
{
    if (_activeCKIO) {
        ckio_status status = ckio_get_status(_activeCKIO, NULL);
        
        switch (status)
        {
        case kCKIO_Pending:
            _activeResult = Result::kInProgress;
            break;
        case kCKIO_Canceled:
        case kCKIO_Error:
        case kCKIO_Success:
        case kCKIO_EOF:
            if (status == kCKIO_Canceled) {
                _activeResult = Result::kAborted;
            }
            else if (status == kCKIO_Error) {
                _activeResult = Result::kFailed;
            }
            else {
                _activeResult = Result::kSuccess;
            }
            
            if (_callback) {
                _callback(_activeResult, std::move(_activeAsset));
                _callback = nullptr;
            }
            
            ckio_close(_activeCKIO);
            _activeCKIO = NULL;
            break;
        };
    }
    else {
        _activeResult = Result::kIdle;
    }
}
    
    }   /* namespace ove */
}   /* namespace cinek */