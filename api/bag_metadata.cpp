#include "bag_metadata.h"
#include "bag_private.h"
#include "bag_metadata_import.h"
#include "bag_trackinglist.h"
#include "bag_layer.h"

#include <h5cpp.h>

namespace BAG
{

Metadata::Metadata()
{
    bagError err = bagInitMetadata(&m_metaStruct);
    err;
}

Metadata::~Metadata()
{
    bagFreeMetadata(&m_metaStruct);
}

Metadata::Metadata(Dataset &dataset)
    : m_pBagDataset(dataset.shared_from_this())
{
    bagError err = bagInitMetadata(&m_metaStruct);
    err;

    H5::H5File& file = dataset.getFile();
    m_pH5Dataset.reset(new H5::DataSet(file.openDataSet(METADATA_PATH)));

    H5std_string buffer;

    const H5::StrType stringType(*m_pH5Dataset);
    m_pH5Dataset->read(buffer, stringType);

    this->loadFromBuffer(buffer);
}

void Metadata::loadFromFile(const std::string &fileName)
{
    bagError err = bagImportMetadataFromXmlFile(fileName.c_str(), &m_metaStruct, false);
    err;
}

void Metadata::loadFromBuffer(const std::string &xmlBuffer)
{
    bagError err = bagImportMetadataFromXmlBuffer(xmlBuffer.c_str(), (uint32_t)xmlBuffer.size(), &m_metaStruct, false);
    err;
}

const BAG_METADATA& Metadata::getStruct() const
{
    return m_metaStruct;
}

}   //namespace BAG