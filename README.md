# CO3010 Microcontrollers Laboratory

This repository contains firmware, Proteus simulations, and reports produced for
the CO3010 Microcontrollers laboratory course in Semester 261.

## Repository map

- [`labs/`](labs/): lab-by-lab index and progress overview for Labs 1--5.
- [`workspace_1.7.0/`](workspace_1.7.0/): STM32CubeIDE projects and source code.
- [`Proteus_Project/`](Proteus_Project/): Proteus projects and schematic images.
- [`reports/`](reports/): LaTeX report sources and final PDFs.

Lab 1 is complete and remains in its original tool-oriented paths so existing
STM32CubeIDE imports, Proteus projects, and LaTeX image links continue to work.
Labs 2--5 currently have tracked placeholder index folders ready for future
projects and reports.

## Naming convention

- Firmware project: `workspace_1.7.0/LabN_eM`
- Proteus project: `Proteus_Project/LabN_eM`
- Report: `reports/Lab_NN`
- Lab overview: `labs/Lab_NN/README.md`

Here, `N` is the lab number and `M` is the exercise number. Keep generated IDE
metadata, compiled `Debug`/`Release` output, LaTeX auxiliary files, and Proteus
backups out of version control.

Open or import individual firmware projects from `workspace_1.7.0/` in
STM32CubeIDE. Each lab overview lists the relevant firmware, simulation, and
report paths.
