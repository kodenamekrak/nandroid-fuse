use fuser::MountOption;

use crate::connection::Connection;
use crate::inode_table::InodeTable;

use std::ffi::OsStr;
use std::path::PathBuf;
use std::process::Child;
use std::io;

use libc::ENOENT;

const IGNORE_FILES: &[&str] = &[".Trash", ".Trash-1000", ".xdg-volume-info", "autorun.inf"];

pub struct Filesystem {
    connection: Connection,
    _daemon_process: Child,
    inode_table: InodeTable,
}

impl Filesystem {
    pub fn new(connection: Connection, daemon_process: Child) -> Self {
        let mut inode_table = InodeTable::new();
        let _ = inode_table.add("/");
        
        Filesystem {
            connection,
            _daemon_process: daemon_process,
            inode_table,
        }
    }

    pub fn mount(self, path: &str) -> io::Result<()> {
        let options = [MountOption::AutoUnmount];
        fuser::mount2(self, path, &options)
    }

    fn get_device_path(path: &str, name: &OsStr) -> PathBuf {
        PathBuf::from("/sdcard")
            .join(path.strip_prefix("/").unwrap_or(path))
            .join(name.to_string_lossy().into_owned())
    }
}

impl fuser::Filesystem for Filesystem {

    fn lookup(&mut self, _req: &fuser::Request<'_>, parent: u64, name: &std::ffi::OsStr, reply: fuser::ReplyEntry) {
        if parent == 1 && IGNORE_FILES.contains(&name.to_string_lossy().into_owned().as_str()) {
            reply.error(ENOENT);
            return;
        }

        let Some(path) = self.inode_table.get_path(parent) else {
            reply.error(ENOENT);
            return;
        };

        let device_path = Self::get_device_path(path, name);

        reply.error(ENOENT);
    }
}