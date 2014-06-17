#ifndef BAGTRACKINGLIST_H
#define BAGTRACKINGLIST_H

#include "bag.h"
#include "bag_config.h"
#include "bagdataset.h"

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

#endif  //BAGTRACKINGLIST_H