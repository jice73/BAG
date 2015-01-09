#ifndef BAG_TYPES_H
#define BAG_TYPES_H

#include "bag_c_types.h"

namespace BAG
{
    typedef BAG_LAYER_TYPE LayerType;
    typedef BAG_DATA_TYPE DataType;
    typedef BAG_OPEN_MODE OpenMode;
    typedef BAG_GROUP_TYPE GroupType;

    typedef bagTrackingItem TrackingItem;

    struct Data
    {
        Data(){};
        virtual ~Data(){};
    };

}   //namespace BAG

#endif  //BAG_TYPES_H