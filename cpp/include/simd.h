#ifndef SIMD_H
#define SIMD_H

#ifdef ARCH_X86

#ifdef __AVX2__
#include <immintrin.h>

#define LANES 8

#define __veci __m256i
#define __vecf __m256

#define _vec_storei(target, value) _mm256_storeu_si256((__veci *)(target), (value))
#define _vec_storef _mm256_storeu_ps
#define _vec_loadi(src) _mm256_loadu_si256((__veci *)(src))
#define _vec_loadf _mm256_loadu_ps

#define _vec_addi _mm256_add_epi32
#define _vec_addf _mm256_add_ps
#define _vec_subi _mm256_sub_epi32
#define _vec_subf _mm256_sub_ps
#define _vec_muli _mm256_mullo_epi32
#define _vec_mulf _mm256_mul_ps
#define _vec_divf _mm256_div_ps

#define _vec_castfi _mm256_castps_si256
#define _vec_castif _mm256_castsi256_ps

#define _vec_cmplti(a, b) _mm256_cmpgt_epi32((b), (a))
#define _vec_cmpltf(a, b) _mm256_cmp_ps((a), (b), _CMP_LT_OQ)
#define _vec_cmplef(a, b) _mm256_cmp_ps((a), (b), _CMP_LE_OQ)
#define _vec_cmpgtf(a, b) _mm256_cmp_ps((a), (b), _CMP_GT_OQ)
#define _vec_cmpgef(a, b) _mm256_cmp_ps((a), (b), _CMP_GE_OQ)
#define _vec_cmpeqi _mm256_cmpeq_epi32
#define _vec_cmpeqf(a, b) _mm256_cmp_ps((a), (b), _CMP_EQ_OQ)
#define _vec_cmpnef(a, b) _mm256_cmp_ps((a), (b), _CMP_NEQ_OQ)

#define _vec_andi _mm256_and_si256
#define _vec_andf _mm256_and_ps
#define _vec_ori _mm256_or_si256
#define _vec_orf _mm256_or_ps
#define _vec_xori _mm256_xor_si256
#define _vec_andnoti _mm256_andnot_si256
#define _vec_andnotf _mm256_andnot_ps

#define _vec_sli _mm256_slli_epi32
#define _vec_sri _mm256_srli_epi32

#define _vec_bcsti _mm256_set1_epi32
#define _vec_bcstf _mm256_set1_ps


#elifdef __SSE4_1__
#include <smmintrin.h>

#define LANES 4

#define __veci __m128i
#define __vecf __m128

#define _vec_storei(target, value) _mm_storeu_si128((__veci *)(target), (value))
#define _vec_storef _mm_storeu_ps
#define _vec_loadi(src) _mm_loadu_si128((__veci *)(src))
#define _vec_loadf _mm_loadu_ps

#define _vec_addi _mm_add_epi32
#define _vec_addf _mm_add_ps
#define _vec_subi _mm_sub_epi32
#define _vec_subf _mm_sub_ps
#define _vec_muli _mm_mullo_epi32
#define _vec_mulf _mm_mul_ps
#define _vec_divf _mm_div_ps

#define _vec_castfi _mm_castps_si128
#define _vec_castif _mm_castsi128_ps

#define _vec_cmplti _mm_cmplt_epi32
#define _vec_cmpltf _mm_cmplt_ps
#define _vec_cmplef _mm_cmple_ps
#define _vec_cmpgtf _mm_cmpgt_ps
#define _vec_cmpgef _mm_cmpge_ps
#define _vec_cmpeqi _mm_cmpeq_epi32
#define _vec_cmpeqf _mm_cmpeq_ps
#define _vec_cmpnef _mm_cmpneq_ps

#define _vec_andi _mm_and_si128
#define _vec_andf _mm_and_ps
#define _vec_ori _mm_or_si128
#define _vec_orf _mm_or_ps
#define _vec_xori _mm_xor_si128
#define _vec_andnoti _mm_andnot_si128
#define _vec_andnotf _mm_andnot_ps

#define _vec_sli _mm_slli_epi32
#define _vec_sri _mm_srli_epi32

#define _vec_bcsti _mm_set1_epi32
#define _vec_bcstf _mm_set1_ps

#else
#error "SIMD requires at least SSE4.1"
#endif

#endif

#endif