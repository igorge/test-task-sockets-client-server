//================================================================================================================================================
//
//
//
//================================================================================================================================================
//#include "stdafx.h"
//================================================================================================================================================
#include "wire.hpp"
#include "sockets.hpp"

#include "gie/log.hpp"
#include "gie/safe_main.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <cmds.hpp>
//================================================================================================================================================


namespace ttobj {

    void server_command_loop( std::function<std::vector<char> const&(std::string const)> const& blob_getter, std::shared_ptr<ttobj::socket_t> state){

        gie::main([&](){
            auto const& s = *state;

            auto const handshake = read_string(s);
            if (handshake==cmd::hello) {
                GIE_LOG("["<<s<<"] got "<<cmd::hello);

                write_string(s, cmd::hello);
            } else {
                throw exception::wire_error("Invalid handshake");
            }

            while( true ){

                auto const req = read_string(s);
                if(req==cmd::get){
                    GIE_LOG("["<<s<<"] got "<<cmd::get);

                    auto const key = read_string(s);
                    GIE_LOG("["<<s<<"] " << cmd::get << " key: '" << key <<"'");

                    std::vector<char> const * data = nullptr;

                    try{
                        data = &(blob_getter(key));
                    } catch (exception::not_found const&){
                        write_string(s, cmd::not_found);
                        continue;
                    }

                    assert(data);
                    write_string(s, cmd::ok);
                    write_vector(s,*data);

                } else if (req==cmd::bye) {
                    GIE_LOG("["<<s<<"] got "<<cmd::bye);

                    write_string(s, cmd::bye);
                    break;

                } else {
                    throw exception::wire_error("Invalid request");
                }

            }

            GIE_LOG("Command loop terminated for peer: " << *state);
        });

    }


}
//================================================================================================================================================
