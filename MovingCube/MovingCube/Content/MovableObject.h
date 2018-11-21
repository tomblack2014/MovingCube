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
		~MovableObject() {};

		//interface
		virtual void CreateDeviceDependentResources() {};
		virtual void ReleaseDeviceDependentResources() {};
		virtual void Update(const DX::StepTimer& timer) {};
		virtual void Render() {};

		// Repositions the sample hologram.
		virtual void PositionHologram(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose) {};

		virtual bool GetNewPos() { return false; };

		virtual int GetStat() { return 0; };

		// Property accessors.
		void SetPosition(Windows::Foundation::Numerics::float3 pos) { m_position = pos; }
		Windows::Foundation::Numerics::float3 GetPosition() { return m_position; }

	private:
		Windows::Foundation::Numerics::float3           m_position = { 0.f, 0.f, -2.f };
	};
}