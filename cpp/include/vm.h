#ifndef VM_H
#define VM_H

#include <stdint.h>
#include <vector>
#include <smmintrin.h>

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

    /* Stack operations. */
    int simd_push_const(float constant) {
        __m128 const_vec = _mm_set1_ps(constant);
        _mm_storeu_ps(stack.data[stack.sp+1].f32, const_vec);
        return 0;
    }
    int simd_push_const(int32_t constant) {
        __m128i const_vec = _mm_set1_epi32(constant);
        _mm_storeu_si128((__m128i *)stack.data[stack.sp].i32, const_vec);
        return 0;
    }
    int simd_push_const(bool constant) {
        __m128i const_vec = _mm_set1_epi32(constant ? -1 : 0);
        _mm_storeu_si128((__m128i *)stack.data[stack.sp].b, const_vec);
        return 0;
    }

    int simd_load_var(TypeTag type, int slot);
    int simd_store_var(TypeTag type, int slot);

    /* Mathematical operations. */
    int simd_add(TypeTag type);
    int simd_sub(TypeTag type);
    int simd_mul(TypeTag type);
    int simd_div(TypeTag type);
    int simd_mod(TypeTag type);

    /* Comparison operations. */
    int simd_cmp_lt(TypeTag type);
    int simd_cmp_lte(TypeTag type);
    int simd_cmp_gt(TypeTag type);
    int simd_cmp_gte(TypeTag type);
    int simd_cmp_eq(TypeTag type);
    int simd_cmp_ne(TypeTag type);

    /* Logical operations. */
    int simd_and(TypeTag type);
    int simd_or(TypeTag type);
    int simd_not(TypeTag type);

    /* Branching operations. */
    int simd_select(TypeTag type);

    /* Random number generator operations. */
    int simd_rand();

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
};

#endif