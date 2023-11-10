
#include <array>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utility>


#include <boost/filesystem.hpp>
#include <boost/asio.hpp>

using  boost::asio::ip::tcp ;

class handler_memory 
{
public:
    handler_memory() : in_use(false) {}
    handler_memory(const handler_memory&) = delete;
    handler_memory& operator=(const handler_memory&) = delete;

    void* allocate(std::size_t size)
    {
        if(!in_use_false && size < sizeof(storage))
        {
            in_use = true;
            return  &storage;
        }
        else 
         {
            return ::operator new(size);
         }
    }
    void deallocate (void* pointer) 
    {
        if (pointer == &storage)
        {
            in_use = false;
        }
        else
        {
            ::operator delete(pointer);
        }
    }   

private:
    typename  std::aligned_storage<1024>::type storage;
    bool in_use; 
};


template<typename T>
class handler_allocator
{
public:
    using value_type = T ;
    explicit handler_allocator(handler_memory& mem) : memory_(mem)
    {   
        
    }

    template <typename U >
    handler_allocator(const handler_allocator<U>& other) noexcept
        :memory_(other.memory_)
    {}
    bool operator == (const handler_allocator& other) const noexcept
    {
        return &memory_ == &other.memory_;
    }

    bool operator != ( const handler_allocator& other ) const noexcept
    {
        return &memory_ != &other.memory_;
    }

    T* allocate(std::size_t n ) const 
    {
        return static_cast<T*>(memory_.allocate(sizeof(T)*n));
    }

    void deallocate(T* p , std::size_t/*n*/) const
    {
        return memory_.deallocate(p);
    }
private:
    template <typename> friend class handler_allocator;
    handler_memory& memory_;
};

class session : public std::enable_shared_from_this<session> 
{
    
public:
    session(tcp::socket socket) : socket_(std::move(socket))
    {

    }
    void start()
    {
        do_read();
    }



private:
 
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_);
        boost::asio::bind_allocator(
            handler_allocator<int>(handler_memory),
            [this,self](boost::system::error_code ec ,std::size_t length)
            {
                if(!ec)
                {
                    do_write(length);
                }
            }));
        
    }
    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_,boost::asio::buffer(data,length),boost::asio::bind_allocator(handler_memory_)),
        [this,self](boost::system::error_code ec , std::size_t /*length*/)
        {
            if(!ec)
            {
                do_read();
            }
        }
    }

    tcp::socket socket_;
    std::array<char,1024> data_;
    handler_memory handler_memory_;

};

class server
{
    server(boost::asio::io_context& io_context,short port) //: accepter
    { do_accept();}




private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this] (boost::system::error_code ec , tcp::socket socket)
            {
                if(!ec)
                {
                    std::make_shared<session>(std::move(socket)) -> start();
                }

                do_accept();
            }
        )

    }
    tcp::acceptor acceptor_;
};

int main (int argc , char* arv[])
{
    try
    {
        if(argc != 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }
        boost::asio::io_context io_context;
        server(io_context,std::atoi(argv[1]));
        io_context.run();
    }
    catch( std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0; 
}



































