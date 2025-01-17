
#[repr(u8)]
pub enum RequestType {
    // Followed by a singular string - the full file/directory path.
    StatFile,
    // Followed by a singular string - the full directory path.
    ListDirectory,
    // Followed by a singular string - the full directory path.
    CreateDirectory,
    // Followed by the file path.
    // Checks if it is possible to remove the file at the given path.
    // 
    // This is necessary as dokan has a separate callback for checking it can delete before it actually releases
    // the file for us to clean-up.
    CheckRemoveFile,
    // Followed by directory path.
    // Checks if it is possible to remove the directory at the given path.
    CheckRemoveDirectory,
    // Followed by file path.
    RemoveFile,
    // Followed by directory path.
    RemoveDirectory,
    // Moves the entry at the specified path to a different location
    MoveEntry,
    // Followed by OpenHandleArgs
    OpenHandle,
    // Followed by the FILE_HANDLE
    CloseHandle,
    // Followed by ReadHandleArgs
    // Response is the number of bytes read (uint32_t), followed by the data read.
    ReadHandle,
    // Followed by WriteHandleArgs
    WriteHandle,
    // Followed by TruncateHandleArgs
    TruncateHandle,
    // Followed by SetFileTimeArgs
    SetFileTime,
    // No additional arguments
    // Gives a DiskStats as its response.
    GetDiskStats
}

impl Into<u8> for RequestType {
    fn into(self) -> u8 {
        self as u8
    }
}