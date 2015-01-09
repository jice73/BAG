#include "bag.h"
#include "bag_dataset.h"
#include "bag_metadatav2.h"
#include "bag_layer.h"

bagError bagFileOpen(bagHandle *handle, BAG_OPEN_MODE accessMode, const char *fileName)
{
    BAG::Dataset *pDataset = new BAG::Dataset(std::string(fileName), accessMode);
    *handle = pDataset;

    return 0;
}

bagError bagFileClose(bagHandle handle)
{
    delete (BAG::Dataset*)handle;
    return 0;
}

bagError bagCreateFromFile(const char *file_name, const char *metaDataFile, bagHandle *handle)
{
    BAG::MetadataV2 metadata;
    metadata.loadFromFile(std::string(metaDataFile));

    BAG::Dataset *pDataset = new BAG::Dataset(std::string(file_name), metadata);
    *handle = pDataset;

    return 0;
}

bagError bagCreateFromBuffer(const char *file_name, const char *metaDataBuffer, uint32_t metaDataBufferSize, bagHandle *handle)
{
    BAG::MetadataV2 metadata;
    metadata.loadFromBuffer(std::string(metaDataBuffer, metaDataBufferSize));

    BAG::Dataset *pDataset = new BAG::Dataset(std::string(file_name), metadata);
    *handle = pDataset;

    return 0;
}

bagError bagCreateLayer(bagHandle handle, BAG_LAYER_TYPE type)
{
    BAG::Dataset *pDataset = (BAG::Dataset *)handle;
    pDataset->addLayer(type);

    return 0;
}

bagError bagGetGridDimensions(bagHandle handle, uint32_t *rows, uint32_t *cols)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;
    pDataset->getDims(*rows, *cols);

    return 0;
}

bagError bagGetNodeSpacing(bagHandle handle, double *xSpacing, double *ySpacing)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;

    const BAG_METADATA metadata = pDataset->getMetadata().getStruct();

    return 0;
}

bagError bagGetGeoCover(bagHandle handle, double *llx, double *lly, double *urx, double *ury)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;

    const BAG_METADATA metadata = pDataset->getMetadata().getStruct();

    return 0;
}

const BAG_METADATA *bagGetMetaData(bagHandle handle)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;
    return &pDataset->getMetadata().getStruct();
}

bagError bagGetMinMax(bagHandle handle, BAG_LAYER_TYPE type, double *minValue, double *maxValue)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;

    const BAG::Layer &layer =  pDataset->getLayer(type);
    layer.getExtents(*minValue, *maxValue);

    return 0;
}

bagError bagSetMinMax(bagHandle handle, BAG_LAYER_TYPE type, double *minValue, double *maxValue)
{
    return 0;
}

bagError bagGetNumLayers(bagHandle handle, uint32_t *numLayers)
{
    return 0;
}

bool bagContainsLayer(bagHandle handle, BAG_LAYER_TYPE type)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;

    try
    {
        pDataset->getLayer(type);
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bagError bagRead (bagHandle handle, uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, BAG_LAYER_TYPE type, uint8_t *data, double *x, double *y)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;

    const BAG::Layer &layer =  pDataset->getLayer(type);
    layer.read(start_row, start_col, end_row, end_col, data);

    //todo - populate x and y
    x;
    y;

    return 0;
}

bagError bagWrite (bagHandle handle, uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, BAG_LAYER_TYPE type, uint8_t *data)
{
    BAG::Dataset *pDataset = (BAG::Dataset *)handle;

    BAG::Layer &layer =  pDataset->getLayer(type);
    layer.write(start_row, start_col, end_row, end_col, data);

    return 0;
}

bagError bagGetErrorString(bagError code, uint8_t **error)
{
    return 0;
}

bagError bagComputePostion(bagHandle handle, uint32_t row, uint32_t col, double *x, double *y)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;
    pDataset->gridToGeo(row, col, *x, *y);

    return 0;
}

void *bagGetNullValue(BAG_LAYER_TYPE type)
{
    switch (type)
    {
        case Elevation:
        case Uncertainty:
        case Hypothesis_Strength:
        case Shoal_Elevation:
        case Std_Dev:
        case Average_Elevation:
        case Nominal_Elevation:
            return NULL;

        case Num_Hypotheses:
        case Num_Soundings:
            return NULL;
    }
	
    return NULL;
}

bagError bagComputeIndex(bagHandle handle, double x, double y, uint32_t *row, uint32_t *col)
{
    const BAG::Dataset *pDataset = (const BAG::Dataset *)handle;
    pDataset->geoToGrid(x, y, *row, *col);

    return 0;
}

bagError bagSetCompressionLevel(uint8_t level)
{
    return 0;
}

uint8_t *bagAllocateBuffer(uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, BAG_LAYER_TYPE type)
{
    int32_t elementSize = 0;

    switch (type)
    {
        case Elevation:
        case Uncertainty:
        case Hypothesis_Strength:
        case Shoal_Elevation:
        case Std_Dev:
        case Average_Elevation:
        case Nominal_Elevation:
            elementSize = sizeof(float);
            break;

        case Num_Hypotheses:
        case Num_Soundings:
            elementSize = sizeof(uint32_t);
            break;
    }

    const uint32_t numRows = (end_row - start_row) + 1;
    const uint32_t numCols = (end_col - start_col) + 1;
    return new uint8_t[numRows * numCols * elementSize];
}

uint8_t *bagAllocate(uint32_t numBytes)
{
    return new uint8_t[numBytes];
}

bagError bagFree (uint8_t *buffer)
{
    delete [] buffer;
    return 0;
}

bagError bagReadCorrectorVerticalDatum  (bagHandle handle, uint32_t, uint8_t * datum)
{
    return 0;
}

bagError bagWriteCorrectorVerticalDatum (bagHandle handle, uint32_t, uint8_t * datum)
{
    return 0;
}

bagError bagReadCorrectedDataset(bagHandle bagHandle, uint32_t corrIndex, uint32_t surfIndex, float *data)
{
    return 0;
}

bagError bagReadCorrectedRegion (bagHandle bagHandle, uint32_t startrow, uint32_t endrow, uint32_t startcol, uint32_t endcol, uint32_t corrIndex, uint32_t surfIndex, float *data)
{
    return 0;
}

bagError bagReadCorrectedRow    (bagHandle bagHandle, uint32_t row, uint32_t corrIndex, uint32_t surfIndex, float *data)
{
    return 0;
}

bagError bagReadCorrectedNode   (bagHandle bagHandle, uint32_t row, uint32_t col, uint32_t corrIndex, uint32_t surfIndex, float *data)
{
    return 0;
}

bagError bagGetNumSurfaceCorrectors (bagHandle handle_opt, uint32_t *num)
{
    return 0;
}

bagError bagGetSurfaceCorrectionTopography(bagHandle handle, uint8_t *type)
{
    return 0;
}

bagError bagCreateCorrectorDataset   (bagHandle handle, uint32_t numCorrectors, uint8_t type)
{
    return 0;
}

bagError bagWriteCorrectorDefinition (bagHandle handle, bagVerticalCorrectorDef *def)
{
    return 0;
}

bagError bagReadCorrectorDefinition  (bagHandle handle, bagVerticalCorrectorDef *def)
{
    return 0;
}

bagError bagTrackingListLength (bagHandle handle, uint32_t *len)
{
    return 0;
}

bagError bagReadTrackingListNode(bagHandle handle, uint32_t row, uint32_t col, bagTrackingItem **items, uint32_t *length)
{
    return 0;
}

bagError bagReadTrackingListCode(bagHandle handle, uint8_t code, bagTrackingItem **items, uint32_t *length)
{
    return 0;
}

bagError bagReadTrackingListSeries(bagHandle handle, uint16_t index, bagTrackingItem **items, uint32_t *length)
{
    return 0;
}

bagError bagWriteTrackingListItem(bagHandle handle, bagTrackingItem *item)
{
    return 0;
}

bagError bagSortTrackingListByNode (bagHandle handle)
{
    return 0;
}

bagError bagSortTrackingListBySeries (bagHandle handle)
{
    return 0;
}

bagError bagSortTrackingListByCode (bagHandle handle)
{
    return 0;
}

uint8_t *bagComputeMessageDigest(const char *file, uint32_t signatureID, uint32_t *nBytes)
{
    return NULL;
}

uint8_t *bagSignMessageDigest(uint8_t *md, uint32_t mdLen, uint8_t *secKey, bagError *errcode)
{
    return NULL;
}

bagError bagReadCertification(const char *file, uint8_t *sig, uint32_t nBuffer, uint32_t *sigID)
{
    return 0;
}

bagError bagWriteCertification(const char *file, uint8_t *sig, uint32_t sigID)
{
    return 0;
}

bool bagVerifyCertification(uint8_t *sig, uint8_t *pubKey, uint8_t *md, uint32_t mdLen)
{
    return false;
}

uint8_t *bagComputeFileSignature(const char *name, uint32_t sigID, uint8_t *secKey)
{
    return NULL;
}

bool bagSignFile(const char *name, uint8_t *secKey, uint32_t sigID)
{
    return false;
}

bool bagVerifyFile(const char *name, uint8_t *pubKey, uint32_t sigID)
{
    return false;
}

bagError bagGenerateKeyPair(uint8_t **pubKey, uint8_t **secKey)
{
    return 0;
}

bagError bagConvertCryptoFormat(uint8_t *object, bagCryptoObject objType, bagConvDir convDir, uint8_t **converted)
{
    return 0;
}
