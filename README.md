# MaSQLite

Welcome to the SQLite Clone project! This repository contains the source code for a simple SQLite clone, built from scratch. The goal of this project is to understand the inner workings of a database management system and gain hands-on experience with database development.

## Table of Contents

- [Introduction](#introduction)
- [Getting Started](#getting-started)
- [Build and Run](#build-and-run)
- [Features](#features)
- [Future Improvements](#future-improvements)
- [License](#license)

## Introduction

This project aims to create a simplified version of SQLite, a lightweight and widely-used SQL database engine. It covers various aspects of database management, including storage management, query processing, and transaction management.

## Getting Started

To get started with the SQLite Clone project, follow these steps:

1. Clone the repository:
   ```sh
   git clone https://github.com/jaxoj/masqlite.git
   ```

2. Navigate to the project directory:
   ```sh
   cd masqlite
   ```

3. Set up your development environment (compiler, debugger, etc.).

## Build and Run

To build and run the project, use the following commands:

1. Build the project using the `Makefile`:
   ```sh
   meson setup builddir
   meson compile -C buildir
   ```

2. Run the executable:
   ```sh
   ./builddir/src/masqlite
   ```

This will create a simple database file and test the read/write functionality of the storage engine.

## Features

The current implementation includes the following features:

- Basic storage engine for reading and writing pages to and from the disk.
- Simple file-based storage system.
- Test cases to verify the functionality of the storage engine.

## Future Improvements

Some potential future improvements for the project include:

- Implementing the SQL parser and tokenizer.
- Developing the virtual machine for executing bytecode.
- Adding transaction management and logging.
- Optimizing performance with indexing and caching.
- Supporting more advanced SQL operations (e.g., JOINs, subqueries).

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.