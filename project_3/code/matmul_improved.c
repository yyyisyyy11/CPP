#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arm_neon.h>
#include <omp.h>

/*
 * Micro-kernel dimensions for register blocking.
 *
 * MR x NR = 8 x 16: the micro-kernel computes an 8-row x 16-col
 * sub-block of C entirely in NEON registers.
 *
 * C accumulators: 8 rows * 4 float32x4_t = 32 registers (v0-v31).
 * This fully utilizes AArch64's 32 NEON register file.
 */
#define MR 8
#define NR 16

/*
 * BLIS-style multi-level tiling parameters.
 *
 * The idea: partition the matrix multiplication into blocks sized to
 * fit each level of the cache hierarchy.
 *
 *   MC (rows of A panel)  × KC (shared dim) → fits in L2
 *   KC (shared dim)       × NC (cols of B panel) → fits in L3 / memory BW
 *   MR × NR (micro-kernel) → fits in registers
 *
 * Tuned for Apple M5 (128 KB L1d, 4 MB L2 per P-core):
 *   MC=128, KC=256 → packed_A = 128×256×4 = 128 KB (fits L1/L2 boundary)
 *   NC=512         → packed_B = 256×512×4 = 512 KB (fits in L2)
 *
 * MC must be a multiple of MR=8, NC must be a multiple of NR=16.
 */
#define MC 128
#define KC 256
#define NC 512

/**
 * @brief 8x16 micro-kernel: C[8][16] += packed_A[kc][MR] * packed_B[kc][NR].
 *
 * All 32 NEON registers are used as C accumulators. Both A and B are
 * read from contiguous packed buffers with stride MR and NR respectively.
 *
 * Software prefetch hints are inserted to pre-load data 2 iterations
 * ahead, hiding memory latency on the M5's deep pipeline.
 */
static inline void micro_kernel_8x16(const float *packed_A, const float *packed_B,
                                      float *C, size_t ldc, size_t kc) {
    /* Load existing C[8][16] into 32 NEON accumulators */
    float32x4_t vc00 = vld1q_f32(&C[0 * ldc + 0]);
    float32x4_t vc01 = vld1q_f32(&C[0 * ldc + 4]);
    float32x4_t vc02 = vld1q_f32(&C[0 * ldc + 8]);
    float32x4_t vc03 = vld1q_f32(&C[0 * ldc + 12]);
    float32x4_t vc10 = vld1q_f32(&C[1 * ldc + 0]);
    float32x4_t vc11 = vld1q_f32(&C[1 * ldc + 4]);
    float32x4_t vc12 = vld1q_f32(&C[1 * ldc + 8]);
    float32x4_t vc13 = vld1q_f32(&C[1 * ldc + 12]);
    float32x4_t vc20 = vld1q_f32(&C[2 * ldc + 0]);
    float32x4_t vc21 = vld1q_f32(&C[2 * ldc + 4]);
    float32x4_t vc22 = vld1q_f32(&C[2 * ldc + 8]);
    float32x4_t vc23 = vld1q_f32(&C[2 * ldc + 12]);
    float32x4_t vc30 = vld1q_f32(&C[3 * ldc + 0]);
    float32x4_t vc31 = vld1q_f32(&C[3 * ldc + 4]);
    float32x4_t vc32 = vld1q_f32(&C[3 * ldc + 8]);
    float32x4_t vc33 = vld1q_f32(&C[3 * ldc + 12]);
    float32x4_t vc40 = vld1q_f32(&C[4 * ldc + 0]);
    float32x4_t vc41 = vld1q_f32(&C[4 * ldc + 4]);
    float32x4_t vc42 = vld1q_f32(&C[4 * ldc + 8]);
    float32x4_t vc43 = vld1q_f32(&C[4 * ldc + 12]);
    float32x4_t vc50 = vld1q_f32(&C[5 * ldc + 0]);
    float32x4_t vc51 = vld1q_f32(&C[5 * ldc + 4]);
    float32x4_t vc52 = vld1q_f32(&C[5 * ldc + 8]);
    float32x4_t vc53 = vld1q_f32(&C[5 * ldc + 12]);
    float32x4_t vc60 = vld1q_f32(&C[6 * ldc + 0]);
    float32x4_t vc61 = vld1q_f32(&C[6 * ldc + 4]);
    float32x4_t vc62 = vld1q_f32(&C[6 * ldc + 8]);
    float32x4_t vc63 = vld1q_f32(&C[6 * ldc + 12]);
    float32x4_t vc70 = vld1q_f32(&C[7 * ldc + 0]);
    float32x4_t vc71 = vld1q_f32(&C[7 * ldc + 4]);
    float32x4_t vc72 = vld1q_f32(&C[7 * ldc + 8]);
    float32x4_t vc73 = vld1q_f32(&C[7 * ldc + 12]);

    /* Main loop: accumulate C += A * B along k dimension */
    for (size_t ki = 0; ki < kc; ki++) {
        /* Prefetch A and B data 2 iterations ahead */
        __builtin_prefetch(&packed_A[(ki + 2) * MR], 0, 3);
        __builtin_prefetch(&packed_B[(ki + 2) * NR], 0, 3);

        /* Load one row of packed B: 16 floats = 4 NEON vectors */
        const float *bp = &packed_B[ki * NR];
        float32x4_t vb0 = vld1q_f32(bp + 0);
        float32x4_t vb1 = vld1q_f32(bp + 4);
        float32x4_t vb2 = vld1q_f32(bp + 8);
        float32x4_t vb3 = vld1q_f32(bp + 12);

        /* Load one column of packed A: 8 scalars, broadcast each */
        const float *ap = &packed_A[ki * MR];
        float32x4_t va;

        va = vdupq_n_f32(ap[0]);
        vc00 = vfmaq_f32(vc00, va, vb0); vc01 = vfmaq_f32(vc01, va, vb1);
        vc02 = vfmaq_f32(vc02, va, vb2); vc03 = vfmaq_f32(vc03, va, vb3);

        va = vdupq_n_f32(ap[1]);
        vc10 = vfmaq_f32(vc10, va, vb0); vc11 = vfmaq_f32(vc11, va, vb1);
        vc12 = vfmaq_f32(vc12, va, vb2); vc13 = vfmaq_f32(vc13, va, vb3);

        va = vdupq_n_f32(ap[2]);
        vc20 = vfmaq_f32(vc20, va, vb0); vc21 = vfmaq_f32(vc21, va, vb1);
        vc22 = vfmaq_f32(vc22, va, vb2); vc23 = vfmaq_f32(vc23, va, vb3);

        va = vdupq_n_f32(ap[3]);
        vc30 = vfmaq_f32(vc30, va, vb0); vc31 = vfmaq_f32(vc31, va, vb1);
        vc32 = vfmaq_f32(vc32, va, vb2); vc33 = vfmaq_f32(vc33, va, vb3);

        va = vdupq_n_f32(ap[4]);
        vc40 = vfmaq_f32(vc40, va, vb0); vc41 = vfmaq_f32(vc41, va, vb1);
        vc42 = vfmaq_f32(vc42, va, vb2); vc43 = vfmaq_f32(vc43, va, vb3);

        va = vdupq_n_f32(ap[5]);
        vc50 = vfmaq_f32(vc50, va, vb0); vc51 = vfmaq_f32(vc51, va, vb1);
        vc52 = vfmaq_f32(vc52, va, vb2); vc53 = vfmaq_f32(vc53, va, vb3);

        va = vdupq_n_f32(ap[6]);
        vc60 = vfmaq_f32(vc60, va, vb0); vc61 = vfmaq_f32(vc61, va, vb1);
        vc62 = vfmaq_f32(vc62, va, vb2); vc63 = vfmaq_f32(vc63, va, vb3);

        va = vdupq_n_f32(ap[7]);
        vc70 = vfmaq_f32(vc70, va, vb0); vc71 = vfmaq_f32(vc71, va, vb1);
        vc72 = vfmaq_f32(vc72, va, vb2); vc73 = vfmaq_f32(vc73, va, vb3);
    }

    /* Store C[8][16] back to memory */
    vst1q_f32(&C[0 * ldc + 0], vc00); vst1q_f32(&C[0 * ldc + 4], vc01);
    vst1q_f32(&C[0 * ldc + 8], vc02); vst1q_f32(&C[0 * ldc + 12], vc03);
    vst1q_f32(&C[1 * ldc + 0], vc10); vst1q_f32(&C[1 * ldc + 4], vc11);
    vst1q_f32(&C[1 * ldc + 8], vc12); vst1q_f32(&C[1 * ldc + 12], vc13);
    vst1q_f32(&C[2 * ldc + 0], vc20); vst1q_f32(&C[2 * ldc + 4], vc21);
    vst1q_f32(&C[2 * ldc + 8], vc22); vst1q_f32(&C[2 * ldc + 12], vc23);
    vst1q_f32(&C[3 * ldc + 0], vc30); vst1q_f32(&C[3 * ldc + 4], vc31);
    vst1q_f32(&C[3 * ldc + 8], vc32); vst1q_f32(&C[3 * ldc + 12], vc33);
    vst1q_f32(&C[4 * ldc + 0], vc40); vst1q_f32(&C[4 * ldc + 4], vc41);
    vst1q_f32(&C[4 * ldc + 8], vc42); vst1q_f32(&C[4 * ldc + 12], vc43);
    vst1q_f32(&C[5 * ldc + 0], vc50); vst1q_f32(&C[5 * ldc + 4], vc51);
    vst1q_f32(&C[5 * ldc + 8], vc52); vst1q_f32(&C[5 * ldc + 12], vc53);
    vst1q_f32(&C[6 * ldc + 0], vc60); vst1q_f32(&C[6 * ldc + 4], vc61);
    vst1q_f32(&C[6 * ldc + 8], vc62); vst1q_f32(&C[6 * ldc + 12], vc63);
    vst1q_f32(&C[7 * ldc + 0], vc70); vst1q_f32(&C[7 * ldc + 4], vc71);
    vst1q_f32(&C[7 * ldc + 8], vc72); vst1q_f32(&C[7 * ldc + 12], vc73);
}

/**
 * @brief Pack a panel of A into contiguous MR-wide column-major strips.
 *
 * Input:  A in row-major with stride lda (large, causes cache conflicts)
 * Output: packed_A with stride MR (compact, sequential access)
 *
 * Layout: packed_A[ki * MR + ir] = A[ir][ki]
 */
static inline void pack_A_panel(const float *src, float *dst,
                                size_t src_ld, size_t actual_m,
                                size_t actual_k) {
    for (size_t ki = 0; ki < actual_k; ki++) {
        size_t ir;
        for (ir = 0; ir < actual_m; ir++) {
            dst[ki * MR + ir] = src[ir * src_ld + ki];
        }
        for (; ir < MR; ir++) {
            dst[ki * MR + ir] = 0.0f;
        }
    }
}

/**
 * @brief Pack a panel of B into contiguous NR-wide row-major strips.
 *
 * Input:  B in row-major with stride ldb (= n, large)
 * Output: packed_B with stride NR (compact, sequential access)
 *
 * Layout: packed_B[ki * NR + jr] = B[ki][jr]
 */
static inline void pack_B_panel(const float *src, float *dst,
                                size_t src_ld, size_t actual_n,
                                size_t actual_k) {
    for (size_t ki = 0; ki < actual_k; ki++) {
        size_t jr;
        for (jr = 0; jr < actual_n; jr++) {
            dst[ki * NR + jr] = src[ki * src_ld + jr];
        }
        for (; jr < NR; jr++) {
            dst[ki * NR + jr] = 0.0f;
        }
    }
}

/**
 * @brief Scalar fallback for edge cases (rows < MR or cols < NR).
 */
static inline void scalar_edge(const float *A, const float *B, float *C,
                               size_t lda, size_t ldb, size_t ldc,
                               size_t rows, size_t cols, size_t depth) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t ki = 0; ki < depth; ki++) {
            float a_val = A[i * lda + ki];
            for (size_t j = 0; j < cols; j++) {
                C[i * ldc + j] += a_val * B[ki * ldb + j];
            }
        }
    }
}

bool matmul_improved(const Matrix *a, const Matrix *b, Matrix *c) {
    /* Parameter validation */
    if (a == NULL || b == NULL || c == NULL) {
        fprintf(stderr, "Error: NULL matrix pointer in matmul_improved.\n");
        return false;
    }
    if (a->data == NULL || b->data == NULL || c->data == NULL) {
        fprintf(stderr, "Error: NULL data pointer in matmul_improved.\n");
        return false;
    }
    if (a->cols != b->rows) {
        fprintf(stderr, "Error: incompatible dimensions for multiplication "
                "(%zu x %zu) * (%zu x %zu).\n",
                a->rows, a->cols, b->rows, b->cols);
        return false;
    }
    if (c->rows != a->rows || c->cols != b->cols) {
        fprintf(stderr, "Error: output matrix dimensions mismatch. "
                "Expected (%zu x %zu), got (%zu x %zu).\n",
                a->rows, b->cols, c->rows, c->cols);
        return false;
    }

    size_t m = a->rows;
    size_t n = b->cols;
    size_t k = a->cols;

    const float *data_a = a->data;
    const float *data_b = b->data;
    float *data_c = c->data;

    memset(data_c, 0, m * n * sizeof(float));

    /*
     * ============================================================
     *  BLIS-style 5-loop matrix multiplication
     * ============================================================
     *
     *  C (m×n)  =  A (m×k)  ×  B (k×n)
     *
     *  We partition the three dimensions into blocks:
     *
     *    n  is split into NC-wide   column panels  (loop variable: jc)
     *    k  is split into KC-deep   slices          (loop variable: pc)
     *    m  is split into MC-tall   row panels      (loop variable: ic)
     *
     *  Within each (jc, pc, ic) block, we further split into
     *  micro-kernel tiles of MR×NR (loops: ir, jr).
     *
     *  The loop nest looks like:
     *
     *    for jc = 0..n step NC          ← column panel of B & C
     *      for pc = 0..k step KC        ← depth slice (shared dim)
     *        ★ Pack B[pc:pc+KC, jc:jc+NC] → packed_B   (stays in L2)
     *        for ic = 0..m step MC      ← row panel of A & C  [PARALLEL]
     *          ★ Pack A[ic:ic+MC, pc:pc+KC] → packed_A  (stays in L1)
     *          for jr = 0..NC step NR   ← micro-panel of B
     *            for ir = 0..MC step MR ← micro-panel of A
     *              micro_kernel(packed_A[ir], packed_B[jr], C[ic+ir, jc+jr])
     *
     *  Cache hierarchy mapping:
     *    packed_B (KC×NC = 512 KB) → stays in L2, reused by all ic blocks
     *    packed_A (MC×KC = 128 KB) → stays in L1, reused by all jr columns
     *    C tile   (MR×NR = 512 B)  → stays in registers
     */

    /* Allocate packing buffers on heap (too large for stack) */
    float *packed_B = (float *)malloc(KC * NC * sizeof(float));
    if (packed_B == NULL) {
        fprintf(stderr, "Error: failed to allocate packed_B.\n");
        return false;
    }

    /* ---- Loop 1: jc — iterate over NC-wide column panels ---- */
    for (size_t jc = 0; jc < n; jc += NC) {
        size_t nc = (jc + NC <= n) ? NC : (n - jc);

        /* ---- Loop 2: pc — iterate over KC-deep slices ---- */
        for (size_t pc = 0; pc < k; pc += KC) {
            size_t kc = (pc + KC <= k) ? KC : (k - pc);

            /*
             * Pack B[pc..pc+kc][jc..jc+nc] into packed_B.
             *
             * We pack NR columns at a time. Each NR-panel is kc×NR floats,
             * stored contiguously. The panels are laid out sequentially:
             *   packed_B[jr * kc ... (jr+NR) * kc - 1]
             */
            for (size_t jr = 0; jr < nc; jr += NR) {
                size_t nr_actual = (jr + NR <= nc) ? NR : (nc - jr);
                pack_B_panel(
                    &data_b[pc * n + jc + jr],   /* B[pc][jc+jr] */
                    &packed_B[jr * kc],           /* destination in packed_B */
                    n,                            /* source stride = n */
                    nr_actual, kc
                );
            }

            /*
             * ---- Loop 3: ic — iterate over MC-tall row panels ----
             *
             * This is the OpenMP-parallel loop. Each thread:
             *   - gets its own packed_A buffer (stack-allocated inside loop)
             *   - writes to distinct rows of C (no conflicts)
             *   - reads the shared packed_B (read-only, no conflicts)
             */
            #pragma omp parallel for schedule(static)
            for (size_t ic = 0; ic < m; ic += MC) {
                size_t mc = (ic + MC <= m) ? MC : (m - ic);

                /* Thread-private packed_A buffer (stack, fits in L1) */
                float packed_A[MC * KC];

                /*
                 * Pack A[ic..ic+mc][pc..pc+kc] into packed_A.
                 * MR rows at a time, column-major within each panel.
                 */
                for (size_t ir = 0; ir < mc; ir += MR) {
                    size_t mr_actual = (ir + MR <= mc) ? MR : (mc - ir);
                    pack_A_panel(
                        &data_a[(ic + ir) * k + pc],  /* A[ic+ir][pc] */
                        &packed_A[ir * kc],            /* destination */
                        k,                             /* source stride = k */
                        mr_actual, kc
                    );
                }

                /* ---- Loop 4: jr — micro-panels of B ---- */
                for (size_t jr = 0; jr + NR <= nc; jr += NR) {

                    /* ---- Loop 5: ir — micro-panels of A ---- */
                    for (size_t ir = 0; ir + MR <= mc; ir += MR) {
                        micro_kernel_8x16(
                            &packed_A[ir * kc],            /* this thread's A */
                            &packed_B[jr * kc],            /* shared B (read-only) */
                            &data_c[(ic + ir) * n + jc + jr],  /* C output */
                            n,                             /* ldc */
                            kc
                        );
                    }

                    /* Bottom edge: remaining rows < MR */
                    size_t ir_rem = (mc / MR) * MR;
                    if (ir_rem < mc) {
                        scalar_edge(
                            &data_a[(ic + ir_rem) * k + pc],
                            &data_b[pc * n + jc + jr],
                            &data_c[(ic + ir_rem) * n + jc + jr],
                            k, n, n,
                            mc - ir_rem, NR, kc
                        );
                    }
                }

                /* Right edge: remaining columns < NR */
                size_t jr_rem = (nc / NR) * NR;
                if (jr_rem < nc) {
                    for (size_t ir = 0; ir < mc; ir++) {
                        for (size_t ki = 0; ki < kc; ki++) {
                            float a_val = data_a[(ic + ir) * k + pc + ki];
                            for (size_t j = jr_rem; j < nc; j++) {
                                data_c[(ic + ir) * n + jc + j] +=
                                    a_val * data_b[(pc + ki) * n + jc + j];
                            }
                        }
                    }
                }
            } /* end parallel ic */
        } /* end pc */
    } /* end jc */

    free(packed_B);
    return true;
}
