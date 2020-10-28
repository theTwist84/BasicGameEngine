#ifndef __ENGINE_COMMON_CAMERA_H
#define __ENGINE_COMMON_CAMERA_H

#include "game_object.h"
#include <DirectXMath.h>
#include "../engine_definitions.h"

using namespace DirectX;

namespace engine
{
	class c_camera : c_game_object
	{
	public:
		c_camera(float32 aspect_ratio, float32 fov, float32 near_plane_distance, float32 far_plane_distance);

		void init();
		void update();

		void apply_transform(const XMFLOAT4X4* const transform);
		void set_position(const XMFLOAT3* const position);

	private:
		XMFLOAT4X4 m_view_matrix;
		XMFLOAT4X4 m_projection_matrix;
		XMFLOAT4X4 m_view_projection_matrix;

		bool m_view_matrix_dirty;
		bool m_projection_matrix_dirty;

		float32 m_near_plane_distance;
		float32 m_far_plane_distance;
		float32 m_fov;
		float32 m_aspect_ratio;


		XMFLOAT3 m_position;
		XMFLOAT3 m_up;
		XMFLOAT3 m_right;
		XMFLOAT3 m_forward;

		void update_projection_matrix();
		void update_view_matrix();



	};
}


#endif