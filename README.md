# nandroidfs-fuse
WIP Linux port of [lauriethefish/nandroidfs](https://github.com/Lauriethefish/nandroidfs) written in Rust using [fuser](https://github.com/cberner/fuser)

## Cloning
```bash
git clone https://github.com/kodenamekrak/nandroid-fuse
git submodule update --init
# fixes incorrect capitalisation in daemon build file
sed -i 's/Nandroid/nandroid/' ./nandroidfs/nandroid_daemon/Android.mk
```

## Building
```bash
# Requires ANDROID_NDK_HOME environment variable set
pwsh ./nandroidfs/nandroid_daemon/build.ps1
cargo build
```
