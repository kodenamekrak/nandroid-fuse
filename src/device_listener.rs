use crate::adb::Adb;
use crate::connection::Connection;
use crate::filesystem::Filesystem;

use std::thread::sleep;
use std::time::Duration;

use std::io;

use tracing::info;
use tracing::error;

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

    fn poll_new_devices(&mut self) -> io::Result<()> {
        let devices = self.list_devices()?;

        let to_connect: Vec<String> = devices.into_iter()
            .filter(|d| !self.connected_devices.contains(d))
            .collect();

        for device in &to_connect {
            info!("Connecting device {}", device);
            if let Err(e) = self.connect_device(device) {
                error!("Failed to connect device: {}", e.to_string());
            }
        }

        Ok(())
    }

    pub fn listen(&mut self) {
        loop {
            let _ = self.poll_new_devices();
            sleep(Duration::from_secs(3));
        }
    }

    // TODO: Add some way to set manual mountpoints
    // /etc config?
    fn connect_device(&mut self, device: &str) -> io::Result<()> {
        let daemon_process = self.adb.push_and_exec_daemon(device, 28933)?;
        //  TODO: properly increment ports when multiple devices are connected
        let connection = Connection::connect("localhost", 28933)?;

        let fs = Filesystem::new(connection, daemon_process);
        fs.mount("./mount")?;

        self.connected_devices.push(String::from(device));

        Ok(())
    }

    pub fn list_devices(&self) -> io::Result<Vec<String>> {
        let out = self.adb.run_command(&["devices"])?;

        String::from_utf8(out.stdout).and_then(|s| {
            // Remove empty lines
            Ok(s.split("\n")
            .filter(|s| s.split_whitespace().count() > 0)
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
            .collect())
        }).map_err(|e| io::Error::new(io::ErrorKind::InvalidData, e.to_string()))
    }
}