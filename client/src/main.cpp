
#include "sockets.hpp"
#include "exceptions.hpp"
#include "format.hpp"
#include "wire.hpp"
#include "cmds.hpp"

#include "gie/util-scope-exit.hpp"
#include "gie/safe_main.hpp"


char const*const msg_args = "Usage: objclient.exe server-hostname-or-ip obj-file-path";


int main(int argc, char *argv[]) {
    return gie::main([&](){

        if( argc<3 ) throw ttobj::exception::invalid_arguments(GIE_FORMAT("Expected at least 2 arguments.\n" << msg_args));

        ttobj::sockets_startup();
        GIE_SCOPE_EXIT([]{ ttobj::sockets_cleanup(); });


        addrinfo hint = {};
        hint.ai_family = AF_INET;
        hint.ai_socktype = SOCK_STREAM;
        hint.ai_protocol = IPPROTO_TCP;

        auto const addr = ttobj::getaddrinfo(argv[1], "8888", &hint);

        auto const sock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
        if( !ttobj::is_socket(sock) ) {
            throw ttobj::exception::socket_error("Failed to create socket.");
        }

        GIE_SCOPE_EXIT([&]{
            auto ret = ttobj::close_socket(sock);
            assert( !ttobj::is_socket_error(ret) );
        });


        if( connect(sock, addr->ai_addr, addr->ai_addrlen) !=0 ){
            auto const errcode = ttobj::get_socket_last_error();
            throw ttobj::exception::socket_error(GIE_FORMAT("connect() had failed, error code: " << errcode));
        }


        { // handshake
            ttobj::write_string(sock, ttobj::cmd::hello);
            auto const reply = ttobj::read_string(sock);

            if(reply!=ttobj::cmd::hello) throw ttobj::exception::wire_error(GIE_FORMAT("Expected '"<<ttobj::cmd::hello<<"' got '"<<reply<<"'"));
        }

        { // request data

            auto const key = argv[2];
            ttobj::write_string(sock, ttobj::cmd::get);
            ttobj::write_string(sock, key);

            auto const reply = ttobj::read_string(sock);

            if( reply==ttobj::cmd::ok ) {
                auto const data = ttobj::read_string(sock);
                std::cout << data << std::endl;
            } else  if( reply==ttobj::cmd::not_found ) {
                std::cerr << "The requested resource '" << key << "' was not found on server." << std::endl;
            } else {
                throw ttobj::exception::wire_error(GIE_FORMAT("Unexpected server response: '"<<reply<<"'"));
            }
        }


        { // gracefully close connection

            ttobj::write_string(sock, ttobj::cmd::bye);
            auto reply = ttobj::read_string(sock);

            if(reply!=ttobj::cmd::bye) throw ttobj::exception::wire_error(GIE_FORMAT("Expected '"<<ttobj::cmd::bye<<"' got '"<<reply<<"'"));
        }


    });
}
