//================================================================================================================================================
// FILE: obj_blob_loader.h
//
//
//================================================================================================================================================
#ifndef H_GUARD_TTOBJ_OBJ_BLOB_LOADER
#define H_GUARD_TTOBJ_OBJ_BLOB_LOADER
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "exceptions.hpp"
#include "format.hpp"

#include "gie/log.hpp"

#include <algorithm>
#include <vector>
#include <map>
#include <fstream>

#include <cassert>
//================================================================================================================================================
namespace ttobj{

    template <class Input, class End>
    auto load_obj_meshes(Input i, End const& end, size_t const max_blob_size = 50*1024*1024) -> std::map<std::string, std::vector<char> > {

        std::map<std::string, std::vector<char> > blobs;

        std::for_each(i, end, [&](char const*const file_name){
            assert(file_name);

            GIE_LOG("Loading: "<<file_name);

            std::ifstream is {file_name, std::ios::binary};
            if(!is.is_open()) throw exception::io_error( GIE_FORMAT("Failed to open file: " << file_name) );

            is.seekg(0, std::ios::end);
            if(!is.good()) throw exception::io_error( "seekg() failed" );

            auto const file_size = is.tellg();
            if(!is.good()) throw exception::io_error( "tellg() failed" );
            if(file_size==std::streamoff(-1))  throw exception::io_error( "tellg() unsupported/failed" );

            size_t const size = static_cast<size_t>(file_size);

            is.seekg(0, std::ios::beg);
            if(!is.good()) throw exception::io_error( "seekg() failed" );

            if(size>max_blob_size) throw exception::invalid_blob_size( GIE_FORMAT("Size of file '" << file_name << "' is too large, max allowed: "<<max_blob_size<<", requested: "<<size ) );

            std::vector<char> tmp;
            tmp.resize(size);

            is.read(tmp.data(), tmp.size());

            if( static_cast<size_t>(is.gcount())!=size) throw exception::invalid_blob_size( GIE_FORMAT("Read of file '" << file_name << "' failed") );

            auto const& r= blobs.insert( std::make_pair(std::string(file_name), std::move(tmp)) );
            if(!r.second) throw exception::already_exists( GIE_FORMAT("File '" << file_name << "' already loaded.") );


        });

        return blobs;
    }

}
//================================================================================================================================================
#endif
//================================================================================================================================================
