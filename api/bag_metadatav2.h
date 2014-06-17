#ifndef BAG_METADATAV2_H
#define BAG_METADATAV2_H

#include "bag_config.h"
#include "bag_metadatatypes.h"
#include "bag_dataset.h"

namespace BAG
{

//! Version 2 of the BAG metadata.
class BAG_API MetadataV2 : public Metadata
{
public:
    explicit MetadataV2(const std::string &xmlBuffer);
    explicit MetadataV2(Dataset &dataset);
    explicit MetadataV2(const MetadataV2 &other);
    virtual ~MetadataV2();

    virtual std::auto_ptr<Metadata> newCopy() const;

    virtual const BAG_METADATA& getStruct() const;

private:
    
    Data *m_pData;
};

}   //namespace BAG

#endif  //BAG_METADATAV2_H