use std::collections::HashMap;

pub struct InodeTable {
    map: HashMap<u64, String>,
    // When inodes are removed then those lower bounds can be stored and later reused
    free_entries: Vec<u64>,
}

impl InodeTable {
    pub fn new() -> Self {
        InodeTable {
            map: HashMap::default(),
            free_entries: Vec::default(),
        }
    }

    pub fn add(&mut self, path: &str) -> u64 {
        let next_inode = self.free_entries.pop()
            .unwrap_or(self.map.len() as u64 + 1);
        self.map.insert(next_inode, String::from(path));
        
        next_inode
    }

    pub fn remove_inode(&mut self, inode: u64) {
        if self.map.remove(&inode).is_some() {
            self.free_entries.push(inode);
        }
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