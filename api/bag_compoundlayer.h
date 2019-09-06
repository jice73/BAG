#ifndef BAG_COMPOUNDLAYER
#define BAG_COMPOUNDLAYER

#include "bag_layer.h"
#include "bag_types.h"

#include <string>

namespace BAG
{

class CompoundLayer : public Layer
{
public:

    class Definition
    {
    public:
        Definition(const std::string& name, BAG_DATA_TYPE type);
        ~Definition();

        std::string m_name;
        BAG_DATA_TYPE m_type;
    };

    CompoundLayer(Dataset &dataset);
    ~CompoundLayer();

    uint32_t getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const override;
    const std::vector<Definition>& getDefinition() const;

    uint8_t* read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const override;
    void write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) override;

private:

    std::vector<Definition> m_definition;
};

}   //namespace BAG

#endif  //BAG_COMPOUNDLAYER
