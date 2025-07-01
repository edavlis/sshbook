# **sshbook**

Minimal terminal-based SSH launcher for saving ssh entries.

sshbook is a simple TUI tool for managing and launching SSH connections. It stores entries in a plain-text file and lets you add, delete, and connect to hosts from a scrollable CLI.

### Features
 1. Manage and connect to SSH entries
 2. Scrollable TUI menu with keyboard navigation & vim mode

### Installation

#### Dependencies
1. GCC
2. Unix-like OS

#### Build & Install
```
make
sudo make install
```

### Usage
```
sshbook
```

#### Controls
```
↑ / k: Go upwards
↓ / j: Go downwards
[Enter] Select entry
a: Add entry
d: delete entry
q: exit
h: help
```

#### Descriptions
Add a description to your addresses by adding a '/' and then a description.
```
126.42.98.92 / My server
126.42.98.92 / My router
```

![usagegif](./usagegif.gif)
