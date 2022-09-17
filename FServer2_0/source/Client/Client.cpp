#include "Client.hpp"
#include <iostream>

namespace client
{

void connect_handler(const error_code & ec)
{
    std::cout <<ec.message();
// here we know we connected successfully
// if ec indicates success
}

void myfoo()
{
    io_service service;
ip::tcp::endpoint ep( ip::address::from_string("127.0.0.1"), 2001);
ip::tcp::socket sock(service);
sock.async_connect(ep, connect_handler);
service.run();
}

}