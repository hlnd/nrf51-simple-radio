extern "C" 
{
    #include "packet_queue.h"
    #include "error.h"
    #include "nrf51.h"
    #include "nrf51_bitfields.h"
}

#include "CppUTest/TestHarness.h"

#include <limits.h>
#include <string.h>


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

    radio_packet_t packet;
    packet_queue_add(&queue, &packet);

    LONGS_EQUAL(false, packet_queue_is_empty(&queue));
}

TEST(packet_queue, test_queue_full_queue_is_full)
{
    LONGS_EQUAL(false, packet_queue_is_full(&queue));

    for (int i = 0; i < PACKET_QUEUE_SIZE; i++)
    {
        radio_packet_t packet;
        packet_queue_add(&queue, &packet);
    }
    LONGS_EQUAL(true, packet_queue_is_full(&queue));
}
    
TEST(packet_queue, test_queue_is_fifo)
{
    radio_packet_t in_packets[PACKET_QUEUE_SIZE];

    for (int i = 0; i < PACKET_QUEUE_SIZE; i++)
    {
        in_packets[i].data[0] = rand()*INT_MAX;
        packet_queue_add(&queue, &in_packets[i]);
    }

    for (int i = 0; i < PACKET_QUEUE_SIZE; i++)
    {
        radio_packet_t * p_packet = 0;
        packet_queue_get(&queue, &p_packet);

        CHECK(0 != p_packet);
        LONGS_EQUAL(0, memcmp(p_packet, &in_packets[i], sizeof(in_packets[i])));
    }
}
