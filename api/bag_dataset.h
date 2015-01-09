#ifndef BAG_DATASET_H
#define BAG_DATASET_H

#include "bag_config.h"
#include "bag_types.h"
#include "bag_stdint.h"
#include "bag_fordec.h"

#include <vector>

namespace BAG
{

class BAG_API Dataset
{
public:
    Dataset(const std::string &fileName, OpenMode openMode);
    Dataset(const std::string &fileName, const Metadata &metadata);
    ~Dataset();

    std::vector<LayerType> getLayerTypes() const;

    Layer& addLayer(LayerType type);

    Layer& getLayer(LayerType type);
    const Layer& getLayer(LayerType type) const;

    TrackingList& getTrackingList();
    const TrackingList& getTrackingList() const;

    const Metadata& getMetadata() const;
    
    void gridToGeo(uint32_t row, uint32_t column, double &x, double &y) const;
    void geoToGrid(double x, double y, uint32_t &row, uint32_t &column) const;

    void getDims(uint32_t &numRows, uint32_t &numCols) const;
        
    uint32_t getCompressionLevel() const;
    uint32_t getChunkSize() const;

protected:
    void* getFile();

private:

    Data *m_pData;

    friend class Layer;
    friend class MetadataV2;
    friend class TracklingList;
};

}   //namespace BAG

#endif  //BAG_DATASET_H