//================================================================================================================================================
// FILE: wire.h
// (c)
//
//================================================================================================================================================
#ifndef H_GUARD_TTOBJ_WIRE
#define H_GUARD_TTOBJ_WIRE
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "sockets.hpp"

#include <cinttypes>
#include <limits>
#include <vector>
#include <string>
//================================================================================================================================================
namespace ttobj {

    size_t const wire_max_transaction_size = 100*1024*1024;

    inline std::uint32_t read_uint32(ttobj::socket_t const& s){
        std::array<uint8_t, 4> data;

        recv(s, data);

        return (static_cast<std::uint32_t>(data[0]) | (data[1]<<8) | (data[2]<<16) | data[3]<<24);
    }

    inline void write_uint32(ttobj::socket_t const &s, std::uint32_t value){

        using T = std::uint32_t;

        constexpr auto const r_shift = (std::numeric_limits<T>::digits-8);

        std::array<uint8_t, 4> buffer ={
                static_cast<uint8_t>((value << (std::numeric_limits<T>::digits-8)) >> r_shift),
                static_cast<uint8_t>((value << (std::numeric_limits<T>::digits-16)) >> r_shift),
                static_cast<uint8_t>((value << (std::numeric_limits<T>::digits-24)) >> r_shift),
                static_cast<uint8_t>((value << (std::numeric_limits<T>::digits-32)) >> r_shift)
        };

        send(s, buffer);
    }

    inline void check_max_stransaction(size_t size){
        if (size>wire_max_transaction_size) throw exception::invalid_arguments( GIE_FORMAT("max transaction is too large, expected max: "<<wire_max_transaction_size << ", got: "<<size) );
    }

    inline void write_vector(ttobj::socket_t const& s, std::vector<char> const& vec){
        if( vec.size() > std::numeric_limits<uint32_t>::max() ) throw exception::invalid_arguments("vector size too large");
        check_max_stransaction(vec.size());

        write_uint32( s, static_cast<uint32_t>(vec.size()) );
        if(vec.size()!=0) send(s, vec.data(), vec.size() );
    }


    inline std::vector<char> read_vector(ttobj::socket_t const& s){
        auto const size = read_uint32( s );
        check_max_stransaction(size);
        std::vector<char> tmp;

        if(size!=0){
            tmp.resize(size);
            recv(s, tmp.data(), size);

        }

        return tmp;
    }


    inline void write_string(ttobj::socket_t const& s, std::string const& str){
        if( str.size() > std::numeric_limits<uint32_t>::max() ) throw exception::invalid_arguments("string size too large");
        check_max_stransaction(str.size());

        write_uint32( s, static_cast<uint32_t>(str.size()) );
        if(str.size()!=0) send(s, str.data(), str.size() );
    }


    inline std::string read_string(ttobj::socket_t const& s){

        auto const size = read_uint32( s );
        check_max_stransaction(size);

        if (size!=0) {
            std::vector<char> tmp;
            tmp.resize(size);
            recv(s, tmp.data(), size);
            return std::string(tmp.begin(), tmp.end());
        } else {
            return std::string();
        }

    }

}
//================================================================================================================================================
#endif
//================================================================================================================================================
