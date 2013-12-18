extern "C" 
{
    #include "queue.h"
    #include "error.h"
    #include "nrf51.h"
    #include "nrf51_bitfields.h"
}

#include "CppUTest/TestHarness.h"

#include <limits.h>
#include <string.h>

#define TEST_QUEUE_SIZE 8
#define TEST_QUEUE_ELEMENTS (TEST_QUEUE_SIZE-1)

extern "C"
{
    void error_handler(uint32_t err_code, uint32_t line_num, char * file_name)
    {

    }
}


TEST_GROUP(queue)
{
    queue_t queue;

    void setup(void)
    {
        QUEUE_INIT(queue, TEST_QUEUE_SIZE, sizeof(radio_packet_t));
    }
    
    void teardown(void)
    {
    }
};

TEST(queue, test_queue_init_succeed)
{
    queue_t queue;
    LONGS_EQUAL(SUCCESS, queue_init(&queue));
}

TEST(queue, test_queue_new_queue_is_empty)
{
    LONGS_EQUAL(true, queue_is_empty(&queue));

    radio_packet_t packet;
    queue_add(&queue, (uint8_t *) &packet);

    LONGS_EQUAL(false, queue_is_empty(&queue));
}

TEST(queue, test_queue_full_queue_is_full)
{
    LONGS_EQUAL(false, queue_is_full(&queue));

    for (int i = 0; i < TEST_QUEUE_ELEMENTS; i++)
    {
        radio_packet_t packet;
        queue_add(&queue, (uint8_t *) &packet);
    }
    LONGS_EQUAL(true, queue_is_full(&queue));

    for (int i = 0; i < 3; i++)
    {
        radio_packet_t in_packet;
        radio_packet_t out_packet;

        queue_add(&queue, (uint8_t *) &in_packet);
        queue_get(&queue, (uint8_t *) &out_packet);
    }

    for (int i = 0; i < TEST_QUEUE_ELEMENTS; i++)
    {
        radio_packet_t packet;
        queue_add(&queue, (uint8_t *) &packet);
    }
    LONGS_EQUAL(true, queue_is_full(&queue));
}

TEST(queue, test_queue_new_succeeds_on_empty_queue)
{
    radio_packet_t * packet = 0;
    LONGS_EQUAL(SUCCESS, queue_new(&queue, (uint8_t **) &packet));

    CHECK(0 != packet);
}

TEST(queue, test_queue_add_succeeds_on_empty_queue)
{
    radio_packet_t packet;
    packet.data[0] = rand()*INT_MAX;
    LONGS_EQUAL(SUCCESS, queue_add(&queue, (uint8_t *) &packet));

    LONGS_EQUAL(0, memcmp(&packet, &queue.elements[0], sizeof(packet)));
}

TEST(queue, test_queue_add_succeeds_on_non_empty_queue)
{
    radio_packet_t packet;
    queue_add(&queue, (uint8_t *) &packet);

    packet.data[0] = rand()*INT_MAX;
    LONGS_EQUAL(SUCCESS, queue_add(&queue, (uint8_t *) &packet));

    LONGS_EQUAL(0, memcmp(&packet, &queue.elements[(queue.tail-1)*queue.element_size], sizeof(packet)));
}

TEST(queue, test_queue_add_fails_on_full_queue)
{
    for (int i = 0; i < TEST_QUEUE_ELEMENTS; i++)
    {
        radio_packet_t packet;
        LONGS_EQUAL(SUCCESS, queue_add(&queue, (uint8_t *) &packet));
    }

    radio_packet_t packet;
    LONGS_EQUAL(ERROR_NO_MEMORY, queue_add(&queue, (uint8_t *) &packet));
}

TEST(queue, test_queue_get_fails_on_empty_queue)
{
    radio_packet_t packet;
    LONGS_EQUAL(ERROR_NOT_FOUND, queue_get(&queue, (uint8_t *) &packet));

}

TEST(queue, test_queue_get_returns_correct_pointer)
{
    radio_packet_t in_packet;
    queue_add(&queue, (uint8_t *) &in_packet);

    radio_packet_t out_packet;
    queue_get(&queue, (uint8_t *) &out_packet);

    LONGS_EQUAL(0, memcmp(&in_packet, &out_packet, sizeof(in_packet)));
}

TEST(queue, test_queue_add_get_packet)
{
    radio_packet_t in_packet;
    in_packet.data[0] = rand()*INT_MAX;
    queue_add(&queue, (uint8_t *) &in_packet);

    radio_packet_t out_packet;
    queue_get(&queue, (uint8_t *) &out_packet);
    
    LONGS_EQUAL(0, memcmp(&in_packet, &out_packet, sizeof(in_packet)));
}
    
TEST(queue, test_queue_is_fifo)
{
    radio_packet_t in_packets[TEST_QUEUE_ELEMENTS];

    for (int i = 0; i < TEST_QUEUE_ELEMENTS; i++)
    {
        in_packets[i].data[0] = rand()*INT_MAX;
        LONGS_EQUAL(SUCCESS, queue_add(&queue, (uint8_t *) &in_packets[i]));
    }

    radio_packet_t out_packets[TEST_QUEUE_ELEMENTS] = {0};
    for (int i = 0; i < TEST_QUEUE_ELEMENTS; i++)
    {
        LONGS_EQUAL(SUCCESS, queue_get(&queue, (uint8_t *) &out_packets[i]));
    }

    LONGS_EQUAL(0, memcmp(&out_packets[0], &in_packets[0], sizeof(in_packets[0])));
    LONGS_EQUAL(0, memcmp(&out_packets[1], &in_packets[1], sizeof(in_packets[1])));
    LONGS_EQUAL(0, memcmp(&out_packets[2], &in_packets[2], sizeof(in_packets[2])));
    LONGS_EQUAL(0, memcmp(&out_packets[3], &in_packets[3], sizeof(in_packets[3])));
    LONGS_EQUAL(0, memcmp(&out_packets[4], &in_packets[4], sizeof(in_packets[4])));
    LONGS_EQUAL(0, memcmp(&out_packets[5], &in_packets[5], sizeof(in_packets[5])));
    LONGS_EQUAL(0, memcmp(&out_packets[6], &in_packets[6], sizeof(in_packets[6])));
}
