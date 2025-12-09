# anti-idle

A smart anti-idle program for **Linux and Windows** that sends hardware-level keyboard input to prevent applications from timing out due to inactivity. Uses Linux uinput (Linux) or Windows SendInput API (Windows) to create real keyboard events that games and applications will accept. Successfully tested with Old School RuneScape (OSRS).

## Features

- **Hardware-level input**: Uses Linux uinput (Linux) or Windows SendInput API (Windows) to create real keyboard events that games accept
- **Simple and reliable**: Sends arrow key presses (Left, Right, Up, Down) directly to the focused window
- **Advanced anti-cheat bypass**: Sophisticated human behavior simulation
  - Weighted timing patterns (short intervals more common, long intervals rare)
  - Micro-variations in timing (±5 seconds) for natural feel
  - Human-like key press durations (30-80ms random)
  - Occasional double-taps (8% chance) simulating uncertainty
  - Misclick corrections (3% chance) - presses wrong key then corrects
  - Extended breaks (5% chance) simulating tab-out/distraction
  - Random arrow key selection (Left, Right, Up, Down)
- **Detailed logging**: Logs every action with timestamps, press durations, and session statistics
- **Test mode included**: Verify input system is working before using the main program
- **Cross-platform**: Works on both Linux and Windows
- **Tested and working**: Successfully tested with Old School RuneScape (OSRS)
- **Upcoming**: Window switching support for background operation

## Requirements

### Linux
- Linux with X11
- X11 development libraries (`libX11-dev`)
- GCC compiler
- `uinput` kernel module (usually built-in)
- Root access OR membership in the `input` group

### Windows
- Windows 7 or later
- C compiler (MinGW-w64 GCC or Microsoft Visual C++)
- No additional dependencies required

### Installing Dependencies

#### Linux

**Arch Linux:**
```bash
sudo pacman -S libx11 base-devel
sudo modprobe uinput
sudo usermod -a -G input $USER
```

**Debian/Ubuntu:**
```bash
sudo apt-get install libx11-dev build-essential
sudo modprobe uinput
sudo usermod -a -G input $USER
```

**Fedora:**
```bash
sudo dnf install libX11-devel gcc
sudo modprobe uinput
sudo usermod -a -G input $USER
```

**Note:** After adding yourself to the `input` group, you need to log out and log back in for the changes to take effect.

#### Windows

**Option 1: MSYS2 (Recommended)**
1. If you don't have MSYS2, download from: https://www.msys2.org/
2. Install and open MSYS2 terminal
3. Run: `pacman -S mingw-w64-x86_64-gcc`
4. Add `C:\msys64\mingw64\bin` to your PATH (optional, build.bat handles this)
5. Verify: `gcc --version`

**Option 2: MinGW-w64 via Chocolatey**
1. Install Chocolatey (if needed): https://chocolatey.org/install
2. Run: `choco install mingw -y`
3. Close and reopen command prompt
4. Verify: `gcc --version`

**Option 3: MinGW-w64 Manual Install**
1. Download from: https://winlibs.com/
2. Choose: "Win64 - GCC 13.2.0 + LLVM/Clang/LLD/LLDB 17.0.6 + MinGW-w64 11.0.1"
3. Extract to `C:\mingw64`
4. Add `C:\mingw64\bin` to your PATH environment variable
5. Verify: `gcc --version`

**Option 4: Microsoft Visual Studio**
1. Install Visual Studio with C/C++ development tools
2. Use "Developer Command Prompt for VS" to build

### Why hardware-level input?

This program uses hardware-level input APIs to create real input events that games like RuneLite will accept:
- **Linux**: Uses `uinput` to create real hardware events (unlike XTest which creates synthetic X11 events)
- **Windows**: Uses `SendInput` API to inject input at a low level

Both approaches create input events that games cannot distinguish from actual keyboard/mouse input.

## Project Structure

```
anti-idle/
├── src/              # Source code
│   └── antiidle.c
├── bin/              # Compiled binary (created on build)
├── build/            # Build artifacts (created on build)
├── Makefile          # Linux build configuration
├── build.bat         # Windows build script
├── README.md         # This file
└── .gitignore        # Git ignore rules
```

## Building

### Linux

```bash
make
```

The compiled binary will be in the `bin/` directory.

### Windows

**Using build.bat (Recommended):**
```batch
build.bat
```

The script will automatically:
- Detect MinGW GCC (checks PATH and common installation locations like `C:\msys64\mingw64\bin`, `C:\mingw64\bin`)
- Add necessary directories to PATH for DLL resolution
- Compile with appropriate flags and libraries
- Create `bin\antiidle.exe`

**Manual build with MinGW (if GCC is in PATH):**
```batch
gcc -Wall -Wextra -O2 -o bin\antiidle.exe src\antiidle.c -luser32 -lgdi32
```

**Manual build with MSVC:**
```batch
cl /W3 /O2 /Fe:bin\antiidle.exe src\antiidle.c user32.lib gdi32.lib
```

**Note:** If you get "libgmp-10.dll not found" or similar DLL errors, make sure the MinGW `bin` directory is in your PATH, or use `build.bat` which handles this automatically.

## Usage

### Linux
```bash
./bin/antiidle [startup_delay] [--test]
```

### Windows
```batch
bin\antiidle.exe [startup_delay] [--test]
```

**Arguments:**
- `startup_delay`: Optional delay in seconds before starting actions (default: 10 seconds)
- `--test` or `-t`: Test mode - sends arrow keys every second for 10 seconds to verify input system is working

### Quick Start (OSRS Example)

1. **Start the program:**
   - **Linux:** `./bin/antiidle`
   - **Windows:** `bin\antiidle.exe`

2. **Click on your game window** when prompted (e.g., RuneLite/OSRS)

3. **During the 10-second startup delay, double-click your game window** to ensure it has focus

4. The program will start sending arrow key presses automatically

### Examples

**Linux:**
```bash
# Start with default 10-second delay
./bin/antiidle

# Start with 5-second delay (less time to focus window)
./bin/antiidle 5

# Test mode - verify input system is working
./bin/antiidle --test
```

**Windows:**
```batch
REM Start with default 10-second delay
bin\antiidle.exe

REM Start with 5-second delay (less time to focus window)
bin\antiidle.exe 5

REM Test mode - verify input system is working
bin\antiidle.exe --test
```

### Test Mode

Before using the program, you should verify that the input system is working correctly:

1. **Open a text editor or terminal** (any application that accepts keyboard input)
2. **Focus that window**
3. **Run test mode:**
   - **Linux:** `./bin/antiidle --test`
   - **Windows:** `bin\antiidle.exe --test`
4. **Watch for cursor movement** - if the cursor moves every second, the input system is working!

**Troubleshooting:**
- **Linux:** If test mode shows no cursor movement:
  - Check that you focused a text editor/terminal
  - Verify the device exists: `cat /proc/bus/input/devices | grep -A4 "Anti-Idle"`
  - Try running as root: `sudo ./bin/antiidle --test`
  - Check error messages for permission issues
- **Windows:** If test mode shows no cursor movement:
  - Check that you focused a text editor/terminal
  - Make sure you're running as a user with appropriate permissions
  - Some applications may block programmatic input - try a different application

### How It Works

1. **Window Selection**: When you run the program, click on the window you want to keep active (this is just for logging - the program sends events to whatever window has focus)
2. **Startup Delay**: The program waits for the specified delay (default 10 seconds)
3. **Focus Your Window**: **IMPORTANT** - During the startup delay, double-click your game window (e.g., OSRS/RuneLite) to ensure it has focus. The program sends events to whatever window is currently focused.
4. **Automatic Actions**: The program will automatically:
   - Randomly press arrow keys (Left, Right, Up, Down)
   - Use the same reliable method as test mode (proven to work)
   - Vary the timing between actions (30 seconds to 4 minutes)
   - Send hardware-level input events directly (uinput on Linux, SendInput on Windows)
   - Log detailed information about each action including press duration

**Important**: 
- **Double-click your game window** (e.g., OSRS) after starting the program to ensure it has focus
- The program sends events to whatever window currently has focus
- If you switch to another window, events will go to that window instead

**Upcoming Feature**: Window switching support is being developed to allow switching between different windows while the program runs in the background.

### Anti-Cheat Bypass Logic

The program uses sophisticated human behavior simulation to avoid detection:

#### Timing Patterns (Weighted Random)
- **Short intervals** (30-60 seconds): 40% probability - most common, simulates active play
- **Medium intervals** (1-2 minutes): 30% probability - normal activity
- **Long intervals** (2-3 minutes): 20% probability - occasional pauses
- **Very long intervals** (3-4 minutes): 10% probability - rare, simulates distraction
- **Micro-variations**: ±5 seconds added to each interval for natural timing feel
- **Maximum interval**: 4 minutes (240 seconds) - ensures actions occur before OSRS 5-minute timeout

#### Human Behavior Simulation
- **Key press duration**: Random 30-80ms (human-like, not mechanical)
- **Double-tap**: 8% chance - simulates uncertainty/hesitation
- **Misclick correction**: 3% chance - presses wrong key, pauses 100-300ms, then corrects
- **Extended breaks**: 5% chance after 10+ actions - adds 1-3 minutes (simulates tab-out/distraction)
- **Key variety**: Randomly selects from Left, Right, Up, Down arrow keys

#### Why This Works
- **Weighted patterns** make short intervals common (like real players)
- **Micro-variations** prevent perfect timing patterns that bots exhibit
- **Human behaviors** (double-taps, misclicks) add realistic imperfection
- **Extended breaks** simulate real player behavior (checking phone, etc.)
- **Variable press durations** avoid mechanical precision

This combination creates behavior patterns that are statistically similar to real human players, making detection extremely difficult.

## Installation

### Linux

To install system-wide:

```bash
sudo make install
```

This installs the binary to `/usr/local/bin/antiidle`.

To uninstall:

```bash
sudo make uninstall
```

### Windows

Windows installation is not required - you can run `bin\antiidle.exe` directly from the project directory. If you want to use it from anywhere, add the `bin` directory to your PATH environment variable.

## Logging

The program logs all actions to `antiidle.log` in the current directory with detailed information:
- Timestamp for each action
- Key pressed (LEFT, RIGHT, UP, DOWN)
- Press duration in milliseconds
- Action type (normal, double-tap, misclick correction)
- Session statistics (elapsed time, total actions, keys pressed)
- Next action time and countdown

All actions are logged to help you monitor the program's behavior and verify the anti-cheat logic is working correctly.

## Testing

This program has been **tested and confirmed to work** with:
- **Old School RuneScape (OSRS)** - Successfully prevents 5-minute AFK timeout

The timing is specifically tuned to ensure actions occur before OSRS's 5-minute inactivity timeout.

## Technical Details

### Linux
- Uses Linux `uinput` kernel module for hardware-level input events
- Uses X11 only for initial window selection (logging purposes)
- Sends events directly via uinput - no X11 focus manipulation needed
- Simple and reliable: Uses the same proven method as test mode

### Windows
- Uses Windows `SendInput` API for hardware-level input events
- Uses Windows API for window selection (logging purposes)
- Sends events directly to the currently focused window
- No special permissions required (unlike Linux uinput)

### Anti-Cheat Implementation

The program implements a sophisticated multi-layered anti-cheat bypass system:

1. **Weighted Random Timing**: Uses 4 timing patterns with different probabilities:
   - Short intervals (30-60s): 40% weight - most common
   - Medium intervals (1-2min): 30% weight
   - Long intervals (2-3min): 20% weight
   - Very long intervals (3-4min): 10% weight - rare
   - Micro-variations: ±5 seconds added to each interval

2. **Human Behavior Simulation**:
   - Variable key press durations: 30-80ms (random, not fixed)
   - Double-tap simulation: 8% chance (simulates uncertainty)
   - Misclick correction: 3% chance (presses wrong key, then corrects)
   - Extended breaks: 5% chance after 10+ actions (adds 1-3 minutes)

3. **Key Variety**: Randomly selects from all 4 arrow keys (Left, Right, Up, Down)

4. **Statistical Distribution**: Timing patterns are weighted to match human behavior (short intervals more common than long ones)

### How It Works

**Linux:** The program creates a virtual input device using `uinput` and sends real hardware-level keyboard events. This approach bypasses the limitations of XTest/X11 synthetic events, allowing the program to work with games like RuneLite that filter out synthetic input.

**Windows:** The program uses the `SendInput` API to inject keyboard events at a low level, creating real hardware-level input that games cannot distinguish from actual keyboard input.

The main program uses the exact same event-sending logic as test mode, which has been proven to work. Events are sent directly to whatever window currently has focus.

The anti-cheat bypass works by creating behavior patterns that are statistically similar to real human players, making detection extremely difficult.

**Permissions:**
- **Linux:** The program needs access to `/dev/uinput`, which typically requires:
  - Running as root, OR
  - Being a member of the `input` group (recommended)
- **Windows:** No special permissions required - works as a normal user

## Build Commands

### Linux

**Build the project:**
```bash
make
```

**Clean build artifacts:**
```bash
make clean
```

This removes the `bin/` and `build/` directories and all compiled files.

### Windows

**Build the project:**
```batch
build.bat
```

The script automatically detects and uses available compilers (MinGW GCC or MSVC).

**Clean build artifacts:**
Manually delete the `bin\` and `build\` directories, or:
```batch
rmdir /s /q bin build
```

## Troubleshooting

### Linux Issues

#### "Cannot open /dev/uinput" Error

If you get this error, you need to:

1. **Load the uinput module:**
   ```bash
   sudo modprobe uinput
   ```
   To make it permanent, add `uinput` to `/etc/modules-load.d/uinput.conf`

2. **Add yourself to the input group:**
   ```bash
   sudo usermod -a -G input $USER
   ```
   Then **log out and log back in** for the change to take effect.

3. **Verify uinput is accessible:**
   ```bash
   ls -l /dev/uinput
   ```
   You should see read/write permissions for the `input` group.

### Alternative: Run as Root

If you can't add yourself to the input group, you can run the program as root:
```bash
sudo ./bin/antiidle
```

**Note:** Running as root is less secure but will work if group permissions aren't available.

#### Verifying uinput Device Creation

After starting the program, you can verify the virtual device was created:

```bash
cat /proc/bus/input/devices | grep -A4 "Anti-Idle"
```

You should see output showing the "Anti-Idle Virtual Input" device. If you don't see it, check the error messages when starting the program.

#### Events Not Working in Application

If test mode works (cursor moves in text editor) but events don't work in your target application:

1. **Double-click your game window** - After starting the program, double-click your game window (e.g., OSRS/RuneLite) to ensure it has focus. The program sends events to whatever window is currently focused.
2. **Make sure the application window has focus** - The program sends events to whatever window is focused. If you switch windows, events will go to the new focused window.
3. **Try test mode first** - If test mode works in a text editor, the main program should work too (they use the same logic)
4. **The application may be filtering input** - Some applications only accept input from specific devices or use raw input mechanisms
5. **This is application-specific behavior** - not a bug in this program

**Tip**: If test mode worked for you, the main program uses the exact same event-sending method, so it should work the same way. Just make sure to **double-click your game window** during the startup delay to ensure it has focus.

#### Window Focus Issues

If actions aren't reaching your game:

- **Double-click the game window** after starting the program (during the startup delay)
- The program sends events to whatever window currently has focus
- If you switch to another window, events will go to that window instead
- **Upcoming feature**: Window switching support will allow switching between windows while the program runs in the background

### Windows Issues

#### Build Errors

**"gcc is not recognized" or "No suitable C compiler found":**
- Install MinGW-w64 using one of the methods in the Requirements section
- If you have MSYS2 at `C:\msys64`, the build.bat script will find it automatically
- Make sure to close and reopen your command prompt after adding to PATH

**"libgmp-10.dll not found" or similar DLL errors:**
- This means GCC is found but DLLs are not accessible
- Solution: Add the MinGW `bin` directory to your PATH (e.g., `C:\msys64\mingw64\bin`)
- Or use `build.bat` which automatically adds the directory to PATH during compilation
- Close and reopen command prompt after modifying PATH

**"cl is not recognized":**
- Open "Developer Command Prompt for VS" from Start Menu
- Or install MinGW-w64 as an alternative (recommended)

#### Input Not Working

- Make sure the target window has focus
- Some applications (especially games with anti-cheat) may block programmatic input
- Try test mode first in a simple text editor to verify the build works
- Run as Administrator if needed (though usually not required)
- Windows `SendInput` API works similarly to Linux uinput - it creates low-level input events

## License

This software is provided as-is for educational and personal use.

## Disclaimer

Use this software responsibly and in accordance with the terms of service of any applications you use it with. The authors are not responsible for any consequences resulting from the use of this software.

