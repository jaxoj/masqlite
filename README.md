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

Features
The current implementation includes the following features:

- Basic Storage Engine: Reading and writing pages to and from the disk.

- File-Based Storage System: Simple file-based storage for managing data.

- Page Allocation and Free Space Management: Allocate new pages and manage free space within pages.

- Caching Mechanism: Keep frequently accessed pages in memory for faster retrieval.

- B-tree Implementation: Efficient data retrieval and indexing using B-tree.

- B-tree Operations: Search, insert, and delete operations with special case handling.

- Support for Key-Value Pairs: Store key-value pairs in the B-tree structure.

- Memory Management: Address memory management issues and ensure proper deallocation.

- Virtual Machine Implementation: Design and implementation of a virtual machine (VM) for executing bytecode instructions.

- Function Calls in VM: Support for function calls (CALL and RET operations) in the VM.

## Future Improvements

Some potential future improvements for the project include:

### Implement Logging and Transaction Management:

Implement a simple write-ahead logging (WAL) mechanism to track changes and ensure data integrity.

Add commit and rollback operations to ensure atomic transactions and maintain data integrity.

### Implement Bytecode Instructions:

Implement the remaining bytecode instructions (JMP_IF_ZERO, JMP_IF_NOT_ZERO, etc.).

Add handling for SQL operations (SELECT, INSERT, UPDATE, DELETE).

### Integrate with Storage Engine:

Ensure that the bytecode instructions interact correctly with the storage engine and B-tree.

Implement mechanisms to read and write nodes from and to the disk.

### Advanced Transaction Management:

Continue refining logging and transaction management features.

Implement more advanced logging and recovery mechanisms.

### Persistent Storage Integration:

Handle efficient disk I/O for the VM and B-tree.

Ensure data consistency and durability across transactions.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.