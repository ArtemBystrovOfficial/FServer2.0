#include "../source/Server/IO/buffer.hpp"
#include "../source/Server/IO/io.hpp"
#include "../source/Server/Servise/basic.hpp"
#include "../source/Server/Servise/filter_io.hpp"
#include "../source/Server/FServer.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <chrono> 
using namespace std::chrono_literals;

// namespace resolved
using namespace fclient;
using namespace rf;

#define ENABLE_BUFFERIO
#define ENABLE_I
#define ENABLE_BASIC
#define ENABLE_O
#define ENABLE_FILTER_I
#define ENABLE_FILTER_O
#define ENABLE_LEFT_E2E
#define ENABLE_CENTER_E2E


////////////////////////
// UNIT TESTING
///////////////////////

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

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    BufferIO < MyPocket > buffer_io(_buffer_checkO, _buffer_checkI);

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

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    BufferIO < MyPocket > buffer_io(_buffer_checkO, _buffer_checkI);

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
     cv_ptr _buffer_checkO(new std::condition_variable);
     cv_ptr _buffer_checkI(new std::condition_variable);

     BufferIO < MyPocket > buffer_io(_buffer_checkO, _buffer_checkI);

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
        bool_atc_ptr empty{ new std::atomic<bool>{false} };
    //A
        auto thr = std::thread([&]() {

            acc.accept(*sockserv);

            });

        sockclient.connect(ep);

        //pre-work
        std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
        socket_closed->store(false);

        cv_ptr _buffer_checkO(new std::condition_variable);
        cv_ptr _buffer_checkI(new std::condition_variable);

        bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI ));

        Reciver<MyPocket> recv(sockserv, socket_closed, ptr,1,empty);

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
        bool_atc_ptr empty{ new std::atomic<bool>{false} };
    //A
        auto thr = std::thread([&]() {

            acc.accept(*sockserv);

            });

        sockclient.connect(ep);

        //pre-work
        std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
        socket_closed->store(false);

        cv_ptr _buffer_checkO(new std::condition_variable);
        cv_ptr _buffer_checkI(new std::condition_variable);

        bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

        Reciver<MyPocket> recv(sockserv, socket_closed, ptr,1,empty);

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

    bool_atc_ptr empty{ new std::atomic<bool>{false} };
    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr, 1,empty);

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

    bool_atc_ptr empty{ new std::atomic<bool>{false} };

    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr, 1,empty);

    Pocket_Sys <MyPocket>  pocket{ MyPocket{ 0 }, 11, 2 };

    sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    pocket.fid = 12;    pocket._pocket_id = 1;

    sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    pocket.fid = 13;    pocket._pocket_id = 3;

    sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    std::this_thread::sleep_for(10ms);

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

    bool_atc_ptr empty{ new std::atomic<bool>{false} };

    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr, 1,empty);

    Pocket_Sys <MyPocket>  pocket{ MyPocket{ 0 }, 11, 2 };

    for (int i = 100; i > 0; i--)
    {
        pocket.fid = 100-i;    pocket._pocket_id = i;

        sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    std::this_thread::sleep_for(20ms);

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

    bool_atc_ptr empty{ new std::atomic<bool>{false} };
    //A
    auto thr = std::thread([&]() {

        acc.accept(*sockserv);

        });

    sockclient.connect(ep2);

    //pre-work
    std::shared_ptr<std::atomic <bool>> socket_closed(new std::atomic <bool>{ false });
    socket_closed->store(false);


    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    Reciver<MyPocket> recv(sockserv, socket_closed, ptr, 1,empty);

    Pocket_Sys <MyPocket>  pocket{ MyPocket{ 0 }, 11, 2 };

    for (int i = 100; i > 0; i--)
    {
        pocket.fid = 100 - i;    pocket._pocket_id = i;

        sockclient.write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    std::this_thread::sleep_for(20ms);

    thr.join();

    sockserv->close();
    socket_closed->store(true);

    recv.ext();

}

#endif

#ifdef ENABLE_BASIC

TEST(BasicFServer, ListenTryConnect)
{

    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);
    cv_ptr empty(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, empty, "127.0.0.1", 2001);
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

    std::this_thread::sleep_for(20ms);

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

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);
    cv_ptr empty(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios,empty, "127.0.0.1", 2001);
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
    std::this_thread::sleep_for(20ms);

    int out = basic_server.Disconnect(2);

    sockclient3.connect(ep); //3

    //time for connect
    std::this_thread::sleep_for(20ms);

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
    cv_ptr empty(new std::condition_variable);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios,empty, "127.0.0.1", 2001);
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
    std::this_thread::sleep_for(20ms);

    int out = basic_server.Disconnect(3);


    sockclient3.connect(ep); //3

    std::this_thread::sleep_for(20ms);
    //A

    ASSERT_EQ(out, -1);
}

TEST(BasicFServer, IsFidOnline)
{
    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);
    cv_ptr empty(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios,empty, "127.0.0.1", 2001);
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
    std::this_thread::sleep_for(20ms);
    
    bool out1 = basic_server.IsFidOnline(2);
    bool out2 = basic_server.IsFidOnline(3);

    sockclient3.connect(ep); //3

    std::this_thread::sleep_for(20ms);

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

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);
    cv_ptr empty(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, empty, "127.0.0.1", 2001);
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
    std::this_thread::sleep_for(20ms);

    sockclient2.close();

    std::this_thread::sleep_for(20ms);

    int count_online_before = basic_server.GetOnlineFidList().size();

    std::this_thread::sleep_for(20ms);

    int  count_errors = basic_server.startHealthServer();


    int count_online_after = basic_server.GetOnlineFidList().size();
    //A

    ASSERT_EQ(count_errors, 0);
    ASSERT_EQ(count_online_after, 1);

}

TEST(BasicFServer, _Off)
{
    //A
    io_service ioC;

    // mock without work
    service_ptr ios(new io_service);

    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);
    cv_ptr empty(new std::condition_variable);
    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

#ifdef TEST_REAL_SITUATUON_MODE

    BasicFServer <MyPocket> basic_server(ptr, ios, "192.168.0.200", 21112);
    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);

#else

    BasicFServer <MyPocket> basic_server(ptr, ios, empty, "127.0.0.1", 2001);
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

#endif

    ip::tcp::socket sockclient1(ioC);
    ip::tcp::socket sockclient2(ioC);
    ip::tcp::socket sockclient3(ioC);


    //A
    basic_server.Listen();

    sockclient1.connect(ep); //1

    //time for connect
    std::this_thread::sleep_for(20ms);

    basic_server._Off();

    error_code ec;

    //anti-exception
    sockclient2.connect(ep,ec); //2

    //time for connect
    std::this_thread::sleep_for(20ms);

    bool is1 = basic_server.isWorking();

    basic_server.Listen();

    //extra-time for launch acceptor
    std::this_thread::sleep_for(20ms);

    bool is2 = basic_server.isWorking();

    sockclient2.connect(ep); //2

    //time for connect
        std::this_thread::sleep_for(100ms);

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
    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> ptr(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    //A

    // cv_ptr for extra check about this variable
    sender.start(ptr, _buffer_checkO);

    //extra time raii
    std::this_thread::sleep_for(10ms);

    sender.ext();

    //A

    //no throwing error or deadlock and infinite join
}

TEST(Sender, addOut)
{
    //A
    Sender <MyPocket> sender;

    // tehnlogy wait stack to sender not edit
    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer_io(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    //A

    // cv_ptr for extra check about this variable
    sender.start(buffer_io, _buffer_checkO);

    buffer_io->addOut(Pocket_Sys<MyPocket>{ {5}, 1, 88} );
    buffer_io->addOut(Pocket_Sys<MyPocket>{ {7}, 2, 88} );
    buffer_io->addOut(Pocket_Sys<MyPocket>{ {7}, 2, 88});

    // for time to add out before exit
    std::this_thread::sleep_for(10ms);

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
    cv_ptr _buffer_checkO(new std::condition_variable);
    cv_ptr _buffer_checkI(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer_io(new BufferIO<MyPocket>(_buffer_checkO, _buffer_checkI));

    //A

    // cv_ptr for extra check about this variable
    sender.start(buffer_io, _buffer_checkO);

    for(uint64_t i=0;i<100;i++)
          buffer_io->addOut(Pocket_Sys<MyPocket>{ {5}, 1, 1});


    // for time to add out before exit
    std::this_thread::sleep_for(30ms);

    sender.ext();

    //A

    //no throwing error or deadlock and infinite join
    // 5 1 1
    // 5 2 1
    // 5 2 2
}

#endif

#ifdef ENABLE_FILTER_I

/////////////////////////////////////////////////////////
// !!! Integrations test Reciver - BufferIO - Filter ////
//                          \                    /
//                               BasicFServer
/////////////////////////////////////////////////////////

TEST(ReciverFilter,reciveToUser)
{
    //A
        cv_ptr sender(new std::condition_variable);
        cv_ptr read(new std::condition_variable);

        bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(sender, read));

#ifdef TEST_REAL_SITUATUON_MODE
        service_ptr io(new io_service);
        basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io, "192.168.0.200", 21112));

        ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);
#else
        service_ptr io(new io_service);
        basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io, sender, "127.0.0.1", 2001));

        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
#endif

        ReciverFilter <MyPocket> filterI(buf, read, basic);

        socket_ptr sock_sender1(new ip::tcp::socket(*io));
        socket_ptr sock_sender2(new ip::tcp::socket(*io));
        socket_ptr sock_sender3(new ip::tcp::socket(*io));
           

    //A

        basic->Listen();

        sock_sender1->connect(ep);
        sock_sender2->connect(ep);
        sock_sender3->connect(ep);


        //extra time for sock send
        std::this_thread::sleep_for(10ms);

        Pocket_Sys<MyPocket> pocket{ {5},56, 1};

        pocket._pocket_id = 1;
        pocket.is_active = true;

        sock_sender1->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        pocket.pocket.n = 6;

        pocket._pocket_id = 1;
        pocket.is_active = true;

        sock_sender2->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        pocket.pocket.n = 7;

        pocket._pocket_id = 1;
        pocket.is_active = true;

        sock_sender3->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        auto pock1 = filterI.recv();
        auto pock2 = filterI.recv();
        auto pock3 = filterI.recv();

        int sum = pock1.first.n + pock2.first.n + pock3.first.n;


        basic->_Off();

    //A

        ASSERT_EQ(sum, 18);
        ASSERT_EQ(pock1.second + pock2.second +pock3.second, 6);

}

TEST(ReciverFilter, reciveToServer)
{
    //A
    cv_ptr sender(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);

    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(sender, wait));

#ifdef TEST_REAL_SITUATUON_MODE
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io, "192.168.0.200", 21112));

    ip::tcp::endpoint ep(ip::address::from_string("188.168.25.28"), 21112);
#else
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io, sender, "127.0.0.1", 2001));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
#endif


    ReciverFilter <MyPocket> filterI(buf, wait, basic);

    socket_ptr sock_sender1(new ip::tcp::socket(*io));
    socket_ptr sock_sender2(new ip::tcp::socket(*io));
    socket_ptr sock_sender3(new ip::tcp::socket(*io));


    //A

    basic->Listen();

    sock_sender1->connect(ep);
    sock_sender2->connect(ep);
    sock_sender3->connect(ep);


    //extra time for sock send

    Pocket_Sys<MyPocket> pocket{ {5},56, 1 };

    pocket.command = Pocket_Sys<MyPocket>::commands::CloseMe;

    pocket.is_command = true;

    pocket.is_active = true;

    //pocket.is_command

    pocket._pocket_id = 1;

    sock_sender1->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    pocket.pocket.n = 6;


    sock_sender2->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    pocket.pocket.n = 7;

    pocket.is_command = false;

    sock_sender3->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

    int out = filterI.recv().first.n;

    basic->_Off();

    //A

    ASSERT_EQ(out, 7);
}

#endif

#ifdef ENABLE_FILTER_O


///////////////////////////////////////////////
// !!! Integrations test |   Unit Testing  ////
///////////////////////////////////////////////

TEST(ReciverFilter, send_to_false)
{
    //A

    // indetification BufferIO
    cv_ptr empty(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);
    cv_ptr empty1(new std::condition_variable);
    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(empty, wait));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io,empty1, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    //A
    
    auto out = filter.send_to(MyPocket{ 5 }, 1 );

    //A

    ASSERT_EQ(out, false);
}

TEST(ReciverFilter, send_to_true)
{
    //A

    // indetification BufferIO
    cv_ptr empty(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);
    cv_ptr empty1(new std::condition_variable);
    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(empty, wait));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io,empty1, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    socket_ptr sock(new ip::tcp::socket(*io));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    
    //A


    basic->Listen();

    sock->connect(ep);

    //extra time to connect
    std::this_thread::sleep_for(10ms);

    auto out = filter.send_to(MyPocket{ 5 }, 1);

    Pocket_Sys<MyPocket> pocket;

    buf->getFOut(pocket);

    //A

    ASSERT_EQ(out, true);
    ASSERT_EQ(pocket.fid, 1);
    ASSERT_EQ(pocket.is_command, false);
    ASSERT_EQ(pocket.pocket.n, 5);
    
}


TEST(ReciverFilter, send_to_all_ignore)
{

    //A
    
    // indetification BufferIO
    cv_ptr empty(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);
    cv_ptr empty1(new std::condition_variable);

    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(empty, wait));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io,empty1, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    socket_ptr sock1(new ip::tcp::socket(*io));
    socket_ptr sock2(new ip::tcp::socket(*io));
    socket_ptr sock3(new ip::tcp::socket(*io));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    //A


    basic->Listen();

    sock1->connect(ep);
    sock2->connect(ep);
    sock3->connect(ep);

    //extra time to connect
    std::this_thread::sleep_for(10ms);

    sock3->close();

    //extra time to disconnect
    std::this_thread::sleep_for(10ms);

    basic->startHealthServer();

    auto out = filter.send_to_all(MyPocket{ 5 }, 2);

    

    Pocket_Sys<MyPocket> pocket;

    buf->getFOut(pocket);

    int is = buf->getFOut(pocket);

    //A

    ASSERT_EQ(is, 0);
    ASSERT_EQ(out, 1);
    ASSERT_EQ(pocket.fid, 1);
    ASSERT_EQ(pocket.is_command, false);


}

TEST(ReciverFilter, send_to_all_not_ignore)
{

    //A

    // indetification BufferIO
    cv_ptr empty(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);
    cv_ptr empty1(new std::condition_variable);

    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(empty, wait));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io,empty1, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    socket_ptr sock1(new ip::tcp::socket(*io));
    socket_ptr sock2(new ip::tcp::socket(*io));
    socket_ptr sock3(new ip::tcp::socket(*io));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    //A


    basic->Listen();

    sock1->connect(ep);
    sock2->connect(ep);
    sock3->connect(ep);

    //extra time to connect
    std::this_thread::sleep_for(10ms);

    sock3->close();

    //extra time to disconnect
    std::this_thread::sleep_for(10ms);

    basic->startHealthServer();

    auto out = filter.send_to_all(MyPocket{ 5 }, -1);

    Pocket_Sys<MyPocket> pocket1;
    Pocket_Sys<MyPocket> pocket2;


    buf->getFOut(pocket1);

    buf->getFOut(pocket2);

    int is = buf->getFOut(pocket1);

    //A

    ASSERT_EQ(is, 0);
    ASSERT_EQ(out, 2);
    ASSERT_EQ(pocket1.fid + pocket2.fid, 3);

}

TEST(ReciverFilter, new_group)
{
    //A

    // indetification BufferIO mock
    bufferIO_ptr<MyPocket> buf;

    // indetification Basic mock
    basic_ptr<MyPocket> basic;

    SenderFilter<MyPocket> filter(buf, basic);

    auto gid1 = filter.new_group();
    auto gid2 = filter.new_group();

    ASSERT_EQ(gid1,1);
    ASSERT_EQ(gid2,2);

}

TEST(ReciverFilter, new_group_vector)
{
    //A

    // indetification BufferIO mock
    bufferIO_ptr<MyPocket> buf;

    // indetification Basic mock
    basic_ptr<MyPocket> basic;

    SenderFilter<MyPocket> filter(buf, basic);

    auto gid1 = filter.new_group(std::vector{ 3, 5 });
    auto gid2 = filter.new_group(std::vector{ 1, 3 });

    auto vec = filter.get_group_list(2);

    ASSERT_EQ(gid1, 1);
    ASSERT_EQ(gid2, 2);
    ASSERT_EQ(vec[0], 1);
    ASSERT_EQ(vec[1], 3);

}

TEST(ReciverFilter, delete_from_group_true)
{
    //A

    // indetification BufferIO mock
    bufferIO_ptr<MyPocket> buf;

    // indetification Basic mock
    basic_ptr<MyPocket> basic;

    SenderFilter<MyPocket> filter(buf, basic);

    //A

    auto gid1 = filter.new_group(std::vector{ 3, 5 });
    auto gid2 = filter.new_group(std::vector{ 1, 3 });

    auto is = filter.delete_from_group(2, 1);

    auto vec = filter.get_group_list(2);


    //A

    ASSERT_EQ(gid1, 1);
    ASSERT_EQ(gid2, 2);
    ASSERT_EQ(is, true);
    ASSERT_EQ(vec.size(), 1);
    ASSERT_EQ(vec[0], 3);
}

TEST(ReciverFilter, delete_from_group_false1)
{
    //A

    // indetification BufferIO mock
    bufferIO_ptr<MyPocket> buf;

    // indetification Basic mock
    basic_ptr<MyPocket> basic;

    SenderFilter<MyPocket> filter(buf, basic);

    //A

    auto gid1 = filter.new_group(std::vector{ 3, 5 });
    auto gid2 = filter.new_group(std::vector{ 1, 3 });

    auto is = filter.delete_from_group(3, 1);

    auto vec = filter.get_group_list(2);


    //A

    ASSERT_EQ(gid1, 1);
    ASSERT_EQ(gid2, 2);
    ASSERT_EQ(is, false);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[1], 3);
}

TEST(ReciverFilter, delete_from_group_false2)
{
    //A

    // indetification BufferIO mock
    bufferIO_ptr<MyPocket> buf;

    // indetification Basic mock
    basic_ptr<MyPocket> basic;

    SenderFilter<MyPocket> filter(buf, basic);

    //A

    auto gid1 = filter.new_group(std::vector{ 3, 5 });
    auto gid2 = filter.new_group(std::vector{ 1, 3 });

    auto is = filter.delete_from_group(2, 2);

    auto vec = filter.get_group_list(2);


    //A

    ASSERT_EQ(is, false);
    ASSERT_EQ(vec.size(), 2);
    ASSERT_EQ(vec[1], 3);
}

TEST(ReciverFilter, send_to_group_true)
{

    //A

    // indetification BufferIO
    cv_ptr empty(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);
    cv_ptr empty1(new std::condition_variable);

    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(empty, wait));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io, empty1, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    socket_ptr sock1(new ip::tcp::socket(*io));
    socket_ptr sock2(new ip::tcp::socket(*io));
    socket_ptr sock3(new ip::tcp::socket(*io));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    //A


    basic->Listen();

    sock1->connect(ep);
    sock2->connect(ep);
    sock3->connect(ep);

    //extra time to connect
    std::this_thread::sleep_for(10ms);

    sock1->close();

    //extra time to disconnect
    std::this_thread::sleep_for(10ms);

    int gid = filter.new_group();

    filter.add_to_group(gid, 1);
    filter.add_to_group(gid,2);

    auto out = filter.send_to_group(MyPocket{ 5 }, 1);

    Pocket_Sys<MyPocket> pocket1;

    buf->getFOut(pocket1);

    int is = buf->getFOut(pocket1);

    //A

    ASSERT_EQ(is, 0);
    ASSERT_EQ(out, 1);
    ASSERT_EQ(pocket1.fid, 2);

}

TEST(ReciverFilter, send_to_group_false)
{

    //A

    // indetification BufferIO
    cv_ptr empty(new std::condition_variable);
    cv_ptr wait(new std::condition_variable);
    cv_ptr empty1(new std::condition_variable);

    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(empty, wait));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr<MyPocket> basic(new BasicFServer<MyPocket>(buf, io, empty1, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    socket_ptr sock1(new ip::tcp::socket(*io));
    socket_ptr sock2(new ip::tcp::socket(*io));
    socket_ptr sock3(new ip::tcp::socket(*io));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    //A


    basic->Listen();

    sock1->connect(ep);
    sock2->connect(ep);
    sock3->connect(ep);

    //extra time to connect
    std::this_thread::sleep_for(10ms);

    sock1->close();

    //extra time to disconnect
    std::this_thread::sleep_for(10ms);

    int gid = filter.new_group();

    filter.add_to_group(gid, 1);
    filter.add_to_group(gid, 2);

    basic->startHealthServer();

    auto out = filter.send_to_group(MyPocket{ 5 }, 3);

    Pocket_Sys<MyPocket> pocket1;

    int is = buf->getFOut(pocket1);

    //A

    ASSERT_EQ(is, 0);
    ASSERT_EQ(out, -1);

}

#endif

//!!!! DISABLE ALL DEBUG DEFINE IN ALL FILES FOR NEXT TESTS

#ifdef ENABLE_LEFT_E2E


////////////////////////
// End to End TESTING //
////////////////////////

// please skip DEBUG_SENDER in io.hpp

TEST(BasicFServer, E2E_LEFT_SEND_SOCKET_OUT)
{
    //A
     
    // indetification BufferIO
    cv_ptr senderfilter(new std::condition_variable);
    cv_ptr reciverfilter(new std::condition_variable);

    bufferIO_ptr<MyPocket> buf(new BufferIO<MyPocket>(senderfilter, reciverfilter));

    // indetification Basic
    service_ptr io(new io_service);
    basic_ptr <MyPocket> basic(new BasicFServer<MyPocket>(buf, io, senderfilter, "127.0.0.1", 2001));

    SenderFilter<MyPocket> filter(buf, basic);

    socket_ptr sock1(new ip::tcp::socket(*io));
    socket_ptr sock2(new ip::tcp::socket(*io));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    
    //A

    basic->Listen();

    sock1->connect(ep);
    sock2->connect(ep);

    //Extra connect
    std::this_thread::sleep_for(10ms);

    MyPocket pocket{ 5 };
    Pocket_Sys<MyPocket> recv1;
    Pocket_Sys<MyPocket> recv2;

    filter.send_to(pocket,1);

    pocket.n = 7;

    filter.send_to(pocket, 2);

    sock1->read_some(buffer(&recv1, sizeof(Pocket_Sys<MyPocket>)));
    sock2->read_some(buffer(&recv2, sizeof(Pocket_Sys<MyPocket>)));
    
    //A

    ASSERT_EQ(recv1.pocket.n,5);
    ASSERT_EQ(recv2.pocket.n,7);
}

#endif

//30 pockets 3 sockets
#ifdef ENABLE_CENTER_E2E


using Out = _Out<MyPocket>;

TEST(FServer, BasicStrain)
{

    io_service ios;

    FServer<MyPocket> fserver("127.0.0.1", 2001);

    socket_ptr sock1(new ip::tcp::socket(ios));
    socket_ptr sock2(new ip::tcp::socket(ios));
    socket_ptr sock3(new ip::tcp::socket(ios));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    fserver.start();

    sock1->connect(ep);
    sock2->connect(ep);
    sock3->connect(ep);

    //extra connect seconds
    std::this_thread::sleep_for(10ms);

    Pocket_Sys<MyPocket> pocket;

    pocket.is_command = false;


    for (int i = 0; i < 10; i++)
    {
        pocket.pocket.n = i;
        pocket.is_active = true;
        pocket._pocket_id = i + 1;

        sock1->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }
    for (int i = 0; i < 10; i++)
    {
        pocket.pocket.n = i;
        pocket._pocket_id = i + 1;
        pocket.is_active = true;

        sock2->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }
    for (int i = 0; i < 10; i++)
    {
        pocket.pocket.n = i;
        pocket._pocket_id = i + 1;
        pocket.is_active = true;

        sock3->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    std::pair <MyPocket, int> data;

    for (int i = 0; i < 30; i++)
    {
        fserver >> data;

        auto& [pocket, fid] = data;

        pocket.n *= 2;
        fserver << Out{ pocket,FType::FID,fid };
    }

    int sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sock1->read_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        ASSERT_EQ(pocket._pocket_id, i + 1);
        sum += pocket.pocket.n;

    }
    ASSERT_EQ(sum, 90);
    sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sock2->read_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        ASSERT_EQ(pocket._pocket_id, i + 1);
        sum += pocket.pocket.n;
    }
    ASSERT_EQ(sum, 90);
    sum = 0;
    for (int i = 0; i < 10; i++)
    {
        sock3->read_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        ASSERT_EQ(pocket._pocket_id, i + 1);
        sum += pocket.pocket.n;
    }
    ASSERT_EQ(sum, 90);

    //fserver << Out{ MyPocket{5}, FType::FID, 1 };

}

//150 000 pockets 3 sockets paralell real sitution with purpose

TEST(FServer, HighStrain)
{

    io_service ios;

    FServer<MyPocket> fserver("127.0.0.1", 2001);

    socket_ptr sock1(new ip::tcp::socket(ios));
    socket_ptr sock2(new ip::tcp::socket(ios));
    socket_ptr sock3(new ip::tcp::socket(ios));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    fserver.start();

    sock1->connect(ep);
    sock2->connect(ep);
    sock3->connect(ep);

    //extra connect seconds
    std::this_thread::sleep_for(100ms);

    Pocket_Sys<MyPocket> pocket;

    pocket.is_command = false;


    for (int i = 0; i < 50000; i++)
    {
        pocket.pocket.n = i;
        pocket._pocket_id = i + 1;
        pocket.is_active = true;

        sock1->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        sock2->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));

        sock3->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    std::pair <MyPocket, int> data;

    std::thread thr1(
        [&] {
            long long sum = 0;
            Pocket_Sys<MyPocket> pocket1;
            for (int i = 0; i < 50000; i++)
            {

                sock1->read_some(buffer(&pocket1, sizeof(Pocket_Sys <MyPocket>)));

                //ASSERT_EQ(pocket._pocket_id, i + 1);
                sum += pocket1.pocket.n;

            }
            ASSERT_EQ(sum, 2'499'950'000);

        }
    );

    std::thread thr2(
        [&] {
            long long sum = 0;
            Pocket_Sys<MyPocket> pocket2;
            for (int i = 0; i < 50000; i++)
            {

                sock2->read_some(buffer(&pocket2, sizeof(Pocket_Sys <MyPocket>)));

                //ASSERT_EQ(pocket._pocket_id, i + 1);
                sum += pocket2.pocket.n;
            }
            ASSERT_EQ(sum, 2'499'950'000);

        }
    );

    std::thread thr3(
        [&] {
            long long sum = 0;
            Pocket_Sys<MyPocket> pocket3;
            for (int i = 0; i < 50000; i++)
            {

                sock3->read_some(buffer(&pocket3, sizeof(Pocket_Sys <MyPocket>)));

                //ASSERT_EQ(pocket._pocket_id, i + 1);
                sum += pocket3.pocket.n;
            }
           ASSERT_EQ(sum, 2'499'950'000);

        }
    );

    int sumin = 0;

    for (int i = 0; i < 150000; i++)
    {
        fserver >> data;

        auto& [pocket, fid] = data;

        sumin += pocket.n;
        pocket.n *= 2;
        fserver << Out{ pocket,FType::FID,fid };
    }
    ASSERT_EQ(sumin, 3'749'925'000);

    thr1.join();
    thr2.join();
    thr3.join();

}


TEST(FServer, ManySocketsSend)
{
    io_service ios;

    FServer <MyPocket> fserver("127.0.0.1", 2001);

    std::vector <socket_ptr> ptrs;

    for (int i = 0; i < 100; i++)
        ptrs.push_back(socket_ptr(new ip::tcp::socket(ios)));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    fserver.start();

    for (int i = 0; i < 100; i++)
    {
        ptrs[i]->connect(ep);
    }

    //extra connect seconds
    std::this_thread::sleep_for(100ms);




    Pocket_Sys<MyPocket> pocket;

    pocket.is_command = false;

    for (int i = 0; i < 100; i++)
    {
        if(i%2==0)
        ptrs[i]->close();
    }

    std::this_thread::sleep_for(100ms);

    fserver.refreshOnline();

    auto list = fserver.getOnlineList();

    for (int i = 0; i < 100; i++)
    {
        int id = -1;
        for (auto& sock : ptrs)
        {
            id++;
            if (id % 2 == 0) continue;
            pocket.pocket.n = i + 1;
            pocket._pocket_id = i + 1;
            pocket.is_active = true;
            sock->write_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
        }
    }

    int sumin = 0;
    std::pair<MyPocket, int> data;

    for (int i = 0; i < 5000; i++)
    {
        fserver >> data;

        auto& [pocket, fid] = data;

        sumin += pocket.n;
    }

    ASSERT_EQ(sumin, 252'500);
    ASSERT_EQ(list.size(), 50);
}

TEST(FServer, ManySocketsRecive)
{
    io_service ios;

    FServer <MyPocket> fserver("127.0.0.1", 2001);

    std::vector <socket_ptr> ptrs;

    for (int i = 0; i < 100; i++)
        ptrs.push_back(socket_ptr(new ip::tcp::socket(ios)));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    fserver.start();

    for (int i = 0; i < 100; i++)
    {
        ptrs[i]->connect(ep);
    }

    //extra connect seconds
    std::this_thread::sleep_for(100ms);

    bool is = true;

    auto list = fserver.getOnlineList();

    Pocket_Sys<MyPocket> pocket;

    pocket.is_command = false;

    MyPocket pock;


    for (int i = 0; i < 10000; i++)
    {

        pock.n = i + 1;
        fserver << Out{ pock ,FType::FID, i % 100 + 1 };

    }
    int sumin = 0;
    for (int i = 0; i < 100; i++)
    {

        for (auto& sock : ptrs)
        {
            sock->read_some(buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
            sumin += pocket.pocket.n;
        }
    }

    ASSERT_EQ(sumin, 50'005'000);
}

#endif


//using Out = _Out<MyPocket>;

int main(int argc,char** argv)
{

    // check memory leaks
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();

}
