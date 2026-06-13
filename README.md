# purelibm

PureLibm is a C reference implementation project for readable, portable, correctly rounded mathematical functions.

## Development

```sh
cmake -S . -B build -G Ninja -DPURELIBM_REQUIRE_MPFR=ON -DPURELIBM_REQUIRE_CORE_MATH=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Initialize submodules before configuring a fresh checkout:

```sh
git submodule update --init --recursive
```

## License

```c
// SPDX-License-Identifier: MIT OR Apache-2.0
```

`purelibm` is licensed under either of

- MIT license ([LICENSE-MIT](LICENSE-MIT))
- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE))

at your option.
