# **sshbook**

Minimal terminal-based SSH launcher for saving ssh entries.

sshbook is a simple TUI tool for managing and launching SSH connections. It stores entries in a plain-text file and lets you add, delete, and connect to hosts from a scrollable list, from a cli.
 Features

    Add, remove, and connect to SSH entries

    Scrollable TUI menu with keyboard navigation & vim mode

    Minimal dependencies

**Installation**

Requirements

    C compiler (optional)

    Unix system (Linux, macOS, etc.)

**Build & Install**

**Option 1:**

    make
    sudo make install

This installs sshbook to /usr/local/bin/.

**Option 2:**
    
    sudo ./install

this copies the sshbook executable to /usr/local/bin/.

![usagegif](./usagegif.gif)



**Usage**


Run:

    sshbook

Controls

    ↑ / k: Go upwards
    ↓ / j: Go downwards
    [Enter] Select entry
    a: Add entry
    d: delete entry
    [q]: exit
