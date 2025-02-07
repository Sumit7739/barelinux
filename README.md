# BareLinux

BareLinux is a minimal Linux distribution built for learning purposes, using BusyBox as the core utility set and a lightweight window manager.

## Features
- Lightweight and minimal
- Uses BusyBox for essential utilities
- Simple window manager for basic GUI support
- Designed for learning and experimentation

## Getting Started
### Prerequisites
- A Linux environment (Ubuntu, Arch, etc.)
- `qemu` (for testing)
- `make`, `gcc`, and other essential build tools

### Building BareLinux
```sh
# Clone the repository
git clone https://github.com/yourusername/barelinux.git
cd barelinux

# Build the system
make

# Run in QEMU
make run
```

## Roadmap
- [ ] Init system setup
- [ ] BusyBox integration
- [ ] Basic shell environment
- [ ] Window manager support
- [ ] Package management (optional)

## Contributing
Contributions are welcome! Feel free to open an issue or submit a pull request.

## License
This project is licensed under the MIT License.
