extern "C" 
{
    #include "packet_queue.h"
    #include "error.h"
    #include "nrf51.h"
    #include "nrf51_bitfields.h"
}

#include "CppUTest/TestHarness.h"


TEST_GROUP(packet_queue)
{

    void setup(void)
    {
    }
    
    void teardown(void)
    {
    }
};

TEST(packet_queue, test_packet_queue_init_succeed)
{
    packet_queue_t queue;
    LONGS_EQUAL(SUCCESS, packet_queue_init(&queue));
}

