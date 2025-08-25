const express = require('express');
const path = require('path');
const fs = require('fs');

// Load WASM binary
const wasmFilePath = path.join(__dirname, 'convexhull.wasm');
const wasmBinary = fs.readFileSync(wasmFilePath);

// Import Emscripten module
const createWasmModule = require('./convexhull.js');

const app = express();
app.use(express.json());

let wasmModule;

createWasmModule({ wasmBinary })
  .then((module) => {
    wasmModule = module;
    console.log("WebAssembly module loaded.");
  })
  .catch((err) => {
    console.error("Failed to initialize WebAssembly module:", err);
  });

// Serve HTML and JS
app.get('/', (req, res) => res.sendFile(path.join(__dirname, 'index.html')));
app.get('/main.js', (req, res) => res.sendFile(path.join(__dirname, 'main.js')));

// Convex hull endpoint
app.post('/convexhull', (req, res) => {
  if (!wasmModule) return res.status(503).json({ error: "WASM module not ready." });

  const pointsData = req.body.points || [];
  if (pointsData.length < 3) return res.json({ hull: [] });

  // Prepare VectorDouble
  const vectorDouble = new wasmModule.VectorDouble();
  pointsData.forEach(p => { vectorDouble.push_back(p.x); vectorDouble.push_back(p.y); });

  // Compute hull
  const flatHullResult = wasmModule.computeConvexHull(vectorDouble);

  // Convert result to JS array
  const hullPoints = [];
  for (let i = 0; i < flatHullResult.size(); i += 2) {
    hullPoints.push({ x: flatHullResult.get(i), y: flatHullResult.get(i + 1) });
  }

  // Free WASM memory
  flatHullResult.delete();
  vectorDouble.delete();

  res.json({ hull: hullPoints });
});

app.post('/concavehull', (req, res) => {
    if (!wasmModule) return res.status(503).json({ error: "WASM module not ready." });

    const pointsData = req.body.points || [];
    const alpha = req.body.alpha || 0.1; // Default alpha if not provided

    if (pointsData.length < 3) return res.json({ hull: [] });

    let inputVector = null;
    let outputVector = null;
    
    try {
        inputVector = new wasmModule.VectorDouble();
        pointsData.forEach(p => { inputVector.push_back(p.x); inputVector.push_back(p.y); });

        // Call the new C++ function from Wasm module, passing the alpha value.
        outputVector = wasmModule.computeConcaveHull(inputVector, alpha);

        const hullPoints = [];
        const hullSize = outputVector.size();
        for (let i = 0; i < hullSize; i += 2) {
            hullPoints.push({ x: outputVector.get(i), y: outputVector.get(i + 1) });
        }
        
        res.json({ hull: hullPoints });

    } finally {
        if (inputVector) inputVector.delete();
        if (outputVector) outputVector.delete();
    }
});

// Start server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});
