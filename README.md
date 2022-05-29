<img src="https://github.com/Garfield1002/yacdb/blob/master/yacdb.png?raw=true" alt="YACDB logo" title="YACDB" align="right" height="60" />

# YACDB

**Yet Another C(rappy) DataBase** is a database **built from scratch** in C. It is heavely inspired by [SQLite](https://www.sqlite.org/) and would not be possible without their documentation.

## ⭐ Features

Currently, YACDB remains a very simple database, but it is planned to add more features in the future.

Supported features:

- Creation of tables

- Insertion of rows

- Selection of rows, with or without conditions

YACDB was built as a client/server database so you can use it remotely.

YACDB also implements a limited cache system to speed up the database access.

## 🛠 Building

YACDB can be built using CMake.

We have provided some utility scriots to help you with the build process.

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

## 💻 Console usage

1. Launch the server

```bash
server-side/server
```

2. Launch any number of clients

```bash
client-side/client
```

The available commands can be found in the documentation under the [Query Format](https://garfield1002.github.io/yacdb/md_client_side_query_format.html) section.

## Queries in file

It is possible to execute a series of query stored in a file :

```bash
client-side/client file_path
```

If the server is running, this will send the instruction to the server and execute them.

## Documentation

Detailed documentation is available on the [github page](https://garfield1002.github.io/yacdb/).

It was bult using [Doxygen](https://www.doxygen.nl/) and [Doxygen-Awesome](https://jothepro.github.io/doxygen-awesome-css/).
