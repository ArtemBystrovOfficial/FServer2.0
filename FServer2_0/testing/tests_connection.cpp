#include "../Client/FClient.hpp"
#include "../Server/FServer.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include <chrono> 

////////////////////////
// INTEGRATION TESTING
///////////////////////

using namespace std::chrono_literals;

struct MyPocket
{
    int n;
};


using fec = ReciverSingle<MyPocket>::f_error;

#define ENABLE_BANNED
#define ENABLE_PAUSE

#ifdef ENABLE_BANNED

TEST(FServer, BannedFunctions)
{
    FServer<MyPocket> sv("192.168.0.200",21112);
    FClient<MyPocket> cl("188.168.25.28",21112);

    sv.banIp("188.168.25.28");

    auto list = sv.getBanListIp();

    ASSERT_NE(list.find("188.168.25.28"), list.end());

    sv.unbanIp("188.168.25.28");

    list = sv.getBanListIp();

    ASSERT_TRUE(list.empty());
}

TEST(FServer, BannedPerson)
{
    FServer<MyPocket> sv("192.168.0.200", 21112);
    FClient<MyPocket> cl("188.168.25.28", 21112);

    sv.start();

    sv.banIp("188.168.25.28");

    cl.connect();

    ASSERT_EQ(cl.isBanned(), false);

    MyPocket pocket;

    std::pair< MyPocket, fec > r_pocket;

    cl >> r_pocket;

    ASSERT_EQ(r_pocket.second, fec::ban);

    ASSERT_EQ(cl.isBanned(), true);

    ASSERT_EQ(cl.isConnected(), false);

    sv.unbanIp("188.168.25.28");

    cl.connect();

    std::this_thread::sleep_for(10ms);

    pocket.n = 5;

    sv << _Out<MyPocket>{pocket, FType::ALL, -1};

    cl >> r_pocket;

    ASSERT_EQ(r_pocket.second, fec::none);

    ASSERT_EQ(pocket.n,5);
        
}

TEST(FServer, ListIP)
{
    FServer<MyPocket> sv("192.168.0.200", 21112);
    FClient<MyPocket> cl("188.168.25.28", 21112);

    sv.start();

    cl.connect();

    //wait connect accept
    std::this_thread::sleep_for(10ms);

    auto list = sv.getOnlineIpList();

    ASSERT_EQ(list[0].first, 1);
    ASSERT_EQ(list[0].second, "188.168.25.28");
}

TEST(FServer, IpByFid)
{
    FServer<MyPocket> sv("192.168.0.200", 21112);
    FClient<MyPocket> cl("188.168.25.28", 21112);

    sv.start();

    cl.connect();

    //wait connect accept
    std::this_thread::sleep_for(10ms);

    auto ip1 = sv.getIpByFid(1);
    auto ip2 = sv.getIpByFid(2);

    ASSERT_EQ(ip1, "188.168.25.28");
    ASSERT_EQ(ip2, "");
}
#endif

#ifdef ENABLE_PAUSE

TEST(FServer, stopServer)
{
    auto * sv = new FServer<MyPocket>("192.168.0.200", 21112);
    FClient<MyPocket> cl("188.168.25.28", 21112);

    std::pair<MyPocket, int> pack;

    sv->start();

    //wait connect accept
    std::this_thread::sleep_for(10ms);

    std::thread handler(

        [&] {
                *sv >> pack;
        }
    );

    cl.connect();

    //wait connect accept
    std::this_thread::sleep_for(10ms);

    sv->stop();

    std::pair< MyPocket, fec > r_pocket;

    cl << MyPocket{ 5 };

    cl >> r_pocket;

    ASSERT_EQ(r_pocket.second, fec::pause);

    delete sv;

    handler.join();
}

#endif

int main(int argc, char** argv)
{

    // check memory leaks
  //  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();

}
