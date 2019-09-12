#include "bag_dataset.h"
#include "bag_trackinglist.h"
#include "bag_layer.h"
#include "bag_interleavedlayer.h"
#include "bag_private.h"
#include "bag_metadata.h"

#include <h5cpp.h>
#include <map>

namespace BAG
{

void Dataset::openDataset(const std::string &fileName, BAG_OPEN_MODE openMode)
{
    m_pFile.reset(new H5::H5File(fileName.c_str(),
        (openMode == BAG_OPEN_READONLY) ? H5F_ACC_RDONLY : H5F_ACC_RDWR));

    const H5::Group bagGroup = m_pFile->openGroup(ROOT_PATH);

    //Read the version.
    const H5::Attribute versionAtt = bagGroup.openAttribute(BAG_VERSION_NAME);
    versionAtt.read(versionAtt.getDataType(), m_version);

    //Create all of our layers.
    for (int i = Elevation; i < Nominal_Elevation; ++i)
    {
        const auto layerType = static_cast<LayerType>(i);
        const char* internalPath = Layer::getInternalPath(layerType);
        if (internalPath == nullptr)
            continue;

        const hid_t id = H5Dopen2(bagGroup.getLocId(), internalPath, H5P_DEFAULT);
        if (id < 0)
            continue;

        H5Dclose(id);

        m_layerMap.emplace(layerType,
            std::unique_ptr<Layer>(new Layer(*this, layerType)));
    }

    //Special case for v1.5 bags.
    if (m_version == "1.5.0")
    {
        hid_t id = H5Dopen2(bagGroup.getLocId(), NODE_GROUP_PATH, H5P_DEFAULT);
        if (id >= 0)
        {
            H5Dclose(id);

            m_layerMap.emplace(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Hypothesis_Strength, NODE)));

            m_layerMap.emplace(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Num_Hypotheses, NODE)));
        }

        id = H5Dopen2(bagGroup.getLocId(), ELEVATION_SOLUTION_GROUP_PATH, H5P_DEFAULT);
        if (id >= 0)
        {
            H5Dclose(id);

            m_layerMap.emplace(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Shoal_Elevation, ELEVATION)));

            m_layerMap.emplace(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Std_Dev, ELEVATION)));

            m_layerMap.emplace(Hypothesis_Strength,
                std::unique_ptr<Layer>(new InterleavedLayer(*this, Num_Soundings, ELEVATION)));
        }
    }

    //Create the correct metadata depending on the version.
    m_pMetadata.reset(new Metadata(*this));

    //Create the tracking list.
    m_pTrackingList.reset(new TrackingList(*this));
}

void Dataset::createDataset(const std::string &fileName, const Metadata &metadata)
{
    m_pFile.reset(new H5::H5File(fileName.c_str(), H5F_ACC_RDWR));

    //TODO - implement
}

std::shared_ptr<Dataset> Dataset::create(const std::string &fileName, OpenMode openMode)
{
    std::shared_ptr<Dataset> pRet(new Dataset());
    pRet->openDataset(fileName, openMode);
    return pRet;
}

std::shared_ptr<Dataset> Dataset::create(const std::string &fileName, const Metadata &metadata)
{
    std::shared_ptr<Dataset> pRet(new Dataset());
    pRet->createDataset(fileName, metadata);
    return pRet;
}

void Dataset::getDims(uint32_t &numRows, uint32_t &numCols) const
{
    //Get the elevation HD5 dataset.
    H5::DataSet elevationDataset = H5::DataSet(m_pFile->openDataSet(Layer::getInternalPath(Elevation)));

    const H5::DataSpace space = elevationDataset.getSpace();
    if (!space.isSimple())
    {
        //throw
    }

    hsize_t size[RANK];
    const int nDimsRank = space.getSimpleExtentNdims();
    const int dimsRank = space.getSimpleExtentDims(size, nullptr);

    if (nDimsRank != RANK || dimsRank != RANK)
    {
        //throw
    }

    numRows = static_cast<uint32_t>(size[0]);
    numCols = static_cast<uint32_t>(size[1]);
}

uint32_t Dataset::getCompressionLevel() const
{
    //Get the elevation HD5 dataset.
    H5::DataSet elevationDataset = H5::DataSet(m_pFile->openDataSet(Layer::getInternalPath(Elevation)));

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
    //Get the elevation HD5 dataset.
    H5::DataSet elevationDataset = H5::DataSet(m_pFile->openDataSet(Layer::getInternalPath(Elevation)));

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
    std::vector<LayerType> typeList;

    for (auto&& layer : m_layerMap)
        typeList.push_back(layer.first);

    return typeList;
}
   
Layer& Dataset::getLayer(LayerType type)
{
    auto iter = m_layerMap.find(type);
    if (iter == m_layerMap.end())
    {
        //throw
    }

    return *(*iter).second;
}

const Layer& Dataset::getLayer(LayerType type) const
{
    auto iter = m_layerMap.find(type);
    if (iter == m_layerMap.end())
    {
        //throw
    }

    return *(*iter).second;
}

Layer& Dataset::addLayer(LayerType type)
{
    //Make sure it doesn't already exist.
    auto iter = m_layerMap.find(type);
    if (iter != m_layerMap.end())
    {
        //throw
    }

    //Create the new layer...

    m_layerMap.insert(std::make_pair(type,
        std::unique_ptr<Layer>(new Layer(*this, type))));

    return *m_layerMap[type];
}

void Dataset::gridToGeo(uint32_t row, uint32_t column, double &x, double &y) const
{
    x = m_pMetadata->getStruct().spatialRepresentationInfo->llCornerX +
        (row * m_pMetadata->getStruct().spatialRepresentationInfo->rowResolution);
    y = m_pMetadata->getStruct().spatialRepresentationInfo->llCornerY +
        (column * m_pMetadata->getStruct().spatialRepresentationInfo->columnResolution);
}

void Dataset::geoToGrid(double x, double y, uint32_t &row, uint32_t &column) const
{
}

H5::H5File& Dataset::getFile()
{
    return *m_pFile;
}

TrackingList& Dataset::getTrackingList()
{
    return *m_pTrackingList;
}

const TrackingList& Dataset::getTrackingList() const
{
    return *m_pTrackingList;
}

const Metadata& Dataset::getMetadata() const
{
    return *m_pMetadata;
}

}   //namespace BAG
