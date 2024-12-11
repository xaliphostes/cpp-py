#pragma once
#include "source.h"

struct Triangle {
    Point3D v1, v2, v3; // vertices of the triangle
};

/**
 * "triangular dislocation elements" or "triangular displacement discontinuity
 * elements" in the literature. Let me create a version for a triangular
 * element.
 * This is a more complex case than the point source because it involves:
 *
 * Line Integration:
 * - The solution requires integrating along the edges of the triangle
 * - Uses Gaussian quadrature for numerical integration
 * - Each edge contributes to the total stress field
 *
 * Key Components:
 * - Triangle defined by three vertices in 3D space
 * - Burgers vector (displacement discontinuity) defined over the triangle
 * - Observer point can be anywhere in 3D space
 * - Uses material properties (μ, ν)
 *
 * Implementation Notes:
 *
 * This is a simplified version - a complete implementation would include:
 * - Full angular dislocation formulae
 * - Proper handling of near-field terms
 * - Complete Gauss point and weight definitions (number of Gauss points = 2, 3,
 *   4, 5, 6, 8, or 10)
 * - Additional terms for triangle interior
 *
 * Common Applications:
 * - Crack modeling in 3D
 * - Fault slip analysis in geomechanics
 * - BEM implementations
 * - Contact mechanics
 */
class TriangleSource : public Source {
  public:
    TriangleSource(const Point3D &p1, const Point3D &p2, const Point3D &p3,
                   const Vec3 &burgers, double shear, double poisson,
                   uint8_t n_gauss = 8);

    StressField run(const Coordinates &coordinates) const override;

  private:
    Stress stress(const Point3D &at) const;
    // Compute line integral contribution along a triangle edge
    std::array<double, 6> line_integral_contribution(const Point3D &p,
                                                     const Point3D &start,
                                                     const Point3D &end) const;

    double mu_{0};
    double nu_{0};
    Vec3 b_{0, 0, 0};
    Triangle tri_;
    uint8_t n_gauss_{8};
};
