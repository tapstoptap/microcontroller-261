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
| 1 | GPIO and LEDs (10 exercises) | [`Lab1_e1` … `Lab1_e10`](workspace_1.7.0/) – full projects | [`Lab1_eM`](Proteus_Project/) |
| 2 | Timer interrupts, 7-segment and LED matrix (10 exercises) | [`Lab2/`](workspace_1.7.0/Lab2/) – `.ioc` + `main.c` snapshots | [`Lab2/`](Proteus_Project/Lab2/) |
| 3 | Buttons and switches – traffic light with mode buttons | [`Lab3/Lab3_exercises`](workspace_1.7.0/Lab3/Lab3_exercises/) – `.ioc` + application `Core` sources | [`Lab3/`](Proteus_Project/Lab3/) |
| 4 | Cooperative scheduler | [`Lab4/Lab4_exercises`](workspace_1.7.0/Lab4/Lab4_exercises/) – full project | [`Lab4/`](Proteus_Project/Lab4/) ([preview](Proteus_Project/Lab4/lab4.png)) |
| 5 | UART | – | – |

Lab 1 remains in its original tool-oriented paths so existing STM32CubeIDE
imports and Proteus projects continue to work.

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

- **Lab 1 and Lab 4:** import the project folder into STM32CubeIDE and build.
- **Lab 2 and Lab 3:** these are source snapshots. Open the `.ioc` file, generate
  the project, then restore the committed `Core` sources before building (see the
  [Lab 2 firmware README](workspace_1.7.0/Lab2/README.md)).

Compiled `.hex`/`.elf` files are not committed. Build the firmware first, then
select it as the STM32 **Program File** in the matching Proteus project.
