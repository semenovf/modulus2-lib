# modulus2-lib
(mod[ul]us vivendi, mod[ul]us operandi)

# Usage
## Clone from github repository
```sh
$ git clone https://github.com/semenovf/modulus2-lib.git modulus2

# Update third-party dependencies
$ git submodule update --init
```

If need specific third party libraries / backends it's better to update
repositories manually:

```sh
# Mandatory. Common library
$ git submodule update --init --remote --merge -- 3rdparty/pfs/common

# Optional. If need RocksDB support for settings plugin
$ git submodule update --init -- 3rdparty/rocksdb

# Optional. If need spdlog support for logger
$ git submodule update --init -- 3rdparty/spdlog
```

## Build tests, examples, demos on Linux

`$ cd scripts` and run appropriate script `build.cxxZZZ.sh`.
Binary files can be found at `builds` subdirectory at the project parent directory.