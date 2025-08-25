// --- Global State & Element References ---
const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');
const hullTypeSelect = document.getElementById('hull-type');
const alphaSliderContainer = document.getElementById('alpha-slider-container');
const alphaSlider = document.getElementById('alpha-slider');
const alphaValueSpan = document.getElementById('alpha-value');
let points = [];

// --- Event Listeners ---

// Add a point when the canvas is clicked
canvas.addEventListener('click', (event) => {
    const rect = canvas.getBoundingClientRect();
    points.push({ x: event.clientX - rect.left, y: event.clientY - rect.top });
    getHull();
});

// Update the UI and recalculate when the hull type changes
hullTypeSelect.addEventListener('change', () => {
    alphaSliderContainer.style.display = (hullTypeSelect.value === 'concave') ? 'flex' : 'none';
    getHull();
});

// Update the display and recalculate when the alpha slider is moved
alphaSlider.addEventListener('input', () => {
    alphaValueSpan.textContent = parseFloat(alphaSlider.value).toFixed(4);
    getHull();
});

// Expose the clearCanvas function to the global scope so the button's onclick can find it
window.clearCanvas = () => {
    points = [];
    draw([], []); // Clear the canvas by drawing with empty point arrays
};

// --- Core Logic: Fetch and Draw ---

// This single function handles all requests to the backend.
async function getHull() {
    let hull = [];
    if (points.length >= 3) {
        const hullType = hullTypeSelect.value;
        const endpoint = `/${hullType}hull`; // Dynamically builds '/convexhull' or '/concavehull'
        const body = {
            points: points,
            alpha: parseFloat(alphaSlider.value)
        };

        try {
            const response = await fetch(endpoint, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(body),
            });
            const data = await response.json();
            hull = data.hull || [];
        } catch (error) {
            console.error(`Error fetching ${hullType} hull:`, error);
        }
    }
    // After any action, redraw the entire canvas with the latest data.
    draw(points, hull);
}

// --- Drawing ---

// This single function handles all top-level drawing operations.
function draw(allPoints, hullPoints) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // 1. Draw all the points the user has clicked
    ctx.fillStyle = 'blue';
    for (const p of allPoints) {
        ctx.beginPath();
        ctx.arc(p.x, p.y, 5, 0, 2 * Math.PI);
        ctx.fill();
    }

    // 2. Decide which specialized drawing function to call for the hull
    const hullType = hullTypeSelect.value;
    if (hullType === 'convex') {
        drawConvexHull(hullPoints);
    } else {
        drawConcaveHull(hullPoints);
    }
}

// Specialized drawing function for the CONVEX hull (ordered points)
function drawConvexHull(hullPoints) {
    if (hullPoints.length < 2) return;
    ctx.strokeStyle = 'red';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(hullPoints[0].x, hullPoints[0].y);
    for (let i = 1; i < hullPoints.length; i++) {
        ctx.lineTo(hullPoints[i].x, hullPoints[i].y);
    }
    if (hullPoints.length > 2) {
        ctx.closePath();
    }
    ctx.stroke();
}

// Specialized drawing function for the CONCAVE hull (unordered points)
function drawConcaveHull(hullPoints) {
    if (hullPoints.length < 2) return;

    // --- SORTING LOGIC TO FIX "SPIDERWEB" DRAWING ---
    // 1. Find the geometric center (centroid) of the hull points.
    let avgX = 0;
    let avgY = 0;
    for (const p of hullPoints) {
        avgX += p.x;
        avgY += p.y;
    }
    avgX /= hullPoints.length;
    avgY /= hullPoints.length;
    const center = { x: avgX, y: avgY };

    // 2. Sort the points by the angle they make with the center.
    hullPoints.sort((a, b) => {
        const angleA = Math.atan2(a.y - center.y, a.x - center.x);
        const angleB = Math.atan2(b.y - center.y, b.x - center.x);
        return angleA - angleB;
    });
    // --- END OF SORTING LOGIC ---

    // Now, draw the sorted points to form a clean polygon
    ctx.strokeStyle = 'green';
    ctx.lineWidth = 2;
    ctx.beginPath();
    ctx.moveTo(hullPoints[0].x, hullPoints[0].y);
    for (let i = 1; i < hullPoints.length; i++) {
        ctx.lineTo(hullPoints[i].x, hullPoints[i].y);
    }
    if (hullPoints.length > 2) {
        ctx.closePath();
    }
    ctx.stroke();
}