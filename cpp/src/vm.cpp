#include "simd.h"

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
        __veci const_vec = _vec_bcsti(instruction.const_int);
        _vec_storei(stack.data[sp].i32, const_vec);
    } else if(instruction.type == F32) {
        __vecf const_vec = _vec_bcstf(instruction.const_float);
        _vec_storef(stack.data[stack.sp].f32, const_vec);
    } else if(instruction.type == BOOL) {
        __veci const_vec = _vec_bcsti(instruction.const_bool ? -1 : 0);
        _vec_storei(stack.data[sp].b, const_vec);
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
        _vec_storei(
            stack.data[sp].i32, 
            _vec_loadi(slots.i32_slot[instruction.slot])
        );
    } else if(instruction.type == F32) {
        _vec_storef(
            stack.data[sp].f32, 
            _vec_loadf(slots.f32_slot[instruction.slot])
        );
    } else if(instruction.type == BOOL) {
        _vec_storei(
            stack.data[sp].b, 
            _vec_loadi(slots.bool_slot[instruction.slot])
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
        _vec_storei(
            slots.i32_slot[instruction.slot],
            _vec_loadi(stack.data[sp].i32)
        );
    } else if(instruction.type == F32) {
        _vec_storef(
            slots.f32_slot[instruction.slot],
            _vec_loadf(stack.data[sp].f32)
        );
    } else if(instruction.type == BOOL) {
        _vec_storei(
            slots.bool_slot[instruction.slot],
            _vec_loadi(stack.data[sp].b)
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);

        __veci result = _vec_addi(a, b);

        _vec_storei(stack.data[sp-1].i32, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_addf(a, b);

        _vec_storef(stack.data[sp-1].f32, result);
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);

        __veci result = _vec_subi(a, b);

        _vec_storei(stack.data[sp-1].i32, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_subf(a, b);

        _vec_storef(stack.data[sp-1].f32, result);
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);

        __veci result = _vec_muli(a, b);

        _vec_storei(stack.data[sp-1].i32, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_mulf(a, b);

        _vec_storef(stack.data[sp-1].f32, result);
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
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_divf(a, b);

        _vec_storef(stack.data[sp-1].f32, result);
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);
        __veci result = _vec_cmplti(a, b);

        _vec_storei(stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_cmpltf(a, b);

        _vec_storei(
            stack.data[sp-1].b, 
            _vec_castfi(result) 
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);

        __veci ones = _vec_bcsti(-1);
        __veci result = _vec_xori(
            _vec_cmplti(b, a),
            ones
        );

        _vec_storei(stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_cmplef(a, b);
        _vec_storei(
            stack.data[sp-1].b,
            _vec_castfi(result)
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);
        __veci result = _vec_cmplti(b, a);

        _vec_storei(stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_cmpgtf(a, b);
        _vec_storei(
            stack.data[sp-1].b,
            _vec_castfi(result)
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);
       
        __veci ones = _vec_bcsti(-1);
        __veci result = _vec_xori(
            _vec_cmplti(a, b),
            ones
        );

        _vec_storei(stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_cmpgef(a, b);
        _vec_storei(
            stack.data[sp-1].b,
            _vec_castfi(result)
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);
        __veci result = _vec_cmpeqi(a, b);

        _vec_storei(stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_cmpeqf(a, b);
        _vec_storei(
            stack.data[sp-1].b,
            _vec_castfi(result)
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
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);
        
        __veci ones = _vec_bcsti(-1);
        __veci result = _vec_xori(
            _vec_cmpeqi(b, a),
            ones
        );

        _vec_storei(stack.data[sp-1].b, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);

        __vecf result = _vec_cmpnef(a, b);
        _vec_storei(
            stack.data[sp-1].b,
            _vec_castfi(result)
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
        __veci a = _vec_loadi(stack.data[sp-1].b);
        __veci b = _vec_loadi(stack.data[sp].b);

        __veci result = _vec_andi(a, b);

        _vec_storei(stack.data[sp-1].b, result);
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
        __veci a = _vec_loadi(stack.data[sp-1].b);
        __veci b = _vec_loadi(stack.data[sp].b);

        __veci result = _vec_ori(a, b);

        _vec_storei(stack.data[sp-1].b, result);
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
        __veci a = _vec_loadi(stack.data[sp].b);
        __veci ones = _vec_bcsti(-1);

        __veci result = _vec_xori(a, ones);

        _vec_storei(stack.data[sp].b, result);
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

    __veci cond = _vec_loadi(stack.data[sp-2].b);

    if(instruction.type == I32) {
        __veci a = _vec_loadi(stack.data[sp-1].i32);
        __veci b = _vec_loadi(stack.data[sp].i32);

        // Select statement is (cond) ? a : b
        __veci result = _vec_ori(
            _vec_andi(cond, a),
            _vec_andnoti(cond, b)
        );

        _vec_storei(stack.data[sp-2].i32, result);
    } else if(instruction.type == F32) {
        __vecf a = _vec_loadf(stack.data[sp-1].f32);
        __vecf b = _vec_loadf(stack.data[sp].f32);
        __vecf cond_ps = _vec_castif(cond);

        // Select statement is (cond) ? a : b
        __vecf result = _vec_orf(
            _vec_andf(cond_ps, a),
            _vec_andnotf(cond_ps, b)
        );

        _vec_storef(stack.data[sp-2].f32, result);
    } else if(instruction.type == BOOL) {
        __veci a = _vec_loadi(stack.data[sp-1].b);
        __veci b = _vec_loadi(stack.data[sp].b);

        // Select statement is (cond) ? a : b
        __veci result = _vec_ori(
            _vec_andi(cond, a),
            _vec_andnoti(cond, b)
        );

        _vec_storei(stack.data[sp-2].b, result);
    } else {
        return -1;
    }

    stack.sp -= 2;
    return 0;
}

/*
 * Random number generator that implements xorshift32.
 * Arguments:
 *     __veci x - Seed.
 * Returns:
 *     __veci - Random number.
 */
static inline __veci xorshift32(__veci x) {
    x = _vec_xori(x, _vec_sli(x, 13));
    x = _vec_xori(x, _vec_sri(x, 17));
    x = _vec_xori(x, _vec_sli(x, 5));
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

    __veci mantissa = _vec_sri(rng_state, 9);    // Keep 23 bits
    __veci one = _vec_bcsti(0x3F800000);           // 1.0f

    __vecf f = _vec_castif(_vec_ori(mantissa, one));
    f = _vec_subf(f, _vec_bcstf(1.0f));

    _vec_storef(stack.data[sp].f32, f);

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
        __veci results = _vec_loadi(stack.data[stack.sp].i32);
        _vec_storei(retval.result_int, results);
    } else if(retval.type == KERNEL_F32) {
        __vecf results = _vec_loadf(stack.data[stack.sp].f32);
        _vec_storef(retval.result_float, results);
    } else if(retval.type == KERNEL_BOOL) {
        __veci results = _vec_loadi(stack.data[stack.sp].b);
        _vec_storei(retval.result_bool, results);
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
    int fd = open("/dev/random", O_RDONLY);
    read(fd, &rng_seed, sizeof(rng_seed));
    rng_state = _vec_loadi(rng_seed);
}

/*
 * Set the return type of the kernel being run on the VM.
 * Arguments:
 *     VMReturnType type - Return type of the kernel.
 */
void VM::set_return_type(VMReturnType type) {
    this->retval.type = type;
}