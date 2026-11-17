# Legends-of-Azeroth Pandaria 5.4.8 — Agent Instructions

## Build

```
mkdir build && cd build
cmake .. -D<OPTION>=<VAL>
make -j$(nproc)
```

Default build type is `Release`. CMake disallows in-source builds.

**Key CMake options** (`cmake/options.cmake`):
- `SCRIPTS=1` — build core with custom scripts (default ON)
- `TOOLS=0` — map/vmap/mmap extractors (default OFF; enable for map tooling)
- `ELUNA=1` — Lua engine (default ON)
- `PLAYERBOTS=1` — player bot AI (default ON; see README quirks below)
- `COREPCH/SCRIPTPCH=1` — precompiled headers (default ON; set to 0 for faster incremental rebuilds)
- `BUILD_DEPLOY=1` — Unix only (default ON)

Debug builds: `cmake .. -DCMAKE_BUILD_TYPE=Debug`

## Requirements

- **Compiler**: GCC ≥ 13 or Clang ≥ 12 (Windows: MSVC ≥ 2019 v16.4)
- **CMake** ≥ 3.16 (README says 3.27.2)
- **MySQL** 5.7 or 8.0-8.1 (note: OpenSSL 3.2.0 not supported with MySQL ≥ 8.0.33)
- **Boost** ≥ 1.85 (Linux), ≥ 1.81 MSVC
- **OpenSSL** 1.1.1 or 3.0–3.1.1
- **C++20** (enforced via `CMAKE_CXX_STANDARD`)

## Architecture

This is a **WoW private server** (Mists of Pandaria 5.4.8 client patch).

- `src/server/shared/` — shared library (packets, networking, data stores, threading)
- `src/server/authserver/` — authentication server
- `src/server/worldserver/` — game world server
- `src/server/game/` — core game logic (entities, spells, quests, combat, maps, AI, etc.)
- `src/server/scripts/` — custom C++ game scripts, organized by region/expansion (Pandaria, EasternKingdoms, Kalimdor, etc.)
- `src/server/database/` — database record definitions
- `modules/` — pluggable module system (loaded via `ModulesLoader`)
- `mod_playerbots/` — AI player bot module (early stage)
- `src/tools/` — map_extractor, vmap4_extractor/assembler, mmaps_generator
- `dep/` — vendored dependencies (Boost, fmt, StormLib, g3dlite, MySQL client, OpenSSL, RecastNavigation, etc.)
- `sql/base/` — base DB schema (`auth.sql`, `characters.sql`, `world.sql`)
- `sql/updates/` — incremental DB migration SQL, organized by date
- `contrib/lua_scripts/` — live Eluna Lua scripts (deployed alongside binaries)

## SQL / Database

- Base schema lives in `sql/base/` — apply these first
- Incremental updates in `sql/updates/` — ordered by filename prefix (date-based)
- Three databases: `auth`, `characters`, `world`
- Playerbots module uses its own DB: `mop_playerbots` (configured in `worldserver.conf`)

## Eluna Lua Engine

When `ELUNA=1`, the server loads `contrib/lua_scripts/` at runtime.
- **Quirk**: `mod_playerbots` is **HIGHLY recommended to disable** when Eluna is enabled (conflicts/instability).

## Player Bots Quirks

To enable bots beyond building:
1. Import playerbots database
2. Copy `playerbots.conf` into your build directory
3. Must use **enUS** DBC files
4. Add to `worldserver.conf`:
   ```
   PlayerbotsDatabaseInfo = "127.0.0.1;3306;root;root;mop_playerbots"
   PlayerbotsDatabase.WorkerThreads = 1
   PlayerbotsDatabase.SynchThreads = 1
   Logger.playerbots = 3,Console Server
   ```
5. First startup is slow — let it load and randomize

## Code Style

- Sun/Oracle C++ conventions
- 4 spaces (no tabs)
- LF line endings only (`core.autocrlf=true`)
- No trailing whitespace
- Squash PR commits; keep PRs tested (compile + functional)
- See `doc/code_standards.md` for full details

## CI

- Linux GCC: `.github/workflows/linux_gcc.yml` — GCC 13, Ubuntu 24.04
- Windows: `.github/workflows/windows-build-release.yml` — VS 2022, Boost 1.87, OpenSSL 3.1.1
- SQL changes are ignored in CI (`paths-ignore: sql/**`)
- Travis CI (`build/` dir, `cmake .. -DSCRIPTS=1 -DTOOLS=1`) is legacy
