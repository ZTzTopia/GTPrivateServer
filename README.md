# Growtopia Private Server
[![GitHub Release](https://img.shields.io/github/release/ZTzTopia/GTPriavteServer.svg)](https://github.com/ZTzTopia/GTPriavteServer/releases/latest) 
![Github Action](https://github.com/ZTzTopia/GTPrivateServer/actions/workflows/cmake.yml/badge.svg)

This is a private server for Growtopia uses [spdlog](https://github.com/gabime/spdlog), [zlib](https://github.com/madler/zlib), [brotli](https://github.com/google/brotli), [MariaDB](https://mariadb.org/), [sqlpp11](https://github.com/rbock/sqlpp11), [Redis](https://redis.io/), [libuv](https://github.com/libuv/libuv), [uvw](https://github.com/skypjack/uvw/), [hiredis](https://github.com/redis/hiredis), and [redis++](https://github.com/sewenew/redis-plus-plus).

## 📜 Features
- [ ] Server gateway (This is to connect to sub-servers it will use load balancer to choose which sub-servers are suitable to join).
- [ ] Load balancer (Weighted least connection algorithm and check load avg for linux).
- [ ] Sub-server (This is vertical scaling, if u have 4 cpu cores it will run 4 sub-servers. I will make horizontal scaling soon).
- [x] Database (MariaDB server or you can use MySql server).
- [ ] Anti cheat.
- [x] Player database. (Guest account)
- [x] Player inventory.
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

## 📝 Requirements
You need download [Conan, the C/C++ Package Manager](https://conan.io) and install it. CMake will install needed package for windows with [Conan, the C/C++ Package Manager](https://conan.io/).
Required python >= 3.5
```shell
$ pip install conan
```

## 🔨 Building The Source
```shell
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

## 📦 Running The Program
- Run [MySQL](https://www.mysql.com/)/[MariaDB](https://mariadb.org/) server
- Run [Redis](https://redis.io/) server.
- Move your latest `items.dat` to `path/to/program/data/`.
- Run the program
- Enjoy

## ✨ Credits
- Thanks to one of my friends helped a lot to this project.
