#include "../source/Client/FClient.hpp"
#include "../source/Client/Servise/basic.hpp"
#include "../source/Client/Servise/filter_io.hpp"

using namespace asio;

#include <gtest/gtest.h>
#include <gmock/gmock.h>

////////////////////////
// UNIT TESTING
///////////////////////

#include <chrono> 
using namespace std::chrono_literals;

#define ENABLE_BASIC
#define ENABLE_FILTER_I
#define ENABLE_FILTER_0
#define ENABLE_CENTER_E2E

struct MyPocket
{
    int n;
};

using fec = ReciverSingle<MyPocket>::f_error;

#ifdef ENABLE_BASIC

TEST(BasicFClient, connect)
{
    //A
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    BasicFClient<MyPocket> server(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    socket_ptr sock(new ip::tcp::socket(*io_s));
    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock);
        }
    );

    auto is = server.connect();

    th.join();
    //A
    ASSERT_EQ(is, true);

}

TEST(BasicFClient, connectBad)
{
    //A
    setlocale(LC_ALL, "russian");
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    BasicFClient<MyPocket> server(buffer, io_s, w_sender, "127.0.0.1", 2001);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    //A
    auto is1 = server.connect();
    //A
    ASSERT_EQ(is1, false);
    
}

TEST(BasicFClient, connectDisconnectConnect)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    BasicFClient<MyPocket> server(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    socket_ptr sock2(new ip::tcp::socket(*io_s));
    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    auto is1 = server.connect();

    th.join();

    auto is = server.disconnect();

    std::thread th1(
        [&] {
            acceptor.accept(*sock2);
        }
    );

    auto is2 = server.connect();

    th1.join();
    //A
    ASSERT_EQ(is1, true);
    ASSERT_EQ(is2, true);

}

TEST(BasicFClient, disconnectBad)
{

    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    BasicFClient<MyPocket> server(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);
    //A
    socket_ptr sock1(new ip::tcp::socket(*io_s));
    socket_ptr sock2(new ip::tcp::socket(*io_s));


    auto is = server.disconnect();
    //A
    ASSERT_EQ(is, false);

}

TEST(BasicFClient, isConnected)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    BasicFClient<MyPocket> server(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    socket_ptr sock2(new ip::tcp::socket(*io_s));
    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    server.connect();

    th.join();

    auto is1 = server.isConnected();

    server.disconnect();

    std::thread th1(
        [&] {
            acceptor.accept(*sock2);
        }
    );

    auto is2 = server.isConnected();

    server.connect();

    th1.join();
    //A
    ASSERT_EQ(is1, true);
    ASSERT_EQ(is2, false);
}

TEST(BasicFClient, StartHealthClient)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    BasicFClient<MyPocket> server(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    socket_ptr sock2(new ip::tcp::socket(*io_s));
    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    server.connect();

    th.join();

    sock1->close();

    std::this_thread::sleep_for(20ms);

    bool is1 = server.startHealthClient();

    bool is2 = server.isConnected();
    //A
    ASSERT_EQ(is1, true);
    ASSERT_EQ(is2, false);

}

#endif

template <class _Pocket>
using basic_ptr = std::shared_ptr<BasicFClient<_Pocket>>;

#ifdef ENABLE_FILTER_I

TEST(ReciverSingle, Recv)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    basic_ptr<MyPocket> server = std::make_shared< BasicFClient<MyPocket> >(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    ReciverSingle <MyPocket>reciverfilter(buffer, w_reciver, server);

    socket_ptr sock1(new ip::tcp::socket(*io_s));

    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    server->connect();

    th.join();
    //extra time for sock send

    std::this_thread::sleep_for(20ms);

    Pocket_Sys<MyPocket> pocket;

    pocket.is_command = false;

    for (int i = 0; i < 100; i++)
    {
        pocket._pocket_id = i + 1;
        pocket.pocket.n = i + 1;
        pocket.is_active = true;
        sock1->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    int sum = 0;
    for (int i = 0; i < 100; i++)
    {
        sum+=reciverfilter.recv().first.n;
    }

    ASSERT_EQ(sum, 5050);

}

TEST(ReciverSingle, Recv2Socks)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buffer(new BufferIO<MyPocket>(w_sender, w_reciver));
    basic_ptr<MyPocket> server = std::make_shared< BasicFClient<MyPocket> >(buffer, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    ReciverSingle <MyPocket>reciverfilter(buffer, w_reciver, server);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    socket_ptr sock2(new ip::tcp::socket(*io_s));

    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    server->connect();

    th.join();
    //extra time for sock send

    std::this_thread::sleep_for(20ms);

    Pocket_Sys<MyPocket> pocket;

    pocket.is_command = false;

    for (int i = 0; i < 100; i++)
    {
        pocket._pocket_id = i + 1;
        pocket.pocket.n = i + 1;
        pocket.is_active = true;
        sock1->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    int sum = 0;
    for (int i = 0; i < 100; i++)
    {
        sum += reciverfilter.recv().first.n;
    }

    server->disconnect();

    std::thread th2(
        [&] {
            acceptor.accept(*sock2);
        }
    );

    server->connect();

    th2.join();
    //extra time for sock send

    std::this_thread::sleep_for(20ms);

    pocket.is_command = false;

    for (int i = 0; i < 100; i++)
    {
        pocket._pocket_id = i + 1;
        pocket.pocket.n = i + 1;
        pocket.is_active = true;
        sock2->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys <MyPocket>)));
    }

    for (int i = 0; i < 100; i++)
    {
        sum += reciverfilter.recv().first.n;
    }
    //A    
    ASSERT_EQ(sum, 10100);

}


#endif

#ifdef ENABLE_FILTER_0

TEST(SenderSingle, send_to)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buff = std::make_shared< BufferIO <MyPocket> >(w_sender, w_reciver);
    basic_ptr<MyPocket> server = std::make_shared< BasicFClient <MyPocket> >(buff, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    SenderSingle <MyPocket> senderfilter(buff, server);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    server->connect();

    th.join();
    //extra time for sock send

    std::this_thread::sleep_for(20ms);

    for (int i = 0; i < 100; i++)
    {
        senderfilter.send_to(MyPocket{ 5 });
    }

    Pocket_Sys<MyPocket> pocket_sys;

    int sum_pock = 0;
    int sum = 0;

    for (int i = 0; i < 100; i++)
    {
        sock1->read_some(asio::buffer(&pocket_sys, sizeof(Pocket_Sys <MyPocket>)));
        sum_pock += pocket_sys._pocket_id;
        sum += pocket_sys.pocket.n;

    }
    server->disconnect();

    bool is = senderfilter.send_to(MyPocket{ 5 });

    //A    
    ASSERT_EQ(is, false);
    ASSERT_EQ(sum, 500);
    ASSERT_EQ(sum_pock, 5050);

}

TEST(SenderSingle, send_to_double)
{
    service_ptr io_s(new io_service);

    cv_ptr w_sender(new std::condition_variable);
    cv_ptr w_reciver(new std::condition_variable);

    bufferIO_ptr<MyPocket> buff = std::make_shared< BufferIO <MyPocket> >(w_sender, w_reciver);
    basic_ptr<MyPocket> server = std::make_shared< BasicFClient <MyPocket> >(buff, io_s, w_sender, "127.0.0.1", 2001);

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(*io_s, ep);

    SenderSingle <MyPocket> senderfilter(buff, server);

    socket_ptr sock1(new ip::tcp::socket(*io_s));
    socket_ptr sock2(new ip::tcp::socket(*io_s));

    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    server->connect();

    th.join();
    //extra time for sock send

    std::this_thread::sleep_for(20ms);

    for (int i = 0; i < 100; i++)
    {
        senderfilter.send_to(MyPocket{ 5 });
    }

    Pocket_Sys<MyPocket> pocket_sys;

    int sum_pock = 0;
    int sum = 0;

    for (int i = 0; i < 100; i++)
    {
        sock1->read_some(asio::buffer(&pocket_sys, sizeof(Pocket_Sys <MyPocket>)));
        sum_pock += pocket_sys._pocket_id;
        sum += pocket_sys.pocket.n;

    }
    //A    
    ASSERT_EQ(sum, 500);
    ASSERT_EQ(sum_pock, 5050);

    server->disconnect();

    //A
    std::thread th2(
        [&] {
            acceptor.accept(*sock2);
        }
    );

    server->connect();

    th2.join();

    std::this_thread::sleep_for(20ms);

    for (int i = 0; i < 100; i++)
    {
        senderfilter.send_to(MyPocket{ 5 });
    }

    sum_pock = 0;
    sum = 0;

    for (int i = 0; i < 100; i++)
    {
        sock2->read_some(asio::buffer(&pocket_sys, sizeof(Pocket_Sys <MyPocket>)));
        sum_pock += pocket_sys._pocket_id;
        sum += pocket_sys.pocket.n;

    }
    //A    
    ASSERT_EQ(sum, 500);
    ASSERT_EQ(sum_pock, 5050);

}

#endif

#ifdef ENABLE_CENTER_E2E

TEST(FClient, BasicStrain)
{
    //A
    io_service io_s;

    FClient <MyPocket> fclient("127.0.0.1", 2001);

    socket_ptr sock1(new ip::tcp::socket(io_s));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(io_s, ep);

    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    fclient.connect();

    th.join();

    std::this_thread::sleep_for(20ms);

    Pocket_Sys<MyPocket> pocket;

    for (int i = 0; i < 100; i++)
    {
        pocket.pocket.n = i + 1;
        pocket._pocket_id = 100 - i;
        pocket.is_active = true;
        sock1->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys<MyPocket>)));
    }

   std::pair < MyPocket, fec > pock;
    int sum = 0;
    for (int i = 0; i < 100; i++)
    {
        fclient >> pock;
        pock.first.n *= 2;
        fclient << pock.first;
    }

    for (int i = 0; i < 100; i++)
    {
        sock1->read_some(asio::buffer(&pocket, sizeof(Pocket_Sys<MyPocket>)));
        sum += pocket.pocket.n;
    }

    fclient.disconect();

    //A
    ASSERT_EQ(sum, 10100);
}

TEST(FClient, Basic2Strain)
{
    //A
    io_service io_s;

    FClient <MyPocket> fclient("127.0.0.1", 2001);

    socket_ptr sock1(new ip::tcp::socket(io_s));
    socket_ptr sock2(new ip::tcp::socket(io_s));

    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);

    ip::tcp::acceptor acceptor(io_s, ep);

    //A
    std::thread th(
        [&] {
            acceptor.accept(*sock1);
        }
    );

    fclient.connect();

    th.join();

    std::this_thread::sleep_for(20ms);

    Pocket_Sys<MyPocket> pocket;

    for (int i = 0; i < 100; i++)
    {
        pocket.pocket.n = i + 1;
        pocket._pocket_id = 100 - i;
        pocket.is_active = true;
        sock1->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys<MyPocket>)));
    }

    std::pair < MyPocket, fec > pock;
    int sum = 0;
    for (int i = 0; i < 100; i++)
    {
        fclient >> pock;
        pock.first.n *= 2;
        fclient << pock.first;
    }

    for (int i = 0; i < 100; i++)
    {
        sock1->read_some(asio::buffer(&pocket, sizeof(Pocket_Sys<MyPocket>)));
        sum += pocket.pocket.n;
    }

    fclient.disconect();


    std::thread th2(
        [&] {
            acceptor.accept(*sock2);
        }
    );

    fclient.connect();

    th2.join();

    std::this_thread::sleep_for(20ms);

    for (int i = 0; i < 100; i++)
    {
        pocket.pocket.n = i + 1;
        pocket._pocket_id = i + 1;
        pocket.is_active = true;
        sock2->write_some(asio::buffer(&pocket, sizeof(Pocket_Sys<MyPocket>)));
    }

    int sum1 = 0;
    for (int i = 0; i < 100; i++)
    {
        fclient >> pock;
        pock.first.n *= 2;
        fclient << pock.first;
    }

    for (int i = 0; i < 100; i++)
    {
        sock2->read_some(asio::buffer(&pocket, sizeof(Pocket_Sys<MyPocket>)));
        sum1 += pocket.pocket.n;
    }


    //A
    ASSERT_EQ(sum, 10100);
    ASSERT_EQ(sum1, 10100);
}

#endif

int main(int argc,char** argv)
{

    // check memory leaks
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    testing::InitGoogleTest(&argc,argv);
    return RUN_ALL_TESTS();

}
