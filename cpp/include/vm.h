#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <smmintrin.h>
#include <string.h>

constexpr int LANES = 4;
constexpr int MAX_STACK = 64;
constexpr int MAX_SLOTS = 32;

/* Type of an expression. */
enum TypeTag {
    I32,
    F32,
    BOOL,
};

/* Stack can have multiple data types. */
union StackSlot {
    uint32_t i32[LANES];
    float f32[LANES];
    int32_t b[LANES];
};

/* Stack for execution. */
struct Stack {
    StackSlot data[MAX_STACK];
    int sp;
};

/* Variable slots for variable lookup. */
struct Slots {
    int32_t i32_slot[MAX_SLOTS][LANES];
    float f32_slot[MAX_SLOTS][LANES];
    uint32_t bool_slot[MAX_SLOTS][LANES];
};

enum OpCode {
    /* Stack operations. */
    PUSH_CONST,
    LOAD_VAR,
    STORE_VAR,

    /* Mathematical operations. */
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    /* Comparison operations. */
    CMP_LT,
    CMP_LTE,
    CMP_GT,
    CMP_GTE,
    CMP_EQ,
    CMP_NE,

    /* Logical operations. */
    AND,
    OR,
    NOT,

    /* Branching operation. */
    SELECT,

    /* Random number generation operations. */
    RAND,

    /* Return operations. */
    RETURN,
};

/* Instruction with (optional) arguments. */
struct Instruction {
    OpCode opcode;
    TypeTag type;
    union {
        int32_t const_int;
        float const_float;
        bool const_bool;
        int slot;
    };
};

enum VMReturnType {
    KERNEL_I32,
    KERNEL_F32,
    KERNEL_BOOL,
    KERNEL_ERROR,
};

struct VMReturnValue {
    VMReturnType type;
    union {
        int32_t result_int[LANES];
        float result_float[LANES];
        uint32_t result_bool[LANES];
    };
};

class VM {
private:
    const Instruction *bytecode;
    int pc;

    Stack stack;
    Slots slots;

    __m128i rng_state;

    VMReturnValue retval;

    using OpHandler = int (VM::*)(const Instruction&);

    static const OpHandler dispatch[];

    /* Stack operations. */
    int simd_push_const(const Instruction& instruction);
    int simd_load_var(const Instruction& instruction);
    int simd_store_var(const Instruction& instruction);

    /* Mathematical operations. */
    int simd_add(const Instruction& instruction);
    int simd_sub(const Instruction& instruction);
    int simd_mul(const Instruction& instruction);
    int simd_div(const Instruction& instruction);
    int simd_mod(const Instruction& instruction);

    /* Comparison operations. */
    int simd_cmp_lt(const Instruction& instruction);
    int simd_cmp_lte(const Instruction& instruction);
    int simd_cmp_gt(const Instruction& instruction);
    int simd_cmp_gte(const Instruction& instruction);
    int simd_cmp_eq(const Instruction& instruction);
    int simd_cmp_ne(const Instruction& instruction);

    /* Logical operations. */
    int simd_and(const Instruction& instruction);
    int simd_or(const Instruction& instruction);
    int simd_not(const Instruction& instruction);

    /* Branching operations. */
    int simd_select(const Instruction& instruction);

    /* Random number generator operations. */
    int simd_rand(const Instruction& instruction);

    /* Return from the VM. */
    int simd_return(const Instruction& instruction);

public:
    VM(const Instruction *bytecode) 
        : bytecode(bytecode), pc(0) {
            stack.sp = -1;
            memset(&slots, 0, sizeof(slots));
            memset(&retval, 0, sizeof(retval));
            rng_state = _mm_set_epi32(0x12345678, 0x87654321, 0xCAFEBABE, 0xDEADBEEF);
        }
    ~VM() = default; 

    VMReturnValue& run();
    void reset();
    void set_return_type(VMReturnType type);
};

#endif