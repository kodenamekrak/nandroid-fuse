use std::collections::HashMap;

pub struct InodeTable {
    map: HashMap<u64, String>,
}

impl InodeTable {
    pub fn new() -> Self {
        InodeTable {
            map: HashMap::default(),
        }
    }

    pub fn add(&mut self, path: &str) -> u64 {
        self.map.insert(self.map.len() as u64 + 1, String::from(path));
        self.map.len() as u64
    }

    pub fn get_inode(&self, path: &str) -> Option<u64> {
        self.map.iter()
            .find(|(_i, p)| p.as_str().cmp(path).is_eq())
            .map(|pair| pair.0.clone())
    }

    pub fn get_path(&self, inode: u64) -> Option<&str> {
        self.map.get(&inode)
            .map(|p| p.as_str())
    }
}