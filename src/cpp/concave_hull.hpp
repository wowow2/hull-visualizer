#include <vector>
#include <set>
#include <cmath>
#include "point.hpp"
#include "delaunator.hpp"

class ConcaveHull {
public:
    static std::vector<Point> compute(const std::vector<Point>& points, double alpha) {
        if (points.size() < 3 || alpha <= 0) return {};

        // Check if all points are collinear
        bool all_collinear = true;
        for (size_t i = 2; i < points.size(); ++i) {
            if (orientation(points[0], points[1], points[i]) != 0) {
                all_collinear = false;
                break;
            }
        }
        if (all_collinear) return {};

        double max_radius_sq = 1.0 / (alpha * alpha);

        // Flatten points for delaunator
        std::vector<double> coords;
        coords.reserve(points.size() * 2);
        for (const auto& p : points) {
            coords.push_back(p.x);
            coords.push_back(p.y);
        }

        delaunator::Delaunator d(coords);

        // Mark triangles whose circumradius is below threshold
        std::vector<bool> valid_halfedges(d.halfedges.size(), false);
        for (size_t i = 0; i < d.triangles.size(); i += 3) {
            Point p1 = points[d.triangles[i]];
            Point p2 = points[d.triangles[i + 1]];
            Point p3 = points[d.triangles[i + 2]];
            if (circumradiusSq(p1, p2, p3) < max_radius_sq) {
                valid_halfedges[i] = valid_halfedges[i + 1] = valid_halfedges[i + 2] = true;
            }
        }

        // Extract boundary points
        std::set<size_t> boundary_indices;
        for (size_t i = 0; i < d.halfedges.size(); ++i) {
            if (valid_halfedges[i]) {
                size_t opp = d.halfedges[i];
                if (opp == delaunator::INVALID_INDEX || !valid_halfedges[opp]) {
                    // Include both endpoints of edges whose opposite is missing or invalid, marking them as boundary points.
                    boundary_indices.insert(d.triangles[i]);
                    size_t next_edge = (i % 3 == 2) ? i - 2 : i + 1;
                    boundary_indices.insert(d.triangles[next_edge]);
                }
            }
        }

        std::vector<Point> hull;
        for (size_t idx : boundary_indices) hull.push_back(points[idx]);
        return hull;
    }

private:
    // Compute squared circumradius of triangle
    static double circumradiusSq(const Point& p1, const Point& p2, const Point& p3) {
        double d = 2 * (p1.x*(p2.y - p3.y) + p2.x*(p3.y - p1.y) + p3.x*(p1.y - p2.y));
        if (std::abs(d) < 1e-9) return 1e99; // Collinear → large radius
        double p1sq = p1.x*p1.x + p1.y*p1.y;
        double p2sq = p2.x*p2.x + p2.y*p2.y;
        double p3sq = p3.x*p3.x + p3.y*p3.y;
        double ux = (p1sq*(p2.y - p3.y) + p2sq*(p3.y - p1.y) + p3sq*(p1.y - p2.y)) / d;
        double uy = (p1sq*(p3.x - p2.x) + p2sq*(p1.x - p3.x) + p3sq*(p2.x - p1.x)) / d;
        return (p1.x - ux)*(p1.x - ux) + (p1.y - uy)*(p1.y - uy);
    }

    // Check for collinearity.
    static int orientation(const Point& p1, const Point& p2, const Point& p3) {
        double val = (p2.y - p1.y) * (p3.x - p2.x) - (p2.x - p1.x) * (p3.y - p2.y);
        if (std::abs(val) < 1e-9) return 0;
        return (val > 0) ? 1 : -1;
}
};
