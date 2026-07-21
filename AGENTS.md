# Legends-of-Azeroth Pandaria 5.4.8 — Agent Instructions

## Build System
- CMake 3.16+ (recommended ≥3.27.2), C++20, no extensions
- Standard build: `mkdir build && cd build && cmake .. && make -j$(nproc) && make install`
- Default build type: `Release`; override with `-DCMAKE_BUILD_TYPE=Debug`
- Output goes to CMAKE_INSTALL_PREFIX (default: build dir)

### CMake Options (via `-D`)
- `SERVERS=1` — build worldserver + authserver (default ON)
- `SCRIPTS=1` — embed game scripts (default ON)
- `TOOLS=1` — map/vmap/mmap extraction tools (default OFF)
- `ELUNA=1` — embed Eluna Lua Engine (default ON; **disable with playerbots**)
- `PLAYERBOTS=1` — embed playerbots module (default ON)
- `USE_COREPCH=1`, `USE_SCRIPTPCH=1` — precompiled headers (default ON; set `NOPCH=1` to disable all PCH)
- `WITH_WARNINGS=1` — show compiler warnings (default OFF)
- `WITH_COREDEBUG=1` — extra debug code (default OFF)
- `WITH_SANITIZER=1` — AddressSanitizer (default OFF)
- `AUTH_SERVER=1` — build authserver separately (default ON)
- `UPDATER=1` — build database updater (default OFF)
- `USE_MODULES=1` — enable module system (default ON)

## Architecture
- **Binaries**: `authserver` (authentication), `worldserver` (game world)
- **Entry points**: `src/server/authserver/`, `src/server/worldserver/`
- **Core game logic**: `src/server/game/` — organized by subsystem (Entities, AI, Spells, Quests, Instances, Movement, etc.)
- **Scripts**: `src/server/scripts/` — organized by region (EasternKingdoms, Kalimdor, Northrend, Outland, Maelstrom, Pandaria) and type (Spells, Commands, Battlegrounds)
- **Scripts registration**: All scripts registered via `AddSC_*()` functions in `src/server/scripts/ScriptLoader/ScriptLoader.cpp`
- **Shared code**: `src/server/shared/` — packets, networking, containers, utilities
- **Common code**: `src/common/` — cross-module utilities, logging, cryptography, data stores
- **Modules**: `modules/` — auto-discovered subdirectories (currently `mod_playerbots`, `mod_exemple`); loader generated at cmake time from `ModulesLoader.cpp.in.cmake`
- **Dependencies**: `dep/` — bundled third-party libraries (Boost, OpenSSL, MySQL, StormLib, fmt, g3dlite, RecastNavigation, etc.)

### Key script categories
- `scripts/Commands/` — in-game GM commands
- `scripts/Spells/` — custom spell effects
- `scripts/Battlegrounds/` — BG-specific logic
- `scripts/Pandaria/` — MoP zones, dungeons, bosses
- `scripts/Custom/` — custom server-specific scripts

## Database
- **MySQL 5.7 or 8.0-8.1** (8.0.33+ requires OpenSSL 3.0-3.1.1; OpenSSL 3.2.0 NOT supported)
- SQL files in `sql/`:
  - `sql/base/` — initial schema (auth.sql, characters.sql, world.sql)
  - `sql/updates/` — incremental migrations (master/, world/, characters/)
  - `sql/archive/` — applied/old migrations
- **Playerbots database**: separate DB configured in worldserver.conf (`PlayerbotsDatabaseInfo`)

## Scripts Convention
- Every script file defines an `AddSC_<name>()` function that registers its scripts
- Scripts are auto-loaded via `ScriptLoader.cpp` — add declaration + call there for new scripts
- `SCRIPTS` compile flag guards all world script registration (`#ifdef SCRIPTS`)
- SmartAI scripts use `<name>SmartScripts()` registration

## Platform-Specific Notes
### Windows
- CMakeSettings.json targets VS2019 x64 (Desktop)
- Requires Windows SDK 10.0.22621+
- Copy MySQL `libmysql.dll` and OpenSSL `libcrypto-3-x64.dll` + `libssl-3-x64.dll` to output dir
- Boost via `install-boost` action or manual; version 1.85.0+ (MSVC toolset)

### Linux
- GCC ≥13.0 or Clang ≥12.0
- Packages: `libboost-all-dev`, `libreadline-dev`, `libbz2-dev`, `libssl-dev`, `libmysqlclient-dev`
- CI uses Ubuntu 24.04 with GCC 13

### macOS
- ARM64 builds supported (see `.github/workflows/macos-arm-build.yml`)

## Playerbots (module)
- Early-stage feature; **disable ELUNA when using playerbots**
- Requires enUS DBC files for bot functionality
- Setup: copy `playerbots.conf` to build dir, configure `PlayerbotsDatabaseInfo` in worldconf
- Database: separate `mop_playerbots` DB with connection string in config

## CI
- `linux_gcc.yml` — Ubuntu 24.04, GCC 13, builds with `-DTOOLS=1 -DELUNA=0`
- `windows-build-release.yml` — Windows 2022, VS2019, RelWithDebInfo
- Both skip `sql/` path changes (SQL-only commits don't trigger CI)

## Code Standards
- 4-space indent, no tabs, LF line endings (no CRLF)
- No trailing whitespace
- Follow Sun/Oracle C++ conventions
- Squash commits in PRs; PRs must compile and work

## Gotchas
- CMake build directory should be separate from source (in-source build disabled: `CMAKE_DISABLE_IN_SOURCE_BUILD ON`)
- Precompiled headers can cause slow incremental rebuilds on header changes; set `NOPCH=1` if needed
- `modules/` subdirectories are auto-discovered at configure time — no need to edit CMakeLists for new modules
- `ModulesLoader.cpp` is auto-generated; edits to it may be overwritten on reconfigure
- Revision hash is auto-generated at build time via `cmake/genrev.cmake`
