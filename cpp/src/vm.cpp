#include <stdint.h>
#include <vector>
#include <x86intrin.h>

constexpr int LANES = 4;
constexpr int MAX_STACK = 64;
constexpr int MAX_SLOTS = 32;

using Mask = uint8_t;   // Lower LANES bits used
constexpr Mask ALL_LANES = (1 << LANES) - 1;

/* Stack data structure for the stack-based SIMD VM. */
struct Stack {
    float data[MAX_STACK][LANES];
    int sp;                         // Single shared stack pointer
};

/* Variable slots for variable lookup. */
struct Slots {
    int32_t i32_slot[MAX_SLOTS][LANES];
    float f32_slot[MAX_SLOTS][LANES];
    bool bool_slot[MAX_SLOTS][LANES];
};

class VM {
private:
    const char *bytecode;
    int pc;

    Mask mask;
    std::vector<Mask> mask_stack;

    Stack stack;
    Slots slots;

    inline __m128 compute_mask() {
        return _mm_castsi128_ps(
            _mm_set_epi32(
                (this->mask & 8) ? -1 : 0,
                (this->mask & 4) ? -1 : 0,
                (this->mask & 2) ? -1 : 0,
                (this->mask & 1) ? -1 : 0
            )
        );
    }

    /*
     * Add two values (SIMD optimized).
     */
    void simd_add() {
        int sp = this->stack.sp;

        __m128 a = _mm_load_ps(this->stack.data[sp-2]);
        __m128 b = _mm_load_ps(this->stack.data[sp-1]);

        __m128 result = _mm_add_ps(a, b);

        // Apply mask
        __m128 mask = compute_mask();
        result = _mm_or_ps(
            _mm_and_ps(mask, result),
            _mm_andnot_ps(mask, a)      // Leave the inactive lanes untouched
        );

        _mm_store_ps(this->stack.data[sp-2], result);
        this->stack.sp--;
    }

    void simd_sub() {
        int sp = this->stack.sp;

        __m128 a = _mm_load_ps(this->stack.data[sp-2]);
        __m128 b = _mm_load_ps(this->stack.data[sp-1]);

        __m128 result = _mm_sub_ps(a, b);

        // Apply mask
        __m128 mask = compute_mask();
        result = _mm_or_ps(
            _mm_and_ps(mask, result),
            _mm_andnot_ps(mask, a)      // Leave the inactive lanes untouched
        );

        _mm_store_ps(this->stack.data[sp-2], result);
        this->stack.sp--;
    }

public:
    VM(const char *bytecode) : 
        bytecode(bytecode), pc(0), mask(ALL_LANES) {}

    ~VM() = default;
};