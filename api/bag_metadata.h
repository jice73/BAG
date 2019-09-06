#ifndef BAG_METADATA_H
#define BAG_METADATA_H

#include "bag_config.h"
#include "bag_dataset.h"
#include "bag_metadatatypes.h"

#include <memory>

namespace H5
{
    class DataSet;
}

namespace BAG
{

class BAG_API Metadata
{
public:
    Metadata();
    explicit Metadata(Dataset &dataset);
    Metadata(const Metadata &other) = delete;
    ~Metadata();

    const BAG_METADATA& getStruct() const;

    void loadFromFile(const std::string &fileName);
    void loadFromBuffer(const std::string &xmlBuffer);

private:

    std::shared_ptr<Dataset> m_pBagDataset;
    BAG_METADATA m_metaStruct;
    std::unique_ptr<H5::DataSet> m_pH5Dataset;
};

}   //namespace BAG

#endif  //BAG_METADATA_H