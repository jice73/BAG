#include "bag_metadata.h"
#include "bag_private.h"
#include "bag_metadata_import.h"

#include <h5cpp.h>
#include <memory>

namespace BAG
{

namespace
{

struct MetadataData : public Data
{
    MetadataData(Dataset &dataset)
        : m_bagDataset(dataset)
    {
        bagInitMetadata(&this->m_metaStruct);
    }

    Dataset &m_bagDataset;
    BAG_METADATA m_metaStruct;
    std::unique_ptr<H5::DataSet> m_pH5Dataset;
};

}   //namespace

Metadata::Metadata()
{
}

Metadata::Metadata(Dataset &dataset)
    : m_pData(NULL)
{
    H5::H5File *pFile = reinterpret_cast<H5::H5File *>(dataset.getFile());

    MetadataData *pData = new MetadataData(dataset);
    pData->m_pH5Dataset.reset(new H5::DataSet(pFile->openDataSet(METADATA_PATH)));
    m_pData = pData;

    const size_t memorySize = pData->m_pH5Dataset->getInMemDataSize();

    H5std_string buffer;

    const H5::StrType stringType(*pData->m_pH5Dataset);
    pData->m_pH5Dataset->read(buffer, stringType);

    this->loadFromBuffer(buffer);
}

Metadata::~Metadata()
{
}

void Metadata::loadFromFile(const std::string &fileName)
{
    MetadataData *pData = dynamic_cast<MetadataData *>(this->m_pData);
    bagError err = bagImportMetadataFromXmlFile(fileName.c_str(), &pData->m_metaStruct, false);
    err;
}

void Metadata::loadFromBuffer(const std::string &xmlBuffer)
{
    MetadataData *pData = dynamic_cast<MetadataData *>(this->m_pData);
    bagError err = bagImportMetadataFromXmlBuffer(xmlBuffer.c_str(), (uint32_t)xmlBuffer.size(), &pData->m_metaStruct, false);
    err;
}

const BAG_METADATA& Metadata::getStruct() const
{
    MetadataData *pData = dynamic_cast<MetadataData *>(this->m_pData);
    return pData->m_metaStruct;
}

}   //namespace BAG