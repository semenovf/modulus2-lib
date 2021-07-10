# modulus2-lib
(mod[ul]us vivendi, mod[ul]us operandi)

# Usage
## Clone from github repository
```sh
$ git clone https://github.com/semenovf/modulus2-lib.git modulus2

# Update third-party dependecies
$ git submodule update --init
```

## Build tests, examples, demos on Linux

### Using bash script
```sh
$ export SOURCE_DIR=modulus2
$ export BUILD_TYPE=Debug    # optional (default is Debug)
$ export BUILD_STRICT=OFF    # optional (default is ON)
$ export CXX_STANDARD=11     # optional (default is unset)
$ export CXX_COMPILER=g++-9  # optional (default is unset)
$ export C_COMPILER=gcc-9    # optional (default is unset)
$ export BUILD_TESTS=ON      # optional (default is unset)
$ export BUILD_DEMO=OFF      # optional (default is unset)
$ export ENABLE_COVERAGE=OFF # optional (default is unset)
$ $SOURCE_DIR/build.sh
```

### Manually
```sh

```
