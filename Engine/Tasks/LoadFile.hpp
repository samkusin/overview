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
    
    std::vector<uint8_t> acquireBuffer();
    const std::string& name() const { return _name; }
    
    virtual const TaskClassId& classId() const override { return kUUID; }
    
    virtual void onFileLoaded();
    
protected:
    virtual void onBegin() override;
    virtual void onUpdate(uint32_t deltaTimeMs) override;
    virtual void onCancel() override;
    
    std::vector<uint8_t>& buffer() { return _buffer; }
    const std::vector<uint8_t>& buffer() const { return _buffer; }
    std::string& name() { return _name; }
    
private:
    void close();
    
    std::string _name;
    std::vector<uint8_t> _buffer;
    ckio_handle *_file;
};
    
    }  /* namespace ove */
}  /* namespace cinek */


#endif /* LoadFile_hpp */
