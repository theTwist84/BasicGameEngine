#include <iostream>
#include "engine_debug.h"
#include <cmath>
#include <random>
#include "data_structures/data_array.h"
#include "data_structures/queue.h"

using namespace engine;

void data_array_test()
{
    struct test_datum : s_datum_header
    {
        int64 new_value_1;
        int16 new_value_2;
        int32 new_value_3;
        int8 new_value_4;
    };


    c_data_array<test_datum>* array = create_new_data_array<test_datum>("test_array", 16);

    datum_handle handles[3];

    for (int i = 0; i < 3; i++)
    {
        datum_handle handle = array->datum_new();

        test_datum* data = array->datum_get(handle);

        data->new_value_1 = 0xCDCDCDCDCDCDCDCDu;
        data->new_value_2 = 0xCECEu;
        data->new_value_3 = 0xCFCFCFCFu;
        data->new_value_4 = 0xCCu;

        handles[i] = handle;
    }

    array->datum_delete(handles[1]);
    array->datum_delete(handles[2]);


    c_data_array_iterator<test_datum> test_iterator(array);

    array->dispose_data_array();

    
}


int main()
{
    c_queue<int64>* queue = create_new_queue<int64>(10);

    auto is_empty = queue->is_empty();

    for (int i = 0; i < 10; i++)
    {
        int64 val = i;
        queue->put(&val);
    }

    auto is_full = queue->is_full();

    int64 front = *queue->front();
    queue->pop();

    int64 second = *queue->front();



    int64 back = *queue->back();


    for (int i = 0; i < 9; i++)
    {
        queue->pop();
    }
    

    is_empty &= queue->is_empty();


    if (is_empty && is_full && front == 0 && second == 1 && back == 9)
    {
        printf("Queue test succeeded!");
    }


    return 0;
}
