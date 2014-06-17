#include "bag_interleavedlayer.h"
#include "bag.h"
#include "bag_private.h"

namespace BAG
{

InterleavedLayer::InterleavedLayer(Dataset &dataset, LayerType type, GroupType groupType)
: Layer(dataset, type, (groupType == NODE) ? NODE_GROUP_PATH : ELEVATION_SOLUTION_GROUP_PATH),
  m_groupType(groupType)
{
}

InterleavedLayer::~InterleavedLayer()
{
}

GroupType InterleavedLayer::getGroupType() const
{
    return this->m_groupType;
}

uint8_t InterleavedLayer::getComplexElementSize() const
{
    switch (this->m_groupType)
    {
    case NODE:
        return sizeof(float) + sizeof(uint32_t);

    case ELEVATION:
        return sizeof(float) + sizeof(float) + sizeof(uint32_t);
    }

    return 0;
}

uint32_t InterleavedLayer::getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const
{
    const uint32_t numRows = (rowEnd - rowStart) + 1;
    const uint32_t numCols = (columnEnd - columnStart) + 1;
    const uint8_t elementSize = this->getComplexElementSize();

    return numRows * numCols * elementSize;
}

uint8_t* InterleavedLayer::read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const
{
    //Read the complex element.
    uint8_t *complexBuffer = NULL;
    complexBuffer = Layer::read(rowStart, columnStart, rowEnd, columnEnd, complexBuffer);

    //If the output buffer is not initialized, do it now.
    if (buffer == NULL)
    {
        const uint32_t bufferSize = Layer::getReadBufferSize(rowStart, columnStart, rowEnd, columnEnd);
        buffer = new uint8_t[bufferSize];
    }

    const LayerType type = this->getLayerType();

    const uint32_t numRows = (rowEnd - rowStart) + 1;
    const uint32_t numCols = (columnEnd - columnStart) + 1;

    const uint32_t sizeOfSourceRow = numCols * this->getComplexElementSize();
    const uint32_t sizeOfDestRow = numCols * this->getElementSize();

    for (uint32_t row = 0; row < numRows; row++)
    {
        const uint32_t sourceRowOffset = sizeOfSourceRow * row;
        const uint32_t destRowOffset = sizeOfDestRow * row;

        for (uint32_t col = 0; col < numCols; col++)
        {
            if (this->m_groupType == NODE)
            {
                const bagOptNodeGroup *rowOfSourceData = (const bagOptNodeGroup *)complexBuffer[sourceRowOffset];

                if (type == Hypothesis_Strength)
                {
                    float *rowOfDestData = (float*)buffer[destRowOffset];
                    rowOfDestData[col] = rowOfSourceData[col].hyp_strength;
                }
                else if (type == Num_Hypotheses)
                {
                    uint32_t *rowOfDestData = (uint32_t*)buffer[destRowOffset];
                    rowOfDestData[col] = rowOfSourceData[col].num_hypotheses;
                }
            }
            else
            {
                const bagOptElevationSolutionGroup *rowOfSourceData = (const bagOptElevationSolutionGroup *)complexBuffer[sourceRowOffset];

                if (type == Shoal_Elevation)
                {
                    float *rowOfDestData = (float*)buffer[destRowOffset];
                    rowOfDestData[col] = rowOfSourceData[col].shoal_elevation;
                }
                else if (type == Std_Dev)
                {
                    float *rowOfDestData = (float*)buffer[destRowOffset];
                    rowOfDestData[col] = rowOfSourceData[col].stddev;
                }
                else if (type == Num_Soundings)
                {
                    uint32_t *rowOfDestData = (uint32_t*)buffer[destRowOffset];
                    rowOfDestData[col] = rowOfSourceData[col].num_soundings;
                }
            }
        }
    }

    return buffer;
}

void InterleavedLayer::write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer)
{
    //Throw
}

}   //namespace BAG