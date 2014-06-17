#ifndef BAGDATASET_H
#define BAGDATASET_H

#include "bag_config.h"
#include "bagtypes.h"
#include "bagstdint.h"
#include "bagfordec.h"

#include <string>
#include <vector>
#include <map>

namespace BAG
{

class BAG_API Dataset
{
public:
    explicit Dataset(const std::string &fileName);
    Dataset(const Metadata &metadata, const std::string &fileName);
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
    friend class Metadata2;
    friend class TracklingList;
};

}   //namespace BAG

#endif  //BAGDATASET_H