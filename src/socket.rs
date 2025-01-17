use std::io;
use std::io::Read;
use std::io::Write;
use std::net::TcpStream;

pub struct Socket {
    stream: TcpStream,
}

impl Socket {
    pub fn connect(address: &str, port: u16) -> io::Result<Socket> {
        let stream = TcpStream::connect(format!("{}:{}", address, port))?;
        Ok(Socket {
            stream
        })
    }

    pub fn read_byte(&mut self) -> io::Result<u8> {
        let mut buf = [0u8; 1];
        self.stream.read_exact(&mut buf)?;
        Ok(u8::from_le_bytes(buf))
    }

    pub fn read_u16(&mut self) -> io::Result<u16> {
        let mut buf = [0u8; 2];
        self.stream.read_exact(&mut buf)?;
        Ok(u16::from_le_bytes(buf))
    }

    pub fn read_u32(&mut self) -> io::Result<u32> {
        let mut buf = [0u8; 4];
        self.stream.read_exact(&mut buf)?;
        Ok(u32::from_le_bytes(buf))
    }

    pub fn read_u64(&mut self) -> io::Result<u64> {
        let mut buf = [0u8; 8];
        self.stream.read_exact(&mut buf)?;
        Ok(u64::from_le_bytes(buf))
    }

    pub fn read_string(&mut self) -> io::Result<String> {
        let size = self.read_u16()?;
        let mut buf = vec![0u8; size.into()];
        self.stream.read_exact(&mut buf)?;

        String::from_utf8(buf).or_else(|e| {
            Err(io::Error::new(io::ErrorKind::InvalidData, e.to_string()))
        })
    }

    pub fn write_byte(&mut self, data: u8) -> io::Result<()> {
        self.stream.write_all(data.to_le_bytes().as_mut())
    }

    pub fn write_u16(&mut self, data: u16) -> io::Result<()> {
        self.stream.write_all(data.to_le_bytes().as_mut())
    }

    pub fn write_u32(&mut self, data: u32) -> io::Result<()> {
        self.stream.write_all(data.to_le_bytes().as_mut())
    }

    pub fn write_u64(&mut self, data: u64) -> io::Result<()> {
        self.stream.write_all(data.to_le_bytes().as_mut())
    }
    
    pub fn write_string(&mut self, data: &str) -> io::Result<()> {
        self.write_u16(data.len() as u16)?;
        self.stream.write_all(data.as_bytes())
    }
    
    pub fn flush(&mut self) -> io::Result<()> {
        self.stream.flush()
    }
}