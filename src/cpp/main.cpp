#include <vector>
#include <emscripten/bind.h>
#include "convex_hull.hpp"
#include "concave_hull.hpp"


using namespace emscripten;

std::vector<double> computeConvexHull(const std::vector<double>& flatPoints) {
    std::vector<Point> pts;
    pts.reserve(flatPoints.size() / 2);
    for (size_t i = 0; i < flatPoints.size(); i += 2) {
        pts.push_back(Point(flatPoints[i], flatPoints[i + 1]));
    }
    std::vector<Point> hull = ConvexHull::compute(pts);
    std::vector<double> result;
    result.reserve(hull.size() * 2);
    for (const auto& p : hull) {
        result.push_back(p.x);
        result.push_back(p.y);
    }
    return result;
}

std::vector<double> computeConcaveHull(const std::vector<double>& flatPoints, double alpha) {
    std::vector<Point> pts;
    pts.reserve(flatPoints.size() / 2);
    for (size_t i = 0; i < flatPoints.size(); i += 2) {
        pts.push_back(Point(flatPoints[i], flatPoints[i + 1]));
    }
    std::vector<Point> hull = ConcaveHull::compute(pts, alpha);
    std::vector<double> result;
    result.reserve(hull.size() * 2);
    for (const auto& p : hull) {
        result.push_back(p.x);
        result.push_back(p.y);
    }
    return result;
}

EMSCRIPTEN_BINDINGS(my_module) {
    register_vector<double>("VectorDouble");
    function("computeConvexHull", &computeConvexHull);
    function("computeConcaveHull", &computeConcaveHull);
}