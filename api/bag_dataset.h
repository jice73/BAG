#ifndef BAG_DATASET_H
#define BAG_DATASET_H

#include "bag_config.h"
#include "bag_types.h"
#include "bag_stdint.h"
#include "bag_fordec.h"

#ifdef _MSC_VER
//#pragma warning(push, 3)
#pragma warning(disable: 4251)
#endif

#include <vector>
#include <memory>
#include <string>
#include <map>

#ifdef _MSC_VER
//#pragma warning(pop)
#endif

namespace H5
{
    class H5File;
}

namespace BAG
{

class BAG_API Dataset : public std::enable_shared_from_this<Dataset>
{
public:

    static std::shared_ptr<Dataset> create(const std::string &fileName, OpenMode openMode);
    static std::shared_ptr<Dataset> create(const std::string &fileName, const Metadata &metadata);
    ~Dataset() = default;

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

    H5::H5File& getFile();

private:

    Dataset() = default;

    void openDataset(const std::string &fileName, OpenMode openMode);
    void createDataset(const std::string &fileName, const Metadata &metadata);

    std::string m_version;
    std::unique_ptr<H5::H5File> m_pFile;
    std::map<LayerType, std::unique_ptr<Layer> > m_layerMap;
    std::unique_ptr<TrackingList> m_pTrackingList;
    std::unique_ptr<Metadata> m_pMetadata;

    friend class Layer;
    friend class Metadata;
    friend class TracklingList;
};

}   //namespace BAG

#endif  //BAG_DATASET_H