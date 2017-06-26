//================================================================================================================================================
// FILE: exceptions.h
//
//================================================================================================================================================
#ifndef H_GUARD_OBJ_EXCEPTIONS
#define H_GUARD_OBJ_EXCEPTIONS
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include <stdexcept>
//================================================================================================================================================
namespace ttobj { namespace exception {



    struct invalid_arguments : virtual std::runtime_error {
        invalid_arguments(char const*const what) : std::runtime_error(what){}
        invalid_arguments(std::string const& what) : std::runtime_error(what){}
    };

    struct io_error : virtual std::runtime_error {
        io_error(char const*const what) : std::runtime_error(what){}
        io_error(std::string const& what) : std::runtime_error(what){}
    };

    struct socket_error : virtual std::runtime_error {
        socket_error(char const*const what) : std::runtime_error(what){}
        socket_error(std::string const& what) : std::runtime_error(what){}
    };

    struct socket_initialization_error : virtual socket_error {
        socket_initialization_error(char const*const what) : std::runtime_error(what),  socket_error(what) {}
        socket_initialization_error(std::string const& what) : std::runtime_error(what), socket_error(what){}
    };


    struct invalid_blob_size : virtual std::runtime_error {
        invalid_blob_size(char const*const what) : std::runtime_error(what){}
        invalid_blob_size(std::string const& what) : std::runtime_error(what){}
    };


    struct already_exists : virtual std::runtime_error {
        already_exists(char const*const what) : std::runtime_error(what){}
        already_exists(std::string const& what) : std::runtime_error(what){}
    };

    struct not_found : virtual std::runtime_error {
        not_found(char const*const what) : std::runtime_error(what){}
        not_found(std::string const& what) : std::runtime_error(what){}
    };


    struct wire_error : virtual std::runtime_error {
        wire_error(char const*const what) : std::runtime_error(what){}
        wire_error(std::string const& what) : std::runtime_error(what){}
    };


} }
//================================================================================================================================================
#endif
//================================================================================================================================================
