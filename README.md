# LLVM Fork for Pass Plugins

The official LLVM toolchains are not built with plugin support (on Windows). This
repository distributes toolchains (starting from LLVM 10.x.x) for Linux, MacOS and
Windows with plugin support enabled.

Toolchains are also distributed with Rust compatibility (the Rust compiler
internally uses a fork of LLVM). For example, [this toolchain] can be used for
building Rust code (with `rustc` 1.59) and also be able to apply custom LLVM pass
plugins on the LLVM IR.

[this toolchain]: https://github.com/jamesmth/llvm-project/releases/tag/v13.0.0-rust-1.59

Using another version of `rustc` than the one specified by a distributed toolchain
should be fine, as long as your `rustc` uses the same LLVM version. For instance,
the previous example could work with `rustc` 1.58 as well, since it also uses LLVM 13.

## Installation

Just download one of the release archives, and extract it wherever you want. For the
remaining of this README, we assume a toolchain is installed into `~/llvm`.

## LTO Pass Plugins for Optimizing C++

Let's say you have an LLVM plugin that implements a new pass `hello`.

You can optimize C++ code at link time (LTO) with your custom pass, by using
the `lld` linker and supplying it with your plugin as an LTO pass plugin:

<details>
 <summary><em>On Linux</em></summary>

 ```shell
$ clang++ main.cc \
    -flto \
    -B~/llvm/bin \
    -fuse-ld=lld \
    -Xlinker --load-pass-plugin=./libplugin.so \
    -Xlinker --lto-newpm-passes='lto<O3>,hello'
 ```

</details>

<details>
 <summary><em>On MacOS</em></summary>

 ```shell
$ clang++ main.cc \
    -flto \
    -B~/llvm/bin \
    -fuse-ld=lld \
    -Xlinker --load-pass-plugin=libplugin.dylib \
    -Xlinker --lto-newpm-passes='lto<O3>,hello'
 ```

</details>

<details>
 <summary><em>On Windows</em></summary>

 ```shell
$ clang++ main.cc \
    -flto \
    -B~/llvm/bin \
    -fuse-ld=lld \
    -Xlinker /load-pass-plugin:plugin.dll \
    -Xlinker /lto-newpm-passes:'lto<O3>,hello'
 ```

</details>

***Note**: the `lto<O3>` pass is called before the custom pass `hello`, because
we might still want the default LTO pipeline to be run first.*

***Note**: On Windows, the pass plugin must link `lld.lib`.*

## LTO Pass Plugins for Optimizing Rust

Let's say you have an LLVM plugin that implements a new pass `hello`.

You can optimize Rust code at link time (LTO) with your custom pass, by using
the `lld` linker and supplying it with your plugin as an LTO pass plugin:

<details>
 <summary><em>On Linux</em></summary>

 ```shell
 $ RUSTFLAGS=" \
     -Clinker-plugin-lto \
     -Clink-arg=-B~/llvm/bin \
     -Clink-arg=-fuse-ld=lld \
     -Clink-arg=-Xlinker \
     -Clink-arg=--load-pass-plugin=/path/to/libplugin.so \
     -Clink-arg=-Xlinker \
     -Clink-arg=--lto-newpm-passes=hello \
 " \
 cargo b --release
 ```

</details>

<details>
 <summary><em>On MacOS</em></summary>

 ```shell
 $ RUSTFLAGS=" \
     -Clinker-plugin-lto \
     -Clinker=~/llvm/bin/lld \
     -Clink-arg=--load-pass-plugin=/path/to/libplugin.dylib \
     -Clink-arg=--lto-newpm-passes=hello \
 " \
 cargo b --release
 ```

</details>

<details>
 <summary><em>On Windows</em></summary>

 ```shell
 $ RUSTFLAGS=" \
     -Clinker-plugin-lto \
     -Clinker=~/llvm/bin/lld.exe \
     -Clink-arg=/load-pass-plugin:/path/to/plugin.dll \
     -Clink-arg=/lto-newpm-passes:hello \
 " \
 cargo b --release
 ```

</details>

***Note**: in a default `cargo` build, a custom pass would be called in parallel on
each codegen unit being linked by `lld`. If you want your pass to be called only
once, on the fully merged codegen units, you can add `lto=true` in your
[Cargo Profile].*

[Cargo Profile]: https://doc.rust-lang.org/cargo/reference/profiles.html

***Warning**: `cargo` also builds proc-macros and build-scripts, both requiring a linking
 phase. As a result, your custom passes will be called on these targets as well.
 Therefore, you should make sure to filter these out in your passes. For instance, you
 could check for strings like `"build_script_build"` in the LLVM IR module name. For
 proc-macros, there is no clear hint in their module names, so you may need to resort
 to some blacklisting scheme.*

***Note**: On Windows, the pass plugin must link `lld.lib`.*
