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

extern "C"
{
    void error_handler(uint32_t err_code, uint32_t line_num, char * file_name)
    {

    }
}


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

    for (int i = 0; i < PACKET_QUEUE_ELEMENTS; i++)
    {
        radio_packet_t packet;
        packet_queue_add(&queue, &packet);
    }
    LONGS_EQUAL(true, packet_queue_is_full(&queue));
}

TEST(packet_queue, test_queue_add_succeeds_on_empty_queue)
{
    radio_packet_t packet;
    packet.data[0] = rand()*INT_MAX;
    LONGS_EQUAL(SUCCESS, packet_queue_add(&queue, &packet));

    LONGS_EQUAL(0, memcmp(&packet, &queue.packets[0], sizeof(packet)));
}

TEST(packet_queue, test_queue_add_succeeds_on_non_empty_queue)
{
    radio_packet_t packet;
    packet_queue_add(&queue, &packet);

    packet.data[0] = rand()*INT_MAX;
    LONGS_EQUAL(SUCCESS, packet_queue_add(&queue, &packet));

    LONGS_EQUAL(0, memcmp(&packet, &queue.packets[queue.tail-1], sizeof(packet)));
}

TEST(packet_queue, test_queue_add_fails_on_full_queue)
{
    for (int i = 0; i < PACKET_QUEUE_ELEMENTS; i++)
    {
        radio_packet_t packet;
        LONGS_EQUAL(SUCCESS, packet_queue_add(&queue, &packet));
    }

    radio_packet_t packet;
    LONGS_EQUAL(NO_MEMORY, packet_queue_add(&queue, &packet));
}

TEST(packet_queue, test_queue_get_fails_on_empty_queue)
{
    radio_packet_t * packet;
    LONGS_EQUAL(NOT_FOUND, packet_queue_get(&queue, &packet));

}

TEST(packet_queue, test_queue_get_returns_correct_pointer)
{
    radio_packet_t in_packet;
    packet_queue_add(&queue, &in_packet);

    radio_packet_t * out_packet = 0;
    packet_queue_get(&queue, &out_packet);

    POINTERS_EQUAL(out_packet, &queue.packets[0]);
}

TEST(packet_queue, test_queue_add_get_packet)
{
    radio_packet_t in_packet;
    in_packet.data[0] = rand()*INT_MAX;
    packet_queue_add(&queue, &in_packet);

    radio_packet_t * out_packet;
    packet_queue_get(&queue, &out_packet);
    
    LONGS_EQUAL(0, memcmp(&in_packet, out_packet, sizeof(in_packet)));
}
    
TEST(packet_queue, test_queue_is_fifo)
{
    radio_packet_t in_packets[PACKET_QUEUE_ELEMENTS];

    for (int i = 0; i < PACKET_QUEUE_ELEMENTS; i++)
    {
        in_packets[i].data[0] = rand()*INT_MAX;
        LONGS_EQUAL(SUCCESS, packet_queue_add(&queue, &in_packets[i]));
    }

    radio_packet_t * out_packets[PACKET_QUEUE_ELEMENTS] = {0};
    for (int i = 0; i < PACKET_QUEUE_ELEMENTS; i++)
    {
        LONGS_EQUAL(SUCCESS, packet_queue_get(&queue, &out_packets[i]));
    }

    CHECK(0 != out_packets[0]);
    LONGS_EQUAL(0, memcmp(out_packets[0], &in_packets[0], sizeof(in_packets[0])));
    CHECK(0 != out_packets[1]);
    LONGS_EQUAL(0, memcmp(out_packets[1], &in_packets[1], sizeof(in_packets[1])));
    CHECK(0 != out_packets[2]);
    LONGS_EQUAL(0, memcmp(out_packets[2], &in_packets[2], sizeof(in_packets[2])));
    CHECK(0 != out_packets[3]);
    LONGS_EQUAL(0, memcmp(out_packets[3], &in_packets[3], sizeof(in_packets[3])));
    CHECK(0 != out_packets[4]);
    LONGS_EQUAL(0, memcmp(out_packets[4], &in_packets[4], sizeof(in_packets[4])));
    CHECK(0 != out_packets[5]);
    LONGS_EQUAL(0, memcmp(out_packets[5], &in_packets[5], sizeof(in_packets[5])));
    CHECK(0 != out_packets[6]);
    LONGS_EQUAL(0, memcmp(out_packets[6], &in_packets[6], sizeof(in_packets[6])));
    CHECK(0 != out_packets[7]);
    LONGS_EQUAL(0, memcmp(out_packets[7], &in_packets[7], sizeof(in_packets[7])));
    CHECK(0 != out_packets[8]);
    LONGS_EQUAL(0, memcmp(out_packets[8], &in_packets[8], sizeof(in_packets[8])));
    CHECK(0 != out_packets[9]);
    LONGS_EQUAL(0, memcmp(out_packets[9], &in_packets[9], sizeof(in_packets[9])));
}
