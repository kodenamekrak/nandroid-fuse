use std::fs::File;
use std::io::Write;
use std::process;
use std::process::Child;
use std::process::Output;

use std::io;
use std::vec;

pub struct Adb {
    adb_path: Option<String>,
}

const DAEMON_BYTES: &[u8] = include_bytes!("../nandroidfs/nandroid_daemon/libs/arm64-v8a/nandroid-daemon");

const DAEMON_TARGET_LOCATION: &str = "/data/local/tmp/nandroid-daemon";

const DAEMON_PORT: u16 = 28933;

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

    pub fn run_command(&self, args: &[&str]) -> io::Result<Output> {
        let out = 
            std::process::Command::new(&self.adb_path.as_ref().unwrap())
                .args(args)
                .output()?;

        Ok(out)
    }

    pub fn run_command_with_device(&self,  device: &str, args: &[&str]) -> io::Result<Output> {
        let mut v = vec!["-s", device];
        v.extend_from_slice(args);
        self.run_command(&v)
    }

    pub fn push_daemon(&self, device: &str) -> io::Result<()> {
        let tmp_file = std::env::temp_dir().join("nandroid-daemon");
        if !tmp_file.exists() {
            let mut file = File::create(&tmp_file)?;
            file.write_all(DAEMON_BYTES)?;
        }

        let _ = self.run_command_with_device(device, &["push", tmp_file.to_str().unwrap(), &DAEMON_TARGET_LOCATION])?;
        let _ = self.run_command_with_device(device, &["shell", "chmod", "777", &DAEMON_TARGET_LOCATION])?;

        Ok(())
    }

    pub fn forward_port(&self, device: &str, local_port: u16, remote_port: u16) -> io::Result<()> {
        self.run_command_with_device(device, &["forward", &format!("tcp:{}", local_port), &format!("tcp:{}", remote_port)])?;

        Ok(())
    }

    pub fn push_and_exec_daemon(&self, device: &str, port: u16) -> io::Result<Child> {
        self.forward_port(device, port, DAEMON_PORT)?;
        self.push_daemon(device)?;
        
        process::Command::new(&self.adb_path.as_ref().unwrap())
            .arg("shell")
            .arg(&DAEMON_TARGET_LOCATION)
            .spawn()
    }
}