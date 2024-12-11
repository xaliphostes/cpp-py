#include <CppLib/triangle.h>
#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

// Gauss-Legendre quadrature points and weights for different orders.
// Defined on interval [-1, 1].
class GaussQuadrature {
  public:
    // Available quadrature orders
    static const std::vector<int> available_orders;

    // Get points and weights for specified order
    static std::pair<std::vector<double>, std::vector<double>> get_rule(int n) {
        switch (n) {
        case 2:
            return {{-0.577350269189626, 0.577350269189626},
                    {1.000000000000000, 1.000000000000000}};
        case 3:
            return {{-0.774596669241483, 0.000000000000000, 0.774596669241483},
                    {0.555555555555556, 0.888888888888889, 0.555555555555556}};
        case 4:
            return {{-0.861136311594053, -0.339981043584856, 0.339981043584856,
                     0.861136311594053},
                    {0.347854845137454, 0.652145154862546, 0.652145154862546,
                     0.347854845137454}};
        case 5:
            return {{-0.906179845938664, -0.538469310105683, 0.000000000000000,
                     0.538469310105683, 0.906179845938664},
                    {0.236926885056189, 0.478628670499366, 0.568888888888889,
                     0.478628670499366, 0.236926885056189}};
        case 6:
            return {{-0.932469514203152, -0.661209386466265, -0.238619186083197,
                     0.238619186083197, 0.661209386466265, 0.932469514203152},
                    {0.171324492379170, 0.360761573048139, 0.467913934572691,
                     0.467913934572691, 0.360761573048139, 0.171324492379170}};
        case 8:
            return {{-0.960289856497536, -0.796666477413627, -0.525532409916329,
                     -0.183434642495650, 0.183434642495650, 0.525532409916329,
                     0.796666477413627, 0.960289856497536},
                    {0.101228536290376, 0.222381034453374, 0.313706645877887,
                     0.362683783378362, 0.362683783378362, 0.313706645877887,
                     0.222381034453374, 0.101228536290376}};
        case 10:
            return {{-0.973906528517172, -0.865063366688985, -0.679409568299024,
                     -0.433395394129247, -0.148874338981631, 0.148874338981631,
                     0.433395394129247, 0.679409568299024, 0.865063366688985,
                     0.973906528517172},
                    {0.066671344308688, 0.149451349150581, 0.219086362515982,
                     0.269266719309996, 0.295524224714753, 0.295524224714753,
                     0.269266719309996, 0.219086362515982, 0.149451349150581,
                     0.066671344308688}};
        default:
            throw std::invalid_argument("Unsupported number of Gauss points");
        }
    }
};

// Transform Gauss point from [-1,1] to [0,1] interval
double transform_gauss_point(double xi) { return 0.5 * (xi + 1.0); }

// Transform Gauss weight from [-1,1] to [0,1] interval
double transform_gauss_weight(double w) { return 0.5 * w; }

// ---------------------------------------------

TriangleSource::TriangleSource(const Point3D &p1, const Point3D &p2,
                               const Point3D &p3, const Vec3 &burgers,
                               double shear, double poisson, uint8_t n_gauss)
    : mu_(shear), nu_(poisson), b_(burgers), n_gauss_(n_gauss) {
    tri_.v1 = p1;
    tri_.v2 = p2;
    tri_.v3 = p3;
}

StressField TriangleSource::run(const Coordinates &coords) const {
    Point3D pos;
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

// --------------- private -----------------------------------------

Stress TriangleSource::stress(const Point3D &p) const {
    std::array<double, 6> stress = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    // Compute contribution from each edge of the triangle
    auto stress1 = line_integral_contribution(p, tri_.v1, tri_.v2);
    auto stress2 = line_integral_contribution(p, tri_.v2, tri_.v3);
    auto stress3 = line_integral_contribution(p, tri_.v3, tri_.v1);

    // Sum contributions from all edges
    for (int i = 0; i < 6; ++i) {
        stress[i] = stress1[i] + stress2[i] + stress3[i];
    }
    
    return stress;
}

/**
 * Compute line integral contribution along a triangle edge (xx, xy, xz, yy, yz,
 * zz)
 */
std::array<double, 6> TriangleSource::line_integral_contribution(
    const Point3D &p, const Point3D &start, const Point3D &end) const {

    // Get Gauss quadrature points and weights
    auto [points, weights] = GaussQuadrature::get_rule(n_gauss_);

    // Compute edge vector
    Point3D edge = {end[0] - start[0], end[1] - start[1], end[2] - start[2]};

    // Edge length
    double L =
        std::sqrt(edge[0] * edge[0] + edge[1] * edge[1] + edge[2] * edge[2]);
        
    if (L < 1e-10) {
        return Stress{0, 0, 0, 0, 0, 0};
    }

    // Unit edge vector
    Point3D t = {edge[0] / L, edge[1] / L, edge[2] / L};

    double xx{0}, xy{0}, xz{0}, yy{0}, yz{0}, zz{0};

    // Integration using Gauss-Legendre quadrature
    for (int i = 0; i < n_gauss_; ++i) {
        // Transform Gauss point and weight to [0,1] interval
        double xi = transform_gauss_point(points[i]);
        double w = transform_gauss_weight(weights[i]);

        // Position along the edge
        Point3D pos = {start[0] + xi * edge[0], start[1] + xi * edge[1],
                       start[2] + xi * edge[2]};

        // Relative position vector from Gauss point to observer
        Point3D R = {p[0] - pos[0], p[1] - pos[1], p[2] - pos[2]};

        double R_mag = std::sqrt(R[0] * R[0] + R[1] * R[1] + R[2] * R[2]);
        if (R_mag < 1e-10)
            continue;

        // Unit vector in R direction
        Point3D n = {R[0] / R_mag, R[1] / R_mag, R[2] / R_mag};

        // Cross product t × n
        Point3D t_cross_n = {t[1] * n[2] - t[2] * n[1],
                             t[2] * n[0] - t[0] * n[2],
                             t[0] * n[1] - t[1] * n[0]};

        // Compute angular dislocation contribution
        double c1 = mu_ / (4.0 * M_PI * (1.0 - nu_));
        double c2 = 1.0 - 2.0 * nu_;

        // Scale factor for line integral
        double scale = w * L; // include edge length in quadrature

        // Stress contributions at this Gauss point
        // σxx
        xx += scale * c1 *
              (b_[0] * (c2 / R_mag + 3.0 * n[0] * n[0] / R_mag) +
               b_[1] * (3.0 * n[0] * n[1] / R_mag) +
               b_[2] * (3.0 * n[0] * n[2] / R_mag));

        // σyy
        yy += scale * c1 *
              (b_[0] * (3.0 * n[1] * n[0] / R_mag) +
               b_[1] * (c2 / R_mag + 3.0 * n[1] * n[1] / R_mag) +
               b_[2] * (3.0 * n[1] * n[2] / R_mag));

        // σzz
        zz += scale * c1 *
              (b_[0] * (3.0 * n[2] * n[0] / R_mag) +
               b_[1] * (3.0 * n[2] * n[1] / R_mag) +
               b_[2] * (c2 / R_mag + 3.0 * n[2] * n[2] / R_mag));

        // σyz
        yz += scale * c1 *
              (b_[0] * (3.0 * n[1] * n[2] * n[0] / R_mag) +
               b_[1] * (c2 * n[2] / R_mag + 3.0 * n[1] * n[2] * n[1] / R_mag) +
               b_[2] * (c2 * n[1] / R_mag + 3.0 * n[1] * n[2] * n[2] / R_mag));

        // σxz
        xz += scale * c1 *
              (b_[0] * (c2 * n[2] / R_mag + 3.0 * n[0] * n[2] * n[0] / R_mag) +
               b_[1] * (3.0 * n[0] * n[2] * n[1] / R_mag) +
               b_[2] * (c2 * n[0] / R_mag + 3.0 * n[0] * n[2] * n[2] / R_mag));

        // σxy
        xy += scale * c1 *
              (b_[0] * (c2 * n[1] / R_mag + 3.0 * n[0] * n[1] * n[0] / R_mag) +
               b_[1] * (c2 * n[0] / R_mag + 3.0 * n[0] * n[1] * n[1] / R_mag) +
               b_[2] * (3.0 * n[0] * n[1] * n[2] / R_mag));
    }

    return Stress{xx, xy, xz, yy, yz, zz};
}
