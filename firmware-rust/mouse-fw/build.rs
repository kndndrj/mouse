//! This build script copies the `memory.x` file from the crate root into
//! a directory where the linker can always find it at build time.
//! For many projects this is optional, as the linker always searches the
//! project root directory -- wherever `Cargo.toml` is. However, if you
//! are using a workspace or have a more complicated build setup, this
//! build script becomes required. Additionally, by requesting that
//! Cargo re-run the build script whenever `memory.x` is changed,
//! updating `memory.x` ensures a rebuild of the application with the
//! new memory settings.

use std::env;
use std::fs::File;
use std::io::Write;
use std::path::PathBuf;
use std::process::Command;

fn main() {
    // Put `memory.x` in our output directory and ensure it's
    // on the linker search path.
    let out = &PathBuf::from(env::var_os("OUT_DIR").unwrap());
    File::create(out.join("memory.x"))
        .unwrap()
        .write_all(include_bytes!("memory.x"))
        .unwrap();
    println!("cargo:rustc-link-search={}", out.display());

    // Rerun build script when the following paths change:
    println!("cargo:rerun-if-changed=memory.x");
    println!("cargo:rerun-if-changed=.git/");

    // Embed git hash/tag into code (e.g. USB strings)
    // TODO: fix windows command
    let output = if cfg!(target_os = "windows") {
        Command::new("cmd")
            .args(&[
                "/C",
                "git describe --exact-match --tags || git rev-parse --short HEAD",
            ])
            .output()
            .expect("failed to run cmd command")
    } else {
        Command::new("sh")
            .args(&[
                "-c",
                "git describe --exact-match --tags || git rev-parse --short HEAD",
            ])
            .output()
            .expect("failed to run shell command")
    };

    // Fallback
    let mut version: String = String::from("mouse");
    if output.status.success() {
        version = String::from_utf8(output.stdout).unwrap();
    }
    println!("cargo:rustc-env=USB_SERIAL_NUMBER={}", version);
}
