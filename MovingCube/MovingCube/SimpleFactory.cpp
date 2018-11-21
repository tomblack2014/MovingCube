#include "pch.h"
#include "SimpleFactory.h"
#include "NamedObj.h"
#include "Content\SpinningCubeRenderer.h"
#include "Root.h"
#include "Model.h"

SimpleFactory::SimpleFactory(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	m_deviceResources = deviceResources;
}

std::shared_ptr<NamedObj> SimpleFactory::createProduct(int type)
{
	std::shared_ptr<NamedObj> ret;
	if (type == 0) {
		ret = std::make_shared<Root>();
	}
	else if (type == 1) {
		ret = std::make_shared<MovingCube::SpinningCubeRenderer>(m_deviceResources);
	}
	else if (type == 2){
		ret = std::make_shared<Model>();
	}
	else {
		throw "";
	}

	return ret;
}