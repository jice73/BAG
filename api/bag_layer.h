#ifndef BAG_LAYER_H
#define BAG_LAYER_H

#include "bag_config.h"
#include "bag_types.h"
#include "bag_dataset.h"

#include <memory>

namespace H5
{
    class DataSet;
}

namespace BAG
{

class BAG_API Layer
{
public:

    virtual ~Layer() = default;

    const char* getName() const;
    DataType getDataType() const;
    LayerType getLayerType() const;

    void getExtents(double &min, double &max) const;

    virtual uint8_t getElementSize() const;
    virtual uint32_t getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const;

    virtual uint8_t* read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const;
    virtual void write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer);

    static const char* getInternalPath(LayerType type);

protected:

    Layer(Dataset &dataset, LayerType type, const char* internalPath = nullptr);

private:

    int64_t m_internalTypeId;
    LayerType m_type;
    std::shared_ptr<Dataset> m_pBagDataset;
    std::unique_ptr<H5::DataSet> m_pH5Dataset;

    friend class Dataset;
};

}   //namespace BAG

#endif  //BAG_LAYER_H