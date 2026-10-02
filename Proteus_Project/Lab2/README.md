# Lab 2 - Proteus simulations

Three schematics cover exercises 1-10. The author confirmed the simulations
working before this source snapshot was committed.

| Exercises | Proteus project | Schematic preview | Saved firmware target |
| --- | --- | --- | --- |
| 1 | [Lab2_e1.pdsprj](Lab2_e1.pdsprj) | [scmt1.png](scmt1.png) | `Lab2_e1/Debug/Lab2_e1.hex` |
| 2-8 | [Lab2_e2-8.pdsprj](Lab2_e2-8.pdsprj) | [scmt2.png](scmt2.png) | `Lab2_e8/Debug/Lab2_e8.hex` |
| 9-10 | [Lab2_e9-10.pdsprj](Lab2_e9-10.pdsprj) | [scmt3.png](scmt3.png) | `Lab2_e9/Debug/Lab2_e9.hex` |

## Running an exercise

1. Generate and build the selected exercise using the
   [firmware instructions](../../workspace_1.7.0/Lab2/README.md).
2. Open the matching `.pdsprj` file and stop any active simulation.
3. In the STM32 component properties, select the chosen exercise's generated
   `.hex` or `.elf` as **Program File**. For example, select Exercise 10's binary
   to run its animation on the shared Exercise 9-10 schematic.
4. Start the simulation.

The saved program paths are relative to this folder and point under
`../../workspace_1.7.0/Lab2/`. Compiled binaries are not included in Git;
build them locally before running a fresh clone.

The Exercise 9-10 project includes the working `VCC/VDD = 3.3 V` power-rail
configuration. Its matrix rows are active LOW, and the STM32 matrix-enable
signals are active LOW through the ULN2803/pull-up circuit. Preserve these
settings when reusing the project.

PNG previews are included for viewing on GitHub. Duplicate BMP exports,
automatic backups, and per-user Proteus workspace files are kept local.
