# Chip 8 Emulator

A simple Chip8 emulator built using SDL2

Includes a memory visualizer

![image](https://github.com/BurntIce16/Chip8Emu/assets/40308888/a7ec3b19-be03-4acc-8e22-7e04df90d580)

## Building

Currently only linux/wsl is supported

### Step 1 clone this repo

```bash
git clone https://github.com/BurntIce16/Chip8Emu
```

### Step 2 make

to compile the program run 

```bash
cd chip chip8
make
```

### Step 3 run

to execute the program run using the following command

```bash
./chip8 <chip 8 program>
```

To terminate press ctl+c in the console for the time being

## Resources

This project was made possible thanks to:

- https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
- https://github.com/corax89/chip8-test-rom
- https://github.com/Timendus/chip8-test-suite


## Test Suites Passed:

- [X] CHIP-8 splash screen
- [X] IBM logo
- [X] Corax+ opcode test
- [ ] Flags test
- [ ] Quirks test
- [ ] Keypad test
- [ ] Beep test
- [ ] Scrolling test
