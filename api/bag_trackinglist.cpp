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

std::vector<bagTrackingItem>::iterator TrackingList::begin()
{
    return this->m_trackingItems.begin();
}

std::vector<bagTrackingItem>::const_iterator TrackingList::begin() const
{
    return this->m_trackingItems.begin();
}

std::vector<bagTrackingItem>::iterator TrackingList::end()
{
    return this->m_trackingItems.end();
}

std::vector<bagTrackingItem>::const_iterator TrackingList::end() const
{
    return this->m_trackingItems.end();
}

}   //namespace BAG