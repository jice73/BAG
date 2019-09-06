#ifndef BAG_TRACKINGLIST_H
#define BAG_TRACKINGLIST_H

#include "bag_config.h"
#include "bag_dataset.h"
#include "bag_types.h"

#include <memory>

namespace BAG
{

class BAG_API TrackingList
{
public:
    typedef bagTrackingItem value_type;

    TrackingList(Dataset &dataset);
    ~TrackingList();

    int32_t size() const;
    bagTrackingItem operator[] (const int32_t index) const;
    
protected:
    std::shared_ptr<Dataset> m_pBagDataset;
};

}   //BAG

#endif  //BAG_TRACKINGLIST_H