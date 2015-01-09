#ifndef BAG_TRACKINGLIST_H
#define BAG_TRACKINGLIST_H

#include "bag_config.h"
#include "bag_dataset.h"
#include "bag_types.h"

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
    Dataset &m_dataset;
};

}   //BAG

#endif  //BAG_TRACKINGLIST_H