#include "camera.h"
#include "../input/input_manager.h"
#include "../globals/engine_globals.h"
#include "../debug_graphics/debug_graphics.h"
#include <math.h>

using namespace DirectX;

namespace engine
{
	/// <summary>
	/// Compute the perspective transformation matrix from FOV (x axis), aspect ratio, near plane and far plane.
	/// Near plane and far plane can be reversed for inversing the z-order.
	/// Matrix is computed for Right Hand side coordinate system.
	/// </summary>
	/// <param name="fov"> FOV on the X axis, in radians</param>
	/// <param name="aspect_ratio"> width / height, e.g. 16/9 for 16:9 aspect ratio.</param>
	/// <param name="near_plane">Z distance of near plane</param>
	/// <param name="far_plane">Z distance of far plane</param>
	/// <returns></returns>
	XMMATRIX compute_perspective_matrix_fov_RH(float32 fov, float32 aspect_ratio, float32 near_plane, float32 far_plane)
	{
		assert(fov > 0.f && fov < g_XMTwoPi[0]);
		float32 fov_y = 2.0f * std::asinf(0.5f * aspect_ratio * std::sinf(fov * 0.5f));
		return XMMatrixPerspectiveFovRH(fov_y, aspect_ratio, near_plane, far_plane);
	}

	c_camera::c_camera(float32 aspect_ratio, float32 fov, float32 near_plane_distance, float32 far_plane_distance) :
		m_view_matrix(),
		m_projection_matrix(),
		m_view_projection_matrix(),
		m_view_matrix_dirty(true),
		m_projection_matrix_dirty(true),
		m_near_plane_distance(near_plane_distance),
		m_far_plane_distance(far_plane_distance),
		m_position(0.0f, 0.0f, 0.0f),
		m_up(0.0f, 1.0f, 0.0f),
		m_right(1.0f, 0.0f, -1.0f),
		m_forward(-1.0f, -1.0f, -1.0f),
		m_aspect_ratio(aspect_ratio),
		m_fov(fov)
	{
		m_view_projection_matrix_updated = true;
	}

	void c_camera::init()
	{
		m_view_matrix_dirty = true;
		m_projection_matrix_dirty = true;
		// todo: add lookat 0 0 0  RH when we have cube geometry

		update_projection_matrix();

		XMFLOAT3 lookat = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		XMMATRIX lookat_matrix = XMMatrixLookAtRH(XMLoadFloat3(&m_position), XMLoadFloat3(&lookat), XMLoadFloat3(&up));
		XMStoreFloat4x4(&m_view_matrix, lookat_matrix);
		XMMATRIX view_projection_matrix = XMMatrixMultiply(lookat_matrix, XMLoadFloat4x4(&m_projection_matrix));
		XMStoreFloat4x4(&m_view_projection_matrix, view_projection_matrix);
		m_view_matrix_dirty = false;
	}

	void c_camera::update()
	{
		if (m_view_matrix_dirty || m_projection_matrix_dirty)
			m_view_projection_matrix_updated = true;
		else
			m_view_projection_matrix_updated = false;

		update_view_matrix();
		update_projection_matrix();

		

		get_debug_graphics()->print_string(D2D1::Point2F(10.0f, 40.0f), D2D1::ColorF(D2D1::ColorF::White), L"Camera Position x %.3f y %.3f z %.3f ", m_position.x, m_position.y, m_position.z );
		get_debug_graphics()->print_string(D2D1::Point2F(10.0f, 56.0f), D2D1::ColorF(D2D1::ColorF::White), L"Camera Forward  x %.3f y %.3f z %.3f ", m_forward.x, m_forward.y, m_forward.z);
		get_debug_graphics()->print_string(D2D1::Point2F(10.0f, 72.0f), D2D1::ColorF(D2D1::ColorF::White), L"Camera Up       x %.3f y %.3f z %.3f ", m_up.x, m_up.y, m_up.z);
		get_debug_graphics()->print_string(D2D1::Point2F(10.0f, 88.0f), D2D1::ColorF(D2D1::ColorF::White), L"Camera Right    x %.3f y %.3f z %.3f ", m_right.x, m_right.y, m_right.z);
	}

	void c_camera::update_projection_matrix()
	{
		if (m_projection_matrix_dirty)
		{
			XMMATRIX projection_matrix = compute_perspective_matrix_fov_RH(m_fov, m_aspect_ratio, m_near_plane_distance, m_far_plane_distance);
			XMStoreFloat4x4(&m_projection_matrix, projection_matrix);
			XMMATRIX view_projection_matrix = XMMatrixMultiply(XMLoadFloat4x4(&m_view_matrix), projection_matrix);
			XMStoreFloat4x4(&m_view_projection_matrix, view_projection_matrix);
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
			XMMATRIX view_projection_matrix = XMMatrixMultiply(view_matrix, XMLoadFloat4x4(&m_projection_matrix));
			XMStoreFloat4x4(&m_view_projection_matrix, view_projection_matrix);
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

	void c_camera::apply_input_transformation()
	{
		auto input_manager = get_engine_globals()->input_manager;

		float y = -(float)input_manager->mouse()->y() * 0.01f;	// pitch
		float x = -(float)input_manager->mouse()->x() * 0.01f;	// yaw

		float translation_x = 0.0f;
		float translation_z = 0.0f;

		if (input_manager->keyboard()->is_key_down(e_keyboard_keys::_W))
			translation_x += 0.1f;

		if (input_manager->keyboard()->is_key_down(e_keyboard_keys::_S))
			translation_x += -0.1f;

		if (input_manager->keyboard()->is_key_down(e_keyboard_keys::_A))
			translation_z += -0.1f;

		if (input_manager->keyboard()->is_key_down(e_keyboard_keys::_D))
			translation_z += 0.1f;


		if (x != 0.0f || y != 0.0f)
		{
			XMVECTOR right = XMLoadFloat3(&m_right);
			XMMATRIX pitch_matrix = XMMatrixRotationAxis(right, y);
			XMMATRIX yaw_matrix = XMMatrixRotationY(x);
			XMMATRIX transform = XMMatrixMultiply(pitch_matrix, yaw_matrix);

			XMFLOAT4X4 rotation;
			XMStoreFloat4x4(&rotation, transform);
			apply_transform(&rotation);
		}

		if (translation_x != 0 || translation_z != 0)
		{
			XMVECTOR forward = XMLoadFloat3(&m_forward);
			XMVECTOR right = XMLoadFloat3(&m_right);
			XMVECTOR current_position = XMLoadFloat3(&m_position);

			XMVECTOR new_position = translation_x * forward + translation_z * right + current_position;
			XMStoreFloat3(&m_position, new_position);
			m_view_matrix_dirty = true;
		}
	}
}