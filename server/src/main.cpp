

#include "obj_blob_loader.hpp"
#include "server_command_loop.hpp"
#include "sockets.hpp"
#include "exceptions.hpp"
#include "format.hpp"

#include "gie/util-scope-exit.hpp"
#include "gie/safe_main.hpp"


char const*const msg_args = "Usage: objserver.exe <obj-file path> [, <obj-file path>* ]";


int main(int argc, char *argv[]) {
    return gie::main([&](){

        if( argc<2 ) throw ttobj::exception::invalid_arguments(GIE_FORMAT("Expected at least 1 arguments.\n" << msg_args));

        ttobj::sockets_startup();
        GIE_SCOPE_EXIT([]{ ttobj::sockets_cleanup(); });

        auto const first_file_idx = 1;
        auto const past_last_file_idx = first_file_idx + (argc-first_file_idx);

        auto const& blobs = ttobj::load_obj_meshes( &argv[first_file_idx], &argv[past_last_file_idx]);

        auto const& lookup = [&blobs](std::string const&name)->std::vector<char> const&{
            auto it=blobs.find(name);
            if(it==blobs.end()) throw ttobj::exception::not_found( GIE_FORMAT("'"<<name<<"' not found") );
            return it->second;
        };

        auto const sock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
        if( !ttobj::is_socket(sock) ) {
            throw ttobj::exception::socket_error("Failed to create socket.");
        }

        GIE_SCOPE_EXIT([&]{
            auto ret = ttobj::close_socket(sock);
            assert( !ttobj::is_socket_error(ret) );
        });

        sockaddr_in addr = {};

        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons( 8888 );

        if( bind(sock, reinterpret_cast<sockaddr const*>(&addr), sizeof(addr)) !=0 ){
            auto const last_err = ttobj::get_socket_last_error();
            throw ttobj::exception::socket_error( GIE_FORMAT("bind() failed: " << last_err) );
        }

        auto const backlog = 256;
        if( listen(sock , backlog) != 0 ){
            auto const last_err = ttobj::get_socket_last_error();
            throw ttobj::exception::socket_error( GIE_FORMAT("listen() failed: " << last_err) );
        }

        GIE_LOG("Waiting for clients ...");

        while(true){

            sockaddr  peer_addr = {};
            ttobj::socklen_t peer_add_size=sizeof(peer_addr);

            auto s_tmp = accept(sock, &peer_addr, &peer_add_size);
            if( !ttobj::is_socket(s_tmp) ) {
                auto const last_err = ttobj::get_socket_last_error();
                throw ttobj::exception::socket_error( GIE_FORMAT("accept() failed: " << last_err) );
            }

            auto peersocket = ttobj::shared_socket( std::move(s_tmp) );

            GIE_LOG("Got new client connection ("<<*peersocket<<")!");

            std::thread t{ttobj::server_command_loop, lookup, peersocket };
            t.detach();
        }

    });
}
