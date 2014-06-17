#ifndef BAGMETADATA_H
#define BAGMETADATA_H

#include "bag_config.h"
#include "bag_metadata.h"

#include <memory>

namespace BAG
{

class BAG_API Metadata
{
public:
    virtual ~Metadata();

    virtual std::auto_ptr<Metadata> newCopy() const = 0;

    virtual const BAG_METADATA& getStruct() const = 0;

protected:
    Metadata();

};

}   //namespace BAG

#endif  //BAGMETADATA_H