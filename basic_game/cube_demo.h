#ifndef __BASIC_GAME_CUBE_DEMO_H
#define __BASIC_GAME_CUBE_DEMO_H

#include "renderer/renderer.h"

using namespace DirectX;

struct s_constant_buffer_per_object
{
	XMFLOAT4X4 world_view_projection;
};

class c_cube_demo
{
public:
	c_cube_demo();
	~c_cube_demo();

	void init();
	void update();
	void draw();

private:
	XMFLOAT4X4 m_world_matrix;
	ID3D11VertexShader* m_vertex_shader;
	ID3D11PixelShader* m_pixel_shader;
	ID3D11InputLayout* m_input_layout;
	ID3D11Buffer* m_vertex_buffer;
	ID3D11Buffer* m_index_buffer;
	ID3D11Buffer* m_constant_buffer;

	uint32 m_index_count;
	s_constant_buffer_per_object m_constant_buffer_per_object;

	bool m_update_constant_buffer;
};



#endif