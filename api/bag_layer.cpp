#include "bag_layer.h"
#include "bag_private.h"
#include "bag_metadata.h"
#include "bag_trackinglist.h"

#include <h5cpp.h>

namespace BAG
{

namespace
{

DataType layerType2DataType(LayerType layerType)
{
    switch (layerType)
    {
    case Elevation:
    case Uncertainty:
    case Hypothesis_Strength:
    case Shoal_Elevation:
    case Std_Dev:
    case Average_Elevation:
    case Nominal_Elevation:
        return FLOAT32;

    case Num_Hypotheses:
    case Num_Soundings:
        return UINT32;
    }

    return UNKNOWN;
}

}   //namespace

Layer::Layer(Dataset &dataset, LayerType type, const char* internalPath)
    : m_type(type), m_pBagDataset(dataset.shared_from_this())
{
    H5::H5File& file = dataset.getFile();

    const auto* intPath = internalPath ? internalPath : getInternalPath(type);

    m_pH5Dataset = std::make_unique<H5::DataSet>(file.openDataSet(intPath));
    m_internalTypeId = static_cast<int64_t>(H5Dget_type(m_pH5Dataset->getId()));
}

const char* Layer::getInternalPath(LayerType type)
{
    switch (type)
    {
    case Elevation:
        return ELEVATION_PATH;

    case Uncertainty:
        return UNCERTAINTY_PATH;

    case Hypothesis_Strength:
        return "";

    case Num_Hypotheses:
        return NUM_HYPOTHESES_PATH;

    case Shoal_Elevation:
        return "";

    case Std_Dev:
        return STANDARD_DEV_PATH;

    case Num_Soundings:
        return "";

    case Average_Elevation:
        return AVERAGE_PATH;

    case Nominal_Elevation:
        return NOMINAL_ELEVATION_PATH;
    }

    return nullptr;
}

const char* Layer::getName() const
{
    return nullptr;
}

DataType Layer::getDataType() const
{
    return layerType2DataType(m_type);
}

LayerType Layer::getLayerType() const
{
    return m_type;
}

uint8_t Layer::getElementSize() const
{
    switch (this->getDataType())
    {
    case FLOAT32:
        return sizeof(float);

    case UINT32:
        return sizeof(uint32_t);
    }

    return 0;
}

uint32_t Layer::getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const
{
    const uint32_t numRows = (rowEnd - rowStart) + 1;
    const uint32_t numCols = (columnEnd - columnStart) + 1;
    const uint8_t elementSize = this->getElementSize();

    return numRows * numCols * elementSize;
}

void Layer::getExtents(double &min, double &max) const
{
}

uint8_t* Layer::read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const
{
    uint32_t numRows, numCols;
    m_pBagDataset->getDims(numRows, numCols);

    if (columnEnd >= numCols ||
        rowEnd >= numRows ||
        rowStart > rowEnd || 
        columnStart > columnEnd)
    {
        //Throw
        return nullptr;
    }

    hsize_t	dims[RANK];
    dims[0] = (rowEnd - rowStart) + 1;
    dims[1] = (columnEnd - columnStart) + 1;

    hssize_t offset[RANK];
    offset[0] = rowStart;
    offset[1] = columnStart;

    H5::DataSpace fileSpace(RANK, dims);
    fileSpace.offsetSimple(offset);
    //fileSpace.selectHyperslab (H5S_SELECT_SET, dims, offset);

    if (buffer == nullptr)
    {
        const uint32_t bufferSize = this->getReadBufferSize(rowStart, columnStart, rowEnd, columnEnd);
        buffer = new uint8_t[bufferSize];
    }

    m_pH5Dataset->read(buffer, m_internalTypeId, H5::DataSpace::ALL, fileSpace);

    return buffer;
}

void Layer::write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer)
{
    uint32_t numRows, numCols;
    m_pBagDataset->getDims(numRows, numCols);

    if (columnEnd >= numCols ||
        rowEnd >= numRows ||
        rowStart > rowEnd || 
        columnStart > columnEnd)
    {
        //Throw
        return;
    }

    if (buffer == nullptr)
    {
        //Throw
        return;
    }

    hsize_t	dims[RANK];
    dims[0] = (rowEnd - rowStart) + 1;
    dims[1] = (columnEnd - columnStart) + 1;

    hssize_t offset[RANK];
    offset[0] = rowStart;
    offset[1] = columnStart;

    H5::DataSpace fileSpace(RANK, dims);
    fileSpace.offsetSimple(offset);

    m_pH5Dataset->write(buffer, m_internalTypeId, H5::DataSpace::ALL, fileSpace);
}

}   //namespace BAG