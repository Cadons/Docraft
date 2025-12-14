# DocCraft
[![CI/CD](https://gitlab.umbranet.ovh/openprojects/doc-craft/badges/dev/pipeline.svg)](https://gitlab.umbranet.ovh/openprojects/doc-craft/-/pipelines)

DocCraft is a C++23 project built with **CMake**, with a build pipeline that uses **Flex**/**Bison** and runs tests via **CTest** (GoogleTest).

---

## Requirements

### Build tools
- **CMake** ≥ 3.16
- A C++ compiler with **C++23** support  
  - macOS: AppleClang (Xcode) or LLVM/Clang
  - Linux: GCC 14+ or Clang 16+ (recommended)
  - Windows: MSVC (Visual Studio 2022) or LLVM/Clang

### Dependencies
- **Flex**
- **Bison**
- **GoogleTest** (for tests)
- **libharu** (`libhpdf`) if your build links against it

---

## Quick start (command line)

### 1) Clone
```
bash
git clone <your-repo-url>
cd doc-craft
```
### 2) Configure
```
bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```
### 3) Build
```
bash
cmake --build build --config Release -j
```
### 4) Run tests
```
bash
ctest --test-dir build -C Release --output-on-failure
```
---

## Platform notes

## macOS (Homebrew)
Install prerequisites:
```
bash
brew install cmake flex bison
brew install googletest
brew install libharu
```
Notes:
- Homebrew’s `bison`/`flex` may not be the system default. If CMake can’t find them, ensure Homebrew is first in your `PATH`:
  ```bash
  echo 'export PATH="/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/flex/bin:$PATH"' >> ~/.zshrc
  source ~/.zshrc
  ```

## Ubuntu / Debian
```
bash
sudo apt-get update
sudo apt-get install -y cmake make bison flex libgtest-dev libhpdf-dev
```
## Windows

You have a few good options. Pick one:

### Option A: Visual Studio 2022 + vcpkg (recommended)

1) Install:
- **Visual Studio 2022** with workload: **Desktop development with C++**
- **CMake** (either via Visual Studio Installer or from cmake.org)
- **Git**

2) Install and set up vcpkg:
```
powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```
3) Install dependencies (example triplet: x64-windows):
```
powershell
.\vcpkg install flex bison gtest libharu --triplet x64-windows
```
4) Configure your project using the vcpkg toolchain:
```
powershell
cd <path>\to\doc-craft
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_TOOLCHAIN_FILE="<path>\to\vcpkg\scripts\buildsystems\vcpkg.cmake"
```
5) Build and test:
```
powershell
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```
Notes:
- If you use the Visual Studio generator, `--config Release` matters (multi-config).
- If CMake still can’t locate Flex/Bison, verify `flex.exe` / `bison.exe` are installed by vcpkg and that you configured with the toolchain file.

---

## Common issues

### CMake can’t find Flex/Bison
- Confirm they are installed and available:
    - macOS/Linux: `flex --version`, `bison --version`
    - Windows: check the tool installation method (vcpkg/MSYS2/WSL)
- Re-configure after installing dependencies:
```
bash
cmake -S . -B build
```
### Tests don’t run / no tests discovered
- Build first, then run:
```
bash
ctest --test-dir build --output-on-failure -VV
```
---

## Project layout (high level)

- `docraft/` — main library / sources
- `docraft/test/` — tests
- `build/` — build output (generated)
```

