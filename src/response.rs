use std::io;

pub enum ResponseStatus {
    Success,
    GenericFailure,
    AccessDenied,
    NotADirectory,
    NotAFile,
    FileNotFound,
    FileExists, // Considered a success if opening a file with a mode that supports existing files.
    DirectoryNotEmpty,
    NoMoreEntries,
    IOError(io::Error),
}

impl From<u8> for ResponseStatus {
    fn from(value: u8) -> Self {
        match value {
            0 => ResponseStatus::Success,
            1 => ResponseStatus::GenericFailure,
            2 => ResponseStatus::AccessDenied,
            3 => ResponseStatus::NotADirectory,
            4 => ResponseStatus::NotAFile,
            5 => ResponseStatus::FileNotFound,
            6 => ResponseStatus::FileExists,
            7 => ResponseStatus::DirectoryNotEmpty,
            8 => ResponseStatus::NoMoreEntries,
            _ => ResponseStatus::GenericFailure
        }
    }
}

impl From<io::Error> for ResponseStatus {
    fn from(value: io::Error) -> Self {
        Self::IOError(value)
    }
}

pub struct FileStat {
    mode: u16,
    size: u64,
    access_time: u64,
    write_time: u64,
}

pub struct DirectoryEntry {
    name: String,
    stat: FileStat,
}

pub type Result<T> = std::result::Result<T, ResponseStatus>;
