#include "pch.h"
#include "SpinningCubeRenderer.h"
#include "Common\DirectXHelper.h"

using namespace MovingCube;
using namespace Concurrency;
using namespace DirectX;
using namespace Windows::Foundation::Numerics;
using namespace Windows::UI::Input::Spatial;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
SpinningCubeRenderer::SpinningCubeRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
}

// This function uses a SpatialPointerPose to position the world-locked hologram
// two meters in front of the user's heading.
void SpinningCubeRenderer::PositionHologram(SpatialPointerPose^ pointerPose)
{
	//if head->cube | headDirection angle < 10 degree then means it's gazed
	const float3 headPosition = pointerPose->Head->Position;
	const float3 headDirection = pointerPose->Head->ForwardDirection;
	float3 head2cube = GetPosition() - headPosition;

	if (m_moveStat == 0) {
		auto dot = [](float3 v1, float3 v2)->float {
			return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
		};
		auto dis = [](float3 v)->float {
			return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		};

		const float threshold = 6 * 3.141592653 / 180;
		float ang = acos(dot(headDirection, head2cube) / dis(headDirection) / dis(head2cube));
		if (ang < threshold) {
			m_moveStat = 1;
		}
	}
	else if (m_moveStat == 1) {
		constexpr float distanceFromUser = 2.0f; // meters
		const float3 gazeAtTwoMeters = headPosition + (distanceFromUser * headDirection);
		auto pointerPos = gazeAtTwoMeters;
		m_targetPos = pointerPos;
		m_moveStat = 2;
	}

	return;
}

// Called once per frame. Rotates the cube, and calculates and sets the model matrix
// relative to the position transform indicated by hologramPositionTransform.
void SpinningCubeRenderer::Update(const DX::StepTimer& timer)
{
	// Rotate the cube.
	// Convert degrees to radians, then convert seconds to rotation angle.
	const float    radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
	const double   totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
	const float    radians = static_cast<float>(fmod(totalRotation, XM_2PI));
	XMMATRIX modelRotation = XMMatrixRotationY(-radians);
	if (m_moveStat == 1)
		modelRotation = XMMatrixIdentity();

	
	//calculate the movement of cube if it's moving
	if (m_moveStat == 2) {
		if (m_moveTime < 0) {
			m_moveTime = timer.GetTotalSeconds();
		}
		else {
			const float moveSpeed = 0.3f;
			double time = timer.GetTotalSeconds() - m_moveTime;
			m_moveTime += time;
			auto position = GetPosition();

			//calculate distance
			float3 vec = m_targetPos - position;
			double dis = sqrtf((vec.x) * (vec.x) + (vec.y) * (vec.y) + (vec.z) * (vec.z));
			double moveDis = moveSpeed * time;
			if (dis < moveDis) {
				SetPosition(m_targetPos);
				m_moveStat = 0;
				m_moveTime = -0.1;
			}
			else {
				SetPosition(position + vec * moveDis / dis);
			}
		}
	}

	// Position the cube.
	const XMMATRIX modelTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&GetPosition()));

	// Multiply to get the transform matrix.
	// Note that this transform does not enforce a particular coordinate system. The calling
	// class is responsible for rendering this content in a consistent manner.
	const XMMATRIX modelTransform = XMMatrixMultiply(modelRotation, modelTranslation);

	// The view and projection matrices are provided by the system; they are associated
	// with holographic cameras, and updated on a per-camera basis.
	// Here, we provide the model transform for the sample hologram. The model transform
	// matrix is transposed to prepare it for the shader.
	XMStoreFloat4x4(&m_modelConstantBufferData.model, XMMatrixTranspose(modelTransform));

	// Loading is asynchronous. Resources must be created before they can be updated.
	if (!m_loadingComplete)
	{
		return;
	}

	// Use the D3D device context to update Direct3D device-based resources.
	const auto context = m_deviceResources->GetD3DDeviceContext();

	// Update the model transform buffer for the hologram.
	context->UpdateSubresource(
		m_modelConstantBuffer.Get(),
		0,
		nullptr,
		&m_modelConstantBufferData,
		0,
		0
	);
}

// Renders one frame using the vertex and pixel shaders.
// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
// a pass-through geometry shader is also used to set the render 
// target array index.
void SpinningCubeRenderer::Render()
{
	// Loading is asynchronous. Resources must be created before drawing can occur.
	if (!m_loadingComplete)
	{
		return;
	}

	const auto context = m_deviceResources->GetD3DDeviceContext();

	// Each vertex is one instance of the VertexPositionColor struct.
	const UINT stride = sizeof(VertexPositionColor);
	const UINT offset = 0;
	context->IASetVertexBuffers(
		0,
		1,
		m_vertexBuffer.GetAddressOf(),
		&stride,
		&offset
	);
	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	// Attach the vertex shader.
	context->VSSetShader(
		m_vertexShader.Get(),
		nullptr,
		0
	);
	// Apply the model constant buffer to the vertex shader.
	context->VSSetConstantBuffers(
		0,
		1,
		m_modelConstantBuffer.GetAddressOf()
	);

	if (!m_usingVprtShaders)
	{
		// On devices that do not support the D3D11_FEATURE_D3D11_OPTIONS3::
		// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature,
		// a pass-through geometry shader is used to set the render target 
		// array index.
		context->GSSetShader(
			m_geometryShader.Get(),
			nullptr,
			0
		);
	}

	// Attach the pixel shader.
	context->PSSetShader(
		m_pixelShader.Get(),
		nullptr,
		0
	);

	// Draw the objects.
	context->DrawIndexedInstanced(
		m_indexCount,   // Index count per instance.
		2,              // Instance count.
		0,              // Start index location.
		0,              // Base vertex location.
		0               // Start instance location.
	);
}

void SpinningCubeRenderer::CreateDeviceDependentResources()
{
	m_usingVprtShaders = m_deviceResources->GetDeviceSupportsVprt();

	// On devices that do support the D3D11_FEATURE_D3D11_OPTIONS3::
	// VPAndRTArrayIndexFromAnyShaderFeedingRasterizer optional feature
	// we can avoid using a pass-through geometry shader to set the render
	// target array index, thus avoiding any overhead that would be 
	// incurred by setting the geometry shader stage.
	std::wstring vertexShaderFileName = m_usingVprtShaders ? L"ms-appx:///VprtVertexShader.cso" : L"ms-appx:///VertexShader.cso";

	// Load shaders asynchronously.
	task<std::vector<byte>> loadVSTask = DX::ReadDataAsync(vertexShaderFileName);
	task<std::vector<byte>> loadPSTask = DX::ReadDataAsync(L"ms-appx:///PixelShader.cso");

	task<std::vector<byte>> loadGSTask;
	if (!m_usingVprtShaders)
	{
		// Load the pass-through geometry shader.
		loadGSTask = DX::ReadDataAsync(L"ms-appx:///GeometryShader.cso");
	}

	// After the vertex shader file is loaded, create the shader and input layout.
	task<void> createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				fileData.data(),
				fileData.size(),
				nullptr,
				&m_vertexShader
			)
		);

		constexpr std::array<D3D11_INPUT_ELEMENT_DESC, 2> vertexDesc =
		{ {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		} };

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc.data(),
				static_cast<UINT>(vertexDesc.size()),
				fileData.data(),
				static_cast<UINT>(fileData.size()),
				&m_inputLayout
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	task<void> createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				fileData.data(),
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		const CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_modelConstantBuffer
			)
		);
	});

	task<void> createGSTask;
	if (!m_usingVprtShaders)
	{
		// After the pass-through geometry shader file is loaded, create the shader.
		createGSTask = loadGSTask.then([this](const std::vector<byte>& fileData)
		{
			DX::ThrowIfFailed(
				m_deviceResources->GetD3DDevice()->CreateGeometryShader(
					fileData.data(),
					fileData.size(),
					nullptr,
					&m_geometryShader
				)
			);
		});
	}

	// Once all shaders are loaded, create the mesh.
	task<void> shaderTaskGroup = m_usingVprtShaders ? (createPSTask && createVSTask) : (createPSTask && createVSTask && createGSTask);
	task<void> createCubeTask = shaderTaskGroup.then([this]()
	{
		// Load mesh vertices. Each vertex has a position and a color.
		// Note that the cube size has changed from the default DirectX app
		// template. Windows Holographic is scaled in meters, so to draw the
		// cube at a comfortable size we made the cube width 0.2 m (20 cm).
		/*static const std::array<VertexPositionColor, 8> cubeVertices =
		{ {
			{ XMFLOAT3(-0.1f, -0.1f, -0.1f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-0.1f, -0.1f,  0.1f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-0.1f,  0.1f, -0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-0.1f,  0.1f,  0.1f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(0.1f, -0.1f, -0.1f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(0.1f, -0.1f,  0.1f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(0.1f,  0.1f, -0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.1f,  0.1f,  0.1f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		} };*/

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = m_vertices.data();
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(VertexPositionColor) * static_cast<UINT>(m_vertices.size()), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
			)
		);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 2,1,0 means that the vertices with indexes
		// 2, 1, and 0 from the vertex buffer compose the
		// first triangle of this mesh.
		// Note that the winding order is clockwise by default.
		//constexpr std::array<unsigned short, 36> cubeIndices =
		//{ {
		//	2,1,0, // -x
		//	2,3,1,

		//	6,4,5, // +x
		//	6,5,7,

		//	0,1,5, // -y
		//	0,5,4,

		//	2,6,7, // +y
		//	2,7,3,

		//	0,4,6, // -z
		//	0,6,2,

		//	1,3,7, // +z
		//	1,7,5,
		//} };

		m_indexCount = static_cast<unsigned int>(m_indices.size());

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = m_indices.data();
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned short) * static_cast<UINT>(m_indices.size()), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
			)
		);
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]()
	{
		m_loadingComplete = true;
	});
}

void SpinningCubeRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_usingVprtShaders = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_geometryShader.Reset();
	m_modelConstantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}
