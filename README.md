# CPU Pipeline Visualizer

**Contact:** [alexandrubutnaru32@gmail.com](mailto:alexandrubutnaru32@gmail.com)

A comprehensive educational tool designed to help users understand and visualize the inner workings of CPU pipeline architecture. This application provides interactive graphical representations of how instructions move through different pipeline stages, from memory fetch to retirement.

## Features

- **Interactive Pipeline Visualization**: Step through CPU execution cycle by cycle
- **Custom Assembly Language**: CISC architecture emulation with comprehensive instruction set
- **Configurable Architecture**: Adjust clock speed, cache sizes, and module cycle counts
- **Real-time Component Monitoring**: View internal data of pipeline components
- **Exception Handling Simulation**: Complete exception vector system
- **Cache Simulation**: Direct-mapping and K-way set-associative cache implementations

## Architecture Overview

The CPU Pipeline Visualizer simulates a 16-bit CISC architecture with the following components:

### Core Components
- **Clock**: Synchronizes all pipeline modules
- **Memory**: 64kB physical memory space
- **Load/Store (LS)**: Memory interface with K-way set-associative cache
- **Instruction Cache (IC)**: Fetch window forwarder with direct-mapping cache
- **Decode (DE)**: Instruction decoder with 2 fetch window buffer
- **Execute (EX)**: Arithmetic/logic unit with exception handling

### Register Set
- 8 general-purpose registers (R0-R7)
- 4 SIMD registers (Z0-Z3) for parallel operations
- Special registers: IP, FLAGS, ST_BASE, ST_SIZE, SP_REG

## Getting Started

Simply download the latest release from the release page, extract the files, and run the executable. Requires a 64-bit Windows machine.

## Documentation
The project has a dedicated 'Help' section accessible from the main menu, which provides detailed information on the capabilities of the CPU Pipeline Visualizer, with explanations of the CPU model components, assembly language, instruction set, and configuration options.

### Usage

1. **Write Assembly Code**: Use the Input tab to write your assembly code
2. **Convert to Binary**: Click the '->' button to parse assembly into hex
3. **Execute**: Hit 'Execute' to run the simulation
4. **Visualize**: Navigate to the Simulation tab to step through execution
5. **Configure**: Adjust CPU parameters in the Config tab

#### Navigation Controls
- **Slider**: Jump to any cycle
- **Buttons**: Step forward/backward, play/pause
- **Keyboard**: LEFT_ARROW (back), SPACE (pause/play), RIGHT_ARROW (forward)

## Assembly Language Reference

### Memory Layout
| Section | Address Range | Purpose |
|---------|---------------|---------|
| Boot | #fff0 - #ffff | Initial execution point |
| Code/Data | #1000 - #ffef | Program code and variables |
| Stack | #dfee - #efee | Downward-growing stack (default) |
| Exception Vectors | #0000 - #000f | Exception handler addresses |
| Save State | #0010 - #0027 | Exception context storage |

### Instruction Set

#### Arithmetic Operations
- `add dst, src` - Addition
- `sub dst, src` - Subtraction  
- `mul src1, src2` - Multiplication (result in R0:R1 or Z0:Z1)
- `div src1, src2` - Division (quotient in R0/Z0, remainder in R1/Z1)

#### Data Movement
- `mov dst, src` - Move data
- `push src` - Push to stack
- `pop dst` - Pop from stack

#### Control Flow
- `jmp addr` - Unconditional jump
- `je addr` - Jump if equal
- `jl addr` - Jump if less
- `jg addr` - Jump if greater
- `jz addr` - Jump if zero
- `call addr` - Function call
- `ret` - Return from function

#### SIMD Operations
- `gather dst_simd, src_simd` - Gather from memory
- `scatter dst_simd, src_simd` - Scatter to memory

#### System Operations
- `cmp src1, src2` - Compare operands, set FLAGS
- `end_sim` - End simulation
- `excp_exit` - Exit exception handler

### Source Types
- **Immediate**: `0x1234`, `label`
- **Memory**: `[0x1000]`, `[label]`, `[r0]`
- **Registers**: `r0-r7` (general), `Z0-Z3` (SIMD)
- **Stack**: `sp`, `stack_base`, `stack_size`

### Exception System
The CPU handles various exceptions with a complete vector table:

| Exception | Vector | Cause |
|-----------|--------|-------|
| DIV_BY_ZERO | .vector_0 | Division by zero |
| DECODE_ERROR | .vector_2 | Invalid instruction/parameters |
| STACK_OVERFLOW | .vector_4 | Stack over/underflow |
| MISALIGNED_ACCESS | .vector_6 | Odd address memory access |
| MISALIGNED_IP | .vector_8 | Odd address jump/call |

## Configuration Options

### Module Timing
Adjust cycle counts for each pipeline stage to simulate different CPU designs.

### Cache Settings
- **IC Cache**: Direct-mapping cache size and hit timing
- **LS Cache**: K-way set-associative cache configuration

### Memory Behavior
- NOP handling for OpCode 0x0
- Uninitialized memory read behavior (zero vs random)

### Simulation Display
- Clock period and autoplay speed
- Cycle range display options

## Example Program

```assembly
.boot
  jmp main

.code
main:
  mov r0, 10
  mov r1, 5
  add r0, r1
  cmp r0, 15
  je success
  jmp fail

success:
  mov r2, 1
  end_sim

fail:
  mov r2, 0
  end_sim
```

## Educational Benefits

- **Pipeline Concepts**: Visualize instruction pipelining, synchronization, redirects
- **Cache Behavior**: Understand cache hits, misses, and replacement policies  
- **Exception Handling**: Learn about interrupt mechanisms and context switching
- **Architecture Impact**: See how configuration changes affect performance
- **Assembly Programming**: Practice low-level programming concepts

## Contributing

Contributions are welcome! Please feel free to submit pull requests, report bugs, or suggest features.

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).

You are free to use, modify, and redistribute this software under the terms of the GPL v3.  
See the [LICENSE](LICENSE) file for details.

## Acknowledgments

Built with PyQt5 for the GUI and C++ for the CPU simulation backend.

Logo generated with ChatGPT and edited with paint.net.