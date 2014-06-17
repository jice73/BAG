#ifndef BAGMETADATA2_H
#define BAGMETADATA2_H

#include "bag_config.h"
#include "bagmetadata.h"
#include "bagdataset.h"

namespace BAG
{

//! Version 2 of the BAG metadata.
class BAG_API Metadata2 : public Metadata
{
public:
    explicit Metadata2(const std::string &xmlBuffer);
    explicit Metadata2(Dataset &dataset);
    explicit Metadata2(const Metadata2 &other);
    virtual ~Metadata2();

    virtual std::auto_ptr<Metadata> newCopy() const;

    virtual const BAG_METADATA& getStruct() const;

private:
    
    Data *m_pData;
};

}   //namespace BAG

#endif  //BAGMETADATA2_H