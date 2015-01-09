#ifndef BAG_METADATAV2_H
#define BAG_METADATAV2_H

#include "bag_config.h"
#include "bag_metadata.h"
#include "bag_dataset.h"

namespace BAG
{

//! Version 2 of the BAG metadata.
class BAG_API MetadataV2 : public Metadata
{
public:
    MetadataV2();
    explicit MetadataV2(Dataset &dataset);
    virtual ~MetadataV2();

    virtual const BAG_METADATA& getStruct() const;

    void loadFromFile(const std::string &fileName);
    void loadFromBuffer(const std::string &xmlBuffer);

private:
    
    MetadataV2(const MetadataV2 &other);
    Data *m_pData;
};

}   //namespace BAG

#endif  //BAG_METADATAV2_H