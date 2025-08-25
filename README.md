# Hull Visualizer

Hull Visualizer is an interactive web application that lets users create 2D point clouds and visualize **Convex** and **Concave Hulls** in real-time. Points can be added dynamically, and the hulls update instantly. The live application is deployed on Render: [https://hull-visualizer.onrender.com/](https://hull-visualizer.onrender.com/)

## Algorithms Implemented

### Convex Hull (Graham Scan)

The **Convex Hull** is the smallest convex polygon that encloses all points. This project includes a **from-scratch C++ implementation of Graham Scan**:

1. **Pivot Selection**: The point with the lowest y-coordinate (and lowest x-coordinate if tied) is chosen as the pivot.
2. **Sorting by Polar Angle**: All other points are sorted counterclockwise relative to the pivot.
3. **Stack-Based Hull Construction**: Points are processed sequentially:
   - A **left turn** (-1 in orientation function) keeps the point in the hull.
   - A **right turn** (1) indicates concavity, so the previous point is removed.
4. The resulting stack contains the convex hull vertices in counterclockwise order.

Additional steps:
- Collinear points with respect to the pivot are filtered to avoid duplicates.
- Distance squared is used for tie-breaking during sorting.

### Concave Hull (Alpha Shapes)

The **Concave Hull** produces a tighter, non-convex shape. The shape does not have to bound all the points. This project implements **Alpha Shapes from scratch in C++**, using **Delaunay triangulation** (via `delaunator.hpp`) to structure the points:

1. **Delaunay Triangulation**: Compute triangles connecting all points.
2. **Triangle Filtering**: Remove triangles whose circumradius exceeds a threshold `1 / alpha^2`.
   - Small alpha → more triangles are valid → hull approaches the convex hull.
   - Large alpha → fewer triangles are valid → hull becomes more concave and detailed.
3. **Boundary Extraction**: Edges of valid triangles whose opposite edge is missing or invalid are collected to form the concave hull.
4. Collinear points are handled explicitly to avoid degenerate hulls.

Both hull algorithms are fully written in C++, with only the Delaunay triangulation imported. They are compiled to WebAssembly (Wasm) for fast execution in the browser.

## Technical Architecture

- **High-Performance Core**: C++ computations compiled to Wasm using Emscripten.
- **Interactive Frontend**: Vanilla JavaScript and HTML Canvas for adding points and rendering hulls in real-time.
- **Deployment**: Node.js serves the frontend and Wasm module. The application is deployed on Render for easy access.

## Key Features

- Real-time convex and concave hull visualization
- Adjustable alpha parameter for concave hulls
- Efficient, smooth performance for large point clouds
- Clean and intuitive interface for exploring geometric hulls
