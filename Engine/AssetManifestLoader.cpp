//
//  AssetManifestLoader.cpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/26/15.
//
//

#include "AssetManifestLoader.hpp"
#include "AssetManifestFactory.hpp"

namespace cinek {
    namespace ove {
    
AssetManifestLoader::State::State() :
    _class(kNone),
    _assetType(AssetType::kNone)
{
}

AssetManifestLoader::State::State
(
    AssetType assetType,
    JsonValue::ConstMemberIterator iter,
    JsonValue::ConstMemberIterator iterEnd
) :
    _class(kMember),
    _assetType(assetType)
{
    ::new(_iterBuf) JsonValue::ConstMemberIterator(iter);
    ::new(_iterEndBuf) JsonValue::ConstMemberIterator(iterEnd);
}

AssetManifestLoader::State::State
(
    AssetType assetType,
    JsonValue::ConstValueIterator iter,
    JsonValue::ConstValueIterator iterEnd
) :
    _class(kArray),
    _assetType(assetType)
{
    ::new(_iterBuf) JsonValue::ConstValueIterator(iter);
    ::new(_iterEndBuf) JsonValue::ConstValueIterator(iterEnd);
}

AssetManifestLoader::State::~State()
{
    //  iterators are objects that have trivial destructors (they do nothing)
    //  skipping the need to explicitly call destructors
}


auto AssetManifestLoader::State::increment() -> State&
{
    if (!done()) {
        switch (_class)
        {
        case kArray:
            ++iterator();
        case kMember:
            ++memberIterator();
        default:
            break;
        }
    }
    return *this;
}

bool AssetManifestLoader::State::done() const
{
    switch (_class)
    {
    case kArray:
        return (iterator() == iteratorEnd());
    case kMember:
        return (memberIterator() == memberIteratorEnd());
    default:
        return true;
    }
}

const JsonValue::Member* AssetManifestLoader::State::currentMember() const
{
    if (_class != kMember)
        return nullptr;
    
    return &(*memberIterator());
}

const JsonValue* AssetManifestLoader::State::currentValue() const
{
    if (_class != kArray)
        return nullptr;
    
    return &(*iterator());
}

const JsonValue::ConstMemberIterator& AssetManifestLoader::State::memberIterator() const
{
    return *reinterpret_cast<const JsonValue::ConstMemberIterator*>(_iterBuf);
}

JsonValue::ConstMemberIterator& AssetManifestLoader::State::memberIterator()
{
    return *reinterpret_cast<JsonValue::ConstMemberIterator*>(_iterBuf);
}

const JsonValue::ConstMemberIterator& AssetManifestLoader::State::memberIteratorEnd() const
{
    return *reinterpret_cast<const JsonValue::ConstMemberIterator*>(_iterEndBuf);
}

JsonValue::ConstValueIterator& AssetManifestLoader::State::iterator()
{
    return *reinterpret_cast<JsonValue::ConstValueIterator*>(_iterBuf);
}

const JsonValue::ConstValueIterator& AssetManifestLoader::State::iterator() const
{
    return *reinterpret_cast<const JsonValue::ConstValueIterator*>(_iterBuf);
}

const JsonValue::ConstValueIterator& AssetManifestLoader::State::iteratorEnd() const
{
    return *reinterpret_cast<const JsonValue::ConstValueIterator*>(_iterEndBuf);
}


AssetManifestLoader::AssetManifestLoader() :
    _listener(nullptr),
    _manifest(nullptr),
    _requestId(0)
{
}


AssetManifestLoader::AssetManifestLoader
(
    AssetManifest& manifest,
    AssetManfiestFactory& factory
) :
    _listener(&factory),
    _manifest(&manifest),
    _requestId(0)
{
    _stack.reserve(8);
}

AssetManifestLoader::AssetManifestLoader(AssetManifestLoader&& other) :
    _listener(other._listener),
    _manifest(other._manifest),
    _loadCb(std::move(other._loadCb)),
    _stack(std::move(other._stack)),
    _requestId(other._requestId)
{
}

AssetManifestLoader& AssetManifestLoader::operator=(AssetManifestLoader&& other)
{
    _listener = other._listener;
    _manifest = other._manifest;
    _loadCb = std::move(other._loadCb);
    _stack = std::move(other._stack);
    _requestId = other._requestId;
    
    other._requestId = 0;
    other._listener = nullptr;
    other._manifest = nullptr;
    
    return *this;
}
    
void AssetManifestLoader::start(LoadResultCb resultCb)
{
    _loadCb = std::move(resultCb);
    
    _stack.emplace_back(AssetType::kNone,
            _manifest->root("entity").MemberBegin(), _manifest->root("entity").MemberEnd());
}

void AssetManifestLoader::end(LoadResult result)
{
    if (_loadCb) {
        _loadCb(result);
        _loadCb = nullptr;
    }
    
    _stack.clear();
    _requestId = 0;
}

void AssetManifestLoader::cancel()
{
    if (_requestId > 0) {
        _listener->onAssetManifestRequestCancelled(_requestId);
        end(LoadResult::kAborted);
    }
}

void AssetManifestLoader::update()
{
    if (_stack.empty() || _requestId > 0)
        return;
    
    auto& state = _stack.back();
    
    if (state.done()) {
        _stack.pop_back();
        if (_stack.empty()) {
            end(LoadResult::kSuccess);
        }
    }
    else {
        if (state.classType() == State::kMember) {
            auto member = state.currentMember();
            const char* field = member->name.GetString();
            auto valueType = member->value.GetType();
            
            if (!strcmp(field,"textures") &&
                 valueType == rapidjson::kArrayType) {
                _stack.emplace_back(AssetType::kTexture,
                        member->value.Begin(), member->value.End());
            }
            else if (!strcmp(field, "model") &&
                      valueType == rapidjson::kStringType) {
                requestAssetLoad(AssetType::kModel, member->value.GetString());
            }
            else if (valueType == rapidjson::kObjectType) {
                _stack.emplace_back(AssetType::kNone,
                        member->value.MemberBegin(), member->value.MemberEnd());
            }
        }
        else {
            auto value = state.currentValue();
            if (state.assetType() == AssetType::kTexture) {
                requestAssetLoad(state.assetType(), value->GetString());
            }
        }
    
        state.increment();
    }
}

void AssetManifestLoader::requestAssetLoad
(
    AssetType assetType,
    const std::string& name
)
{
    _requestId = _listener->onAssetManifestRequest(assetType, name,
        [this](const std::string&, AssetManfiestFactory::LoadResult r) {
            _requestId = 0;
        });
}
    
    } /* namespace ove */
} /* namespace cinek */