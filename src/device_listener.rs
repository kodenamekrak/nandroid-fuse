use crate::adb::Adb;

use std::default;
use std::thread::sleep;
use std::time::Duration;

use tracing::info;
use tracing::debug;

pub struct DeviceListener {
    adb: Adb,
    connected_devices: Vec<String>,
}

impl DeviceListener {
    pub fn new() -> Self {
        DeviceListener {
            adb: Adb::new(),
            connected_devices: Vec::new(),
        }
    }

    pub fn setup(&mut self) -> Result<(), ()> {
        self.adb.setup()
    }

    pub fn listen(&mut self) {
        loop {
            sleep(Duration::from_secs(3));
            let devices = &self.list_devices();
            let to_connect: Vec<&String> = devices.into_iter().filter(|d| !self.connected_devices.contains(d)).collect();
            if to_connect.len() == 0 {
                continue;
            }

            info!("Found {} new device(s)!", to_connect.len());
            for device in devices {
                let _ = &self.connect_device(device);
            }
        }
    }

    // TODO: Add some way to set manual mountpoints
    // /etc config?
    fn connect_device(&mut self, device: &str) {
        info!("Connecting device {}", device);
        self.connected_devices.push(String::from(device));
        // Create filesystem then mount
    }

    pub fn list_devices(&self) -> Vec<String> {
        let out = self.adb.run_command(["devices"].iter())
            .expect("Failed to list devices!");
        
        let devices: Vec<String> = match String::from_utf8(out.stdout) {
            Ok(s) => {
                // Remove empty lines
                let lines: Vec<String> = s
                    .split("\n")
                    .map(|s| s.to_string())
                    .filter(|s| s.split_whitespace().count() > 0)
                    .collect();

                lines
                    .iter()
                    .skip(1)
                    // Get the device entry from each line
                    .map(|s|
                        s.to_string()
                        .split_whitespace()
                        .take(1)
                        .collect::<Vec<&str>>()
                        .first()
                        .unwrap()
                        .to_string()
                    )
                    .collect()
            },
            Err(..) => return Vec::new(),
        };
        
        devices
    }
}