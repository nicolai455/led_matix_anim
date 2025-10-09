# Panel Layout Setup Guide

This guide helps you configure your LED matrix panels so the test pattern displays correctly.

## What You'll See

The test pattern shows:
- **4 colored panels**: Red, Green, Blue, Yellow (these represent logical panel positions)
- **White corner arrows**: Show the expected orientation of each panel
- **CYAN dot**: Marks where your LED data stream starts
- **MAGENTA arrow**: Shows the panel wiring direction (horizontal or vertical)
- **WHITE zigzag**: Appears only if panel serpentine is enabled

## Step-by-Step Setup

### Step 1: Initial Upload

Upload the code and default config to your ESP32:

```bash
pio run -t upload -t uploadfs
```

Your display will show the test pattern.

### Step 2: Identify Physical Panel Positions

Look at your physical LED matrix and note which color appears in each position:

```
┌─────────────┬─────────────┐
│             │             │
│  Top-Left   │  Top-Right  │
│             │             │
├─────────────┼─────────────┤
│             │             │
│ Bottom-Left │ Bottom-Right│
│             │             │
└─────────────┴─────────────┘
```

Write down what you see:
- Top-Left panel shows: ________ color
- Top-Right panel shows: ________ color
- Bottom-Left panel shows: ________ color
- Bottom-Right panel shows: ________ color

**Goal:** We want Red=Top-Left, Green=Top-Right, Blue=Bottom-Left, Yellow=Bottom-Right

### Step 3: Set Start Corner

The CYAN dot tells you where the LED data stream starts.

**Look at your matrix:**
- Where is the CYAN dot located? (which corner of the 32x32 matrix)

**Edit `data/config/panel_config.json`:**

| CYAN Dot Location | Set `startCorner` to |
|-------------------|----------------------|
| Top-Left corner | `0` |
| Top-Right corner | `1` |
| Bottom-Left corner | `2` |
| Bottom-Right corner | `3` |

Example:
```json
{
  "startCorner": 0,
  ...
}
```

### Step 4: Set Panel Layout Direction

The MAGENTA arrow shows which direction the panels are wired.

**Look at your matrix:**
- Is the MAGENTA arrow pointing **RIGHT** (→) or **DOWN** (↓)?

**Determine your actual wiring:**
- After the first panel is filled (256 LEDs), which panel receives data next?
- Horizontal wiring: Goes to the panel on the right, then next row
- Vertical wiring: Goes to the panel below, then next column

**Edit `data/config/panel_config.json`:**

| Your Wiring Pattern | Set `panelLayout` to |
|---------------------|----------------------|
| Panels wired **horizontally** (left-to-right, then down) | `0` |
| Panels wired **vertically** (top-to-bottom, then right) | `1` |

Example:
```json
{
  "startCorner": 0,
  "panelLayout": 0,
  ...
}
```

### Step 5: Set Panel Serpentine

Panel serpentine means the wiring zigzags between rows or columns of panels.

**Determine your actual wiring:**

Trace how your panels are connected:

**For Horizontal layout (`panelLayout: 0`):**
- **Non-serpentine (straight)**: 0→1, 2→3 (both rows go left-to-right)
- **Serpentine (zigzag)**: 0→1, 3←2 (second row reverses direction)

**For Vertical layout (`panelLayout: 1`):**
- **Non-serpentine (straight)**: 0→2, 1→3 (both columns go top-to-bottom)
- **Serpentine (zigzag)**: 0→2, 3←1 (second column reverses direction)

**Edit `data/config/panel_config.json`:**

| Your Panel Wiring | Set `panelSerpentine` to |
|-------------------|--------------------------|
| Straight (no zigzag between panels) | `false` |
| Zigzag between panel rows/columns | `true` |

Example:
```json
{
  "startCorner": 0,
  "panelLayout": 0,
  "panelSerpentine": false
}
```

**Note:** The WHITE zigzag in the test pattern is a **visual indicator** that only appears when `panelSerpentine` is set to `true` in the config. It doesn't tell you what to set - you need to determine your wiring by tracing the physical connections between panels.

### Step 6: Upload Config and Test

Upload just the filesystem (config file):

```bash
pio run -t uploadfs
```

The ESP32 will reboot and load the new config.

**Check the result:**
- Red panel should be at Top-Left
- Green panel should be at Top-Right
- Blue panel should be at Bottom-Left
- Yellow panel should be at Bottom-Right
- White arrows should point to the outer corners
- CYAN dot should be at the correct start corner
- MAGENTA arrow should match your wiring direction

If the colors are still wrong, continue to Step 7.

### Step 7: Fine-Tune Individual Panels (If Needed)

If colors are still in the wrong positions, you may need to adjust individual panel settings.

#### Option A: Adjust Panel Order

Edit `data/config/panel_config.json` and change the `panelOrder` array:

```json
{
  "panelOrder": [0, 1, 2, 3],  // Default: no remapping
  ...
}
```

The array maps **logical positions** (TL, TR, BL, BR) to **physical LED stream positions**:
- `[0, 1, 2, 3]` = no remapping (works for both horizontal and vertical)

**For Horizontal Layout (`panelLayout: 0`):**
- `[1, 0, 2, 3]` = swap Top-Left with Top-Right
- `[2, 3, 0, 1]` = swap top row with bottom row
- `[0, 1, 3, 2]` = swap Bottom-Left with Bottom-Right

**For Vertical Layout (`panelLayout: 1`):**
- `[2, 1, 0, 3]` = swap Top-Left with Bottom-Left
- `[1, 3, 2, 0]` = swap left column with right column
- `[0, 3, 2, 1]` = swap Top-Right with Bottom-Right

#### Option B: Adjust Panel Rotation

If individual panels are rotated, edit the `panelRotation` array:

```json
{
  "panelRotation": [0, 0, 0, 0],  // Default: no rotation
  ...
}
```

Values: `0` = no rotation, `90` = 90° clockwise, `180` = 180°, `270` = 270° clockwise

#### Option C: Adjust Individual Panel Serpentine

If specific panels have zigzag wiring internally, edit the `serpentine` array:

```json
{
  "serpentine": [true, true, true, true],  // Default: all serpentine
  ...
}
```

After each change, upload and test:
```bash
pio run -t uploadfs
```

## Common Wiring Patterns

### Pattern 1: Standard Horizontal
**Description:** Panels wired left-to-right, starting top-left
```
0 → 1
↓   ↓
2 → 3
```

```json
{
  "startCorner": 0,
  "panelLayout": 0,
  "panelSerpentine": false
}
```

### Pattern 2: Horizontal Zigzag
**Description:** Panels wired in zigzag rows
```
0 → 1
    ↓
3 ← 2
```

```json
{
  "startCorner": 0,
  "panelLayout": 0,
  "panelSerpentine": true
}
```

### Pattern 3: Vertical
**Description:** Panels wired top-to-bottom, starting top-left
```
0   1
↓   ↓
2   3
```

```json
{
  "startCorner": 0,
  "panelLayout": 1,
  "panelSerpentine": false
}
```

### Pattern 4: Vertical Zigzag
**Description:** Panels wired in zigzag columns
```
0   3
↓   ↑
2 → 1
```

```json
{
  "startCorner": 0,
  "panelLayout": 1,
  "panelSerpentine": true
}
```

### Pattern 5: Start Bottom-Right
**Description:** Data starts from bottom-right corner
```
2 ← 1
    ↑
3 ← 0
```

```json
{
  "startCorner": 3,
  "panelLayout": 0,
  "panelSerpentine": false
}
```

## Troubleshooting

### Problem: Colors are in wrong positions

**Solution:**
1. Verify `startCorner` matches where the CYAN dot appears
2. Verify `panelLayout` matches where the MAGENTA arrow points
3. Try toggling `panelSerpentine` between `true` and `false`
4. If still wrong, manually adjust `panelOrder` array

### Problem: White arrows point in wrong directions

**Solution:**
- This means individual panels need rotation
- Identify which panel(s) are wrong
- Add rotation to those panels in `panelRotation` array
- Example: If Top-Right panel is rotated 90°, use `"panelRotation": [0, 90, 0, 0]`

### Problem: Pixels inside a panel are scattered

**Solution:**
- The individual panel has serpentine wiring
- Adjust the corresponding value in the `serpentine` array
- Example: If Top-Left panel is serpentine, use `"serpentine": [true, false, false, false]`

### Problem: Config changes don't take effect

**Solution:**
1. Make sure you ran `pio run -t uploadfs` (not just `upload`)
2. The ESP32 must reboot to load new config
3. Check Serial Monitor for "Loading configuration from file" message

## Visual Reference

```
Expected Test Pattern:
┌──────────────────┬──────────────────┐
│ RED              │ GREEN            │
│   ┌────→         │   ┌────→         │
│   │              │   │              │
│   │              │   │              │
│   ↓              │   ↓              │
│ ●CYAN            │                  │
│ (start)          │                  │
│                  │                  │
├──────────────────┼──────────────────┤
│ BLUE             │ YELLOW           │
│   ┌────→         │   ┌────→         │
│   │              │   │   →MAG       │
│   │              │   │   (layout)   │
│   ↓              │   ↓   ⚡WHITE    │
│                  │       (serpent)  │
│                  │                  │
│                  │                  │
└──────────────────┴──────────────────┘
```

## Quick Reference: Config Values

```json
{
  "matrixWidth": 2,           // Number of panels horizontally
  "matrixHeight": 2,          // Number of panels vertically
  "startCorner": 0,           // 0=TL, 1=TR, 2=BL, 3=BR
  "panelLayout": 0,           // 0=Horizontal, 1=Vertical
  "panelSerpentine": false,   // true=Zigzag, false=Straight
  "panelOrder": [0, 1, 2, 3], // Logical to physical mapping
  "panelRotation": [0, 0, 0, 0],     // 0, 90, 180, or 270 degrees
  "serpentine": [true, true, true, true]  // Per-panel serpentine
}
```

## Still Need Help?

1. Take a photo of your display showing the test pattern
2. Write down which color appears in each physical position
3. Describe your wiring: which panel gets data first, second, third, fourth?
4. Share this info and we can determine the correct config values

Once configured correctly, the system works automatically for all your LED animations!
