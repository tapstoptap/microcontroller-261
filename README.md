# CO3010 Microcontrollers Laboratory

This repository contains firmware, Proteus simulations, and reports produced for
the CO3010 Microcontrollers laboratory course in Semester 261.

## Repository map

- [`workspace_1.7.0/`](workspace_1.7.0/): STM32CubeIDE projects and source code.
- [`Proteus_Project/`](Proteus_Project/): Proteus projects and schematic images.
- [`reports/`](reports/): LaTeX report sources and final PDFs.

Lab 1 is complete and remains in its original tool-oriented paths so existing
STM32CubeIDE imports, Proteus projects, and LaTeX image links continue to work.
Lab 2 contains all ten exercises as source/configuration snapshots in
[`workspace_1.7.0/Lab2/`](workspace_1.7.0/Lab2/) and three shared Proteus
schematics with PNG previews in [`Proteus_Project/Lab2/`](Proteus_Project/Lab2/).
Lab 2 simulations have been tested by the author; the report is pending.

## Naming convention

- Lab 1 firmware: `workspace_1.7.0/Lab1_eM`
- Lab 1 Proteus project: `Proteus_Project/Lab1_eM`
- Lab 2 firmware snapshot: `workspace_1.7.0/Lab2/Lab2_eM`
- Lab 2 shared Proteus projects: `Proteus_Project/Lab2/`
- Report: `reports/Lab_NN`

Here, `N` is the lab number and `M` is the exercise number. Keep generated IDE
metadata, compiled `Debug`/`Release` output, LaTeX auxiliary files, and Proteus
backups out of version control.

Open or import the complete Lab 1 firmware projects in STM32CubeIDE. For Lab 2,
follow its firmware README to regenerate a project from the `.ioc` file and
restore the saved `Core/Src/main.c` before building. Generated project files and
compiled firmware are intentionally not committed for Lab 2.
