# Nand2Tetris: Building a Modern Computer from First Principles

![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg) ![Language](https://img.shields.io/badge/language-Jack-green.svg) ![Language](https://img.shields.io/badge/Hardware-HDL-orange.svg)

> **"What I cannot create, I do not understand."** - Richard Feynman

This repository contains my full implementation of the **Nand2Tetris** (The Elements of Computing Systems) curriculum. I built a general-purpose computer system from the ground up, starting from elementary logic gates (NAND) to a fully functioning Operating System and a high-level game application.

## 🚀 Project Overview & Pipeline

The core value of this project lies in the **seamless translation chain** that converts high-level object-oriented code into binary machine instructions. I implemented every tool in this toolchain using **C++**.

### The Compilation Hierarchy
I constructed the following software hierarchy to bridge the gap between human-readable code and hardware logic:

```mermaid
graph LR
    A[Jack Source Code] -- "Compiler" --> B[VM Code]
    B -- "VM Translator" --> C[Assembly Code]
    C -- "Assembler" --> D[Binary Machine Code]
    D -- "CPU Execution" --> E[Hack Hardware]
