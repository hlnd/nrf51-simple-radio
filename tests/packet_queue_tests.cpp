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
    packet_queue_t queue;

    void setup(void)
    {
        packet_queue_init(&queue);
    }
    
    void teardown(void)
    {
    }
};

TEST(packet_queue, test_queue_init_succeed)
{
    packet_queue_t queue;
    LONGS_EQUAL(SUCCESS, packet_queue_init(&queue));
}

TEST(packet_queue, test_queue_new_queue_is_empty)
{
    LONGS_EQUAL(true, packet_queue_is_empty(&queue));
}

TEST(packet_queue, test_queue_add_queue_is_not_empty)
{
    radio_packet_t packet;
    packet_queue_add(&queue, &packet);

    LONGS_EQUAL(false, packet_queue_is_empty(&queue));
}


    
