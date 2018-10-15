#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "ShaderStructures.h"

namespace MovingCube
{
	//base class of a movable object
	class MovableObject
	{
	public:
		//interface
		virtual void CreateDeviceDependentResources() PURE;
		virtual void ReleaseDeviceDependentResources() PURE;
		virtual void Update(const DX::StepTimer& timer) PURE;
		virtual void Render() PURE;

		// Repositions the sample hologram.
		virtual void PositionHologram(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose) PURE;

		// Property accessors.
		void SetPosition(Windows::Foundation::Numerics::float3 pos) { m_position = pos; }
		Windows::Foundation::Numerics::float3 GetPosition() { return m_position; }

	private:
		Windows::Foundation::Numerics::float3           m_position = { 0.f, 0.f, -2.f };
	};
}