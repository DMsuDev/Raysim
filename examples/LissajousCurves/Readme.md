# Lissajous Curves

Interactive visualization of Lissajous curves: the figures traced by two
perpendicular sinusoidal oscillators with different frequencies and a variable
phase shift between them.

## Parametric equations

```math
x(t) = A · sin(nx · t + φ)
```

```math
y(t) = A · sin(ny · t)
```

| Symbol     | Meaning                                   |
| ---------- | ----------------------------------------- |
| `nx`, `ny` | Oscillation frequency on each axis        |
| `φ`        | Phase shift between the two oscillators   |
| `A`        | Amplitude (curve radius, fixed at 300 px) |
| `t`        | Time parameter, sampled over `[0, 2π]`    |

The **ratio nx : ny** determines the shape of the curve. Integer ratios produce
closed, periodic figures; irrational ratios produce dense, space-filling paths.

## Built-in presets

| Key | Name            | Ratio |
| --- | --------------- | ----- |
| `1` | Diagonal Line   | 1 : 1 |
| `2` | Figure Eight    | 2 : 1 |
| `3` | Parabola        | 1 : 2 |
| `4` | 3-Petal Rose    | 3 : 2 |
| `5` | 4-Petal Rose    | 3 : 1 |
| `6` | Complex Pattern | 5 : 4 |

## Interactive controls

| Input     | Action                                    |
| --------- | ----------------------------------------- |
| `←` / `→` | Decrease / increase X frequency (`nx`)    |
| `↑` / `↓` | Increase / decrease Y frequency (`ny`)    |
| `P`       | Set phase rotation direction -> forward   |
| `O`       | Set phase rotation direction -> backward  |
| `+` / `-` | Increase / decrease phase rotation speed  |
| `1`–`6`   | Load a preset (resets phase to 0)         |
| `R`       | Reset to defaults (nx = 3, ny = 2, φ = 0) |

> Frequency changes are smoothed with exponential interpolation to avoid
> abrupt visual jumps.

---

## References

- [Lissajous curve - Wikipedia](https://en.wikipedia.org/wiki/Lissajous_curve)
