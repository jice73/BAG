#include "bag_compoundlayer.h"
#include "bag_metadata.h"
#include "bag_trackinglist.h"

#include <h5cpp.h>

namespace BAG
{

CompoundLayer::Definition::Definition(const std::string& name, BAG_DATA_TYPE type)
    : m_name(name), m_type(type)
{

}

CompoundLayer::Definition::~Definition()
{

}




CompoundLayer::CompoundLayer(Dataset &dataset)
    : Layer(dataset, Compound)
{

}

CompoundLayer::~CompoundLayer()
{

}

uint32_t CompoundLayer::getReadBufferSize(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd) const
{
    return 0;
}

const std::vector<CompoundLayer::Definition>& CompoundLayer::getDefinition() const
{
    return m_definition;
}

uint8_t* CompoundLayer::read(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer) const
{
    return nullptr;
}

void CompoundLayer::write(uint32_t rowStart, uint32_t columnStart, uint32_t rowEnd, uint32_t columnEnd, uint8_t *buffer)
{

}

}