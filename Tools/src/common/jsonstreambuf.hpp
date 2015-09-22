//
//  StreamBufRapidJson.hpp
//  Overview
//
//  Created by Samir Sinha on 7/14/13.
//  Copyright (c) 2013 Cinekine. All rights reserved.
//

#ifndef CINEK_StreamBufRapidJson_hpp
#define CINEK_StreamBufRapidJson_hpp

#include <streambuf>

namespace rapidjson {

    class StdStreamBuf
    {
    public:
        typedef char Ch;

        StdStreamBuf(std::streambuf& buf) : _streambuf(buf), _count(0) { }
        Ch Peek() const {
            Ch c = _streambuf.sgetc();
            return c!=EOF ? c : '\0';
        }
        Ch Take() {
            Ch c = _streambuf.sbumpc();
            if (c != EOF) { ++_count; } else { c = '\0'; }
            return c;
        }
        size_t Tell() const {
            return _count;
        }
        void Put(Ch c) {
            _streambuf.sputc(c);
        }
        Ch* PutBegin() { return 0; }
        size_t PutEnd(Ch*) { return 0; }
        
        void Flush() {  }
        
        
    private:
        std::streambuf& _streambuf;
        size_t _count;
    };

}

#endif
