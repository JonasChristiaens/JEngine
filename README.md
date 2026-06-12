# JEngine — Programming 4 Game Engine Project

**JEngine** is a 2D game engine built in C++20 for the *Programming 4* course at [DAE](https://digitalartsandentertainment.be/). The engine recreates **Bomberman** (1983), a classic arcade game, while putting game programming patterns and engine architecture into practice.

**Source control**: [github.com/JonasChristiaens/JEngine](https://github.com/JonasChristiaens/JEngine)

**Live web build**: [JEngine](https://jonaschristiaens.github.io/JEngine/)

[![Build Status](https://github.com/JonasChristiaens/JEngine/actions/workflows/cmake.yml/badge.svg)](https://github.com/JonasChristiaens/JEngine)
[![Build Status](https://github.com/JonasChristiaens/JEngine/actions/workflows/emscripten.yml/badge.svg)](https://github.com/JonasChristiaens/JEngine)

---

## Project Structure

```
JEngine/
├── Minigin/          # Engine library (static lib)
│   ├── Core/         # Game loop, time, singleton CRTP
│   ├── Scene/        # GameObject (ECS), Scene, SceneManager
│   ├── Components/   # Transform, Render, Collision, Animation
│   ├── Input/        # Keyboard + gamepad (XInput/SDL), command binding
│   ├── Rendering/    # SDL3 renderer wrapper, texture cache
│   ├── Resources/    # Texture & text file loading
│   ├── EventQueue/   # Observer-based event system with queued dispatch
│   ├── Audio/        # Service locator, SDL3_mixer (dedicated thread)
│   ├── Commands/     # Abstract Command base class
│   └── State/        # Generic finite state machine
└── Bomberman/        # Game executable
	├── Scenes/       # Scene builder, player/mode setup
	├── State/        # Title, Game, End, Transition scene states
	├── Components/   # Health, Bomb, Enemy AI, Camera, HUD, etc.
	├── Commands/     # Move, SpawnBomb, Detonate, SkipLevel, ToggleMute
	├── Observers/    # Bomb event handler, entity death handler
	├── Managers/     # Explosion effects, hidden items, high scores
	├── Powerups/     # Strategy pattern: Flames, ExtraBomb, Detonator, Skate
	├── Factories/    # EnemyFactory
	├── Level/        # Binary level loader (.bin), playfield grid
	└── Config/       # Enemy configs, game mode enum
```

**Two CMake targets**: `minigin` (static library) and `bomberman` (executable). The engine has no knowledge of the game, all game-specific code lives in `Bomberman/`.

---

## Engine Architecture & Design Choices

### Game Loop

`Minigin::RunOneFrame()` runs at ~60 FPS with a fixed sleep-based cap:

```
Process SDL events → Input dispatch → Reset camera → Queued events → Scene.Update() → Scene.LateUpdate() → Render → Sleep(~16ms)
```

Delta time is accumulated each frame and exposed via `GameTime::GetDeltaTime()`.

### Entity-Component System (ECS)

`GameObject` acts as the entity. Components derive from `BaseComponent` and are stored in a `vector<unique_ptr<BaseComponent>>` indexed by `type_index` for O(1) lookup. One component per type enforced. Components are created via templated `AddComponent<T>(args...)`.

**Why this over data-oriented ECS**: For a game of this scale (tens of objects, not thousands), the flexibility of runtime component attachment and the simplicity of virtual `Update()` seemed like the perfect choice.

### Command Pattern

Abstract `Command` with `Execute()` decouples input from action. `InputManager` binds keyboard keys and gamepad buttons to `unique_ptr<Command>`. Concrete commands (`MoveCommand`, `SpawnBombCommand`, etc.) hold a back-pointer to their `GameObject` actor.

### Observer & Event Queue

`EventManager` is both Singleton and `Subject`. Events carry a compile-time SDBM hash ID (`make_sdbm_hash("PlaceBombEvent")`) and up to 8 variant arguments (`int | float | void*`). I have set up 2 dispatch modes:
- **Queued**: `BroadcastEvent()` defers to next frame's `ProcessQueuedEvents()`, which prevents ordering issues when events trigger cascading changes.
- **Immediate**: `BroadcastImmediate()` fires synchronously. This is used for audio and critical state changes.

Gameplay systems communicate exclusively through events (`PlaceBombEvent`, `DetonateBombEvent`, `ChangeHealthEvent`, `ChangeScoreEvent`, `EntityDied`). No system directly calls another system's methods.

### State Machine

Generic `State` + `StateMachine` (`OnEnter` / `OnExit` / `Update` / `Render`). Used for scene-level flow (Title → Game → End) and enemy AI (Idle). Scene states self-manage input bindings and scene creation/teardown. Transition between states is type-safe via `unique_ptr<State>`.

### Service Locator (Audio)

`ServiceLocator` provides global access to `ISoundService` with a null-object fallback. The concrete `SoundServiceSdlMixer` runs a **dedicated audio thread** with a command queue (Preload, Play, Stop, StopAll), communicating via `mutex` + `condition_variable` + `atomic`. Audio events flow through `AudioEventObserver` (registered once globally), which translates `PlayAudioEvent` → `ISoundService::PlaySound()`.

### Collision

`CollisionGrid` is a spatial hash grid for broad-phase AABB queries. `CollisionComponent` supports triggers (non-blocking overlap callbacks), collision filters (`function<bool(GameObject*)>`), and on-collision callbacks. Movement uses speculative queries: `WouldCollide(hypotheticalPosition)` tests before committing a move.

### Rendering

SDL3 renderer with camera offset applied to all draw calls. `RenderComponent` supports spritesheet source rects, independent scale, pivot points, and destination size overrides. `SpriteAnimatorComponent` drives the source rect orthogonally via grid-based or explicit frame lists. Render order is done by assigned layer.

### Resource Management

`ResourceManager` caches textures by filename via `unordered_map`. Data path resolved at startup (`./Data/` or `../Data/`). `Texture2D` is an RAII move-only wrapper around `SDL_Texture` with nearest-neighbor filtering (pixel art).

---

## Game Features

- **3 game modes**: Solo, Co-op, Versus (1v1: player vs. player-controlled enemy)
- **5 stages** with randomized soft block placement, increasing difficulty
- **4 enemy types** (Balloom, Oneal, Doll, Minvo) with distinct AI: none / chase-Y / chase-X / chase-both with line-of-sight
- **4 power-ups**: Extra Bomb, Flames, Detonator, Skate — hidden inside soft blocks
- **Chain scoring**: successive kills within 0.5s multiply points (100 → 200 → 400 → … → 10,000)
- **Persistent highscore list** (4 entries, 4-letter arcade-style name entry, saved to disk)
- **Level skip** (F1), **mute toggle** (F2)
- **Keyboard + gamepad** fully supported in all menus and gameplay
- **WebAssembly build** for browser play (Emscripten)

---

## Controls

### Keyboard (Player 1)

| Key | Action |
|-----|--------|
| **W** | Move up |
| **A** | Move left |
| **S** | Move down |
| **D** | Move right |
| **R** | Place bomb |
| **B** | Detonate bomb (requires Detonator power-up) |

### Gamepad

| Button | Action |
|--------|--------|
| **D-Pad** | Move |
| **Y** | Place bomb |
| **B** | Detonate bomb (requires Detonator power-up) |

### Menus (Title / High Score)

| Input | Action |
|-------|--------|
| **Left / Right** (or **Up / Down** for score entry) | Navigate |
| **Enter** | Confirm |
| **D-Pad** | Navigate |
| **X** | Confirm |

### Global

| Key | Action |
|-----|--------|
| **F1** | Skip current level |
| **F2** | Toggle mute |

### Versus Mode

Player 1 uses keyboard (WASD + R + B). Opponent controlled by a second human on **Controller 0** (D-Pad to move). Any additional controllers (2–3) join as extra enemies.

---

## Patterns Used

| Pattern | Where |
|---------|-------|
| **Component (ECS)** | `GameObject` + `BaseComponent` hierarchy |
| **Command** | Input binding: `MoveCommand`, `SpawnBombCommand`, etc. |
| **Observer** | `EventManager` + `IObserver` across all gameplay systems |
| **Event Queue** | `EventManager::BroadcastEvent()` → deferred dispatch |
| **State** | `State` + `StateMachine` for scene flow and enemy AI |
| **Singleton** | `Renderer`, `InputManager`, `ResourceManager`, `SceneManager`, `GameTime`, `EventManager` |
| **Service Locator** | `ServiceLocator::GetSoundService()` |
| **Null Object** | `NullSoundService` (Emscripten fallback) |
| **Factory** | `EnemyFactory`, `GameplaySceneBuilder` |
| **PIMPL** | `KeyboardInput`, `ControllerInput`, `SoundServiceSdlMixer` |

---

## Dependencies

- **SDL3** v3.4.0 — windowing, input, rendering
- **SDL3_mixer** v3.2.0 — audio playback
- **glm** v1.0.3 — vector math
- **Dear ImGui** v1.92.7 — debug UI
- **Steamworks SDK 1.63** — (optional) Steam achievements integration

All fetched via CMake `FetchContent` except Steamworks which is present locally. Windows debug builds optionally use Visual Leak Detector.

---

## Credits

- **Project starting point**: [Minigin](https://github.com/avadae/minigin) by Alex Vanden Abeele
- **Patterns reference**: [Game Programming Patterns](https://gameprogrammingpatterns.com/) by Robert Nystrom
- **SDL3**, **SDL3_mixer**, **glm**, **Dear ImGui** — open-source libraries under their respective licenses
