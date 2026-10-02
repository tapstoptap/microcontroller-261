# Lab 2 - STM32 firmware snapshots

This folder contains the saved `.ioc` configuration and complete `Core/Src/main.c`
for each of the ten exercises. These are source/configuration snapshots, not
complete, directly importable STM32CubeIDE projects.

Target: **STM32F103C6Ux**. The configurations specify STM32Cube FW_F1 **V1.8.7**
and the STM32CubeIDE toolchain.

| Exercise | CubeMX configuration | Application source |
| --- | --- | --- |
| 1 | [Lab2_e1.ioc](Lab2_e1/Lab2_e1.ioc) | [main.c](Lab2_e1/Core/Src/main.c) |
| 2 | [Lab2_e2.ioc](Lab2_e2/Lab2_e2.ioc) | [main.c](Lab2_e2/Core/Src/main.c) |
| 3 | [Lab2_e3.ioc](Lab2_e3/Lab2_e3.ioc) | [main.c](Lab2_e3/Core/Src/main.c) |
| 4 | [Lab2_e4.ioc](Lab2_e4/Lab2_e4.ioc) | [main.c](Lab2_e4/Core/Src/main.c) |
| 5 | [Lab2_e5.ioc](Lab2_e5/Lab2_e5.ioc) | [main.c](Lab2_e5/Core/Src/main.c) |
| 6 | [Lab2_e6.ioc](Lab2_e6/Lab2_e6.ioc) | [main.c](Lab2_e6/Core/Src/main.c) |
| 7 | [Lab2_e7.ioc](Lab2_e7/Lab2_e7.ioc) | [main.c](Lab2_e7/Core/Src/main.c) |
| 8 | [Lab2_e8.ioc](Lab2_e8/Lab2_e8.ioc) | [main.c](Lab2_e8/Core/Src/main.c) |
| 9 | [Lab2_e9.ioc](Lab2_e9/Lab2_e9.ioc) | [main.c](Lab2_e9/Core/Src/main.c) |
| 10 | [Lab2_e10.ioc](Lab2_e10/Lab2_e10.ioc) | [main.c](Lab2_e10/Core/Src/main.c) |

## Recreating and building a project

1. Keep a separate copy of the committed `Core/Src/main.c` before generating code.
2. Open the exercise's `.ioc` in STM32CubeMX/STM32CubeIDE and generate its project
   using the configured STM32CubeIDE toolchain. Install the requested STM32F1
   firmware package if necessary. This recreates headers, HAL drivers, startup
   code, interrupt handlers, and the linker script.
3. Replace the generated `Core/Src/main.c` with the saved version from this
   repository, then build the project. Do not compile all exercises together;
   each exercise is a separate application.
4. If using HEX files, enable **Convert to Intel Hex file** in the project's
   MCU post-build output settings. Alternatively, use the generated ELF file.
5. Load that exercise's binary into the matching
   [Proteus schematic](../../Proteus_Project/Lab2/README.md).

Regeneration can overwrite application code, especially the Exercise 3 and 4
files, which do not contain CubeMX user-code markers. Always preserve and restore
the committed `main.c` when generating project scaffolding.

The author confirmed all simulations working. The Exercise 9 and 10 sources
include the corrected LED-matrix polarity. The official report is pending.

Only the requested `.ioc` and `main.c` files are versioned for each exercise.
Local IDE metadata, generated drivers/support code, build outputs, the sample
project, and intermediate `Exercise_7_to_10_Code` copies are intentionally ignored
by this folder's `.gitignore`; they are not deleted from the local workspace.
