#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "point.hpp"

class ConvexHull {
public:
    static std::vector<Point> compute(std::vector<Point> points) {
        if (points.size() < 3) {
            return {};
        }

        findAndPlacePivot(points);
        sortByPolarAngle(points);
        std::vector<Point> unique_points = filterCollinearPoints(points);

        if (unique_points.size() < 3) {
            return {};
        }
        return buildHull(unique_points);
    }

private:
    static void findAndPlacePivot(std::vector<Point>& points) {
        auto pivot_it = std::min_element(points.begin(), points.end(), 
            [](const Point& a, const Point& b) {
                return a.y < b.y || (a.y == b.y && a.x < b.x);
            });
        std::iter_swap(points.begin(), pivot_it);
    }

    static void sortByPolarAngle(std::vector<Point>& points) {
        if (points.empty()) return;
        const Point& pivot = points[0];
        std::sort(points.begin() + 1, points.end(), 
            [&pivot](const Point& a, const Point& b) {
                int o = orientation(pivot, a, b);
                if (o == 0) {
                    return distance2(pivot, a) < distance2(pivot, b);
                }
                return o == -1;
            });
    }

    static std::vector<Point> filterCollinearPoints(const std::vector<Point>& sorted_points) {
        std::vector<Point> unique_points;
        if (sorted_points.empty()) return unique_points;
        unique_points.push_back(sorted_points[0]);
        for (int i = 1; i < sorted_points.size(); i++) {
            while (i < sorted_points.size() - 1 && 
                   orientation(sorted_points[0], sorted_points[i], sorted_points[i+1]) == 0) {
                i++;
            }
            unique_points.push_back(sorted_points[i]);
        }
        return unique_points;
    }

    static std::vector<Point> buildHull(const std::vector<Point>& unique_points) {
        std::vector<Point> hull;
        for (const auto& p : unique_points) {
            while (hull.size() > 1 && orientation(hull[hull.size() - 2], hull.back(), p) == 1) {
                hull.pop_back();
            }
            hull.push_back(p);
        }
        return hull;
    }

    static double distance2(const Point& a, const Point& b) {
        return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
    }

    static int orientation(const Point& a, const Point& b, const Point& c) {
        double val = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if (std::abs(val) < 1e-9) return 0;
        return (val > 0) ? -1 : 1; // -1 for CCW, 1 for CW
    }
};