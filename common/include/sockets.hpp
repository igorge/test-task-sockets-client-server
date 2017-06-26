//================================================================================================================================================
// FILE: sockets.h
//
//
//================================================================================================================================================
#ifndef H_GUARD_TTOBJ_SOCKETS
#define H_GUARD_TTOBJ_SOCKETS
//================================================================================================================================================
#pragma once
//================================================================================================================================================
#include "format.hpp"
#include "exceptions.hpp"

#include "gie/log.hpp"

#include <array>
#include <errno.h>
#include <cassert>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <netdb.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

#include <memory>

//================================================================================================================================================


namespace ttobj {

    #ifdef _WIN32
        using socklen_t = int;
        using socket_t = SOCKET;
    #else
        using socklen_t = ::socklen_t;
        using socket_t = int;
    #endif


    #ifdef _WIN32
        inline SOCKET invalid_socket(){
            return INVALID_SOCKET;
        }
    #else
        inline int invalid_socket(){
            return -1;
    }
    #endif

    inline bool is_socket(socket_t const& s){
        return s!=invalid_socket();
    }

    inline int shutdown(socket_t& s){
        #ifdef _WIN32
            return ::shutdown(s, SD_BOTH);
        #else
            return ::shutdown(s, SHUT_RDWR);
        #endif
    }

    inline bool is_socket_error(int err){
        #ifdef _WIN32
                return err==SOCKET_ERROR;
        #else
                return err==-1;
        #endif
    }

    inline int get_socket_last_error(){
        #ifdef _WIN32
            return WSAGetLastError();
        #else
            return errno;
        #endif
    }


    inline int close_socket(int sock){
        #ifdef _WIN32
            return closesocket(sock);
        #else
            return close(sock);
        #endif
    }

    inline void sockets_startup(){
        #ifdef _WIN32
            WSADATA wsaData = {0};
            auto const ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (ret != 0) {
                throw exception::socket_initialization_error(GIE_FORMAT("WSAStartup() failed: "<<ret));
            }
        #else
        #endif
    }

    inline void sockets_cleanup(){
        #ifdef _WIN32
            auto const ret = WSACleanup();
            assert(ret==0);
        #else
        #endif
    }


    inline void shared_socked_deleter(ttobj::socket_t *s){
        assert(s);

        if (ttobj::is_socket(*s)) {
            GIE_LOG("Closing socket " << *s);
            /*auto ret1 = */ttobj::shutdown(*s);
            /*auto ret2 = */ttobj::close_socket(*s);
            delete s;
        }
    }

    inline std::shared_ptr<socket_t> shared_socket(socket_t&& s){

        try {
            std::shared_ptr<ttobj::socket_t> shared_s(new ttobj::socket_t(s), shared_socked_deleter);
            return shared_s;
        } catch (...){
            auto ret = ttobj::close_socket(s);
            assert(!ttobj::is_socket_error(ret));

            throw;
        }
    }

    inline size_t recv_(socket_t const& s, void *buf, size_t len, int flags){
        assert(buf);

#ifdef _WIN32
        auto const ret_size = ::recv(s, static_cast<char*>(buf), len, flags);
#else
        auto const ret_size = ::recv(s, buf, len, flags);
#endif
        if( is_socket_error(ret_size) ){
            auto const last_error = get_socket_last_error();
            throw exception::socket_error(GIE_FORMAT("revc("<<s<<") failed: " << last_error));
        } else {
            return ret_size;
        }
    }

    inline size_t send_(socket_t const& s, const void *buf, size_t len, int flags){
#ifdef _WIN32
        auto const ret_size = ::send(s, static_cast<char const*>(buf), len, flags);
#else
        auto const ret_size = ::send(s, buf, len, flags);
#endif
        if( is_socket_error(ret_size) ){
            auto const last_error = get_socket_last_error();
            throw exception::socket_error(GIE_FORMAT("send("<<s<<") failed: " << last_error));
        } else {
            return ret_size;
        }
    }


    inline void recv(socket_t const& s, void* buffer, size_t len, int flags = 0){
        auto const ret_size = recv_(s, buffer, len , flags | MSG_WAITALL);
        if(ret_size!=len) {
            throw exception::socket_error(GIE_FORMAT("revc("<<s<<") failed, got " << ret_size << " bytes, expected "<< len));
        }
    }

    inline void send(socket_t const& s, void const* buffer,  size_t len, int flags = 0){
        auto const ret_size = send_(s, buffer, len, flags);
        if(ret_size!=len) {
            throw exception::socket_error(GIE_FORMAT("send("<<s<<") failed, sent " << ret_size << " bytes, expected "<<len));
        }
    }

    template <size_t N>
    void recv(socket_t const& s, std::array<uint8_t ,N>& buffer, int flags = 0){
        recv(s, buffer.data(), buffer.size(), flags);
    }

    template <size_t N>
    void send(socket_t const& s, std::array<uint8_t ,N> const& buffer, int flags = 0){
        send(s, buffer.data(), buffer.size(), flags);
    }




    inline std::shared_ptr<addrinfo> getaddrinfo(char const* node, char const* service_name = nullptr, addrinfo const* hint = nullptr){
        assert(node);

        addrinfo* info = nullptr;

        auto const ret = ::getaddrinfo(node, service_name, hint, &info);

        if( ret !=0 ){
            throw exception::socket_error(GIE_FORMAT("getaddrinfo() for '"<< node <<"' had failed, err code: "<< ret));
        }

        return std::shared_ptr<addrinfo>(  info, [](addrinfo * info){
            if(info) freeaddrinfo(info);
        });



    }



}
//================================================================================================================================================
#endif
//================================================================================================================================================
