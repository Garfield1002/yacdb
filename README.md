# YACDB

Yet Another C DataBase is a database built from scratch in C.

## Installation

CMake is requiered

1. Clone the repositiory
 ```bash
git clone https://github.com/Garfield1002/yacdb.git
cd yacdb
```
2. Setup & Build (server-side)
 ```bash
sh server-side/setup.sh && sh server-side/build.sh
```

3. Build (client-side)
```bash
make -C client-side all
```

## Console usage

1. Launch the server
 ```bash
server-side/server
```

2. Launch any number of clients
 ```bash
client-side/client
```

## Queries in file

It is possible to execute a series of query stored in a file :

```bash
client-side/client file_path
```

If the server is running, this will send the instruction to the server and execute them.

## Documentation
https://garfield1002.github.io/yacdb/
