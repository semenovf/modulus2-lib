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

```sh
$ export SOURCE_DIR=modulus2
$ export BUILD_TYPE=Debug    # optional (default is Debug)
$ export CXX_STANDARD=11     # optional (default is unset)
$ export CXX_COMPILER=g++-9  # optional (default is unset)
$ export C_COMPILER=gcc-9    # optional (default is unset)
$ export PFS_MODULUS2_LIB__BUILD_STRICT=OFF    # optional (default is ON)
$ export PFS_MODULUS2_LIB__BUILD_TESTS=ON      # optional (default is OFF)
$ export PFS_MODULUS2_LIB__BUILD_DEMO=ON       # optional (default is OFF)
$ export PFS_MODULUS2_LIB__ENABLE_COVERAGE=OFF # optional (default is OFF)
$ $SOURCE_DIR/build.sh
```
