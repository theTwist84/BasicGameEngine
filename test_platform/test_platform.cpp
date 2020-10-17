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


    s_data_array* array = create_new_data_array("test_array", 16, sizeof(test_datum));


    datum_handle handles[3];

    for (int i = 0; i < 3; i++)
    {
        datum_handle handle = datum_new(array);

        test_datum* data = (test_datum*)datum_get(array, handle);

        data->new_value_1 = 0xCDCDCDCDCDCDCDCDu;
        data->new_value_2 = 0xCECEu;
        data->new_value_3 = 0xCFCFCFCFu;
        data->new_value_4 = 0xCCu;

        handles[i] = handle;
    }

    datum_delete(array, handles[1]);
    datum_delete(array, handles[2]);

    // std::cout << "TOTAL UPDATES: " << total_updates << std::endl;

    dispose_data_array(array);

    return 0;
}
