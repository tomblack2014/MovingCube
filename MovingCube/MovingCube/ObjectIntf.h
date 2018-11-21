#pragma once

#include "Common\StepTimer.h"

class ObjectIntf
{
public:
	//interface
	virtual void CreateDeviceDependentResources() = 0;
	virtual void ReleaseDeviceDependentResources() = 0;
	virtual void Update(const DX::StepTimer& timer) = 0;
	virtual void Render() = 0;

	// Repositions the sample hologram.
	virtual void PositionHologram(Windows::UI::Input::Spatial::SpatialPointerPose^ pointerPose) = 0;

	virtual void CalMyBytes() = 0;

	virtual void UpdateData(const std::vector<char>& data) = 0;

protected:
	virtual std::vector<char> GetDataBytes() = 0;
};