//
//  FileStreamBuf.hpp
//  Overview
//
//  Created by Samir Sinha on 8/18/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef Overview_FileStreamBuf_hpp
#define Overview_FileStreamBuf_hpp

#include <streambuf>

#include "cinek/cpp/ckalloc.hpp"

namespace cinekine {
    namespace ovengine {
        
    //  An implementation of std::streambuf specialized for the OGE allowing access
    //  to filedata from multiple sources.
    //
    //  Does not support seek operations (streamed, not random access)
    //
    //  This implementation supports either read or write on an opened file, but not both.
    //  Future versions may support both methods concurrently.
    //
    class FileStreamBuf: public std::streambuf
    {
        CK_CLASS_NON_COPYABLE(FileStreamBuf);

    public:
        FileStreamBuf(const char* pathname, std::ios_base::openmode mode = std::ios_base::in,
                  size_t bufferSize=1024,
                  const Allocator& allocator=Allocator());
        ~FileStreamBuf();
        bool operator!() const {
            return isOpen();
        }
        bool isOpen() const {
            return _fileHandle != nullptr;
        }
    protected:
        std::streambuf* setbuf(char* s, std::streamsize n);
        virtual int overflow(int c=EOF);
        virtual int underflow();
        virtual int pbackfail(int c=EOF);
        virtual int sync();
        
    private:
        Allocator _allocator;
        void* _fileHandle;
        std::ios_base::openmode _mode;
        size_t _bufferSize;
        char_type* _buffer;
    };
    
    }   // namespace ovengine
}   // namespace cinekine


#endif

