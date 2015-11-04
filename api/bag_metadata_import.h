#ifndef BAG_METADATAIMPORT_H
#define BAG_METADATAIMPORT_H

#include "bag_types.h"
#include "bag_metadatatypes.h"

bagError bagImportMetadataFromXmlFile(const char *fileName, BAG_METADATA * metadata, bool doValidation);
bagError bagImportMetadataFromXmlBuffer(const char *xmlBuffer, uint32_t bufferSize, BAG_METADATA * metadata, bool doValidation);

void bagSetHomeFolder(const char *homeFolder);

#endif  //BAG_METADATAIMPORT_H