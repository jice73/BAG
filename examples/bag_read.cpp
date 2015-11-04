#include "bag_dataset.h"
#include "bag_layer.h"

#include <stdio.h>
#include <iostream>

int ProcessCommandInput(int argc, char **argv, char *gisFile, char *xmlFile, char *bagFile, int *summaryOnly);

int main(int argc, char **argv)
{
    using namespace BAG;

    Dataset dataset("D:/BathyWork/openns/trunk/examples/sample-data/sample.bag", BAG_OPEN_READONLY);

    uint32_t numRows, numCols;
    dataset.getDims(numRows, numCols);

    double minX, minY, maxX, maxY;
    dataset.gridToGeo(0, 0, minX, minY);
    dataset.gridToGeo(numRows - 1, numCols - 1, maxX, maxY);
    std::cout << minX << " " << minY << " " << maxX << " " << maxY;

    const std::vector<LayerType> layerTypes = dataset.getLayerTypes();
    for (auto type : layerTypes)
    {
        const Layer &layer = dataset.getLayer(type);

        const char* name = layer.getName();
        DataType dataType = layer.getDataType();
        LayerType layerType = layer.getLayerType();

        const uint32_t bufferSize = layer.getReadBufferSize(0, 0, numRows - 1, numCols - 1);

        uint8_t *buffer = new uint8_t[bufferSize];
        layer.read(0, 0, numRows - 1, numCols - 1, buffer);
        
    }

    return 0;
}
