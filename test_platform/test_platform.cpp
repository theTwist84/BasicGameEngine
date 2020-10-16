#include <iostream>
#include "engine_debug.h"
#include <cmath>
#include <random>
#include "memory/data_array.h"

using namespace engine;

int main()
{
    s_data_array* array = create_new_data_array("test_array", 16, sizeof(int64));

    datum_handle handle = array->datum_new();

    int64* data = (int64*)array->datum_get(handle);
    *data = 0xCDCD;

    // std::cout << "TOTAL UPDATES: " << total_updates << std::endl;

    dispose_data_array(array);

    return 0;
}
