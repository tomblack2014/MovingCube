#pragma once

#include "NamedObj.h"
#include "Model.h"
#include <vector>

namespace MovingCube
{
    // This sample renderer instantiates a basic rendering pipeline.
    class SpinningCubeRenderer : public NamedObj
    {
    public:
        SpinningCubeRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		SpinningCubeRenderer(SpinningCubeRenderer&);
        void CreateDeviceDependentResources() override;
        void ReleaseDeviceDependentResources() override;
        void Update(const DX::StepTimer& timer) override;
        void Render() override;

        // Repositions the sample hologram.
        void PositionHologram(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose) override;

        // Property accessors.
        //void SetPosition(Windows::Foundation::Numerics::float3 pos) { m_position = pos;  }
		//Windows::Foundation::Numerics::float3 GetPosition()         { return m_position; }
		void SetV_I(const std::vector<VertexPositionColor>& vertics, const std::vector<unsigned short>& indices) { m_vertices = vertics; m_indices = indices; };

		void SetModel(const std::shared_ptr<Model>& model) { m_vertices = model->GetVertices(); m_indices = model->GetIndices(); };

		bool GetNewPos() override;

		void UpdatePos(Windows::Foundation::Numerics::float3& pos) { if (m_moveStat == 0) SetPosition(pos); };

		int GetStat() override { return m_moveStat; };

		void CalMyBytes() override;

		void UpdateData(const std::vector<char>& data) override;

	protected:
		std::vector<char> GetDataBytes() override;

		int GetType() override { return 1; };

		void UpdateBinding(std::shared_ptr<NamedObj> root) override;

    private:
		void SetGlobalPos(Windows::Foundation::Numerics::float3 pos) { if (m_parent) SetPosition(pos - m_parent->GetGlobalPos()); else SetPosition(pos); };

        // Cached pointer to device resources.
        std::shared_ptr<DX::DeviceResources>            m_deviceResources;

        // Direct3D resources for cube geometry.
        Microsoft::WRL::ComPtr<ID3D11InputLayout>       m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_indexBuffer;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>      m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>    m_geometryShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>       m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>            m_modelConstantBuffer;

        // System resources for cube geometry.
        ModelConstantBuffer                             m_modelConstantBufferData;
        uint32                                          m_indexCount = 0;

        // Variables used with the rendering loop.
        bool                                            m_loadingComplete = false;
        float                                           m_degreesPerSecond = 45.f;

        // If the current D3D Device supports VPRT, we can avoid using a geometry
        // shader just to set the render target array index.
        bool                                            m_usingVprtShaders = false;

		//moving flag
		int												m_moveStat = 0;
		Windows::Foundation::Numerics::float3           m_pointerPos = { 0.f, 0.f, 0.f };
		Windows::Foundation::Numerics::float3           m_targetPos = { 0.f, 0.f, 0.f };
		double											m_moveTime = -0.1;

		//model data
		std::vector<VertexPositionColor>				m_vertices;
		std::vector<unsigned short>						m_indices;
		int												m_modelID;
		int												m_lastModelID;

		//upload data
		Windows::Foundation::Numerics::float3			m_newPos;
		bool											m_isNewPos;
    };
}
