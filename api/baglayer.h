#ifndef BAGLAYER_H
#define BAGLAYER_H

#include "bag_config.h"
#include "bagtypes.h"
#include "bagdataset.h"

#include <string>

namespace BAG
{

class BAG_API Layer
{
public:

    Layer(Dataset &dataset, LayerType type);
    virtual ~Layer();

    const char* getName() const;
    DataType getDataType() const;
    LayerType getLayerType() const;

    virtual uint8_t getElementSize() const;
    virtual uint32_t getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const;

    virtual uint8_t* read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const;
    virtual void write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer);

    static const char* getInternalPath(LayerType type);

protected:
    
    Layer(Dataset &dataset, LayerType type, const char* internalPath);

private:
    
    Data *m_pData;

    friend class Dataset;
};

}   //namespace BAG

#endif  //BAGLAYER_H