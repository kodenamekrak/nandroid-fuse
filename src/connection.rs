use std::io;

use crate::socket::Socket;

use tracing::debug;
use tracing::info;

pub struct Connection {
    socket: Socket,
}

const SOCKET_HANDSHAKE: u32 = 0xFAFE5ABE;

impl Connection {
    pub fn connect(address: &str, port: u16) -> io::Result<Self>
    {
        let socket = Socket::connect(address, port)?;
        
        let mut connection = Connection {
            socket
        };
        connection.handshake()?;
        
        Ok(connection)
    }

    fn handshake(&mut self) -> io::Result<()>
    {
        self.socket.write_u32(SOCKET_HANDSHAKE)?;
        self.socket.flush()?;

        self.socket.read_u32()
            .and_then(|hs| {
                if hs != SOCKET_HANDSHAKE {
                    return Err(io::Error::new(io::ErrorKind::InvalidData, 
                        format!("Recieved handshake did not match expected value! (expected {:X}, recieved {:X})", SOCKET_HANDSHAKE, hs)
                    ));
                }

                info!("Handshake succeeded!");
                Ok(())
            })
    }
}