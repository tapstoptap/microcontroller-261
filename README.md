# CO3010 Microcontrollers Laboratory

This repository contains firmware and Proteus simulations produced for
the CO3010 Microcontrollers laboratory course in Semester 261.
All projects target the **STM32F103C6Ux** and are built with STM32CubeIDE.

## Repository map

- [`workspace_1.7.0/`](workspace_1.7.0/): STM32CubeIDE projects and source code.
- [`Proteus_Project/`](Proteus_Project/): Proteus projects and schematic images.

## Lab status

| Lab | Topic | Firmware | Proteus |
| --- | --- | --- | --- |
| 1 | GPIO and LEDs (10 exercises) | [`Lab1_e1` … `Lab1_e10`](workspace_1.7.0/) – `.ioc` + `main.c` | [`Lab1_eM`](Proteus_Project/) |
| 2 | Timer interrupts, 7-segment and LED matrix (10 exercises) | [`Lab2/`](workspace_1.7.0/Lab2/) – `.ioc` + `main.c` snapshots | [`Lab2/`](Proteus_Project/Lab2/) |
| 3 | Buttons and switches – traffic light with mode buttons | [`Lab3/Lab3_exercises`](workspace_1.7.0/Lab3/Lab3_exercises/) – `.ioc` + application `Core` sources | [`Lab3/`](Proteus_Project/Lab3/) |
| 4 | Cooperative scheduler | [`Lab4/Lab4_exercises`](workspace_1.7.0/Lab4/Lab4_exercises/) – `.ioc` + scheduler and `main.c` | [`Lab4/`](Proteus_Project/Lab4/) ([preview](Proteus_Project/Lab4/lab4.png)) |
| 5 | UART | – | – |

Only the CubeMX configuration (`.ioc`) and hand-written sources are committed.
Drivers, startup code, linker scripts, IDE project files, and other files that
STM32CubeIDE regenerates from the `.ioc` are intentionally left out.

Lab 4 implements a cooperative scheduler
([`scheduler.h`](workspace_1.7.0/Lab4/Lab4_exercises/Core/Inc/scheduler.h) /
[`scheduler.c`](workspace_1.7.0/Lab4/Lab4_exercises/Core/Src/scheduler.c))
driven by a 10 ms TIM2 tick. `main.c` registers five periodic LED tasks
(0.5 s – 2.5 s), a 10 ms tick task, and several one-shot tasks that exercise
task deletion and error reporting.

## Naming convention

- Lab 1 firmware: `workspace_1.7.0/Lab1_eM`
- Lab 1 Proteus project: `Proteus_Project/Lab1_eM`
- Lab 2 firmware snapshot: `workspace_1.7.0/Lab2/Lab2_eM`
- Lab 2 shared Proteus projects: `Proteus_Project/Lab2/`
- Lab 3 onwards: `workspace_1.7.0/LabN/LabN_exercises` and `Proteus_Project/LabN/`

Here, `N` is the lab number and `M` is the exercise number. Keep compiled
`Debug`/`Release` output, IDE `.metadata`, and Proteus backups out of version
control. Lab reports are kept locally and are not published.

## Building

1. Open the exercise's `.ioc` in STM32CubeIDE and generate code. Install the
   STM32F1 firmware package if prompted. This recreates the drivers, startup
   code, linker script, and project files.
2. Replace the generated `Core/Src/main.c` with the committed version and copy
   in any other committed sources (for example Lab 4's `scheduler.c`/`.h`). See the
   [Lab 2 firmware README](workspace_1.7.0/Lab2/README.md) for details.
3. Build, then select the `.hex`/`.elf` as the STM32 **Program File** in the
   matching Proteus project. Compiled binaries are not committed.
