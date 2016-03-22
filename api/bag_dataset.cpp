#include "bag_dataset.h"
#include "bag_trackinglist.h"
#include "bag_layer.h"
#include "bag_interleavedlayer.h"
#include "bag_private.h"
#include "bag_metadata.h"

#include <src/h5cpp.h>
#include <memory>
#include <map>

namespace BAG
{

namespace
{

struct DatasetData : public Data
{
    std::string m_version;
    std::unique_ptr<H5::H5File> m_pFile;
    std::map<LayerType, std::unique_ptr<Layer> > m_layerMap;
    std::unique_ptr<TrackingList> m_pTrackingList;
    std::unique_ptr<Metadata> m_pMetadata;
};

}   //namespace
    

Dataset::Dataset(const std::string &fileName, BAG_OPEN_MODE openMode)
{
    DatasetData *pData = new DatasetData();
    pData->m_pFile.reset(new H5::H5File(fileName.c_str(),
        (openMode == BAG_OPEN_READONLY) ? H5F_ACC_RDONLY : H5F_ACC_RDWR));

    this->m_pData = pData;

    std::unique_ptr<H5::Group> pBagGroup(new H5::Group(pData->m_pFile->openGroup(ROOT_PATH)));

    //Read the version.
    H5::Attribute verstionAtt = pBagGroup->openAttribute(BAG_VERSION_NAME);
    verstionAtt.read(H5T_C_S1, pData->m_version);

    //Create all of our layers.
    for (int i = Elevation; i < Nominal_Elevation; ++i)
    {
        const char* internalPath = Layer::getInternalPath(static_cast<LayerType>(i));
        if (internalPath == NULL)
            continue;

        hid_t id = H5Dopen2( pBagGroup->getLocId(), internalPath, H5P_DEFAULT );
        if (id >= 0)
        {
            H5Dclose(id);
        
            pData->m_layerMap.insert(std::make_pair(static_cast<LayerType>(i),
                std::unique_ptr<Layer>(new Layer(*this, static_cast<LayerType>(i)))));
        }
    }

    //Special case for v1.5 bags.
    {
        hid_t id = H5Dopen2( pBagGroup->getLocId(), NODE_GROUP_PATH, H5P_DEFAULT );
        if (id >= 0)
        {
            H5Dclose(id);

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Hypothesis_Strength, NODE))));

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Num_Hypotheses, NODE))));
        }

        id = H5Dopen2( pBagGroup->getLocId(), ELEVATION_SOLUTION_GROUP_PATH, H5P_DEFAULT );
        if (id >= 0)
        {
            H5Dclose(id);

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Shoal_Elevation, ELEVATION))));

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Std_Dev, ELEVATION))));

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Num_Soundings, ELEVATION))));
        }
    }

    //Create the correct metadata depending on the version.
    pData->m_pMetadata.reset(new Metadata(*this));

    //Create the tracking list.
    pData->m_pTrackingList.reset(new TrackingList(*this));
}

Dataset::Dataset(const std::string &fileName, const Metadata &metadata)
{
    DatasetData *pData = new DatasetData();
    pData->m_pFile.reset(new H5::H5File(fileName.c_str(), H5F_ACC_RDWR));
    this->m_pData = pData;

    //TODO - implement
}

Dataset::~Dataset()
{
    if (this->m_pData != NULL)
        delete this->m_pData;
}

void Dataset::getDims(uint32_t &numRows, uint32_t &numCols) const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    //Get the elevation HD5 dataset.
    H5::DataSet elevationDataset = H5::DataSet(pData->m_pFile->openDataSet(Layer::getInternalPath(Elevation)));

    const H5::DataSpace space = elevationDataset.getSpace();
    if (!space.isSimple())
    {
        //throw
    }

    hsize_t size[RANK];
    const int nDimsRank = space.getSimpleExtentNdims();
    const int dimsRank = space.getSimpleExtentDims(size, NULL);

    if (nDimsRank != RANK || dimsRank != RANK)
    {
        //throw
    }

    numRows = static_cast<uint32_t>(size[0]);
    numCols = static_cast<uint32_t>(size[1]);
}

uint32_t Dataset::getCompressionLevel() const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    //Get the elevation HD5 dataset.
    H5::DataSet elevationDataset = H5::DataSet(pData->m_pFile->openDataSet(Layer::getInternalPath(Elevation)));

    const H5::DSetCreatPropList createPList = elevationDataset.getCreatePlist();

    for (int i = 0; i < createPList.getNfilters(); i++)
    {
        unsigned int flags = 0;
        size_t cd_nelmts = 10, namelen = 64;
        unsigned int cd_values[10];
        char name[64];
        unsigned int filter_config = 0;

        const H5Z_filter_t filter = createPList.getFilter(i, flags, cd_nelmts, cd_values, namelen, name, filter_config);
        if (filter == H5Z_FILTER_DEFLATE)
        {
            if (cd_nelmts >= 1)
                return cd_values[0];
        }
    }

    return 0;
}

uint32_t Dataset::getChunkSize() const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    //Get the elevation HD5 dataset.
    H5::DataSet elevationDataset = H5::DataSet(pData->m_pFile->openDataSet(Layer::getInternalPath(Elevation)));

    const H5::DSetCreatPropList createPList = elevationDataset.getCreatePlist();

    if (createPList.getLayout() == H5D_CHUNKED)
    {
        hsize_t max_dims[RANK];

        const int rank_chunk = createPList.getChunk(RANK, max_dims);
        if (rank_chunk == RANK)
            return static_cast<uint32_t>(max_dims[0]);
    }

    return 0;
}

std::vector<LayerType> Dataset::getLayerTypes() const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    std::vector<LayerType> typeList;

    for (auto iter = pData->m_layerMap.begin();
        iter != pData->m_layerMap.end();
        iter++)
    {
        typeList.push_back((*iter).first);
    }

    return typeList;
}
   
Layer& Dataset::getLayer(LayerType type)
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    auto iter = pData->m_layerMap.find(type);
    if (iter == pData->m_layerMap.end())
    {
        //throw
    }

    return *(*iter).second;
}

const Layer& Dataset::getLayer(LayerType type) const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    auto iter = pData->m_layerMap.find(type);
    if (iter == pData->m_layerMap.end())
    {
        //throw
    }

    return *(*iter).second;
}

Layer& Dataset::addLayer(LayerType type)
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    //Make sure it doesn't already exist.
    auto iter = pData->m_layerMap.find(type);
    if (iter != pData->m_layerMap.end())
    {
        //throw
    }

    //Create the new layer...

    pData->m_layerMap.insert(std::make_pair(type,
        std::unique_ptr<Layer>(new Layer(*this, type))));

    return *pData->m_layerMap[type];
}

void Dataset::gridToGeo(uint32_t row, uint32_t column, double &x, double &y) const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    x = pData->m_pMetadata->getStruct().spatialRepresentationInfo->llCornerX +
        (row * pData->m_pMetadata->getStruct().spatialRepresentationInfo->rowResolution);
    y = pData->m_pMetadata->getStruct().spatialRepresentationInfo->llCornerY +
        (column * pData->m_pMetadata->getStruct().spatialRepresentationInfo->columnResolution);
}

void Dataset::geoToGrid(double x, double y, uint32_t &row, uint32_t &column) const
{
}

void* Dataset::getFile()
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);
    return pData->m_pFile.get();
}

}   //namespace BAG