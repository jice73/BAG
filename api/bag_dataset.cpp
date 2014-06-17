#include "bag_dataset.h"
#include "bag_trackinglist.h"
#include "bag_layer.h"
#include "bag_interleavedlayer.h"
#include "bag_private.h"

#include <h5cpp.h>

namespace BAG
{

namespace
{

struct DatasetData : public Data
{
    ~DatasetData()
    {
        //Delete all of our layers.
        for (std::map<LayerType, Layer*>::iterator iter = this->m_layerMap.begin();
            iter != this->m_layerMap.end();
            iter++)
        {
            delete (*iter).second;
        }

        if (this->m_pFile.get())
            this->m_pFile->close();
    }

    std::string m_version;
    std::auto_ptr<H5::H5File> m_pFile;
    std::map<LayerType, Layer*> m_layerMap;
    std::auto_ptr<TrackingList> m_pTrackingList;
};

}   //namespace
    

Dataset::Dataset(const std::string &fileName)
{
    DatasetData *pData = new DatasetData();
    pData->m_pFile.reset(new H5::H5File(fileName.c_str(), H5F_ACC_RDONLY));
    this->m_pData = pData;

    std::auto_ptr<H5::Group> pBagGroup(new H5::Group(pData->m_pFile->openGroup(ROOT_PATH)));

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
        
            Layer *pLayer = new Layer(*this, static_cast<LayerType>(i));
            pData->m_layerMap.insert(std::make_pair(static_cast<LayerType>(i), pLayer));
        }
    }

    //Special case for v1.5 bags.
    {
        hid_t id = H5Dopen2( pBagGroup->getLocId(), NODE_GROUP_PATH, H5P_DEFAULT );
        if (id >= 0)
        {
            H5Dclose(id);

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength, new InterleavedLayer(*this, Hypothesis_Strength, NODE)));
            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength, new InterleavedLayer(*this, Num_Hypotheses, NODE)));
        }

        id = H5Dopen2( pBagGroup->getLocId(), ELEVATION_SOLUTION_GROUP_PATH, H5P_DEFAULT );
        if (id >= 0)
        {
            H5Dclose(id);

            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength, new InterleavedLayer(*this, Shoal_Elevation, ELEVATION)));
            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength, new InterleavedLayer(*this, Std_Dev, ELEVATION)));
            pData->m_layerMap.insert(std::make_pair(Hypothesis_Strength, new InterleavedLayer(*this, Num_Soundings, ELEVATION)));            
        }
    }

    //Create the tracking list.
    pData->m_pTrackingList.reset(new TrackingList(*this));
}

Dataset::Dataset(const Metadata &metadata, const std::string &fileName)
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

    for (std::map<LayerType, Layer*>::const_iterator iter = pData->m_layerMap.begin();
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

    std::map<LayerType, Layer*>::const_iterator iter = pData->m_layerMap.find(type);
    if (iter == pData->m_layerMap.end())
    {
        //throw
    }

    return *(*iter).second;
}

const Layer& Dataset::getLayer(LayerType type) const
{
    DatasetData *pData = dynamic_cast<DatasetData *>(this->m_pData);

    std::map<LayerType, Layer*>::const_iterator iter = pData->m_layerMap.find(type);
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
    std::map<LayerType, Layer*>::const_iterator iter = pData->m_layerMap.find(type);
    if (iter != pData->m_layerMap.end())
    {
        //throw
    }

    //TODO - create the layer

    Layer *pNewLayer = new Layer(*this, type);
    pData->m_layerMap.insert(std::make_pair(type, pNewLayer));

    return *pNewLayer;
}

void Dataset::gridToGeo(uint32_t row, uint32_t column, double &x, double &y) const
{
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