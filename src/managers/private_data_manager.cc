

#include "private_data_manager.h"
#include <vector>

namespace sonli
{

}
sonli::PrivateDataManager::PrivateDataManager()
{
    name_ = "private_data_config";
}

sonli::PrivateDataManager::~PrivateDataManager() {}

void
sonli::PrivateDataManager::StoreTraceData(const std::shared_ptr<PrivateOutputData> &data)
{
    std::lock_guard<std::mutex> lock(dataLock);
    this->traceData = data;
}

std::shared_ptr<sonli::PrivateOutputData>
sonli::PrivateDataManager::LoadTraceData()
{
    std::lock_guard<std::mutex> lock(dataLock);
    return this->traceData;
}
bool
sonli::PrivateDataManager::Enabled1() const
{
    std::lock_guard<std::mutex> lock(m);
    return enabled;
}
void
sonli::PrivateDataManager::SetEnabled(bool e)
{
    std::lock_guard<std::mutex> lock(m);
    this->enabled = e;
}
