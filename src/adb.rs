use std::process::Output;

use std::io;
use std::process::Stdio;

pub(crate) struct Adb {
    adb_path: Option<String>
}

impl Adb {
    pub fn new() -> Self {
        Adb {
            adb_path: None,
        }
    }
    
    // TODO: Extract ADB
    // Maybe have adb be setup externally and pass executable into constructor
    pub fn setup(&mut self) -> Result<(), ()> {
        self.adb_path = Some(String::from("/usr/bin/adb"));
        Ok(())
    }

    pub fn run_command<'a>(&self, args: impl Iterator<Item = &'a&'a str>) -> std::io::Result<Output> {
        let out = 
            std::process::Command::new(&self.adb_path.as_ref().unwrap())
                .args(args)
                .output()?;

        Ok(out)
    }

    pub fn run_command_with_device(&self, args: &[&str], device: &str) -> io::Result<Output> {
        self.run_command(vec!["-s", device].iter().chain(args))
    }
}