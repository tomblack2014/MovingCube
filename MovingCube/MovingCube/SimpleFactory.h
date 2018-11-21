#pragma once

#include "Common\DirectXHelper.h"
#include "Common\DeviceResources.h"
#include <memory>

class NamedObj;

class SimpleFactory
{
public:
	SimpleFactory(const std::shared_ptr<DX::DeviceResources>& deviceResources);

	std::shared_ptr<NamedObj> createProduct(int type);

private:
	std::shared_ptr<DX::DeviceResources> m_deviceResources;
};