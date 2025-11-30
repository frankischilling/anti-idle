# anti-idle

A smart anti-idle program for Linux that sends hardware-level keyboard input to prevent applications from timing out due to inactivity. Uses Linux uinput to create real keyboard events that games and applications will accept. Successfully tested with Old School RuneScape (OSRS).

## Features

- **Hardware-level input**: Uses Linux uinput to create real keyboard events that games accept
- **Automatic window focusing**: Automatically focuses and raises the target window before each input, ensuring inputs always go to the selected window even when it's out of focus or minimized
- **Simple and reliable**: Sends arrow key presses (Left, Right, Up, Down) directly to the target window
- **Advanced anti-cheat bypass**: Sophisticated human behavior simulation
  - Weighted timing patterns (short intervals more common, long intervals rare)
  - Micro-variations in timing (±5 seconds) for natural feel
  - Human-like key press durations (30-80ms random)
  - Occasional double-taps (8% chance) simulating uncertainty
  - Misclick corrections (3% chance) - presses wrong key then corrects
  - Extended breaks (5% chance) simulating tab-out/distraction
  - Random arrow key selection (Left, Right, Up, Down)
- **Detailed logging**: Logs every action with timestamps, press durations, and session statistics
- **Test mode included**: Verify uinput is working before using the main program
- **Tested and working**: Successfully tested with Old School RuneScape (OSRS)

## Requirements

- Linux with X11
- X11 development libraries (`libX11-dev`)
- GCC compiler
- `uinput` kernel module (usually built-in)
- Root access OR membership in the `input` group

### Installing Dependencies

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

### Why uinput?

This program uses `uinput` to create hardware-level input events that games like RuneLite will accept. Unlike XTest (which creates synthetic X11 events), uinput creates real hardware events that games cannot distinguish from actual keyboard/mouse input.

## Project Structure

```
anti-afk/
├── src/              # Source code
│   └── antiidle.c
├── bin/              # Compiled binary (created on build)
├── build/            # Build artifacts (created on build)
├── Makefile          # Build configuration
├── README.md         # This file
└── .gitignore        # Git ignore rules
```

## Building

```bash
make
```

The compiled binary will be in the `bin/` directory.

## Usage

```bash
./bin/antiidle [startup_delay] [--test]
```

- `startup_delay`: Optional delay in seconds before starting actions (default: 10 seconds)
- `--test` or `-t`: Test mode - sends LEFT arrow key every second for 10 seconds to verify uinput is working

### Quick Start (OSRS Example)

1. **Start the program:**
   ```bash
   ./bin/antiidle
   ```

2. **Click on your game window** when prompted (e.g., RuneLite/OSRS)

3. The program will automatically focus the target window and start sending arrow key presses

### Examples

```bash
# Start with default 10-second delay
./bin/antiidle

# Start with 5-second delay (less time to focus window)
./bin/antiidle 5

# Test mode - verify uinput is working
./bin/antiidle --test
```

### Test Mode

Before using the program, you should verify that uinput is working correctly:

1. **Open a text editor or terminal** (any application that accepts keyboard input)
2. **Focus that window**
3. **Run test mode:**
   ```bash
   ./bin/antiidle --test
   ```
4. **Watch for cursor movement** - if the cursor moves left every second, uinput is working!

If test mode shows no cursor movement:
- Check that you focused a text editor/terminal
- Verify the device exists: `cat /proc/bus/input/devices | grep -A4 "Anti-Idle"`
- Try running as root: `sudo ./bin/antiidle --test`
- Check error messages for permission issues

### How It Works

1. **Window Selection**: When you run the program, click on the window you want to keep active
2. **Startup Delay**: The program waits for the specified delay (default 10 seconds)
3. **Automatic Window Focusing**: Before each input action, the program automatically:
   - Focuses the target window
   - Raises it to the top if needed
   - Unminimizes it if it was minimized
   - Ensures inputs always go to the selected window
4. **Automatic Actions**: The program will automatically:
   - Randomly press arrow keys (Left, Right, Up, Down)
   - Use the same reliable method as test mode (proven to work)
   - Vary the timing between actions (45 seconds to 4 minutes)
   - Send hardware-level input events directly via uinput
   - Log detailed information about each action including press duration

**Important**: 
- The program automatically focuses the target window before each input
- You can click away or minimize the target window - inputs will still go to it
- The window will be automatically focused, raised, and unminimized as needed

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

To install system-wide:

```bash
sudo make install
```

This installs the binary to `/usr/local/bin/antiidle`.

To uninstall:

```bash
sudo make uninstall
```

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

- Uses Linux `uinput` kernel module for hardware-level input events
- Uses X11 for window selection and automatic window focusing
- Uses X11 EWMH (Extended Window Manager Hints) to focus, raise, and unminimize windows
- Sends events directly via uinput after ensuring the target window has focus
- Simple and reliable: Uses the same proven method as test mode

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

The program creates a virtual input device using `uinput` and sends real hardware-level keyboard events. This approach bypasses the limitations of XTest/X11 synthetic events, allowing the program to work with games like RuneLite that filter out synthetic input.

Before each input action, the program uses X11 EWMH to automatically focus, raise, and unminimize the target window. This ensures inputs always go to the selected window, even if you've clicked away or minimized it.

The main program uses the exact same event-sending logic as test mode, which has been proven to work.

The anti-cheat bypass works by creating behavior patterns that are statistically similar to real human players, making detection extremely difficult.

**Permissions:** The program needs access to `/dev/uinput`, which typically requires:
- Running as root, OR
- Being a member of the `input` group (recommended)

## Build Commands

**Build the project:**
```bash
make
```

**Clean build artifacts:**
```bash
make clean
```

This removes the `bin/` and `build/` directories and all compiled files.

## Troubleshooting

### "Cannot open /dev/uinput" Error

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

### Verifying uinput Device Creation

After starting the program, you can verify the virtual device was created:

```bash
cat /proc/bus/input/devices | grep -A4 "Anti-Idle"
```

You should see output showing the "Anti-Idle Virtual Input" device. If you don't see it, check the error messages when starting the program.

### Events Not Working in Application

If test mode works (cursor moves in text editor) but events don't work in your target application:

1. **The program automatically focuses the target window** - You don't need to manually focus it. The program will focus, raise, and unminimize the window before each input.
2. **Try test mode first** - If test mode works in a text editor, the main program should work too (they use the same logic)
3. **The application may be filtering input** - Some applications only accept input from specific devices or use raw input mechanisms
4. **This is application-specific behavior** - not a bug in this program

**Tip**: If test mode worked for you, the main program uses the exact same event-sending method, so it should work the same way. The program automatically handles window focusing for you.

### Window Focus Issues

The program now automatically handles window focusing:

- **Automatic focusing**: The target window is automatically focused before each input action
- **Automatic raising**: The window is raised to the top if needed
- **Automatic unminimizing**: If the window is minimized, it will be automatically restored
- **Works in background**: You can click away or minimize the target window - inputs will still go to it

## License

This software is provided as-is for educational and personal use.

## Disclaimer

Use this software responsibly and in accordance with the terms of service of any applications you use it with. The authors are not responsible for any consequences resulting from the use of this software.

