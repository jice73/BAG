#ifndef BAGINTERLEAVEDLAYER
#define BAGINTERLEAVEDLAYER

#include "baglayer.h"
#include "bagtypes.h"

namespace BAG
{

class InterleavedLayer : public Layer
{
public:
    
    InterleavedLayer(Dataset &dataset, LayerType type, GroupType groupType);
    ~InterleavedLayer();

    GroupType getGroupType() const;
    uint8_t getComplexElementSize() const;
    uint32_t getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const;

    virtual uint8_t* read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const;
    virtual void write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer);

private:

    GroupType m_groupType;
};

}   //namespace BAG

#endif  //BAGINTERLEAVEDLAYER