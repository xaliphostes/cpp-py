#include <CppLib/point.h>
#include <algorithm>
#include <iostream>

PointSource::PointSource(const Vec3 &pos, const Vec3 &U) : pos_(pos), U_(U) {}

StressField PointSource::run(const Coordinates &coords) const {
    Vec3 pos;
    StressField field(coords.size() * 2);

    uint32_t j = 0;
    for (auto i = 0; i < coords.size(); i += 3) {
        pos = {coords[i], coords[i + 1], coords[i + 2]};
        auto stress = this->stress(pos);
        for (auto k = 0; k < 6; ++k) {
            field[j++] = stress[k];
        }
    }

    return field;
}

Stress PointSource::stress(const Vec3 &at) const {
    double x = at[0];
    double y = at[1];
    double z = at[2];

    // Compute relative position
    const double dx = x - pos_[0];
    const double dy = y - pos_[1];
    const double dz = z - pos_[2];
    const double r = std::sqrt(dx * dx + dy * dy + dz * dz);

    if (r < 1e-10) {
        // Avoid singularity at source point
        return Stress{0, 0, 0, 0, 0, 0};
    }

    const double r2 = r * r;
    const double r3 = r2 * r;
    const double r4 = r2 * r2;
    const double r5 = r3 * r2;

    // Compute common factors
    double a = 2.0 * mu;
    // Lamé's first parameter
    const double lambda = a * nu / (1.0 - 2.0 * nu);
    const double c1 = mu / (4.0 * M_PI * (1.0 - nu));
    const double c2 = 3.0 - 4.0 * nu;

    // Unit vector components of relative position
    const double nx = dx / r;
    const double ny = dy / r;
    const double nz = dz / r;

    // Auxiliary terms for displacement gradient
    const double D1 = (c2 / r3) - (3.0 / r5) * (dx * dx);
    const double D2 = (c2 / r3) - (3.0 / r5) * (dy * dy);
    const double D3 = (c2 / r3) - (3.0 / r5) * (dz * dz);
    const double D4 = -(3.0 / r5) * (dx * dy);
    const double D5 = -(3.0 / r5) * (dy * dz);
    const double D6 = -(3.0 / r5) * (dx * dz);

    const double ux = U_[0];
    const double uy = U_[1];
    const double uz = U_[2];

    // First compute displacement gradients
    const double dux_dx = ux * D1 + uy * D4 + uz * D6;
    const double dux_dy = ux * D4 + uy * (-1.0 / r3) + uz * 0.0;
    const double dux_dz = ux * D6 + uy * 0.0 + uz * (-1.0 / r3);

    const double duy_dx = uy * D4 + ux * (-1.0 / r3) + uz * 0.0;
    const double duy_dy = uy * D2 + ux * D4 + uz * D5;
    const double duy_dz = uy * D5 + ux * 0.0 + uz * (-1.0 / r3);

    const double duz_dx = uz * D6 + ux * (-1.0 / r3) + uy * 0.0;
    const double duz_dy = uz * D5 + ux * 0.0 + uy * (-1.0 / r3);
    const double duz_dz = uz * D3 + ux * D6 + uy * D5;

    // Then compute stresses using Hooke's law

    double b = lambda * (dux_dx + duy_dy + duz_dz);
    Stress stress;
    stress[0] = a * dux_dx + b;         // σxx
    stress[1] = mu * (dux_dy + duy_dx); // σxy
    stress[2] = mu * (dux_dz + duz_dx); // σxz
    stress[3] = a * duy_dy + b;         // σyy
    stress[4] = mu * (duy_dz + duz_dy); // σyz
    stress[5] = a * duz_dz + b;         // σzz

    return stress;
}
