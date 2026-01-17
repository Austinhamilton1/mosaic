#include <iostream>
#include <vector>

#include "test.h"
#include "vm.h"

/* If the stack is empty, return should fail. */
bool invalid_return_test() {
    Instruction bytecode[] = {
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* Test PUSH_CONST. */
bool push_const_test() {
    /* Test integer push. */
    Instruction bytecode_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 15 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_int);
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_I32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_int[i] == 15)) return false;
    }

    /* Test float push. */
    Instruction bytecode_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 3.14f },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_float);
    vm.set_return_type(KERNEL_F32);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_F32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_float[i] - 3.14f <= 1e9)) return false;
    }

    /* Test bool push. */
    Instruction bytecode_bool[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_bool);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;
}

/* Program should fail when stack is too big. */
bool stack_size_test() {
    std::vector<Instruction> bytecode;
    for(int i = 0; i <= MAX_STACK; i++) {
        bytecode.push_back({ .opcode = PUSH_CONST, .type= I32, .const_int = 1 });
    }
    bytecode.push_back({ .opcode = RETURN });

    auto vm = VM(bytecode.data());
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* Program should fail when invalid variable slot is used. */
bool invalid_slot_test() {
    Instruction bytecode_too_small[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = STORE_VAR, .type = I32, .slot = -1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_too_small);
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    Instruction bytecode_too_big[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = STORE_VAR, .type = I32, .slot = MAX_SLOTS },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_too_big);
    vm.set_return_type(KERNEL_I32);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* Test STORE_VAR/LOAD_VAR. */
bool store_load_test() {
    /* Test integer store/load. */
    Instruction bytecode_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = STORE_VAR, .type = I32, .slot = 17 },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 10.45f },
        { .opcode = STORE_VAR, .type = F32, .slot = 1 },
        { .opcode = LOAD_VAR, .type = I32, .slot = 17 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_int);
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_I32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_int[i] == 5)) return false;
    }

    /* Test float store/load. */
    Instruction bytecode_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 5.45f },
        { .opcode = STORE_VAR, .type = I32, .slot = 3 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 10 },
        { .opcode = STORE_VAR, .type = I32, .slot = 12 },
        { .opcode = LOAD_VAR, .type = F32, .slot = 3 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_float);
    vm.set_return_type(KERNEL_F32);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_F32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_float[i] - 5.45 <= 1e9)) return false;
    }

    /* Test bool store/load. */
    Instruction bytecode_bool[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = STORE_VAR, .type = I32, .slot = 4 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 10 },
        { .opcode = STORE_VAR, .type = I32, .slot = 9 },
        { .opcode = LOAD_VAR, .type = BOOL, .slot = 4 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_bool);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    return true;
}

/* Test integer math operations */
bool int_math_ops_test() {
    Instruction bytecode[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = STORE_VAR, .type = I32, .slot = 0 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 2 },
        { .opcode = STORE_VAR, .type = I32, .slot = 1 },
        { .opcode = LOAD_VAR, .type = I32, .slot = 0 },
        { .opcode = LOAD_VAR, .type = I32, .slot = 0 },
        { .opcode = MUL, .type = I32 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 3 },
        { .opcode = ADD, .type = I32 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 10 },
        { .opcode = MOD, .type = I32 },
        { .opcode = LOAD_VAR, .type = I32, .slot = 1 },
        { .opcode = SUB, .type = I32 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 2 },
        { .opcode = DIV, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode);
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_I32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_int[i] == 3)) return false;
    }

    return true;
}

/* Test float math operations */
bool float_math_ops_test() {
    Instruction bytecode[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 3.14f },
        { .opcode = STORE_VAR, .type = F32, .slot = 0 },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 2.0f },
        { .opcode = STORE_VAR, .type = F32, .slot = 1 },
        { .opcode = LOAD_VAR, .type = F32, .slot = 1 },
        { .opcode = LOAD_VAR, .type = F32, .slot = 1 },
        { .opcode = MUL, .type = F32 },
        { .opcode = LOAD_VAR, .type = F32, .slot = 0 },
        { .opcode = MUL, .type = F32 },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 4.0f },
        { .opcode = DIV, .type = F32 },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = ADD, .type = F32 },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = SUB, .type = F32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode);
    vm.set_return_type(KERNEL_F32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_F32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_float[i] - 3.14 <= 1e9)) return false;
    }

    return true;
}

/* Ensure invalid integer operations fail. */
bool invalid_operations_int() {
    /* Divide by 0 error check */
    Instruction bytecode_div_0[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 0 },
        { .opcode = DIV, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_div_0);
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    /* Mod by 0 error check */
    Instruction bytecode_mod_0[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 0 },
        { .opcode = MOD, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_mod_0);
    vm.set_return_type(KERNEL_I32);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* Ensure invalid float operations fail. */
bool invalid_operations_float() {
    /* Float mod error check */
    Instruction bytecode_mod[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 9.0f },
        { .opcode = MOD, .type = F32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_mod);
    vm.set_return_type(KERNEL_F32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* Ensure invalid bool operations fail. */
bool invalid_operations_bool() {
    /* All mathematical operations should fail for booleans */
    Instruction bytecode_add[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = ADD, .type = BOOL },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_add);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type = KERNEL_ERROR)) return false;

    /* All mathematical operations should fail for booleans */
    Instruction bytecode_sub[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = SUB, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_sub);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type = KERNEL_ERROR)) return false;

    /* All mathematical operations should fail for booleans */
    Instruction bytecode_mul[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = MUL, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_mul);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type = KERNEL_ERROR)) return false;

    /* All mathematical operations should fail for booleans */
    Instruction bytecode_div[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = DIV, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_div);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type = KERNEL_ERROR)) return false;

    /* All mathematical operations should fail for booleans */
    Instruction bytecode_mod[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = MOD, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_mod);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type = KERNEL_ERROR)) return false;

    return true;
}

/* CMP_LT tests */
bool compare_lt_test() {
    /* Compare less than for two integers */
    Instruction bytecode_lt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_LT, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_lt_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_gt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = CMP_LT, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    /* Compare less than for two floats */
    Instruction bytecode_lt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_LT, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_lt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_gt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = CMP_LT, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    return true;
}

/* CMP_LTE tests */
bool compare_lte_test() {
    /* Compare less than or equal for two integers */
    Instruction bytecode_lt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_LTE, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_lt_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_eq_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_LTE, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_gt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = CMP_LTE, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    /* Compare less than or equal for two floats */
    Instruction bytecode_lt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_LTE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_lt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_eq_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_LTE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;

    Instruction bytecode_gt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = CMP_LTE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    return true;
}

/* CMP_GT tests */
bool compare_gt_test() {
    /* Compare greater than for two integers */
    Instruction bytecode_lt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_GT, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_lt_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    Instruction bytecode_gt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = CMP_GT, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    /* Compare greater than for two floats */
    Instruction bytecode_lt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_GT, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_lt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    Instruction bytecode_gt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = CMP_GT, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;
}

/* CMP_GTE tests */
bool compare_gte_test() {
    /* Compare greater than or equal for two integers */
    Instruction bytecode_lt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_GTE, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_lt_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    Instruction bytecode_eq_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_GTE, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_gt_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = CMP_GTE, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    /* Compare greater than or equal for two floats */
    Instruction bytecode_lt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_GTE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_lt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    Instruction bytecode_eq_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_GTE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;

    Instruction bytecode_gt_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = CMP_GTE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;
}

/* CMP_EQ tests */
bool compare_eq_test() {
    /* Compare equal to for two integers */
    Instruction bytecode_eq_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_EQ, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_eq_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_ne_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = CMP_EQ, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_ne_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    /* Compare equal to for two floats */
    Instruction bytecode_eq_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_EQ, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    Instruction bytecode_ne_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = CMP_EQ, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_ne_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    return true;
}

/* CMP_NE tests */
bool compare_ne_test() {
    /* Compare not equal to for two integers */
    Instruction bytecode_eq_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = CMP_NE, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_eq_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    Instruction bytecode_ne_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 1 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -1 },
        { .opcode = CMP_NE, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_ne_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    /* Compare equal to for two floats */
    Instruction bytecode_eq_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = CMP_NE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    Instruction bytecode_ne_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 1.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = -1.0f },
        { .opcode = CMP_NE, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_ne_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;
}

/* Ensure invalid compares return error. */
bool invalid_cmp_tests() {
    /* Booleans should fail for all CMP_* tests */
    Instruction bytecode_lt[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = CMP_LT, .type = BOOL },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_lt);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    /* Booleans should fail for all CMP_* tests */
    Instruction bytecode_lte[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = CMP_LTE, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_lte);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    /* Booleans should fail for all CMP_* tests */
    Instruction bytecode_gt[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = CMP_GT, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gt);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    /* Booleans should fail for all CMP_* tests */
    Instruction bytecode_gte[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = CMP_GTE, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_gte);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    /* Booleans should fail for all CMP_* tests */
    Instruction bytecode_eq[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = CMP_EQ, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_eq);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    /* Booleans should fail for all CMP_* tests */
    Instruction bytecode_ne[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = CMP_NE, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_ne);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* AND test. */
bool and_test() {
    /* true and true */
    Instruction bytecode_11[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = AND, .type = BOOL },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_11);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    /* true and false */
    Instruction bytecode_10[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = AND, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_10);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    /* false and true */
    Instruction bytecode_01[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = AND, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_01);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }


    /* false and false */
    Instruction bytecode_00[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = AND, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_00);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    return true;
}

/* OR test. */
bool or_test() {
    /* true and true */
    Instruction bytecode_11[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = OR, .type = BOOL },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_11);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    /* true and false */
    Instruction bytecode_10[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = OR, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_10);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    /* false and true */
    Instruction bytecode_01[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = OR, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_01);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }


    /* false and false */
    Instruction bytecode_00[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = OR, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_00);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL))  return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    return true;
}

/* NOT test. */
bool not_test() {
    /* Not true */
    Instruction bytecode_1[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = NOT, .type = BOOL },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_1);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == 0)) return false;
    }

    /* Not false */
    Instruction bytecode_0[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = NOT, .type = BOOL },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_0);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_BOOL)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_bool[i] == -1)) return false;
    }

    return true;
}

/* Ensure invalid logic operations return an error. */
bool invalid_logic() {
    /* Logic with anything besides bools should error. */
    Instruction bytecode_invalid_and_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = AND, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_invalid_and_int);
    vm.set_return_type(KERNEL_BOOL);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    Instruction bytecode_invalid_and_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 5.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 5.0f },
        { .opcode = AND, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_invalid_and_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    Instruction bytecode_invalid_or_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = OR, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_invalid_or_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    Instruction bytecode_invalid_or_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 5.0f },
        { .opcode = PUSH_CONST, .type = F32, .const_float = 5.0f },
        { .opcode = OR, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_invalid_or_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    Instruction bytecode_invalid_not_int[] = {
        { .opcode = PUSH_CONST, .type = I32, .const_int = 5 },
        { .opcode = NOT, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_invalid_not_int);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    Instruction bytecode_invalid_not_float[] = {
        { .opcode = PUSH_CONST, .type = F32, .const_float = 5.0f },
        { .opcode = NOT, .type = F32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_invalid_not_float);
    vm.set_return_type(KERNEL_BOOL);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_ERROR)) return false;

    return true;
}

/* SELECT test. */
bool select_test() {
    Instruction bytecode_true[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = true },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 6 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -6 },
        { .opcode = SELECT, .type = I32 },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode_true);
    vm.set_return_type(KERNEL_I32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_I32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_int[i] == 6)) return false;
    }

    Instruction bytecode_false[] = {
        { .opcode = PUSH_CONST, .type = BOOL, .const_bool = false },
        { .opcode = PUSH_CONST, .type = I32, .const_int = 6 },
        { .opcode = PUSH_CONST, .type = I32, .const_int = -6 },
        { .opcode = SELECT, .type = I32 },
        { .opcode = RETURN },
    };

    vm = VM(bytecode_false);
    vm.set_return_type(KERNEL_I32);
    result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_I32)) return false;

    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_int[i] == -6)) return false;
    }

    return true;
}

/* Random test */
bool random_test() {
    Instruction bytecode[] = {
        { .opcode = RAND },
        { .opcode = RETURN },
    };

    auto vm = VM(bytecode);
    vm.set_return_type(KERNEL_F32);
    auto result = vm.run();

    if(Tester::assert_fail(result.type == KERNEL_F32)) return false;

    /* RAND should return a number in [0.0, 1.0) */
    for(int i = 0; i < LANES; i++) {
        if(Tester::assert_fail(result.result_float[i] < 1.0f && result.result_float[i] >= 0.0f)) return false;
    }

    return true;
}

/*
 * Run several tests on the VM.
 */
int main(int argc, char **argv) {
    Tester test_suite;

    // Stack tests
    test_suite.add_test("Invalid return test", invalid_return_test);
    test_suite.add_test("PUSH_CONST test", push_const_test);
    test_suite.add_test("Invalid stack size test", stack_size_test);
    test_suite.add_test("STORE/LOAD test", store_load_test);
    test_suite.add_test("Invalid slot test", invalid_slot_test);

    // Mathematical operations tests
    test_suite.add_test("Int math operations test", int_math_ops_test);
    test_suite.add_test("Float math operations test", float_math_ops_test);
    test_suite.add_test("Invalid int math operations test", invalid_operations_int);
    test_suite.add_test("Invalid float math operations test", invalid_operations_float);
    test_suite.add_test("Invalid bool math operations test", invalid_operations_bool);

    // Compare operations tests
    test_suite.add_test("CMP_LT test", compare_lt_test);
    test_suite.add_test("CMP_LTE test", compare_lte_test);
    test_suite.add_test("CMP_GT test", compare_gt_test);
    test_suite.add_test("CMP_GTE test", compare_gte_test);
    test_suite.add_test("CMP_EQ test", compare_eq_test);
    test_suite.add_test("CMP_NE test", compare_ne_test);
    test_suite.add_test("Invalid CMP_* test", invalid_cmp_tests);

    // Logical operations tests
    test_suite.add_test("AND test", and_test);
    test_suite.add_test("OR test", or_test);
    test_suite.add_test("NOT test", not_test);
    test_suite.add_test("Invalid logic test", invalid_logic);

    // Select operation test
    test_suite.add_test("SELECT test", select_test);

    // RAND operation test
    test_suite.add_test("RAND test", random_test);

    bool passed = test_suite.run_tests(true);

    if(passed) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << "Tests failed!" << std::endl;
    }

    return 0;
}