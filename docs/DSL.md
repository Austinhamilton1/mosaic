# Mosaic Kernel DSL Specification (v1)

## 1. Overview

The Mosaic Kernel DSL is a **small, statically-typed expression language** designed for distributed data-parallel execution. Each program defines a **single kernel function** that maps **N scalar inputs to one scalar output.**

The DSL is intentionally minimal to allow:
- Easy compilation to bytecode
- Efficient SIMD vectorization at runtime
- Deterministic execution across heterogeneous worker nodes

The language contains **no loops, no function calls, and no heap allocation.** All iteration and vectorization are handled by the Mosaic worker runtime.

---

## 2. Execution Model

- Each kernel is executed independently for each input instance
- A kernel operates only on its input arguments and local variables
- The final expression evaluated is the **implicit return value**
- Workers may evaluate multiple kernel instances simultaneously using SIMD

`Kernel: (x~1, x~2, ..., x~n) -> y`

---

## 3. Types

The DSL provides a small set of built-in primitive types.

| Type | Description |
| ---- | ----------- |
| `i32` | 32-bit signed integer (two's complement) |
| `i64` | 64-bit signed integer (two's complement) |
| `f32` | 32-bit IEEE 754 floating point |
| `f64` | 64-bit IEEE 754 floating point |
| `bool` | Boolean valud (`true` or `false`), stored as 1 byte |

### Type Rules

- All variables are **statically typed**
- Types must be explicitly declared
- No implicit type conversions
- Mixed-type arithmetic is not allowed (must match exactly)

---

## 4. Kernel Structure

### Kernel Signature

Every program begins with a kernel signature.

#### Syntax

```
kernel <kernel_name>(<arg_list>) -> <return_type> {
    <statements>
    <final_expression>
}
```

#### Argument List

`<arg_name>: <type>, <arg_name>: <type>, ...`

- Zero or more arguments
- Arguments are immutable
- Arguments are treated as local variables

#### Return Type

- Exactly one return value
- Determined by the final expression
- Must match the declared return type

---

## 5. Variables

Variables are immutable once defined.

### Syntax 

`let <identifier>: <type> = <expression>;`

### Example

```
let x: f32 = 3.14;
let y: i32 = 42;
```

---

## 6. Expressions

Expressions are pure and side-effect free.

### Arithmetic Operators

| Operator | Description |
| -------- | ----------- |
| `+` | Addition |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Modulo (integers only) |
| `^` | Exponentiation |

### Comparison Operators

| Operator | Result |
| -------- | ----------- |
| `<` | `bool` |
| `<=` | `bool` |
| `>` | `bool` |
| `>=` | `bool` |
| `==` | `bool` |
| `!=` | `bool` |

---

## 7. Conditional Expressions

The DSL supports `if-else` as an **expression**, not a statement.

### Syntax

`if (<condition>) { <expression> } else { <expression> }`

- Condition must evaluate to `bool`
- Both branches must return the **same type**

### Example

`if (x > 0.0) { x } else { -x }`

---

## 8. Random Number Generation

The DSL provides a built-in random number generator.

### `rand()`

`rand() -> f32`

- Returns a uniformly distributed floating-point value in `[0.0, 1.0)`
- Each kernel invocation receives an independent random stream
- The runtime defines determinism guarantees (e.g., seed per task)

### Example

`let r: f32 = rand();`

--- 

## 9. Implicit Return

The **final expression** in the program is the return value.

### Example

```
let x: i64 = 500;

if (x > 250) { 1 } else { 0 }
```

---

## 10. Kernel Constraints

- Exactly one kernel per program
- No nested kernels
- No loops
- No recursion
- No arrays or structs
- No function definitions or calls
- No mutable state
- No heap allocation
- No I/O

These constraints guarantee:

- Safe parallel execution
- Easy vectorization
- Predictable performance

---

## 11. Vectorization Model

The DSL operates on **scalar values only**.

Vectorization is handled entirely by the Mosaic runtime:

- Multiple kernel instances may be executed in parallel
- SIMD lanes correspond to independent inputs
- Control-flow divergence is resolved via masking

The DSL does **not** expose SIMD constructs.

---

## 12. Example Programs

### Monte Carlo $ \pi $ Kernel

```
kernel mc_pi() -> i32 {
    let x: f32 = rand();
    let y: f32 = rand();

    if (x*x + y*y <= 1.0) { 1 } else { 0 }
}
```

### Weighted Sum Kernel

```
kernel weighted_sum(x: f32, w: f32) -> f32 {
    x * w
}
```

---

## 13. Future Extensions (Non-Goals for v1)

- Loops
- Arrays
- Function calls
- User-defined types
- GPU-specific constructs

---