
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>

#include <boost_1_81_0/boost/filesystem.hpp>
#include <boost_1_81_0/boost/asio.hpp>

using boost::asio::ip::tcp;

class handler_memory 
{
public:
    handler_memory() : in_use_false(false) {}
    handler_memory(const handler_memory&) = delete;
    handler_memory& operator=(const handler_memory&) = delete;

    void * allocate(std::size_t size)
    {
        
    }

}












