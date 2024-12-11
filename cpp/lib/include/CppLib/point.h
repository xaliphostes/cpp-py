#pragma once
#include "source.h"

/**
 * Computes the 3D Green's function (elastic fundamental solution) using the
 * fundamental solution for a displacement discontinuity (or dislocation) rather
 * than a point force.
 * Implementation Approach:
 * - First computes displacement gradients using the fundamental solution
 * - Then uses Hooke's law to compute the resulting stresses
 * - Includes both Lamé parameters (μ and λ) for the full elastic response
 *
 * Mathematical Details:
 * - Uses a different set of fundamental solutions based on displacement
 *   discontinuities
 * - Includes terms up to r⁻⁵ for the displacement gradients
 * - Properly handles both deviatoric and volumetric deformation through Lamé
 *   parameters
 *
 * This implementation is particularly useful for:
 * - Dislocation problems
 * - Crack problems
 * - Problems involving prescribed boundary displacements
 * - Material inclusion problems
 */
class PointSource: public Source {
  public:
    PointSource(const Vec3 &pos = {0, 0, 0}, const Vec3 &U = {1, 0, 0});

    StressField run(const Coordinates &coordinates) const override;
    Stress stress(const Vec3 &pos) const;

  private:
    Vec3 pos_{0, 0, 0}; // Source position
    Vec3 U_{1, 0, 0};   // displ. discon.
    double nu{0.25};    // poisson's ratio
    double mu{1};       // shear modulus
};
