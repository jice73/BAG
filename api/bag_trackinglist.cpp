#include "bag_trackinglist.h"
#include "bag_metadata.h"
#include "bag_layer.h"

#include <h5cpp.h>

namespace BAG
{

TrackingList::TrackingList(Dataset &dataset)
: m_pBagDataset(dataset.shared_from_this())
{
}

TrackingList::~TrackingList()
{
}

int32_t TrackingList::size() const
{
    return 0;
}

bagTrackingItem TrackingList::operator[] (const int32_t index) const
{
    index;
    return bagTrackingItem();
}

}   //namespace BAG