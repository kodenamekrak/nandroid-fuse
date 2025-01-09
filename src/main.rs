mod adb;
pub mod device_listener;

use device_listener::DeviceListener;

use tracing_subscriber;

fn main() {
    tracing_subscriber::fmt().init();
    let mut listener = DeviceListener::new();
    // TODO: better logging
    listener.setup().expect("Failed to set up device listenter");
    listener.listen();
}
