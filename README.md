# xmscan

A simple pattern scanner for external process memory on Linux.
You can scan a loaded module of a target process using a byte pattern with wildcards and get the
matching address and offset as a result.

![output](./assets/xmscan.GIF)


## Usage

The following three arguments are required for running the program:

```
Usage:
  xmscan <pid> <module> <pattern>

  pid      Target process ID
  module   Name of the loaded module (e.g. libXau)
  pattern  Byte pattern with wildcards
```


## Example

```
sudo ./xmscan $(pidof -s vesktop) libXau "?3 0F 1? FA 41 56 66 0F EF C0 BA 01 ?? ?? ?? BE 02 ?? ?? ?? 41 55 41 54 55 53 48 83 EC 50 ?4 4?"
```

Output:

```
pattern found in 0x7f317440a6f0 at offset 5872
```

The program scans the selected module and matches the given pattern.
Wildcards (`?` / `??`) are ignored during comparison.

The returned address is the first match found, and the offset is relative
to the module base.

## Installation

Compile using `make`:

```
git clone https://github.com/vehchain/xmscan
cd xmscan
make all
```

you will likely need root permissions to scan external processes (`/proc/<pid>/maps`).
