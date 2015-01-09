#include "bag_trackinglist.h"

namespace BAG
{

TrackingList::TrackingList(Dataset &dataset)
: m_dataset(dataset)
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