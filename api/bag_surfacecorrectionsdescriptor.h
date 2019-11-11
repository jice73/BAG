#ifndef BAG_SURFACECORRECTIONSDESCRIPTOR_H
#define BAG_SURFACECORRECTIONSDESCRIPTOR_H

#include "bag_config.h"
#include "bag_fordec.h"
#include "bag_layerdescriptor.h"
#include "bag_types.h"

#include <memory>


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace BAG {

class BAG_API SurfaceCorrectionsDescriptor final : public LayerDescriptor
{
public:
    static std::shared_ptr<SurfaceCorrectionsDescriptor> create(
        BAG_SURFACE_CORRECTION_TOPOGRAPHY type, uint8_t numCorrections);
    static std::shared_ptr<SurfaceCorrectionsDescriptor> create(
        const Dataset& dataset);

    //TODO Temp, make sure only move operations are used until development is done.
    SurfaceCorrectionsDescriptor(const SurfaceCorrectionsDescriptor&) = delete;
    SurfaceCorrectionsDescriptor(SurfaceCorrectionsDescriptor&&) = delete;
    SurfaceCorrectionsDescriptor& operator=(const SurfaceCorrectionsDescriptor&) = delete;
    SurfaceCorrectionsDescriptor& operator=(SurfaceCorrectionsDescriptor&&) = delete;

    uint8_t getNumCorrectors() const noexcept;
    BAG_SURFACE_CORRECTION_TOPOGRAPHY getSurfaceType() const noexcept;
    const std::string& getVerticalDatums() const & noexcept;
    std::tuple<double, double> getOrigin() const noexcept;
    std::tuple<double, double> getSpacing() const noexcept;

    SurfaceCorrectionsDescriptor& setNumCorrectors(
        uint8_t numCorrectors) & noexcept;
    SurfaceCorrectionsDescriptor& setSurfaceType(
        BAG_SURFACE_CORRECTION_TOPOGRAPHY surfaceType) & noexcept;
    SurfaceCorrectionsDescriptor& setVerticalDatum(
        std::string verticalDatums) & noexcept;
    SurfaceCorrectionsDescriptor& setOrigin(double swX,
        double swY) noexcept;
    SurfaceCorrectionsDescriptor& setSpacing(double xSpacing,
        double ySpacing) noexcept;

protected:
    SurfaceCorrectionsDescriptor(BAG_SURFACE_CORRECTION_TOPOGRAPHY type,
        uint8_t numCorrectors);
    explicit SurfaceCorrectionsDescriptor(const Dataset& dataset);

private:
    uint8_t getElementSizeProxy() const noexcept override;
    SurfaceCorrectionsDescriptor& setElementSizeProxy(uint8_t) & noexcept override;

    //! The type of surface this correction applies to.
    BAG_SURFACE_CORRECTION_TOPOGRAPHY m_surfaceType = BAG_SURFACE_UNKNOWN;
    //! The size of a single "element" in the hdf5 file.
    uint8_t m_elementSize = 0;
    //! Number of correction values.
    uint8_t m_numCorrectors = 0;
    //! The name of the vertical datum(s).
    std::string m_verticalDatums;
    //! South-West corner X's value.
    double m_swX = 0.;
    //! South-West corner Y's value.
    double m_swY = 0.;
    //! Node spacing X.
    double m_xSpacing = 0.;
    //! Node spacing Y.
    double m_ySpacing = 0.;

};

}  // namespace BAG

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif  // BAG_SURFACECORRECTIONSDESCRIPTOR_H
