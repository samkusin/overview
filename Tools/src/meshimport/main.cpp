//
//  main.cpp
//  ckmeshimport
//
//  Created by Samir Sinha on 9/19/15.
//  Copyright (c) 2015 Cinekine. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "parse_common.hpp"

#include "optionparser.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "pugixml/pugixml.hpp"

enum OptionArg
{
    kOptionUnknown,
    kOptionHelp
};

static const option::Descriptor sArgHelp[] = {
    { kOptionUnknown,   0, ""       , ""        , option::Arg::None,
      "usage: meshimport <source filename> [<dest>]\n" },
    { kOptionHelp,      0, ""       , "help"    , option::Arg::None,
      "  --help  \t command line usage." },
    { 0,                0, NULL     , NULL      , 0, NULL }
};


int main(int argc, const char * argv[])
{
    if (argc > 0) {
        --argc;
        ++argv;
    }
    
    option::Stats argStats(sArgHelp, argc, argv);
    std::vector<option::Option> argOpts(argStats.options_max);
    std::vector<option::Option> argBuf(argStats.buffer_max);
    option::Parser argParse(sArgHelp, argc, argv, argOpts.data(), argBuf.data());
    
    if (argParse.error())
        return 1;
    
    int numParams = argParse.nonOptionsCount();

    if (argOpts[kOptionHelp] || argc == 0 || numParams < 1 || numParams > 2) {
        option::printUsage(std::cout, sArgHelp);
        return 0;
    }
    
    //  parse source and target filenames
    //
    std::string srcPathStr = argParse.nonOptions()[0];
    std::string destPathStr;
    
    if (numParams < 2) {
        //  construct a target filename - using the source path minus the
        //  directory and extension (will target the current working dir)
        destPathStr = srcPathStr;
        size_t pos = destPathStr.find_last_of("\\/");
        if (pos != std::string::npos) {
            destPathStr.erase(0, pos+1);
        }
        pos = destPathStr.find_last_of('.');
        if (pos != std::string::npos) {
            destPathStr.erase(pos);
        }
    }
    else {
        destPathStr = argParse.nonOptions()[1];
    }
    if (!destPathStr.empty()) {
        size_t pos = destPathStr.find_last_of("\\/");
        pos = destPathStr.find('.', pos==std::string::npos ? 0: pos);
        if (pos == std::string::npos) {
            destPathStr += ".ckm";
        }
    }
    else {
        option::printUsage(std::cout, sArgHelp);
        return 0;
    }
    
    //  parse options
    //
    try
    {
        std::ifstream inputStream(srcPathStr.c_str());
        if (inputStream.fail())
            throw std::runtime_error("Failed to open file " + srcPathStr);
        pugi::xml_document colladaDoc;
        pugi::xml_parse_result result = colladaDoc.load(inputStream);
        if (!result)
            throw std::runtime_error("Failed to parse file " + srcPathStr);
        
        rapidjson::Document ckmJson;
        cinek::ovtools::MeshTree meshTree = cinek::ovtools::parseColladaToData(colladaDoc);
        std::cout << meshTree.root->name;
    }
    catch(const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Unhandled exception\n";
    }
    return 0;
}
