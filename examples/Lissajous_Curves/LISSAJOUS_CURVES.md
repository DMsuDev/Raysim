# Lissajous Curves: Interactive Mathematical Curves

## What are Lissajous Curves?

Lissajous curves are geometric figures traced by combining two perpendicular oscillatory motions with different frequencies. They are named after the French physicist Jules Antoine Lissajous, who studied them in the 19th century.

### Mathematical Definition

A Lissajous curve is defined by the parametric equations:

$$x(t) = A \sin(n_x \cdot t + \phi)$$
$$y(t) = A \sin(n_y \cdot t)$$

Where:

- $t$: time parameter (from 0 to $2\pi$)
- $A$: amplitude (curve radius)
- $n_x$: frequency along X
- $n_y$: frequency along Y
- $\phi$: phase shift between the two oscillations

### Key Components

1. Frequencies ($n_x$ and $n_y`)
   - Determine how many full waves are drawn on each axis
   - The ratio $n_x:n_y$ defines the curve shape (1:1 line, 2:1 figure-eight ...)

2. Phase shift ($\phi$)
   - Temporal offset between oscillations; changes orientation/rotation

3. Amplitude ($A$)
   - Controls curve size (in this example $A$ = 300 pixels)

## Code point generation

```cpp
float x = 300.0f * std::sin(smoothFrequencyX * t + phaseShift);
float y = 300.0f * std::sin(smoothFrequencyY * t);
curvePoints[i].x = 600.0f + x; // centered at (600, 400)
curvePoints[i].y = 400.0f + y;
```

## Interactive example notes

- Resolution: 1200 points
- Color gradient: 6 colors interpolated across the curve
- Controls: Arrows change frequencies; P/O rotate phase; 1-6 presets; R reset

## Extensions

- 3D Lissajous curves
- Audio-reactive frequencies
- Export to SVG
- Multiple curves overlay

## References

- Wikipedia: Lissajous curve
- Wolfram MathWorld: Lissajous Curve
