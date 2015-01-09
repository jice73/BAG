#ifndef BAG_C_TYPES_H
#define BAG_C_TYPES_H

#include "bag_stdint.h"

typedef uint32_t bagError;
typedef void* bagHandle;

enum BAG_LAYER_TYPE
{
    Elevation = 0,
    Uncertainty = 1,
    Hypothesis_Strength = 2,
	Num_Hypotheses	= 3,
	Shoal_Elevation 	= 4,
	Std_Dev		= 5,
	Num_Soundings   	= 6,
	Average_Elevation = 7,
	Nominal_Elevation = 8
};

/* Definitions for file open access modes */
enum BAG_OPEN_MODE
{ 
    BAG_OPEN_READONLY   = 1,
    BAG_OPEN_READ_WRITE = 2
};

enum BAG_DATA_TYPE
{
    FLOAT32 = 0,
    UINT32,
    UNKNOWN
};

//! The type of interleaved group.
enum BAG_GROUP_TYPE
{
    NODE = 0,
    ELEVATION
};

/* tracking list structure */
typedef struct
{
    //! location of the node of the BAG that was modified
    uint32_t row; uint32_t col;
    //! original depth before this change
    float depth;
    //! original uncertainty before this change
    float uncertainty;
    //! reason code indicating why the modification was made
    uint8_t  track_code;
    //! index number indicating the item in the metadata that describes the modifications 
    uint16_t list_series;
} bagTrackingItem;


#endif  //BAG_C_TYPES_H