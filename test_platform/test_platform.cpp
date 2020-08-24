#include <iostream>
#include "engine_debug.h"
#include "time/engine_clock.h"
#include <chrono>

using namespace engine;
int main()
{
    c_engine_time time = c_engine_time();
    c_engine_clock clock = c_engine_clock();

    for (int i = 0; i < 100; i++)
    {
        clock.reset();
        auto start = std::chrono::high_resolution_clock::now();
        Sleep(10);
        // order matters, they are mostly the same values but it takes time to get either of them
        auto end = std::chrono::high_resolution_clock::now();
        clock.update_engine_time(&time);

        std::chrono::duration<double> elapsed_seconds = (end - start);

        auto time_diff = time.elapsed_time() - 1000 * elapsed_seconds.count();

        //clock.reset();  // don't reset clock here, it takes about 100 us to 200 us to go to the next loop which desyncs the values
        printf("ETimer: %f C++ timer: %f time diff: %f\n", time.elapsed_time(), 1000 * elapsed_seconds.count(), time_diff);
    }
    

    
    return 0;
}

