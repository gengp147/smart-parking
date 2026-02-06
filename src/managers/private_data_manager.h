
#ifndef ACL_DET_YOLO_PRIVATE_DATA_MANAGER_H
#define ACL_DET_YOLO_PRIVATE_DATA_MANAGER_H
#include "vulcan/data_struct.h"
#include "mutex"
#include <memory>

namespace sonli
{

struct PrivateOutputData : public vulcan::BaseData
{
    std::string data;
};

class PrivateDataManager : public vulcan::BaseData
{
    bool enabled = false;
    std::shared_ptr<PrivateOutputData> traceData;

    mutable std::mutex m;
    mutable std::mutex dataLock;
public:
    PrivateDataManager();
    ~PrivateDataManager();

    void
    StoreTraceData(const std::shared_ptr<PrivateOutputData> &data);
    std::shared_ptr<PrivateOutputData>
    LoadTraceData();

    bool
    Enabled1() const;
    void
    SetEnabled(bool e);
};
} // namespace sonli

#endif // ACL_DET_YOLO_PRIVATE_DATA_MANAGER_H
