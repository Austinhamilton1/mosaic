# Mosaic Kernel DSL Specification (v1)

## 1. Overview

The Mosaic Kernel DSL is a **small, statically-typed expression language** designed for distributed data-parallel execution.
Each program defines a **single kernel function** that maps **N scalar inputs to one scalar output.**

The DSL is intentionally minimal to allow:
- Easy compilation to bytecode
- Efficient SIMD vectorization at runtime
- Deterministic execution across heterogeneous worker nodes

The language contains **no loops, no function calls, and no heap allocation.**
All iteration and vectorization are handled by the Mosaic worker runtime.