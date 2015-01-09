#include "bag_metadatav2.h"
#include "bag_private.h"

#include <h5cpp.h>

namespace BAG
{

namespace
{

struct MetadataData : public Data
{
    MetadataData(Dataset &dataset)
        : m_bagDataset(dataset)
    {
    }

    Dataset &m_bagDataset;
    BAG_METADATA m_metaStruct;
    std::auto_ptr<H5::DataSet> m_pH5Dataset;
};

}   //namespace

MetadataV2::MetadataV2()
{
}

MetadataV2::MetadataV2(Dataset &dataset)
: m_pData(NULL)
{
    H5::H5File *pFile = reinterpret_cast<H5::H5File *>(dataset.getFile());

    MetadataData *pData = new MetadataData(dataset);
    pData->m_pH5Dataset.reset(new H5::DataSet(pFile->openDataSet(METADATA_PATH)));
    m_pData = pData;
/*
    if (((* bag_handle)->mta_datatype_id = H5Dget_type((* bag_handle)->mta_dataset_id)) < 0)
        return BAG_HDF_TYPE_NOT_FOUND;

    //Open the filespace for the case where we're reading into a NULL \a metadata
    (* bag_handle)->mta_filespace_id = H5Dget_space((* bag_handle)->mta_dataset_id);
    if ((* bag_handle)->mta_filespace_id < 0)
    {
        return BAG_HDF_DATASPACE_CORRUPTED;
    }
    if (((* bag_handle)->mta_cparms_id = H5Dget_create_plist ((* bag_handle)->mta_dataset_id)) < 0)
        return BAG_HDF_CREATE_PROPERTY_CLASS_FAILURE;
    
    if (H5D_CHUNKED == H5Pget_layout ((* bag_handle)->mta_cparms_id))
    {
        rank_chunk = H5Pget_chunk ((* bag_handle)->mta_cparms_id, 1, chunk_dimsr);
    }

    (* bag_handle)->bag.metadata = (u8 *)calloc (XML_METADATA_MAX_LENGTH, sizeof (u8));
	
    if ((* bag_handle)->bag.metadata == (u8 *) NULL)
    {
        return (BAG_MEMORY_ALLOCATION_FAILED);
    }
*/
}

MetadataV2::~MetadataV2()
{
}

void MetadataV2::loadFromFile(const std::string &fileName)
{
}

void MetadataV2::loadFromBuffer(const std::string &xmlBuffer)
{
}

const BAG_METADATA& MetadataV2::getStruct() const
{
    MetadataData *pData = dynamic_cast<MetadataData *>(this->m_pData);
    return pData->m_metaStruct;
}

}   //namespace BAG