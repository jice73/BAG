#ifndef BAG_TRACKINGLIST_H
#define BAG_TRACKINGLIST_H

#include "bag.h"
#include "bag_config.h"
#include "bag_dataset.h"

#include <vector>

namespace BAG
{

class BAG_API TrackingList
{
public:
    typedef bagTrackingItem value_type;

    TrackingList(Dataset &dataset);
    ~TrackingList();

    std::vector<bagTrackingItem>::iterator begin();
    std::vector<bagTrackingItem>::const_iterator begin() const;
    std::vector<bagTrackingItem>::iterator end();
    std::vector<bagTrackingItem>::const_iterator end() const;
    
protected:
    Dataset &m_dataset;
    std::vector<bagTrackingItem> m_trackingItems;

};

}   //BAG

#endif  //BAG_TRACKINGLIST_H