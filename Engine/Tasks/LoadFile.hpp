//
//  LoadFile.hpp
//  EnginePrototype
//
//  Created by Samir Sinha on 11/30/15.
//
//

#ifndef Oveview_Task_LoadFile_hpp
#define Oveview_Task_LoadFile_hpp

#include <cinek/allocator.hpp>
#include <cinek/task.hpp>
#include <ckio/file.h>

#include <vector>
#include <string>

namespace cinek {
    namespace ove {

class LoadFile : public Task
{
public:
    static const UUID kUUID;
    
    LoadFile(EndCallback cb) : Task(cb), _file(nullptr) {}
    LoadFile(std::string name, EndCallback cb=0);
    virtual ~LoadFile();
    
    void setName(std::string name) { _name = name; }
    
    const uint8_t* buffer() const { return _buffer; }
    uint32_t size() const { return _size; }
    const std::string& name() const { return _name; }
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
    virtual void onFileLoaded();
    
protected:
    virtual void onBegin() override;
    virtual void onUpdate(uint32_t deltaTimeMs) override;
    virtual void onCancel() override;
    
    virtual uint8_t* acquireBuffer(uint32_t size) = 0;
    virtual bool retry(std::string& path) { return false; }
    
private:
    void close();
    
    std::string _name;
    uint8_t* _buffer;
    uint32_t _size;
    
    ckio_handle *_file;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* LoadFile_hpp */
