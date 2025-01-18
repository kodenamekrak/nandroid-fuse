use fuser::MountOption;

use crate::connection::Connection;

use std::process::Child;
use std::io;

pub struct Filesystem {
    connection: Connection,
    _daemon_process: Child,
}

impl Filesystem {
    pub fn new(connection: Connection, daemon_process: Child) -> Self {
        Filesystem {
            connection,
            _daemon_process: daemon_process,
        }
    }

    pub fn mount(self, path: &str) -> io::Result<()> {
        let options = [MountOption::AutoUnmount];
        fuser::mount2(self, path, &options)
    }
}

impl fuser::Filesystem for Filesystem {
}