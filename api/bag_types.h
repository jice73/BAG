#ifndef BAG_TYPES_H
#define BAG_TYPES_H

namespace BAG
{
    enum LayerType
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

    enum DataType
    {
        FLOAT32 = 0,
        UINT32,
        UNKNOWN
    };

    //! The type of interleaved group.
    enum GroupType
    {
        NODE = 0,
        ELEVATION
    };

    struct Data
    {
        Data(){};
        virtual ~Data(){};
    };

}   //namespace BAG

#endif  //BAG_TYPES_H