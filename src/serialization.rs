use std::io;

pub trait Readable {
    // Should read exact amount of bytes required to fill the buffer
    // Else returns error
    fn read(&mut self, buffer: &mut [u8]) -> io::Result<()>;
}

pub trait Writable {
    // Shuld write the entire buffer
    // Else returns error
    fn write(&mut self, buffer: &mut [u8]) -> io::Result<()>;
}

struct DataWriter<W: Writable> {
    writer: W,
}

struct DataReader<R: Readable> {
    reader: R,
}

impl<R> DataReader<R> 
    where R: Readable
{
    fn new(reader: R) -> Self {
        DataReader {
            reader
        }
    }

    fn read_byte(&mut self) -> io::Result<u8> {
        let mut buf = [0u8; 1];
        self.reader.read(&mut buf)?;
        Ok(u8::from_le_bytes(buf))
    }

    fn read_u16(&mut self) -> io::Result<u16> {
        let mut buf = [0u8; 2];
        self.reader.read(&mut buf)?;
        Ok(u16::from_le_bytes(buf))
    }

    fn read_u32(&mut self) -> io::Result<u32> {
        let mut buf = [0u8; 4];
        self.reader.read(&mut buf)?;
        Ok(u32::from_le_bytes(buf))
    }

    fn read_u64(&mut self) -> io::Result<u64> {
        let mut buf = [0u8; 8];
        self.reader.read(&mut buf)?;
        Ok(u64::from_le_bytes(buf))
    }

    fn read_string(&mut self) -> io::Result<String> {
        let size = self.read_u16()?;
        let mut buf = vec![0u8; size.into()];
        self.reader.read(&mut buf)?;
        
        match String::from_utf8(buf) {
            Ok(s) => Ok(s),
            Err(e) => Err(io::Error::new(io::ErrorKind::InvalidData, e.to_string()))
        } 
    }
}

impl<W> DataWriter<W>
    where W: Writable 
{

}