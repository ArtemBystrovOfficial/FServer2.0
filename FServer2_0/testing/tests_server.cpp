
#include "../source/Server/IO/buffer.hpp"
#include "../source/Server/IO/io.hpp"
#include "../source/Server/Servise/basic.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <thread>
#include <atomic>
#include <Windows.h>


//#define ENABLE_BUFFERIO
//#define ENABLE_I
//#define ENABLE_BASIC
//#define ENABLE_O

//#define TEST_REAL_SITUATUON_MODE

//Common test without threads

struct MyPocket
{
    int n;
    friend std::ostream& operator<< (std::ostream& os, const MyPocket& pock);
};

std::ostream& operator<< (std::ostream& os, const MyPocket& pock)
{
    os << pock.n;
    return os;
}

#ifdef ENABLE_BUFFERIO



//Common tests without multithreating
TEST(BufferIO, Out)
{
    //A

    cv_ptr _buffer_check(new std::condition_variable);

    BufferIO < MyPocket > buffer_io(_buffer_check);

    Pocket_Sys <MyPocket> pocket;

    //A

    pocket.fid = 7;

    auto out11 = buffer_io.getFOut(pocket);
    auto out12 = buffer_io.addOut(pocket);
    auto pock1 = pocket;
    auto out13 = buffer_io.getFOut(pocket);


    //A

    ASSERT_EQ(out11, 0); ASSERT_EQ(out12, 1); ASSERT_EQ(pock1.fid, 7); ASSERT_EQ(out13, 1);


}

//Common tests without multithreating
TEST(BufferIO, In)
{
    //A

    cv_ptr _buffer_check(new std::condition_variable);

    BufferIO < MyPocket > buffer_io(_buffer_check);

    Pocket_Sys <MyPocket> pocket;

    //A

    pocket.fid = 7;

    auto out21 = buffer_io.getFIn(pocket);
    auto out22 = buffer_io.addIn(pocket);
    auto pock2 = pocket;
    auto out23 = buffer_io.getFIn(pocket);
    //A

    ASSERT_EQ(out21, 0); ASSERT_EQ(out22, 1); ASSERT_EQ(pock2.fid, 7); ASSERT_EQ(out23, 1);

}

//Common tests without multithreating
TEST(BufferIO, Threating)
{

    unsigned int start_time = clock();

    int t = 20;
 // testing with all solutions
 while (t--)
 {
     //A
     cv_ptr _buffer_check(new std::condition_variable);

     BufferIO < MyPocket > buffer_io(_buffer_check);

     int Count_of_threads = 10;

     std::vector<std::thread> ths(Count_of_threads);

     std::atomic<int> sum = 0;

     //A


     for (int j = 0; j < Count_of_threads; j++)
     {

         ths[j] = std::thread([&]() {

             Pocket_Sys <MyPocket> pocket{ MyPocket{ 5 }, 1, 3 };

             for (int i = 1; i <= 1000; i++)
             {
                 if (i % 2)
                 {
                     pocket.fid = i;
                     buffer_io.addIn(pocket);
                 }
                 else
                 {
                     buffer_io.getFIn(pocket);
                     int num = sum.load();
                     num += pocket.fid;
                     sum.store(num);
                 }

             }
             });
     }

     for (int j = 0; j < Count_of_threads; j++)
     {
         ths[j].join();
     }

     //A

     ASSERT_EQ(sum.load(), 2500000);
 }

    unsigned int end_time = clock();

    std::cout <<"Times after 200000 with 20 thread operation: "<< double(end_time - start_time)/1000<<std::endl;
}

#endif

#ifdef ENABLE_I

TEST(Reciver, SendPockets)
{
    //A
        
        io_service io1;
        io_service io2;

        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
        ip::tcp::socket sockclient(io1); 

        ip::tcp::acceptor acc(io2, ep);
        socket_ptr sockserv(new ip::tcp::socket(io2));
    //A
        auto thr = std::thread([&]() {

            acc.accept(*sockserv);

            });

        sockclient.connect(ep);

        //pre-work
        std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
        socket_closed->store(false);

        cv_ptr _buffer_check(new std::condition_variable);

        bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

        Reciver<MyPocket> recv(sockserv, socket_closed, ptr);

        Pocket_Sys <MyPocket>  pocket{ MyPocket{ 5 }, 1, 3 };

        sockclient.write_some(buffer(&pocket,sizeof(Pocket_Sys <MyPocket>)));

        thr.join();

        //io1.run();
    //A
        std::cout << "IN OF POCKET TEST\n";
        std::cout << "fid, pocket_id, pocket " << pocket.fid << " " << pocket._pocket_id << " " << pocket.pocket << "\n"; // use operator std::cout << pocket in tests
        std::cout << "-----------------\n";


//template<class _Pocket>
//inline int BasicFServer<_Pocket> ::_close_socket(socket_ptr sock, bool_atc_ptr is)
// {
            // mock with basic.hpp

        sockserv->close();
        socket_closed->store(true);
 // }

        recv.ext();

}       
        

TEST(Reciver, extBeforeSockClose)
{
    //A

        io_service io1;
        io_service io2;

        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
        ip::tcp::socket sockclient(io1);

        ip::tcp::acceptor acc(io2, ep);
        socket_ptr sockserv(new ip::tcp::socket(io2));

    //A
        auto thr = std::thread([&]() {

            acc.accept(*sockserv);

            });

        sockclient.connect(ep);

        //pre-work
        std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
        socket_closed->store(false);

        cv_ptr _buffer_check(new std::condition_variable);

        bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

        Reciver<MyPocket> recv(sockserv, socket_closed, ptr);

        thr.join();

        int value = recv.ext();

        sockserv->close();
        socket_closed->store(true);

    //A

        ASSERT_EQ(value, -1);

}


TEST(Reciver, extAfterSockClose)
{
    //A

    io_service io1;
    io_service io2;

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sockclient(io1);

    ip::tcp::acceptor acc(io2, ep);
    socket_ptr sockserv(new ip::tcp::socket(io2));

    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr);

    thr.join();

    sockserv->close();
    socket_closed->store(true);

    int value = recv.ext();

    //A

    ASSERT_EQ(value, 0);
}

TEST(Reciver, checkSystemSortPockets1)
{
    io_service io1;
    io_service io2;

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sockclient(io1);

    ip::tcp::acceptor acc(io2, ep);
    socket_ptr sockserv(new ip::tcp::socket(io2));

    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr);

    Pocket_Sys <MyPocket>  pocket{ MyPocket{ 0 }, 11, 2 };

    sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    pocket.fid = 12;    pocket._pocket_id = 1;

    sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    pocket.fid = 13;    pocket._pocket_id = 3;

    sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    Sleep(1);

    thr.join();

    sockserv->close();
    socket_closed->store(true);

    recv.ext();

}
// fid out -> 99-0
TEST(Reciver, checkSystemSortPockets2)
{
    io_service io1;
    io_service io2;

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sockclient(io1);

    ip::tcp::acceptor acc(io2, ep);
    socket_ptr sockserv(new ip::tcp::socket(io2));

    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr);

    Pocket_Sys <MyPocket>  pocket{ MyPocket{ 0 }, 11, 2 };

    for (int i = 100; i > 0; i--)
    {
        pocket.fid = 100-i;    pocket._pocket_id = i;

        sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    Sleep(1);

    thr.join();

    sockserv->close();
    socket_closed->store(true);

    recv.ext();

}


//Test Before with real out of enthernet fid out -> 99-0
TEST(Reciver, OpenInternet)
{
    io_service io1;
    io_service io2;

    ip::tcp::endpoint ep1(ip::address::from_string("192.168.0.200"), 21112);
    ip::tcp::endpoint ep2(ip::address::from_string("188.168.25.28"), 21112);
    ip::tcp::socket sockclient(io1);

    ip::tcp::acceptor acc(io2, ep1);
    socket_ptr sockserv(new ip::tcp::socket(io2));

    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep2);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr);

    Pocket_Sys <MyPocket>  pocket{ MyPocket{ 0 }, 11, 2 };

    for (int i = 100; i > 0; i--)
    {
        pocket.fid = 100 - i;    pocket._pocket_id = i;

        sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    Sleep(1);

    thr.join();

    sockserv->close();
    socket_closed->store(true);

    recv.ext();

}

#endif

#ifdef ENABLE_BASIC

TEST(BasicFServer, ListenTryConnect)
{

    setlocale(LC_ALL, "russian");

    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);

    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr <MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1
    sockclient2.connect(ep); //2
    sockclient3.connect(ep); //3

    Sleep(20);

    int count_online = basic_server.GetOnlineFidList().size();
    //A

    ASSERT_EQ(count_online, 3);

}


TEST(BasicFServer, DissconectExist)
{

    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);
    cv_ptr _buffer_check(new std::condition_variable);
    bufferIO_ptr <MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1
    sockclient2.connect(ep); //2

    //time for connect
    Sleep(10);

    int out = basic_server.Disconnect(2);

    sockclient3.connect(ep); //3

    //time for connect
    Sleep(10);

    auto online = basic_server.GetOnlineFidList();
    //A

    ASSERT_EQ(online.size(), 2);
    if (online.size() == 2)
    {
        ASSERT_EQ(online[0], 1);
        ASSERT_EQ(online[1], 3);
    }
    ASSERT_EQ(out, 0);
}

TEST(BasicFServer, DissconectNoExist)
{

    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);
    cv_ptr _buffer_check(new std::condition_variable);
    bufferIO_ptr <MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1
    sockclient2.connect(ep); //2

    //time for connect
    Sleep(10);

    int out = basic_server.Disconnect(3);

    //Sleep(100);
    sockclient3.connect(ep); //3

    Sleep(10);
    //A

    ASSERT_EQ(out, -1);
}

TEST(BasicFServer, IsFidOnline)
{
    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);
    cv_ptr _buffer_check(new std::condition_variable);
    bufferIO_ptr <MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1
    sockclient2.connect(ep); //2

    //time for connect
    Sleep(20);
    
    bool out1 = basic_server.IsFidOnline(2);
    bool out2 = basic_server.IsFidOnline(3);

    sockclient3.connect(ep); //3

    Sleep(10);

    //A

    ASSERT_EQ(out1, true);
    ASSERT_EQ(out2, false);
}

TEST(BasicFServer, startHealthServer)
{
    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);
    cv_ptr _buffer_check(new std::condition_variable);
    bufferIO_ptr <MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1
    sockclient2.connect(ep); //2

    //time for connect
    Sleep(10);

    sockclient2.close();

    Sleep(10);

    int count_online_before = basic_server.GetOnlineFidList().size();

    Sleep(10);

    int  count_errors = basic_server.startHealthServer();


    int count_online_after = basic_server.GetOnlineFidList().size();
    //A

    ASSERT_EQ(count_online_before, 2);
    ASSERT_EQ(count_errors, 1);
    ASSERT_EQ(count_online_after, 1);

}

TEST(BasicFServer, _Off)
{
    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);
    cv_ptr _buffer_check(new std::condition_variable);
    bufferIO_ptr <MyPocket> ptr(new BufferIO<MyPocket>(_buffer_check));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1

    //time for connect
    Sleep(10);

    basic_server._Off();

    error_code ec;

    //anti-exception
    sockclient2.connect(ep,ec); //2

    //time for connect
    Sleep(10);

    bool is1 = basic_server.isWorking();

    basic_server.Listen();

    //extra-time for launch acceptor
    Sleep(10);

    bool is2 = basic_server.isWorking();

    sockclient2.connect(ep); //2

    //time for connect
    Sleep(10);

    int count_online = basic_server.GetOnlineFidList().size();
    //A

    ASSERT_EQ(is1, false);
    ASSERT_EQ(is2, true);
    ASSERT_EQ(count_online, 2);

}



#endif

#ifdef ENABLE_O

// please set in io.h #define DEBUG_SENDER

//////////////////////////////////////////////
// !!! Integrations test Sender - BufferIO ////
///////////////////////////////////////////////

TEST(Sender, StartStop)
{
    //A
    Sender <MyPocket> sender;

    // tehnlogy wait stack to sender not edit
    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket>  buffer_io(std::shared_ptr< BufferIO<MyPocket> >(new BufferIO<MyPocket>(_buffer_check)));

    //A

    // cv_ptr for extra check about this variable
    sender.start(buffer_io, _buffer_check);

    //extra time raii
    Sleep(10);

    sender.ext();

    //A

    //no throwing error or deadlock and infinite join
}

TEST(Sender, addOut)
{
    //A
    Sender <MyPocket> sender;

    // tehnlogy wait stack to sender not edit
    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket>  buffer_io(std::shared_ptr< BufferIO<MyPocket> >(new BufferIO<MyPocket>(_buffer_check)));

    //A

    // cv_ptr for extra check about this variable
    sender.start(buffer_io, _buffer_check);

    buffer_io->addOut(Pocket_Sys<MyPocket>{ {5}, 1, 88} );
    buffer_io->addOut(Pocket_Sys<MyPocket>{ {7}, 2, 88} );
    buffer_io->addOut(Pocket_Sys<MyPocket>{ {7}, 2, 88});

    // for time to add out before exit
    Sleep(10);

    sender.ext();

    //A

    //no throwing error or deadlock and infinite join
    // 5 1 1
    // 5 2 1
    // 5 2 2
}

TEST(Sender, addOutHardTest)
{
    //A
    Sender <MyPocket> sender;

    // tehnlogy wait stack to sender not edit
    cv_ptr _buffer_check(new std::condition_variable);

    bufferIO_ptr<MyPocket>  buffer_io(std::shared_ptr< BufferIO<MyPocket> >(new BufferIO<MyPocket>(_buffer_check)));

    //A

    // cv_ptr for extra check about this variable
    sender.start(buffer_io, _buffer_check);

    for(uint64_t i=0;i<100;i++)
          buffer_io->addOut(Pocket_Sys<MyPocket>{ {5}, 1, 1});


    // for time to add out before exit
    Sleep(30);

    sender.ext();

    //A

    //no throwing error or deadlock and infinite join
    // 5 1 1
    // 5 2 1
    // 5 2 2
}

#endif


int main(int argc,char** argv)
{
    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();

}