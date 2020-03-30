%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif
%}

/*
    This interface module wraps up all of the other ones.
    This simplifies the building and generating of python files,
    and simplified the use of those python files.
*/
%module bagpy

//! Common templates used by multiple interfaces
%include <std_pair.i>
%include <stdint.i>
%template(DoublePair) std::pair<double, double>;
%template(FloatPair) std::pair<float, float>;
%template(UInt32Pair) std::pair<uint32_t, uint32_t>;
%template(Cover) std::pair<std::pair<double, double>, std::pair<double, double> >;

%include "bag_c_types.i"
%include "bag_uint8array.i"
%include "bag_metadatatypes.i"
%include "bag_types.i"
%include "bag_metadata.i"
%include "bag_metadata_import.i"
%include "bag_metadata_export.i"
%include "bag_exceptions.i"
%include "bag_layerdescriptor.i"
%include "bag_descriptor.i"
%include "bag_interleavedlayerdescriptor.i"
%include "bag_simplelayerdescriptor.i"
%include "bag_surfacecorrectionsdescriptor.i"
%include "bag_trackinglist.i"
%include "bag_compounddatatype.i"
%include "bag_valuetable.i"
%include "bag_layer.i"
%include "bag_simplelayer.i"
%include "bag_interleavedlayer.i"
%include "bag_surfacecorrections.i"
%include "bag_compoundlayer.i"
%include "bag_compoundlayerdescriptor.i"
%include "bag_dataset.i"
























