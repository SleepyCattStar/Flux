## Windows Build Troubleshooting & Dependency Guide

Building a cross-platform C++ application with graphical (GLFW/ImGui) and mathematical (FFTW3) dependencies on Windows requires resolving explicit compiler, linker, and platform translation targets. Below is a log of common Windows build hurdles and their fixes.

### 1. Environment & PATH Issues
* **Error:** `cmake : The term 'cmake' is not recognized...`
* **Cause:** Windows environment paths do not automatically reload or map newly installed toolchains without a terminal restart or explicit path declaration.
* **Fix:** Call the executable directly using its absolute path or install via `winget` using administrative overrides to force system environment mapping:
  ```powershell
  & "C:\Program Files\CMake\bin\cmake.exe" -DCMAKE_BUILD_TYPE=Release ..
  ```

### 2. Missing Submodules & Source Dependencies
* **Error:** `Cannot find source file: vendor/imgui/imgui.cpp`
* **Cause:** Git handles external frameworks as submodules (pointers) rather than downloading the structural source automatically during a basic clone operation.
* **Fix:** Initialize and pull the missing code tracking chunks, or manually extract the framework files directly into the target vendor tree:
  ```powershell
  git submodule update --init --recursive
  ```

### 3. Compiler Errors (Missing Header Directories)
* **Error:** `fatal error C1083: Cannot open include file: 'GLFW/glfw3.h': No such file or directory`
* **Cause:** The compiler cannot map the layout architecture of third-party interfaces without target header definitions explicitly stated in the lookup paths.
* **Fix:** Download the target OS binaries and place the interface directories directly inside the project's tracking path (e.g., `include/GLFW/`).

### 4. Linker Errors (Static & Import Libraries)
* **Error:** `fatal error LNK1181: cannot open input file 'glfw.lib'` or `'fftw3.lib'`
* **Cause:** Object files successfully compiled, but the final linking engine cannot find or bind the actual target machine code implementations (`.lib`). Additionally, naming conventions differ across distribution binaries (e.g., `glfw3.lib` vs `glfw.lib`).
* **Fix:** 1. For GLFW: Rename the target Visual Studio binary file from `glfw3.lib` to `glfw.lib` and locate it in the build root.
  2. For FFTW3: Utilize the Visual Studio machine library manager to explicitly assemble an input library reference from the definition structure mapping:
     ```powershell
     & "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\[Version]/bin/Hostx64/x64/lib.exe" /def:libfftw3-3.def /machine:x64
     ```

### 5. Cross-Platform Library Discrepancies
* **Error:** `fatal error LNK1181: cannot open input file 'm.lib'`
* **Cause:** Linux architectures separate standard mathematical utilities into an explicit library link target (`libm.so`), whereas the Windows MSVC compiler automatically packs these procedures directly into the primary runtime environment.
* **Fix:** Remove the explicit `m` symbol reference from the `target_link_libraries` entry within `CMakeLists.txt` when targeting Windows pipelines.

### 6. Missing Runtime Binaries (DLLs)
* **Error:** Application crashes instantly or reports missing components on execution.
* **Cause:** Windows requires compiled dynamic-link references (`.dll`) to exist directly alongside the host executable binary within the active working environment at execution runtime.
* **Fix:** Ensure `libfftw3-3.dll` is moved out of the source downloads and pasted directly into the production output destination directory next to the generated `.exe`.
