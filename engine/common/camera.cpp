#include "camera.h"

using namespace DirectX;

namespace engine
{
	c_camera::c_camera(float32 aspect_ratio, float32 fov, float32 near_plane_distance, float32 far_plane_distance) :
		m_view_matrix(),
		m_projection_matrix(),
		m_view_projection_matrix(),
		m_view_matrix_dirty(true),
		m_projection_matrix_dirty(true),
		m_near_plane_distance(near_plane_distance),
		m_far_plane_distance(far_plane_distance),
		m_position(),
		m_up(),
		m_right(),
		m_forward(),
		m_aspect_ratio(aspect_ratio),
		m_fov(fov)
	{
	}

	void c_camera::init()
	{
		m_view_matrix_dirty = true;
		m_projection_matrix_dirty = true;
		update_view_matrix();
		update_projection_matrix();
	}

	void c_camera::update()
	{
		update_view_matrix();
		update_projection_matrix();
	}

	void c_camera::update_projection_matrix()
	{
		if (m_projection_matrix_dirty)
		{
			XMMATRIX projection_matrix = XMMatrixPerspectiveFovRH(m_fov, m_aspect_ratio, m_near_plane_distance, m_far_plane_distance);
			m_projection_matrix_dirty = false;
		}
	}

	void c_camera::update_view_matrix()
	{
		if (m_view_matrix_dirty)
		{
			XMVECTOR position = XMLoadFloat3(&m_position);
			XMVECTOR forward = XMLoadFloat3(&m_forward);
			XMVECTOR up = XMLoadFloat3(&m_up);

			XMMATRIX view_matrix = XMMatrixLookToRH(position, forward, up);
			XMStoreFloat4x4(&m_view_matrix, view_matrix);

			m_view_matrix_dirty = false;
		}
	}

	void c_camera::apply_transform(const XMFLOAT4X4* const transform)
	{
		XMMATRIX transformation = XMLoadFloat4x4(transform);

		XMVECTOR forward = XMLoadFloat3(&m_forward);
		XMVECTOR up = XMLoadFloat3(&m_up);

		forward = XMVector3TransformNormal(forward, transformation);
		forward = XMVector3Normalize(forward);

		up = XMVector3TransformNormal(up, transformation);
		up = XMVector3Normalize(up);

		XMVECTOR right = XMVector3Cross(forward, up);
		up = XMVector3Cross(right, forward);

		XMStoreFloat3(&m_forward, forward);
		XMStoreFloat3(&m_up, up);
		XMStoreFloat3(&m_right, right);

		m_view_matrix_dirty = true;
	}

	void c_camera::set_position(const XMFLOAT3* const position)
	{
		XMVECTOR pos = XMLoadFloat3(position);
		XMStoreFloat3(&m_position, pos);
		m_view_matrix_dirty = true;
	}
}