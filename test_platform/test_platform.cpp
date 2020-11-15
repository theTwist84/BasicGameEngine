#include <iostream>
#include "engine_debug.h"
#include <cmath>
#include <random>
#include "data_structures/data_array.h"
#include "data_structures/queue.h"
#include "IO/config.h"
#include "IO/utils.h"
#include <d3dcompiler.h>
#include <fstream>

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


    c_data_array<test_datum>* array = create_new_data_array<test_datum>("test_array", 16, g_heap_allocator());

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

void data_queue_test()
{
    c_queue<int64>* queue = create_new_queue<int64>(10, g_heap_allocator());

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


}

void config_test()
{
    s_config default_config;
    default_config.window_mode = 1;
    default_config.resolution_height = 1000;
    default_config.resolution_width = 1000;


    s_config config;
    config.window_mode = 0;
    config.resolution_height = 100;
    config.resolution_width = 200;

    auto dir = executable_directory();
    std::wstring file_path;
    path_join(file_path, dir, L"config.json");

    s_config input_config;

    if (read_config(file_path, &input_config, &default_config))
        printf("Sucessfully read config.");
    else
        printf("Failed to read config.");

    if (write_config(file_path, &config))
        printf("Sucessfully wrote config.");
    else
        printf("Failed to write config.");

    if (read_config(file_path, &config, &default_config))
        printf("Sucessfully read config.");
    else
        printf("Failed to read config.");
}

HRESULT compiler_shader(const wchar_t* source_file, const char* entry_point, const char* profile, const wchar_t* out_file)
{
    if(!source_file || !entry_point || !profile)
        return E_INVALIDARG;

    ID3DBlob* blob;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    flags |= D3DCOMPILE_DEBUG;
#endif

    const D3D_SHADER_MACRO defines[] =
    {
        "EXAMPLE_DEFINE", "1",
        NULL, NULL
    };

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(source_file, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, profile, flags, 0, &shaderBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return hr;
    }
    else
    {

        if (shaderBlob)
        {
            std::ofstream f(out_file, std::ios::binary);
            f.write(static_cast<char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize());
            f.close();
            shaderBlob->Release();
        }
            
    }


    return hr;
}

int shader_compiler_test()
{
    // Compile vertex shader shader

    auto dir = executable_directory();
    std::wstring file_path_vs;
    std::wstring file_path_vs_out;
    path_join(file_path_vs_out, dir, L"content\\vs.cso");
    path_join(file_path_vs, dir, L"content\\vs.hlsl");

    HRESULT hr = compiler_shader(file_path_vs.c_str(), "main", "vs_4_0", file_path_vs_out.c_str());
    if (FAILED(hr))
    {
        printf("Failed compiling vertex shader %08X\n", hr);
        return -1;
    }
    std::wstring file_path_ps;
    path_join(file_path_ps, dir, L"content\\ps.hlsl");
    std::wstring file_path_ps_out;
    path_join(file_path_ps_out, dir, L"content\\ps.cso");

    // Compile pixel shader shader
    hr = compiler_shader(file_path_ps.c_str(), "main", "ps_4_0", file_path_ps_out.c_str());
    if (FAILED(hr))
    {
        printf("Failed compiling pixel shader %08X\n", hr);
        return -1;
    }

    printf("Success\n");

    return 0;
}

int main()
{
    return 0;
}
