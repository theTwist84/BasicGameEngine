#include <iostream>
#include "engine_debug.h"
#include <cmath>
#include <random>
#include "memory/data_array.h"

using namespace engine;

int main()
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




    // std::cout << "TOTAL UPDATES: " << total_updates << std::endl;
    array->dispose_data_array();

    return 0;
}
