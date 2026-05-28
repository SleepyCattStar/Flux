## Linux Build Troubleshooting & Dependency Guide

When distributing a pre-compiled C++ binary on Linux (like `FourierAnalyzer`), the executable relies on the host system's shared object libraries (`.so` files) and kernel C-runtime environment (`glibc`). If a user runs the binary on a different Linux distribution or a fresh install, they may encounter dependency or permission errors. 

Below is a guide to diagnosing and fixing common runtime issues on Linux systems.

---

### 1. Missing Shared Libraries (`.so` errors)
* **Error:** `error while loading shared libraries: libglfw.so.3: cannot open shared object file` or `libfftw3.so.3`
* **Cause:** The binary was compiled using dynamic linking. It expects the required third-party libraries (GLFW and FFTW3) to be installed globally on the user's system.
* **Fix:** The user must install the runtime development packages via their distribution's package manager.

#### For Debian / Ubuntu / Pop!_OS / Mint:
```bash
sudo apt update
sudo apt install libglfw3 libfftw3-double3
```

#### For Arch Linux / Manjaro:
```bash
sudo pacman -Syu
sudo pacman -S glfw-x11 fftw
```

#### For Fedora / RHEL:
```bash
sudo dnf install glfw fftw-libs
```

---

### 2. Permission Denied
* **Error:** `bash: ./FourierAnalyzer: Permission denied`
* **Cause:** Browsers and Git download managers strip the "executable" permission flag from raw binary files for security reasons.
* **Fix:** Explicitly grant execution permissions to the binary before launching it:
  ```bash
  chmod +x FourierAnalyzer
  ./FourierAnalyzer
  ```

---

### 3. GLIBC Version Mismatch
* **Error:** `../FourierAnalyzer: /lib/x86_64-linux-gnu/libc.so.6: version 'GLIBC_2.XX' not found`
* **Cause:** Linux binaries are compiled against a specific version of `glibc` (the core GNU C Library). While `glibc` is backwards-compatible, it is **not forwards-compatible**. If you compiled the app on a newer Linux distribution (e.g., a 2025/2026 environment), it cannot run on an older LTS distribution because the older system lacks the newer standard library symbols.
* **Fix:** * **Option A (Recommended for Users):** The user should compile the project directly from source on their own machine to map their native toolchain:
    ```bash
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    ```
  * **Option B (For Developers):** To distribute a truly universal Linux binary that works everywhere without forcing users to compile, compile your official release binary on an older stable environment (like an older Ubuntu LTS container or GitHub Actions runner).

---

### Quick Start Checklist for Linux Users
To run the pre-compiled application instantly on a standard Debian/Ubuntu system, run this single block of commands:

```bash
# Install dependencies
sudo apt update && sudo apt install -y libglfw3 libfftw3-double3

# Mark as executable and run
chmod +x ./FourierAnalyzer
./FourierAnalyzer
```
