/*! \file bag.h
 * \brief Declarations of Open Navigation Surface API.
 ********************************************************************
 *
 * Authors/Date : Fri Jan 27 14:41:13 2006
 *     Brian Calder      (CCOM/JHC)
 *     Rick Brennan      (CCOM/JHC)
 *     Bill Lamey        (CARIS Ltd)
 *     Mark Paton        (IVS Ltd)
 *     Shannon Byrne     (SAIC, Newport)
 *     Jim Case          (SAIC, Newport)
 *     Webb McDonald     (SAIC, Newport)
 *     Dave Fabre        (NAVOCEANO)
 *     Jan Depner        (NAVOCEANO)
 *     Wade Ladner       (NAVOCEANO)
 *     Barry Gallagher   (NOAA HSTP)
 *     Friedhelm Moggert (7Cs GmbH)
 *     Shep Smith        (NOAA)
 *     Jack Riley        (NOAA HSTP)
 *
 * Initial concepts developed during The Open Navigation Surface Workshop
 *   attended by above contributors meeting at CCOM/JHC.
 *
 * Description : 
 *   This is the header file for the Bathymetric Attributed Grid (BAG) 
 *   access library.  This file defines the data structures and function 
 *   prototypes for accessing BAG files.
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 * who when      what
 * --- ----      ----
 *
 * Classification : 
 *  Unclassified
 *
 * Distribution :
 * 
 * Usage And Licensing :
 *
 * References :
 *     Bathymetric Attributed Grid Format Specification Document
 *     http://hdf.ncsa.uiuc.edu/HDF5/
 * 
 ********************************************************************/
#ifndef BAG_H
#define BAG_H

#include "bag_config.h"
#include "bag_version.h"

#ifdef BAG_USE_LEGACY_API
#include "legacy/bag.h"
#else

/* Get the required standard C include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include "bag_stdint.h"
#include "bag_types.h"
#include "bag_metadatatypes.h"

/* Function prototypes */

/* Open/Create/Close */
BAG_EXTERNAL bagError bagFileOpen(bagHandle *bagHandle, BAG_OPEN_MODE accessMode, const char *fileName); 
BAG_EXTERNAL bagError bagFileClose(bagHandle bagHandle);
BAG_EXTERNAL bagError bagCreateFromFile(const char *file_name, const char *metaDataFile, bagHandle *bag_handle);
BAG_EXTERNAL bagError bagCreateFromBuffer(const char *file_name, uint8_t *metaDataBuffer, uint32_t metaDataBufferSize, bagHandle *bag_handle);
BAG_EXTERNAL bagError bagCreateLayer(bagHandle hnd, BAG_LAYER_TYPE type);
BAG_EXTERNAL bagError bagGetGridDimensions(bagHandle hnd, uint32_t *rows, uint32_t *cols);
BAG_EXTERNAL bagError bagGetNodeSpacing(bagHandle handle, double *xSpacing, double *ySpacing);
BAG_EXTERNAL bagError bagGetGeoCover(bagHandle handle, double *llx, double *lly, double *urx, double *ury);
BAG_EXTERNAL const BAG_METADATA *bagGetMetaData(bagHandle handle);

/* Layer access */
BAG_EXTERNAL bagError bagGetMinMax(bagHandle hnd, BAG_LAYER_TYPE type, double *minValue, double *maxValue);
BAG_EXTERNAL bagError bagSetMinMax(bagHandle hnd, BAG_LAYER_TYPE type, double *minValue, double *maxValue);
BAG_EXTERNAL bagError bagGetNumLayers(bagHandle hnd, uint32_t *numLayers);
BAG_EXTERNAL bool bagContainsLayer(bagHandle hnd, BAG_LAYER_TYPE type);
BAG_EXTERNAL bagError bagRead (bagHandle bagHandle, uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, BAG_LAYER_TYPE type, uint8_t *data, double *x, double *y);
BAG_EXTERNAL bagError bagWrite (bagHandle bagHandle, uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, BAG_LAYER_TYPE type, uint8_t *data);

/* Utilities */
BAG_EXTERNAL bagError bagGetErrorString(bagError code, uint8_t **error);
BAG_EXTERNAL bagError bagComputePostion(bagHandle hnd, uint32_t row, uint32_t col, double *x, double *y);
BAG_EXTERNAL void *bagGetNullValue(BAG_LAYER_TYPE type);
BAG_EXTERNAL bagError bagComputeIndex(bagHandle hnd, double x, double y, uint32_t *row, uint32_t *col);
BAG_EXTERNAL bagError bagSetCompressionLevel(uint8_t level);
BAG_EXTERNAL uint8_t *bagAllocateBuffer(uint32_t start_row, uint32_t start_col, uint32_t end_row, uint32_t end_col, BAG_LAYER_TYPE type);
BAG_EXTERNAL uint8_t *bagAllocate(uint32_t numBytes);
BAG_EXTERNAL bagError bagFree (uint8_t *buffer);

/* Corrector surface access */
BAG_EXTERNAL bagError bagReadCorrectorVerticalDatum  (bagHandle hnd, uint32_t, uint8_t * datum);
BAG_EXTERNAL bagError bagWriteCorrectorVerticalDatum (bagHandle hnd, uint32_t, uint8_t * datum);
BAG_EXTERNAL bagError bagReadCorrectedDataset(bagHandle bagHandle, uint32_t corrIndex, uint32_t surfIndex, float *data);
BAG_EXTERNAL bagError bagReadCorrectedRegion (bagHandle bagHandle, uint32_t startrow, uint32_t endrow, uint32_t startcol, uint32_t endcol, uint32_t corrIndex, uint32_t surfIndex, float *data);
BAG_EXTERNAL bagError bagReadCorrectedRow    (bagHandle bagHandle, uint32_t row, uint32_t corrIndex, uint32_t surfIndex, float *data);
BAG_EXTERNAL bagError bagReadCorrectedNode   (bagHandle bagHandle, uint32_t row, uint32_t col, uint32_t corrIndex, uint32_t surfIndex, float *data);
BAG_EXTERNAL bagError bagGetNumSurfaceCorrectors (bagHandle hnd_opt, uint32_t *num);
BAG_EXTERNAL bagError bagGetSurfaceCorrectionTopography(bagHandle hnd, uint8_t *type);
BAG_EXTERNAL bagError bagCreateCorrectorDataset   (bagHandle hnd, uint32_t numCorrectors, uint8_t type);
BAG_EXTERNAL bagError bagWriteCorrectorDefinition (bagHandle hnd, bagVerticalCorrectorDef *def);
BAG_EXTERNAL bagError bagReadCorrectorDefinition  (bagHandle hnd, bagVerticalCorrectorDef *def);

/* Tracking list access */
BAG_EXTERNAL bagError bagTrackingListLength (bagHandle bagHandle, uint32_t *len);
BAG_EXTERNAL bagError bagReadTrackingListNode(bagHandle bagHandle, uint32_t row, uint32_t col, bagTrackingItem **items, uint32_t *length);
BAG_EXTERNAL bagError bagReadTrackingListCode(bagHandle bagHandle, uint8_t code, bagTrackingItem **items, uint32_t *length);
BAG_EXTERNAL bagError bagReadTrackingListSeries(bagHandle bagHandle, uint16_t index, bagTrackingItem **items, uint32_t *length);
BAG_EXTERNAL bagError bagWriteTrackingListItem(bagHandle bagHandle, bagTrackingItem *item);
BAG_EXTERNAL bagError bagSortTrackingListByNode (bagHandle bagHandle);
BAG_EXTERNAL bagError bagSortTrackingListBySeries (bagHandle bagHandle);
BAG_EXTERNAL bagError bagSortTrackingListByCode (bagHandle bagHandle);

/* Digital signature */
BAG_EXTERNAL uint8_t *bagComputeMessageDigest(const char *fileName, uint32_t signatureID, uint32_t *nBytes);
BAG_EXTERNAL uint8_t *bagSignMessageDigest(uint8_t *md, uint32_t mdLen, uint8_t *secKey, bagError *errcode);
BAG_EXTERNAL bagError bagReadCertification(const char *fileName, uint8_t *sig, uint32_t nBuffer, uint32_t *sigID);
BAG_EXTERNAL bagError bagWriteCertification(const char *fileName, uint8_t *sig, uint32_t sigID);
BAG_EXTERNAL bool bagVerifyCertification(uint8_t *sig, uint8_t *pubKey, uint8_t *md, uint32_t mdLen);
BAG_EXTERNAL uint8_t *bagComputeFileSignature(const char *fileName, uint32_t sigID, uint8_t *secKey);
BAG_EXTERNAL bool bagSignFile(const char *fileName, uint8_t *secKey, uint32_t sigID);
BAG_EXTERNAL bool bagVerifyFile(const char *fileName, uint8_t *pubKey, uint32_t sigID);
BAG_EXTERNAL bagError bagGenerateKeyPair(uint8_t **pubKey, uint8_t **secKey);
BAG_EXTERNAL bagError bagConvertCryptoFormat(uint8_t *object, bagCryptoObject objType, bagConvDir convDir, uint8_t **converted);

/* Metadata */
BAG_EXTERNAL bagError bagInitMetadata(BAG_METADATA * metadata);
BAG_EXTERNAL void bagFreeMetadata(BAG_METADATA * metadata);
BAG_EXTERNAL void bagSetHomeFolder(const char *metadataFolder);

#endif // BAG_USE_LEGACY_API

#endif // BAG_H