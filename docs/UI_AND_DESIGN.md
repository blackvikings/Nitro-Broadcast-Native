# Qt UI strategy, wireframe, and design system

## 4. Qt UI strategy

**Choice: Qt 6 + Qt Quick / QML** for the visual shell.  
**C++** for all application logic, models, and media.

| Criterion | Qt Quick | Qt Widgets |
|-----------|----------|------------|
| Modern dark studio UX | Excellent | Requires heavy custom styling |
| Motion / density | Excellent | Harder |
| Preview surface | Custom `QQuickItem` + RHI | Awkward with D3D/OpenGL widgets |
| Fit for Nitro brand | Preferred | Fallback only if QML blocked |

### Patterns

- QML = presentation only (minimal JS in bindings).  
- C++ `QObject` / `QAbstractListModel`: `StudioController`, `SceneListModel`, `SourceListModel`, `MixerModel`, `MetricsModel`, `DestinationModel`.  
- Preview/Program: custom `QQuickItem` consuming engine frames later; placeholders first.  
- Qt Quick Controls 2 + **custom** Nitro styling (not stock Fusion).

## 5. UI wireframe

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ [NITRO]  Profile ▾   ● LIVE / ○ Idle   ● REC   CPU%  GPU%   ⚙ Settings  👤   │
├────────────┬─────────────────────────────────────────────┬───────────────────┤
│ SCENES     │     PREVIEW              PROGRAM            │ SOURCES           │
│ ○ Starting │  ┌──────────────┐     ┌──────────────┐      │ ☑ Display Capture │
│ ● Gameplay │  │  teal edge   │ Cut │  red edge    │      │ ☑ Window Capture  │
│ ○ Webcam   │  │              │Fade │              │      │ ☑ Camera          │
│ ○ BRB      │  └──────────────┘  ▾  └──────────────┘      │ ☑ Microphone      │
│ ○ Ending   │                                             │ ☑ Desktop Audio   │
│ [+ − ⧉]    │                                             │ ☐ Image / Text /  │
│            │                                             │   Browser         │
│            │                                             │ [+ Add]  eye lock │
├────────────┴─────────────────────────────────────────────┴───────────────────┤
│ MIXER  Mic ████  Desktop ███  Game ██  Music ░  Browser ░   vol mute mon ⚙  │
├──────────────────────────────────────────────────────────────────────────────┤
│ Idle · 1080p60 · NVENC · 6000 kbps · drop 0% · CPU · GPU · net               │
│                         [ RECORD ]          [ START STREAM ]                 │
└──────────────────────────────────────────────────────────────────────────────┘
```

### Layout rules

- Resizable panels (scenes | studio | sources | mixer).  
- Preview chrome = teal; Program chrome = red/coral; Live = restrained status dot.  
- START STREAM / RECORD prominent but not oversized.  
- Default empty profile scenes: Starting Soon, Gameplay, Webcam, BRB, Ending.

### Scene / source / mixer affordances

- Scenes: add, delete, duplicate, rename, reorder.  
- Sources: add, delete, reorder, hide, lock, transform, crop, scale, position.  
- Mixer channels: Microphone, Desktop, Game, Music, Browser — volume, mute, monitor, meters, settings.

## 6. Design system (Nitro Studio Theme)

Dark-first. Light supported via token swap.

### Color tokens (dark)

| Token | Hex | Role |
|-------|-----|------|
| `bg.app` | `#0E0F12` | Window |
| `bg.panel` | `#16181D` | Sidebars |
| `bg.elevated` | `#1C1F26` | Dialogs / raised |
| `bg.input` | `#12141A` | Fields |
| `border.subtle` | `#2A2E37` | Dividers |
| `text.primary` | `#E8EAED` | Body |
| `text.secondary` | `#9AA0A6` | Labels |
| `accent` | `#5B8CFF` | Primary actions / focus (restrained blue) |
| `preview` | `#2DD4BF` | Preview border |
| `program` | `#F07178` | Program / danger-adjacent live |
| `success` | `#3DDC97` | Healthy |
| `warning` | `#F5A524` | Warning / hot meter |
| `danger` | `#E85D75` | Stop / error |
| `meter.ok` / `.warn` / `.clip` | success / warning / danger | Meters |

Avoid neon purple cascades, heavy gradients, and glow stacks.

### Typography

- UI: Inter or IBM Plex Sans (bundled).  
- Metrics: IBM Plex Mono or JetBrains Mono.  
- Sizes: 11 / 12 / 13 / 14 / 16 / 20 on a clear hierarchy.

### Spacing and shape

- 4px grid: 4, 8, 12, 16, 24.  
- Radius: 6 panels, 4 controls, 2 chips.  
- One soft shadow: `0 8px 24px rgba(0,0,0,0.35)`.

### Components

Primary / ghost / danger buttons · icon buttons · list rows · eye/lock toggles · meter bars · status dots · toasts · dialogs · segmented Cut/Fade control · text fields · combo boxes.

### Theme architecture

- QML singleton `Theme` + light/dark schemes.  
- Feature QML references tokens only — no hard-coded hex in studio panels.
