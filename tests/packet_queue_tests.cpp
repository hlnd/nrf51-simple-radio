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

TEST(packet_queue, test_queue_add_succeeds_on_empty_queue)
{
    radio_packet_t packet;
    LONGS_EQUAL(SUCCESS, packet_queue_add(&queue, &packet));
}

TEST(packet_queue, test_queue_add_fails_on_full_queue)
{
    for (int i = 0; i < PACKET_QUEUE_SIZE; i++)
    {
        radio_packet_t packet;
        packet_queue_add(&queue, &packet);
    }

    radio_packet_t packet;
    LONGS_EQUAL(NO_MEMORY, packet_queue_add(&queue, &packet));
}

TEST(packet_queue, test_queue_get_fails_on_empty_queue)
{
    radio_packet_t * packet;
    LONGS_EQUAL(NOT_FOUND, packet_queue_get(&queue, &packet));

}

