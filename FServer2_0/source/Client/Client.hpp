
#include <asio.hpp>
/*
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS
*/


using namespace asio;

namespace client
{

typedef std::shared_ptr<ip::tcp::socket> socket_ptr;

void myfoo();

}