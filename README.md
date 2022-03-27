# Growtopia Private Server
This is a private server for Growtopia.

## 📜 Features
- [x] Server gateway (This is to connect to sub-servers it will use load balancer to choose which sub-servers are suitable to join).
- [x] Load balancer (Weighted least connection algorithm and check load avg for linux).
- [x] Sub-server (This is vertical scaling, if u have 4 cpu cores it will run 4 sub-servers. I will make horizontal scaling soon).
- [x] Database (MariaDB server or you can use MySql server).
- [ ] Anti cheat.
- [x] Player database. (Guest)
- [ ] Player inventory.
- [ ] Player stats.
- [ ] Player clothes.
- [ ] Player commands.
- [x] World database (Will check hash of all tile before saving the world to database).
- [x] World block.
- [ ] World object.
- [ ] World plant.
- [ ] World sub-server (Will check load avg before join the world).
- [x] Punch block.
- [ ] Place block.
- [ ] Drop item.
- [ ] Pick up item.

## ⚙️ Setting Up
- Rename `src/config.sample.h` to `src/config.h` and fill it.

## 🔨 Building The Source
- If you on windows, you need to install mariadb connector c from https://mariadb.com/downloads/connectors/
```shell
mkdir build
cd build
cmake ..
cmake --build .
```

## 📦 Running The Program
- If you on windows, you need move all needed library from `out/_deps` (**brotlicommon.dll**, **brotlienc.dll**, **brotlidec.dll**, **spdlogd.dll**, and **zlib1.dll**) to `path/to/program/`.
- Move your latest `items.dat` to `path/to/program/data/`.

## ✨ Credits
- Thanks to one of my friends helped a lot to this project.
