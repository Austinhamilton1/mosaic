#include <smmintrin.h>

#ifndef __SSE4_1__
#error "This VM requires SSE4.1 support. Compile with -msse4.1"
#endif

#include "vm.h"

const VM::OpHandler VM::dispatch[] = {
    &VM::simd_push_const,
    &VM::simd_load_var,
    &VM::simd_store_var,
    &VM::simd_add,
    &VM::simd_sub,
    &VM::simd_mul,
    &VM::simd_div,
    &VM::simd_mod,
    &VM::simd_cmp_lt,
    &VM::simd_cmp_lte,
    &VM::simd_cmp_gt,
    &VM::simd_cmp_gte,
    &VM::simd_cmp_eq,
    &VM::simd_cmp_ne,
    &VM::simd_and,
    &VM::simd_or,
    &VM::simd_not,
    &VM::simd_select,
    &VM::simd_rand,
    &VM::simd_return,
};

/*
 * Pushes a constant to the top of the stack.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_push_const(const Instruction& instruction) {
    stack.sp++;
    int sp = stack.sp;
    if(sp >= MAX_STACK) return -1;

    if(instruction.type == I32) {
        __m128i const_vec = _mm_set1_epi32(instruction.const_int);
        _mm_storeu_si128((__m128i *)stack.data[sp].i32, const_vec);
    } else if(instruction.type == F32) {
        __m128 const_vec = _mm_set1_ps(instruction.const_float);
        _mm_storeu_ps(stack.data[stack.sp].f32, const_vec);
    } else if(instruction.type == BOOL) {
        __m128i const_vec = _mm_set1_epi32(instruction.const_bool ? -1 : 0);
        _mm_storeu_si128((__m128i *)stack.data[sp].b, const_vec);
    }
    
    return 0;
}

/*
 * Push a local variable onto the stack.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_load_var(const Instruction& instruction) {
    stack.sp++;
    int sp = stack.sp;
    if(sp >= MAX_STACK) return -1;
    if(instruction.slot >= MAX_SLOTS || instruction.slot < 0) return -1;
    
    if(instruction.type == I32) {
        _mm_storeu_si128(
            (__m128i *)stack.data[sp].i32, 
            _mm_loadu_si128((__m128i *)slots.i32_slot[instruction.slot])
        );
    } else if(instruction.type == F32) {
        _mm_storeu_ps(
            stack.data[sp].f32, 
            _mm_loadu_ps(slots.f32_slot[instruction.slot])
        );
    } else if(instruction.type == BOOL) {
        _mm_storeu_si128(
            (__m128i *)stack.data[sp].b, 
            _mm_loadu_si128((__m128i *)slots.bool_slot[instruction.slot])
        );
    } else {
        return -1;
    }

    return 0;
}

/*
 * Pop the top value from the stack and set a variable to this value.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_store_var(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 0) return -1;
    if(instruction.slot >= MAX_SLOTS || instruction.slot < 0) return -1;

    if(instruction.type == I32) {
        _mm_storeu_si128(
            (__m128i *)slots.i32_slot[instruction.slot],
            _mm_loadu_si128((__m128i *)stack.data[sp].i32)
        );
    } else if(instruction.type == F32) {
        _mm_storeu_ps(
            slots.f32_slot[instruction.slot],
            _mm_loadu_ps(stack.data[sp].f32)
        );
    } else if(instruction.type == BOOL) {
        _mm_storeu_si128(
            (__m128i *)slots.bool_slot[instruction.slot],
            _mm_loadu_si128((__m128i *)stack.data[sp].b)
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute an ADD instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_add(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);

        __m128i result = _mm_add_epi32(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp-1].i32, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_add_ps(a, b);

        _mm_storeu_ps(stack.data[sp-1].f32, result);
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute a SUB instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_sub(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);

        __m128i result = _mm_sub_epi32(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp-1].i32, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_sub_ps(a, b);

        _mm_storeu_ps(stack.data[sp-1].f32, result);
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute a MUL instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_mul(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);

        __m128i result = _mm_mullo_epi32(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp-1].i32, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_mul_ps(a, b);

        _mm_storeu_ps(stack.data[sp-1].f32, result);
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute a DIV instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_div(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == I32) {
        // Intel does not support vector division of integers, do it manually
        for(int i = 0; i < LANES; i++) {
            int32_t a = stack.data[sp-1].i32[i];
            int32_t b = stack.data[sp].i32[i];

            // Divide by zero error
            if(b == 0) return -1;

            stack.data[sp-1].i32[i] = a / b;
        }
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_div_ps(a, b);

        _mm_storeu_ps(stack.data[sp-1].f32, result);
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute a MOD instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_mod(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == I32) {
        for(int i = 0; i < LANES; i++) {
            // Intel does not support vector modulo of integers, do it manually
            int32_t a = stack.data[sp-1].i32[i];
            int32_t b = stack.data[sp].i32[i];
            
            // Divide by zero error
            if(b == 0) return -1;

            stack.data[sp-1].i32[i] = a % b;
        }
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Compare a < b.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_cmp_lt(const Instruction& instruction) {
    int sp = stack.sp;
    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);
        __m128i result = _mm_cmplt_epi32(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_cmplt_ps(a, b);

        _mm_storeu_si128(
            (__m128i *)stack.data[sp-1].b, 
            _mm_castps_si128(result) 
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Compare a <= b.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_cmp_lte(const Instruction& instruction) {
    int sp = stack.sp;
    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);

        __m128i ones = _mm_set1_epi32(-1);
        __m128i result = _mm_xor_si128(
            _mm_cmplt_epi32(b, a),
            ones
        );

        _mm_storeu_si128((__m128i *)stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_cmple_ps(a, b);
        _mm_storeu_si128(
            (__m128i *)stack.data[sp-1].b,
            _mm_castps_si128(result)
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Compare a > b.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_cmp_gt(const Instruction& instruction) {
    int sp = stack.sp;
    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);
        __m128i result = _mm_cmplt_epi32(b, a);

        _mm_storeu_si128((__m128i *)stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_cmpgt_ps(a, b);
        _mm_storeu_si128(
            (__m128i *)stack.data[sp-1].b,
            _mm_castps_si128(result)
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Compare a >= b.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_cmp_gte(const Instruction& instruction) {
    int sp = stack.sp;
    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);
       
        __m128i ones = _mm_set1_epi32(-1);
        __m128i result = _mm_xor_si128(
            _mm_cmplt_epi32(a, b),
            ones
        );

        _mm_storeu_si128((__m128i *)stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_cmpge_ps(a, b);
        _mm_storeu_si128(
            (__m128i *)stack.data[sp-1].b,
            _mm_castps_si128(result)
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Compare a == b.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_cmp_eq(const Instruction& instruction) {
    int sp = stack.sp;
    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);
        __m128i result = _mm_cmpeq_epi32(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_cmpeq_ps(a, b);
        _mm_storeu_si128(
            (__m128i *)stack.data[sp-1].b,
            _mm_castps_si128(result)
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Compare a != b.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_cmp_ne(const Instruction& instruction) {
    int sp = stack.sp;
    if(sp < 1) return -1;

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);
        
        __m128i ones = _mm_set1_epi32(-1);
        __m128i result = _mm_xor_si128(
            _mm_cmpeq_epi32(b, a),
            ones
        );

        _mm_storeu_si128((__m128i *)stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);

        __m128 result = _mm_cmpneq_ps(a, b);
        _mm_storeu_si128(
            (__m128i *)stack.data[sp-1].b,
            _mm_castps_si128(result)
        );
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute an AND instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_and(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == BOOL) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].b);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].b);

        __m128i result = _mm_and_si128(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp].b, result);
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute an OR instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_or(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 1) return -1;

    if(instruction.type == BOOL) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].b);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].b);

        __m128i result = _mm_or_si128(a, b);

        _mm_storeu_si128((__m128i *)stack.data[sp].b, result);
    } else {
        return -1;
    }

    stack.sp--;
    return 0;
}

/*
 * Execute a NOT instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_not(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 0) return -1;

    if(instruction.type == BOOL) {
        // Intel doesn't support direct NOT but XOR 1 is the same operation
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp].b);
        __m128i ones = _mm_set1_epi32(-1);

        __m128i result = _mm_xor_si128(a, ones);

        _mm_storeu_si128((__m128i *)stack.data[sp].b, result);
    } else {
        return -1;
    }

    return 0;
}

/*
 * Executes a SELECT instruction.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on succes, -1 on failure.
 */
int VM::simd_select(const Instruction& instruction) {
    int sp = stack.sp;

    if(sp < 2) return -1;

    __m128i cond = _mm_loadu_si128((__m128i *)stack.data[sp-2].b);

    if(instruction.type == I32) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].i32);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].i32);

        // Select statement is (cond) ? a : b
        __m128i result = _mm_or_si128(
            _mm_and_si128(cond, a),
            _mm_andnot_si128(cond, b)
        );

        _mm_storeu_si128((__m128i *)stack.data[sp-2].i32, result);
    } else if(instruction.type == F32) {
        __m128 a = _mm_loadu_ps(stack.data[sp-1].f32);
        __m128 b = _mm_loadu_ps(stack.data[sp].f32);
        __m128 cond_ps = _mm_castsi128_ps(cond);

        // Select statement is (cond) ? a : b
        __m128 result = _mm_or_ps(
            _mm_and_ps(cond_ps, a),
            _mm_andnot_ps(cond_ps, b)
        );

        _mm_storeu_ps(stack.data[sp-2].f32, result);
    } else if(instruction.type == BOOL) {
        __m128i a = _mm_loadu_si128((__m128i *)stack.data[sp-1].b);
        __m128i b = _mm_loadu_si128((__m128i *)stack.data[sp].b);

        // Select statement is (cond) ? a : b
        __m128i result = _mm_or_si128(
            _mm_and_si128(cond, a),
            _mm_andnot_si128(cond, b)
        );

        _mm_storeu_si128((__m128i *)stack.data[sp-2].b, result);
    } else {
        return -1;
    }

    stack.sp -= 2;
    return 0;
}

/*
 * Random number generator that implements xorshift32.
 * Arguments:
 *     __m128i x - Seed.
 * Returns:
 *     __m128i - Random number.
 */
static inline __m128i xorshift32(__m128i x) {
    x = _mm_xor_si128(x, _mm_slli_epi32(x, 13));
    x = _mm_xor_si128(x, _mm_srli_epi32(x, 17));
    x = _mm_xor_si128(x, _mm_slli_epi32(x, 5));
    return x;
}

/*
 * Generate a random float in the range [0.0, 1.0).
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 0 on success, -1 on failure.
 */
int VM::simd_rand(const Instruction& instruction) {
    stack.sp++;
    int sp = stack.sp;

    if(sp >= MAX_STACK) return -1;

    // Advance RNG
    rng_state = xorshift32(rng_state);

    __m128i mantissa = _mm_srli_epi32(rng_state, 9);    // Keep 23 bits
    __m128i one = _mm_set1_epi32(0x3F800000);           // 1.0f

    __m128 f = _mm_castsi128_ps(_mm_or_si128(mantissa, one));
    f = _mm_sub_ps(f, _mm_set1_ps(1.0f));

    _mm_storeu_ps(stack.data[sp].f32, f);

    return 0;
}

/* 
 * Return from the VM execution.
 * Arguments:
 *     const Instruction& instruction - Current context.
 * Returns:
 *     int - 1 on success, -1 on failure.
 */
int VM::simd_return(const Instruction& instruction) {
    // Bounds check
    int sp = stack.sp;
    if(sp < 0 || sp >= MAX_STACK) { 
        retval.type = KERNEL_ERROR; 
        return -1; 
    }

    // Aggregate the final return values
    if(retval.type == KERNEL_I32) {
        __m128i results = _mm_loadu_si128((__m128i *)stack.data[stack.sp].i32);
        _mm_storeu_si128((__m128i *)retval.result_int, results);
    } else if(retval.type == KERNEL_F32) {
        __m128 results = _mm_loadu_ps(stack.data[stack.sp].f32);
        _mm_storeu_ps(retval.result_float, results);
    } else if(retval.type == KERNEL_BOOL) {
        __m128i results = _mm_loadu_si128((__m128i *)stack.data[stack.sp].b);
        _mm_storeu_si128((__m128i *)retval.result_bool, results);
    } else {
        return -1;
    }

    return 1;
}

/*
 * Instruction execution dispatcher.
 */
VMReturnValue& VM::run() {
    while(true) {
        const Instruction& instr = bytecode[pc];
        int result = (this->*dispatch[instr.opcode])(instr);
        if(result < 0) {
            retval.type = KERNEL_ERROR;
            return retval;
        } else if(result > 0) {
            return retval;
        }
        pc++;
    }
}

/*
 * Reset the VM to run again.
 */
void VM::reset() {
    pc = 0;
    memset(&stack, 0, sizeof(stack));
    stack.sp = -1;
    memset(&slots, 0, sizeof(slots));
    memset(&retval, 0, sizeof(retval));
    rng_state = _mm_set_epi32(0x12345678, 0x87654321, 0xCAFEBABE, 0xDEADBEEF);
}

/*
 * Set the return type of the kernel being run on the VM.
 * Arguments:
 *     VMReturnType type - Return type of the kernel.
 */
void VM::set_return_type(VMReturnType type) {
    this->retval.type = type;
}