#include "cube_demo.h"

#include <iostream>
#include "engine_debug.h"
#include "time/engine_time.h"
#include "time/engine_clock.h"
#include "debug_ui/fps_display.h"
#include "globals/engine_globals.h"
#include "debug_graphics/debug_graphics.h"
#include "input/input_manager.h"
#include "hidusage.h"
#include "IO/config.h"
#include "IO/utils.h"
#include "common/camera.h"
#include <vector>

using namespace engine;
using namespace std;

class vertex_position_color
{
private:
	inline static const D3D11_INPUT_ELEMENT_DESC _InputElements[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

public:
	vertex_position_color() = default;

	vertex_position_color(const XMFLOAT4 & position, const XMFLOAT4 & color) :
		Position(position), Color(color)
	{
	}

	XMFLOAT4 Position;
	XMFLOAT4 Color;

	static void CreateVertexBuffer(ID3D11Device* device, const vertex_position_color* const vertices, int32 vertex_count, ID3D11Buffer** vertexBuffer)
	{
		D3D11_BUFFER_DESC vertexBufferDesc{ 0 };
		vertexBufferDesc.ByteWidth = vertex_count * sizeof(vertex_position_color);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData{ 0 };
		vertexSubResourceData.pSysMem = &vertices[0];
		if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer)))
			debug_printf("Failed to create vertex buffer");
	}
};

c_cube_demo::c_cube_demo()
{
	auto identiy = XMMatrixIdentity();
	XMStoreFloat4x4(&m_world_matrix, identiy);

	m_vertex_shader = nullptr;
	m_pixel_shader = nullptr;
	m_input_layout = nullptr;
	m_vertex_buffer = nullptr;
	m_index_buffer = nullptr;
	m_constant_buffer = nullptr;

	m_index_count = 0;
	m_update_constant_buffer = true;
}

c_cube_demo::~c_cube_demo()
{
	rendering::release_unknown_object(m_vertex_shader);
	rendering::release_unknown_object(m_pixel_shader);
	rendering::release_unknown_object(m_input_layout);
	rendering::release_unknown_object(m_vertex_buffer);
	rendering::release_unknown_object(m_index_buffer);
	rendering::release_unknown_object(m_constant_buffer);
}

void c_cube_demo::init()
{
	vector<char> compiled_vs;
	vector<char> compiled_ps;
	auto dir = executable_directory();
	std::wstring compiled_vs_path, compiled_ps_path;
	path_join(compiled_vs_path, dir, L"content\\vs.cso");
	path_join(compiled_ps_path, dir, L"content\\ps.cso");

	load_binary_file(compiled_vs_path, &compiled_vs);
	load_binary_file(compiled_ps_path, &compiled_ps);

	auto globals = get_engine_globals();
	auto d3d_device = globals->renderer->d3d_device();

	if (FAILED(d3d_device->CreateVertexShader(&compiled_vs[0], compiled_vs.size(), nullptr, &m_vertex_shader)))
		debug_printf("Failed to create vertex shader!");

	if (FAILED(d3d_device->CreatePixelShader(&compiled_ps[0], compiled_ps.size(), nullptr, &m_pixel_shader)))
		debug_printf("Failed to create pixel shader!");

	const D3D11_INPUT_ELEMENT_DESC input_element_description[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(d3d_device->CreateInputLayout(input_element_description, (uint32)size(input_element_description), &compiled_vs[0], compiled_vs.size(), &m_input_layout)))
		debug_printf("Failed to create input layout!");


	// Create a vertex buffer
	const vertex_position_color vertices[] =
	{
		vertex_position_color(XMFLOAT4(-1.0f, +1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)),
		vertex_position_color(XMFLOAT4(+1.0f, +1.0f, -1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f)),
		vertex_position_color(XMFLOAT4(+1.0f, +1.0f, +1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
		vertex_position_color(XMFLOAT4(-1.0f, +1.0f, +1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f)),

		vertex_position_color(XMFLOAT4(-1.0f, -1.0f, +1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)),
		vertex_position_color(XMFLOAT4(+1.0f, -1.0f, +1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 0.5f, 1.0f)),
		vertex_position_color(XMFLOAT4(+1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)),
		vertex_position_color(XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
	};

	vertex_position_color::CreateVertexBuffer(d3d_device, &vertices[0], 8, &m_vertex_buffer);

	// Create an index buffer
	const uint16_t indices[] =
	{
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		3, 2, 5,
		3, 5, 4,

		2, 1, 6,
		2, 6, 5,

		1, 7, 6,
		1, 0, 7,

		0, 3, 4,
		0, 4, 7
	};

	m_index_count = 36;

	D3D11_BUFFER_DESC indexBufferDesc{ 0 };
	indexBufferDesc.ByteWidth = sizeof(uint16_t) * m_index_count;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubResourceData{ 0 };
	indexSubResourceData.pSysMem = indices;
	if (FAILED(d3d_device->CreateBuffer(&indexBufferDesc, &indexSubResourceData, &m_index_buffer)))
		debug_printf("Failed to create index buffer");

	D3D11_BUFFER_DESC constantBufferDesc{ 0 };
	constantBufferDesc.ByteWidth = sizeof(s_constant_buffer_per_object);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(d3d_device->CreateBuffer(&constantBufferDesc, nullptr, &m_constant_buffer)))
		debug_printf("Failed to create constant buffer");

}

void c_cube_demo::update()
{

}

void c_cube_demo::draw()
{
	auto globals = get_engine_globals();
	auto d3d_device = globals->renderer->d3d_device();
	auto d3d_device_context = globals->renderer->d3d_device_context();

	d3d_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3d_device_context->IASetInputLayout(m_input_layout);

	const uint32_t stride = sizeof(vertex_position_color);
	const uint32_t offset = 0;
	d3d_device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
	d3d_device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R16_UINT, 0);

	d3d_device_context->VSSetShader(m_vertex_shader, nullptr, 0);
	d3d_device_context->PSSetShader(m_pixel_shader, nullptr, 0);

	if (m_update_constant_buffer || get_engine_globals()->camera->view_projection_matrix_updated())
	{
		const XMMATRIX worldMatrix = XMLoadFloat4x4(&m_world_matrix);
		XMMATRIX view_projection_matrix = XMLoadFloat4x4(get_engine_globals()->camera->get_view_projection_matrix());
		
		XMMATRIX wvp = worldMatrix * view_projection_matrix;
		wvp = XMMatrixTranspose(wvp);
		XMStoreFloat4x4(&m_constant_buffer_per_object.world_view_projection, wvp);
		d3d_device_context->UpdateSubresource(m_constant_buffer, 0, nullptr, &m_constant_buffer_per_object, 0, 0);
		m_update_constant_buffer = false;
	}

	d3d_device_context->VSSetConstantBuffers(0, 1, &m_constant_buffer);

	d3d_device_context->DrawIndexed(m_index_count, 0, 0);
}