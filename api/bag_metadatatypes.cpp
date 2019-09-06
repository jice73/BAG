//************************************************************************
//
//      Open Navigation Surface Working Group, 2013
//
//************************************************************************
#include "bag.h"
#include "bag_metadata.h"
#include "bag_errors.h"
#include "bag_trackinglist.h"
#include "bag_layer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <algorithm>

#include <h5cpp.h>

//This may not work for everyone.
#ifndef __cplusplus
#define _strdup strdup
#endif


//************************************************************************
/*!
\brief Initialize a BAG_RESPONSIBLE_PARTY structure.

\param responsibleParty
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e responsibleParty
        is NULL.
*/
//************************************************************************
bool initResponsibleParty(BAG_RESPONSIBLE_PARTY * responsibleParty)
{
    if (responsibleParty == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    (*responsibleParty).individualName = nullptr;
    (*responsibleParty).organisationName = nullptr;
    (*responsibleParty).positionName = nullptr;
    (*responsibleParty).role = nullptr;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_RESPONSIBLE_PARTY structure.

\param responsibleParty
    \li The structure to be freed.
*/
//************************************************************************
void freeResponsibleParty(BAG_RESPONSIBLE_PARTY * responsibleParty)
{
    if (responsibleParty == nullptr)
        return;

    free(responsibleParty->individualName);
    free(responsibleParty->organisationName);
    free(responsibleParty->positionName);
    free(responsibleParty->role);
}

//************************************************************************
/*!
\brief Initialize a BAG_IDENTIFICATION structure.

\param dataIdentificationInfo
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e dataIdentificationInfo
        is NULL.
*/
//************************************************************************
bool initDataIdentificationInfo(BAG_IDENTIFICATION * dataIdentificationInfo)
{
    if (dataIdentificationInfo == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    (*dataIdentificationInfo).title = nullptr;
	(*dataIdentificationInfo).date = nullptr;
	(*dataIdentificationInfo).dateType = nullptr;
	(*dataIdentificationInfo).abstractString = nullptr;
	(*dataIdentificationInfo).status = nullptr;
	(*dataIdentificationInfo).spatialRepresentationType = nullptr;
	(*dataIdentificationInfo).language = nullptr;
    (*dataIdentificationInfo).character_set = nullptr;
	(*dataIdentificationInfo).topicCategory = nullptr;

    (*dataIdentificationInfo).verticalUncertaintyType = nullptr;
    (*dataIdentificationInfo).depthCorrectionType = nullptr;
	(*dataIdentificationInfo).elevationSolutionGroupType = nullptr;
	(*dataIdentificationInfo).nodeGroupType = nullptr;

	(*dataIdentificationInfo).westBoundingLongitude = INIT_VALUE;
	(*dataIdentificationInfo).eastBoundingLongitude = INIT_VALUE;   
	(*dataIdentificationInfo).southBoundingLatitude = INIT_VALUE;       
	(*dataIdentificationInfo).northBoundingLatitude = INIT_VALUE;

    (*dataIdentificationInfo).responsibleParties = nullptr;
    (*dataIdentificationInfo).numberOfResponsibleParties = 0;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_IDENTIFICATION structure.

\param dataIdentificationInfo
    \li The structure to be freed.
*/
//************************************************************************
void freeDataIdentificationInfo(BAG_IDENTIFICATION * dataIdentificationInfo)
{
    if (dataIdentificationInfo == nullptr)
        return;

    free(dataIdentificationInfo->title);
	free(dataIdentificationInfo->date);
	free(dataIdentificationInfo->dateType);
	free(dataIdentificationInfo->abstractString);
	free(dataIdentificationInfo->status);
	free(dataIdentificationInfo->spatialRepresentationType);
	free(dataIdentificationInfo->language);
	free(dataIdentificationInfo->topicCategory);

    free(dataIdentificationInfo->verticalUncertaintyType);
	free(dataIdentificationInfo->depthCorrectionType);
	free(dataIdentificationInfo->elevationSolutionGroupType);
	free(dataIdentificationInfo->nodeGroupType);

    for (uint32_t i = 0; i < dataIdentificationInfo->numberOfResponsibleParties; i++)
        freeResponsibleParty(&dataIdentificationInfo->responsibleParties[i]);
    free(dataIdentificationInfo->responsibleParties);
}

//************************************************************************
/*!
\brief Initialize a BAG_LEGAL_CONSTRAINTS structure.

\param legalConstraints
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e legalConstraints
        is NULL.
*/
//************************************************************************
bool initLegalConstraints(BAG_LEGAL_CONSTRAINTS * legalConstraints)
{
    if (legalConstraints == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

   (*legalConstraints).useConstraints = nullptr;
   (*legalConstraints).otherConstraints = nullptr;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_LEGAL_CONSTRAINTS structure.

\param legalConstraints
    \li The structure to be freed.
*/
//************************************************************************
void freeLegalConstraints(BAG_LEGAL_CONSTRAINTS * legalConstraints)
{
    if (legalConstraints == nullptr)
        return;
    
   free(legalConstraints->useConstraints);
   free(legalConstraints->otherConstraints);
}

//************************************************************************
/*!
\brief Initialize a BAG_SECURITY_CONSTRAINTS structure.

\param securityConstraints
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e securityConstraints
        is NULL.
*/
//************************************************************************
bool initSecurityConstraints(BAG_SECURITY_CONSTRAINTS * securityConstraints)
{
    if (securityConstraints == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }
    
    (*securityConstraints).classification = nullptr;
	(*securityConstraints).userNote = nullptr;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_SECURITY_CONSTRAINTS structure.

\param securityConstraints
    \li The structure to be freed.
*/
//************************************************************************
void freeSecurityConstraints(BAG_SECURITY_CONSTRAINTS * securityConstraints)
{
    if (securityConstraints == nullptr)
        return;
    
    free(securityConstraints->classification);
	free(securityConstraints->userNote);
}

//************************************************************************
/*!
\brief Initialize a BAG_SOURCE structure.

\param sourceInfo
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e sourceInfo
        is NULL.
*/
//************************************************************************
bool initSourceInfo(BAG_SOURCE * sourceInfo)
{
    if (sourceInfo == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    (*sourceInfo).description = nullptr;
    (*sourceInfo).title = nullptr;
    (*sourceInfo).date = nullptr;
    (*sourceInfo).dateType = nullptr;

    (*sourceInfo).responsibleParties = nullptr;
    (*sourceInfo).numberOfResponsibleParties = 0;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_SOURCE structure.

\param sourceInfo
    \li The structure to be freed.
*/
//************************************************************************
void freeSourceInfo(BAG_SOURCE * sourceInfo)
{
    if (sourceInfo == nullptr)
        return;

    free(sourceInfo->description);
    free(sourceInfo->title);
    free(sourceInfo->date);
    free(sourceInfo->dateType);

    for (uint32_t i = 0; i < sourceInfo->numberOfResponsibleParties; i++)
        freeResponsibleParty(&sourceInfo->responsibleParties[i]);
}

//************************************************************************
/*!
\brief Initialize a BAG_PROCESS_STEP structure.

\param processStep
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e processStep
        is NULL.
*/
//************************************************************************
bool initProcessStep(BAG_PROCESS_STEP * processStep)
{
    if (processStep == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    (*processStep).description = nullptr;
    (*processStep).dateTime = nullptr;
    (*processStep).trackingId = nullptr;

    (*processStep).lineageSources = nullptr;
    (*processStep).numberOfSources = 0;

    (*processStep).processors = nullptr;
    (*processStep).numberOfProcessors = 0;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_PROCESS_STEP structure.

\param processStep
    \li The structure to be freed.
*/
//************************************************************************
void freeProcessStep(BAG_PROCESS_STEP * processStep)
{
    if (processStep == nullptr)
        return;

    free(processStep->description);
    free(processStep->dateTime);
    free(processStep->trackingId);

    freeSourceInfo(processStep->lineageSources);
    free(processStep->lineageSources);

    for (uint32_t i = 0; i < processStep->numberOfProcessors; i++)
        freeResponsibleParty(&processStep->processors[i]);
}

//************************************************************************
/*!
\brief Initialize a BAG_DATA_QUALITY structure.

\param dataQualityInfo
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e dataQualityInfo
        is NULL.
*/
//************************************************************************
bool initDataQualityInfo(BAG_DATA_QUALITY * dataQualityInfo)
{
    if (dataQualityInfo == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    (*dataQualityInfo).scope = _strdup("dataset");
    (*dataQualityInfo).lineageProcessSteps = nullptr;
    (*dataQualityInfo).numberOfProcessSteps = 0;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_DATA_QUALITY structure.

\param dataQualityInfo
    \li The structure to be freed.
*/
//************************************************************************
void freeDataQualityInfo(BAG_DATA_QUALITY * dataQualityInfo)
{
    if (dataQualityInfo == nullptr)
        return;

    free(dataQualityInfo->scope);

    freeProcessStep(dataQualityInfo->lineageProcessSteps);
    free(dataQualityInfo->lineageProcessSteps);
}

//************************************************************************
/*!
\brief Initialize a BAG_SPATIAL_REPRESENTATION structure.

\param spatialRepresentationInfo
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e spatialRepresentationInfo
        is NULL.
*/
//************************************************************************
bool initSpatialRepresentationInfo(BAG_SPATIAL_REPRESENTATION * spatialRepresentationInfo)
{
    if (spatialRepresentationInfo == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    (*spatialRepresentationInfo).numberOfRows = 0;
    (*spatialRepresentationInfo).rowResolution = 0.0;
    (*spatialRepresentationInfo).numberOfColumns = 0;
    (*spatialRepresentationInfo).columnResolution = 0.0;
    (*spatialRepresentationInfo).resolutionUnit = nullptr;

    (*spatialRepresentationInfo).cellGeometry = _strdup("point");
    (*spatialRepresentationInfo).transformationParameterAvailability = false;
    (*spatialRepresentationInfo).checkPointAvailability = false;              

    (*spatialRepresentationInfo).llCornerX = INIT_VALUE;                                 
    (*spatialRepresentationInfo).llCornerY = INIT_VALUE;                                  
    (*spatialRepresentationInfo).urCornerX = INIT_VALUE;                               
    (*spatialRepresentationInfo).urCornerY = INIT_VALUE;  

    (*spatialRepresentationInfo).transformationDimensionDescription = nullptr;
    (*spatialRepresentationInfo).transformationDimensionMapping = nullptr;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_SPATIAL_REPRESENTATION structure.

\param spatialRepresentationInfo
    \li The structure to be freed.
*/
//************************************************************************
void freeSpatialRepresentationInfo(BAG_SPATIAL_REPRESENTATION * spatialRepresentationInfo)
{
    if (spatialRepresentationInfo == nullptr)
        return;

    free(spatialRepresentationInfo->resolutionUnit);
    free(spatialRepresentationInfo->cellGeometry);
    if (spatialRepresentationInfo->transformationDimensionDescription)
    {
        free(spatialRepresentationInfo->transformationDimensionDescription);
    }
    if (spatialRepresentationInfo->transformationDimensionMapping)
    {
        free(spatialRepresentationInfo->transformationDimensionMapping);
    }
}

//************************************************************************
/*!
\brief Initialize a BAG_REFERENCE_SYSTEM structure.

\param referenceInfo
    \li The structure to be initialized.
\return
    \li true if the structure is initialized, false if \e referenceInfo
        is NULL.
*/
//************************************************************************
bool initReferenceSystemInfo(BAG_REFERENCE_SYSTEM *referenceInfo)
{
    if (referenceInfo == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return false;
    }

    referenceInfo->definition = nullptr;
    referenceInfo->type = nullptr;

    return true;
}

//************************************************************************
/*!
\brief Free a BAG_REFERENCE_SYSTEM structure.

\param referenceInfo
    \li The structure to be freed.
*/
//************************************************************************
void freeReferenceSystemInfo(BAG_REFERENCE_SYSTEM *referenceInfo)
{
    if (referenceInfo == nullptr)
        return;

    free(referenceInfo->definition);
    free(referenceInfo->type);
}

//************************************************************************
/*!
\brief Initialize the BAG_METADATA structure.

    The caller must call bagFreeMetadata() to ensure no memory leaks
    during cleanup.

\param metadata
    \li The structure to be initialized.
\return
    \li 0 on success, a bagError if an error occurs.
*/
//************************************************************************
bagError bagInitMetadata(BAG_METADATA * metadata)
{
    if (metadata == nullptr)
    {
        fprintf(stderr,"ERROR: Exception when attempting to intialize data structure. Exception message is: Null pointer\n");
        return BAG_METADTA_INVALID_HANDLE;
    }

    metadata->fileIdentifier = nullptr;
    metadata->dateStamp = nullptr;
    metadata->language = _strdup("en");
    metadata->characterSet = _strdup("utf8");
    metadata->hierarchyLevel = _strdup("dataset");
    metadata->metadataStandardName = _strdup("ISO 19115");
    metadata->metadataStandardVersion = _strdup("2003/Cor.1:2006");

    metadata->contact = (BAG_RESPONSIBLE_PARTY *)malloc(sizeof(BAG_RESPONSIBLE_PARTY));
    if (metadata->contact == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initResponsibleParty(metadata->contact))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->spatialRepresentationInfo = (BAG_SPATIAL_REPRESENTATION *)malloc(sizeof(BAG_SPATIAL_REPRESENTATION));
    if (metadata->spatialRepresentationInfo == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initSpatialRepresentationInfo(metadata->spatialRepresentationInfo))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->horizontalReferenceSystem = (BAG_REFERENCE_SYSTEM *)malloc(sizeof(BAG_REFERENCE_SYSTEM));
    if (metadata->horizontalReferenceSystem == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initReferenceSystemInfo(metadata->horizontalReferenceSystem))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->verticalReferenceSystem = (BAG_REFERENCE_SYSTEM *)malloc(sizeof(BAG_REFERENCE_SYSTEM));
    if (metadata->verticalReferenceSystem == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initReferenceSystemInfo(metadata->verticalReferenceSystem))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->identificationInfo = (BAG_IDENTIFICATION *)malloc(sizeof(BAG_IDENTIFICATION));
    if (metadata->identificationInfo == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initDataIdentificationInfo(metadata->identificationInfo))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->dataQualityInfo = (BAG_DATA_QUALITY *)malloc(sizeof(BAG_DATA_QUALITY));
    if (metadata->dataQualityInfo == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initDataQualityInfo(metadata->dataQualityInfo))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->legalConstraints = (BAG_LEGAL_CONSTRAINTS *)malloc(sizeof(BAG_LEGAL_CONSTRAINTS));
    if (metadata->legalConstraints == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initLegalConstraints(metadata->legalConstraints))
        return BAG_MEMORY_ALLOCATION_FAILED;

    metadata->securityConstraints = (BAG_SECURITY_CONSTRAINTS *)malloc(sizeof(BAG_SECURITY_CONSTRAINTS));
    if (metadata->securityConstraints == nullptr)
        return BAG_MEMORY_ALLOCATION_FAILED;
    if (!initSecurityConstraints(metadata->securityConstraints))
        return BAG_MEMORY_ALLOCATION_FAILED;

    return BAG_SUCCESS;
}

//************************************************************************
/*!
\brief Free a BAG_METADATA structure.

\param metadata
    \li The structure to be freed.
*/
//************************************************************************
void bagFreeMetadata(BAG_METADATA * metadata)
{
    if (metadata == nullptr)
        return;

    free(metadata->fileIdentifier);
    free(metadata->dateStamp);
    free(metadata->language);
    free(metadata->characterSet);
    free(metadata->hierarchyLevel);
    free(metadata->metadataStandardName);
    free(metadata->metadataStandardVersion);

    freeResponsibleParty(metadata->contact);
    free(metadata->contact);

    freeSpatialRepresentationInfo(metadata->spatialRepresentationInfo);
    free(metadata->spatialRepresentationInfo);

    freeReferenceSystemInfo(metadata->horizontalReferenceSystem);
    free(metadata->horizontalReferenceSystem);

    freeReferenceSystemInfo(metadata->verticalReferenceSystem);
    free(metadata->verticalReferenceSystem);

    freeDataIdentificationInfo(metadata->identificationInfo);
    free(metadata->identificationInfo);

    freeDataQualityInfo(metadata->dataQualityInfo);
    free(metadata->dataQualityInfo);

    freeLegalConstraints(metadata->legalConstraints);
    free(metadata->legalConstraints);

    freeSecurityConstraints(metadata->securityConstraints);
    free(metadata->securityConstraints);
}
