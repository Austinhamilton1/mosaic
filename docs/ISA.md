# Mosaic Kernel VM - Bytecode Instruction Set (v1)

## 1. Overview

The Mosaic Kernel VM executes **stack-based bytecode** compiled from the Mosaic Kernel DSL. It is designed for:

- Single-kernel, data-parallel execution
- Deterministic, pure operations
- SIMD vectorization (handled at VM runtime)
- Minimal, but expressive, instruction set

### Execution Model

- **Stack-based VM**: All operations take operands from the stack and push results back
- **Variables**: Mapped to stack slots or registers for fast access
- **Control flow**: `if-else` expressed via conditional jumps
- **Random number generation**: RNG is intrinsic (`RAND`), seeded per kernel invocation
- **No loops**: Iteration is handled externally by the worker
- **Type-checked**: Bytecode is typed; invalid operations are rejected at compile time

---

## 2. Registers / Stack

| Component | Description |
| --------- | ----------- |
| **Stack** | LIFO stack for intermediate values (typed) |
| **Local Slots** | Each variable has a dedicated slot |
| **Constants** | Stored in bytecode constant pool |

---

### 3. Types

| Type | Stack Representation |
| ---- | -------------------- |
| `i32` | 32-bit signed integer |
| `i64` | 64-bit signed integer |
| `f32` | 32-bit IEEE float |
| `f64` | 64-bit IEEE float |
| `bool` | 1 byte (0=false, 1=true) |

---

## 4. Instruction Categories

1. **Stack Operations**: push, pop, load, store
2. **Arithmetic Operations**: add, sub, mul, div, mod, pow
3. **Comparison Operations**: lt, le, eq, gt, ge, ne
4. **Logical Operations**: and, or, not
5. **Control Flow**: jump, jump_if_false
6. **RNG**: rand
7. **Return**: return

---

## 5. Instruction Set

### 5.1 Stack Operations

| Opcode | Operands | Description |
| ------ | -------- | ----------- |
| `PUSH_CONST <idx>` | Constant pool index | Pushes a constant onto the stack |
| `LOAD_VAR <slot>` | Slot index | Pushes variable value from local slot onto the stack |
| `STORE_VAR <slot>` | Slot index | Pops value from stack and stores into local slot |
| `POP` | - | Pops top value from stack |

### 5.2 Arithmetic Operations

| Opcode | Stack Behavior | Description |
| ------ | -------------- | ----------- |
| `ADD` | `a b -> a+b` | Pop two operands, push sum |
| `SUB` | `a b -> a-b` | Pop two operands, push difference |
| `MUL` | `a b -> a*b` | Pop two operands, push product |
| `DIV` | `a b -> a/b` | Pop two operands, push quotient |
| `MOD` | `a b -> a%b` | Pop two operands (integers), push remainder |
| `POW` | `a b -> a^b` | Pop two operands, push a raised to b |

### 5.3 Comparison Operations

| Opcode | Stack Behavior | Description |
| ------ | -------------- | ----------- |
| `CMP_LT` | `a b -> a<b` | Pop two operands, push `bool` |
| `CMP_LTE` | `a b -> a<=b` | Pop two operands, push `bool` |
| `CMP_GT` | `a b -> a>b` | Pop two operands, push `bool` |
| `CMP_GE` | `a b -> a>=b` | Pop two operands, push `bool` |
| `CMP_EQ` | `a b -> a==b` | Pop two operands, push `bool`|
| `CMP_NE` | `a b -> a!=b` | Pop two operands, push `bool` |

### 5.4 Logical Operations

| Opcode | Stack Behavior | Description |
| ------ | -------------- | ----------- |
| `AND` | `a b -> a&&b` | Pop two booleans, push AND |
| `OR` | `a b -> a\|\|b` | Pop two booleans, push OR |
| `NOT` | `a -> !a` | Pop boolean, push negation |

### 5.5 Control Flow

| Opcode | Operands | Description |
| ------ | -------- | ----------- |
| `JMP <offset>` | Bytecode offset | Unconditional jump to offset |
| `JMP_IF_FALSE <offset>` | Bytecode offset | Pop top value (bool). If false, jump to offset |

### 5.6 Random Number Generation

| Opcode | Stack Behavior | Description |
| ------ | -------------- | ----------- |
| `RAND` | `-> f32` | Pushes uniform random float `[0.0, 1.0)` onto stack |

- RNG is seeded per kernel invocation
- Deterministic across identical seeds

### 5.7 Return

| Opcode | Stack Behavior | Description |
| ------ | -------------- | ----------- |
| `RETURN` | `top -> output` | Pops top of stack and sets as kernel return value |

---

## 6. Example Bytecode

### DSL Kernel

```
kernel mc_pi() -> i32 {
    let x: f32 = rand();
    let y: f32 = rand();
    if (x*x + y*y <= 1.0) { 1 } else { 0 }
}
```

### Corresponding Bytecode

```
RAND
STORE_VAR {x_slot}
RAND
STORE_VAR {y_slot}
LOAD_VAR {x_slot}
LOAD_VAR {x_slot}
MUL
LOAD_VAR {y_slot}
LOAD_VAR {y_slot}
MUL
ADD
PUSH_CONST {const_1.0}
CMP_LE
JMP_IF_FALSE L1
PUSH_CONST {const_1}
JMP L2
L1:
PUSH_CONST {const_0}
L2:
RETURN
```

## 7. Notes on SIMD Execution

- Each instruction is executed **per-lane**, mapping to a single input instance
- Conditional jumps are masked for inactive lanes
- Random number generator produces **per-lane independent streams**

---

## 8. Metadata Requirements

Each compiled kernel bytecode includes:

- Kernel name
- Argument names and types
- Return type
- Constant pool
- Bytecode length

This metadata is used by:
- Scheduler (to validate tasks)
- Worker (to allocate slots)
- VM (for type checking and execution)

---