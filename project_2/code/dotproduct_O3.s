	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 2	sdk_version 15, 2
	.globl	_DotFloat                       ; -- Begin function DotFloat
	.p2align	2
_DotFloat:                              ; @DotFloat
	.cfi_startproc
; %bb.0:
	cmp	w2, #1
	b.lt	LBB0_3
; %bb.1:
	mov	w8, w2
	cmp	w2, #16
	b.hs	LBB0_4
; %bb.2:
	mov	x9, #0                          ; =0x0
	movi	d0, #0000000000000000
	b	LBB0_7
LBB0_3:
	movi	d0, #0000000000000000
	ret
LBB0_4:
	and	x9, x8, #0xfffffff0
	add	x10, x1, #32
	add	x11, x0, #32
	movi	d0, #0000000000000000
	mov	x12, x9
LBB0_5:                                 ; =>This Inner Loop Header: Depth=1
	ldp	q1, q2, [x11, #-32]
	ldp	q3, q4, [x11], #64
	ldp	q5, q6, [x10, #-32]
	ldp	q7, q16, [x10], #64
	fmul.4s	v1, v1, v5
	mov	s5, v1[3]
	mov	s17, v1[2]
	mov	s18, v1[1]
	fmul.4s	v2, v2, v6
	mov	s6, v2[3]
	mov	s19, v2[2]
	mov	s20, v2[1]
	fmul.4s	v3, v3, v7
	mov	s7, v3[3]
	mov	s21, v3[2]
	mov	s22, v3[1]
	fmul.4s	v4, v4, v16
	mov	s16, v4[3]
	mov	s23, v4[2]
	mov	s24, v4[1]
	fadd	s0, s0, s1
	fadd	s0, s0, s18
	fadd	s0, s0, s17
	fadd	s0, s0, s5
	fadd	s0, s0, s2
	fadd	s0, s0, s20
	fadd	s0, s0, s19
	fadd	s0, s0, s6
	fadd	s0, s0, s3
	fadd	s0, s0, s22
	fadd	s0, s0, s21
	fadd	s0, s0, s7
	fadd	s0, s0, s4
	fadd	s0, s0, s24
	fadd	s0, s0, s23
	fadd	s0, s0, s16
	subs	x12, x12, #16
	b.ne	LBB0_5
; %bb.6:
	cmp	x9, x8
	b.eq	LBB0_9
LBB0_7:
	lsl	x11, x9, #2
	add	x10, x1, x11
	add	x11, x0, x11
	sub	x8, x8, x9
LBB0_8:                                 ; =>This Inner Loop Header: Depth=1
	ldr	s1, [x11], #4
	ldr	s2, [x10], #4
	fmadd	s0, s1, s2, s0
	subs	x8, x8, #1
	b.ne	LBB0_8
LBB0_9:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_DotDouble                      ; -- Begin function DotDouble
	.p2align	2
_DotDouble:                             ; @DotDouble
	.cfi_startproc
; %bb.0:
	cmp	w2, #1
	b.lt	LBB1_3
; %bb.1:
	mov	w8, w2
	cmp	w2, #8
	b.hs	LBB1_4
; %bb.2:
	mov	x9, #0                          ; =0x0
	movi	d0, #0000000000000000
	b	LBB1_7
LBB1_3:
	movi	d0, #0000000000000000
	ret
LBB1_4:
	and	x9, x8, #0xfffffff8
	add	x10, x1, #32
	add	x11, x0, #32
	movi	d0, #0000000000000000
	mov	x12, x9
LBB1_5:                                 ; =>This Inner Loop Header: Depth=1
	ldp	q1, q2, [x11, #-32]
	ldp	q3, q4, [x11], #64
	ldp	q5, q6, [x10, #-32]
	ldp	q7, q16, [x10], #64
	fmul.2d	v1, v1, v5
	mov	d5, v1[1]
	fmul.2d	v2, v2, v6
	mov	d6, v2[1]
	fmul.2d	v3, v3, v7
	mov	d7, v3[1]
	fmul.2d	v4, v4, v16
	mov	d16, v4[1]
	fadd	d0, d0, d1
	fadd	d0, d0, d5
	fadd	d0, d0, d2
	fadd	d0, d0, d6
	fadd	d0, d0, d3
	fadd	d0, d0, d7
	fadd	d0, d0, d4
	fadd	d0, d0, d16
	subs	x12, x12, #8
	b.ne	LBB1_5
; %bb.6:
	cmp	x9, x8
	b.eq	LBB1_9
LBB1_7:
	lsl	x11, x9, #3
	add	x10, x1, x11
	add	x11, x0, x11
	sub	x8, x8, x9
LBB1_8:                                 ; =>This Inner Loop Header: Depth=1
	ldr	d1, [x11], #8
	ldr	d2, [x10], #8
	fmadd	d0, d1, d2, d0
	subs	x8, x8, #1
	b.ne	LBB1_8
LBB1_9:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_DotInt                         ; -- Begin function DotInt
	.p2align	2
_DotInt:                                ; @DotInt
	.cfi_startproc
; %bb.0:
	cmp	w2, #1
	b.lt	LBB2_3
; %bb.1:
	mov	w9, w2
	cmp	w2, #16
	b.hs	LBB2_4
; %bb.2:
	mov	x10, #0                         ; =0x0
	mov	x8, #0                          ; =0x0
	b	LBB2_7
LBB2_3:
	mov	x8, #0                          ; =0x0
	mov	x0, x8
	ret
LBB2_4:
	movi.2d	v0, #0000000000000000
	and	x10, x9, #0xfffffff0
	movi.2d	v1, #0000000000000000
	add	x8, x1, #32
	add	x11, x0, #32
	mov	x12, x10
	movi.2d	v5, #0000000000000000
	movi.2d	v4, #0000000000000000
	movi.2d	v3, #0000000000000000
	movi.2d	v2, #0000000000000000
	movi.2d	v7, #0000000000000000
	movi.2d	v6, #0000000000000000
LBB2_5:                                 ; =>This Inner Loop Header: Depth=1
	ldp	q16, q17, [x11, #-32]
	ldp	q18, q19, [x11], #64
	ldp	q20, q21, [x8, #-32]
	ldp	q22, q23, [x8], #64
	smlal2.2d	v1, v20, v16
	smlal.2d	v0, v20, v16
	smlal2.2d	v4, v21, v17
	smlal.2d	v5, v21, v17
	smlal2.2d	v2, v22, v18
	smlal.2d	v3, v22, v18
	smlal2.2d	v6, v23, v19
	smlal.2d	v7, v23, v19
	subs	x12, x12, #16
	b.ne	LBB2_5
; %bb.6:
	add.2d	v0, v5, v0
	add.2d	v1, v4, v1
	add.2d	v3, v7, v3
	add.2d	v0, v3, v0
	add.2d	v2, v6, v2
	add.2d	v1, v2, v1
	add.2d	v0, v0, v1
	addp.2d	d0, v0
	fmov	x8, d0
	cmp	x10, x9
	b.eq	LBB2_9
LBB2_7:
	lsl	x12, x10, #2
	add	x11, x1, x12
	add	x12, x0, x12
	sub	x9, x9, x10
LBB2_8:                                 ; =>This Inner Loop Header: Depth=1
	ldrsw	x10, [x12], #4
	ldrsw	x13, [x11], #4
	smaddl	x8, w13, w10, x8
	subs	x9, x9, #1
	b.ne	LBB2_8
LBB2_9:
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_DotShort                       ; -- Begin function DotShort
	.p2align	2
_DotShort:                              ; @DotShort
	.cfi_startproc
; %bb.0:
	cmp	w2, #1
	b.lt	LBB3_3
; %bb.1:
	stp	x20, x19, [sp, #-16]!           ; 16-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	mov	w9, w2
	cmp	w2, #4
	b.hs	LBB3_4
; %bb.2:
	mov	x10, #0                         ; =0x0
	mov	x8, #0                          ; =0x0
	b	LBB3_7
LBB3_3:
	mov	x0, #0                          ; =0x0
	ret
LBB3_4:
	mov	x11, #0                         ; =0x0
	mov	x13, #0                         ; =0x0
	mov	x8, #0                          ; =0x0
	mov	x12, #0                         ; =0x0
	and	x10, x9, #0xfffffffc
	add	x14, x1, #4
	add	x15, x0, #4
	mov	x16, x10
LBB3_5:                                 ; =>This Inner Loop Header: Depth=1
	ldursh	x17, [x15, #-4]
	ldursh	x2, [x15, #-2]
	ldrsh	x3, [x15]
	ldrsh	x4, [x15, #2]
	ldursh	x5, [x14, #-4]
	ldursh	x6, [x14, #-2]
	ldrsh	x7, [x14]
	ldrsh	x19, [x14, #2]
	smaddl	x11, w5, w17, x11
	smaddl	x13, w6, w2, x13
	smaddl	x8, w7, w3, x8
	smaddl	x12, w19, w4, x12
	add	x14, x14, #8
	add	x15, x15, #8
	subs	x16, x16, #4
	b.ne	LBB3_5
; %bb.6:
	add	x11, x13, x11
	add	x8, x12, x8
	add	x8, x8, x11
	cmp	x10, x9
	b.eq	LBB3_9
LBB3_7:
	lsl	x12, x10, #1
	add	x11, x1, x12
	add	x12, x0, x12
	sub	x9, x9, x10
LBB3_8:                                 ; =>This Inner Loop Header: Depth=1
	ldrsh	x10, [x12], #2
	ldrsh	x13, [x11], #2
	smaddl	x8, w13, w10, x8
	subs	x9, x9, #1
	b.ne	LBB3_8
LBB3_9:
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_DotChar                        ; -- Begin function DotChar
	.p2align	2
_DotChar:                               ; @DotChar
	.cfi_startproc
; %bb.0:
	cmp	w2, #1
	b.lt	LBB4_3
; %bb.1:
	stp	x20, x19, [sp, #-16]!           ; 16-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	mov	w9, w2
	cmp	w2, #4
	b.hs	LBB4_4
; %bb.2:
	mov	x10, #0                         ; =0x0
	mov	x8, #0                          ; =0x0
	b	LBB4_7
LBB4_3:
	mov	x0, #0                          ; =0x0
	ret
LBB4_4:
	mov	x11, #0                         ; =0x0
	mov	x13, #0                         ; =0x0
	mov	x8, #0                          ; =0x0
	mov	x12, #0                         ; =0x0
	and	x10, x9, #0xfffffffc
	add	x14, x1, #1
	add	x15, x0, #1
	mov	x16, x10
LBB4_5:                                 ; =>This Inner Loop Header: Depth=1
	ldursb	x17, [x15, #-1]
	ldrsb	x2, [x15]
	ldrsb	x3, [x15, #1]
	ldrsb	x4, [x15, #2]
	ldursb	x5, [x14, #-1]
	ldrsb	x6, [x14]
	ldrsb	x7, [x14, #1]
	ldrsb	x19, [x14, #2]
	smaddl	x11, w5, w17, x11
	smaddl	x13, w6, w2, x13
	smaddl	x8, w7, w3, x8
	smaddl	x12, w19, w4, x12
	add	x14, x14, #4
	add	x15, x15, #4
	subs	x16, x16, #4
	b.ne	LBB4_5
; %bb.6:
	add	x11, x13, x11
	add	x8, x12, x8
	add	x8, x8, x11
	cmp	x10, x9
	b.eq	LBB4_9
LBB4_7:
	add	x11, x1, x10
	add	x12, x0, x10
	sub	x9, x9, x10
LBB4_8:                                 ; =>This Inner Loop Header: Depth=1
	ldrsb	x10, [x12], #1
	ldrsb	x13, [x11], #1
	smaddl	x8, w13, w10, x8
	subs	x9, x9, #1
	b.ne	LBB4_8
LBB4_9:
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_BenchmarkFloat                 ; -- Begin function BenchmarkFloat
	.p2align	2
_BenchmarkFloat:                        ; @BenchmarkFloat
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x26, x25, [sp, #64]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #80]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset b8, -88
	.cfi_offset b9, -96
	mov	x21, x0
	sbfiz	x19, x0, #2, #32
	mov	x0, x19
	bl	_malloc
	mov	x20, x0
	mov	x0, x19
	bl	_malloc
	mov	x19, x0
	cbz	x20, LBB5_11
; %bb.1:
	cbz	x19, LBB5_11
; %bb.2:
	mov	w22, w21
	cmp	w21, #1
	b.lt	LBB5_5
; %bb.3:
	movi.2s	v8, #48, lsl #24
	mov	x23, x22
	mov	x24, x20
	mov	x25, x19
LBB5_4:                                 ; =>This Inner Loop Header: Depth=1
	bl	_rand
	scvtf	s0, w0
	fmul	s0, s0, s8
	str	s0, [x24], #4
	bl	_rand
	scvtf	s0, w0
	fmul	s0, s0, s8
	str	s0, [x25], #4
	subs	x23, x23, #1
	b.ne	LBB5_4
LBB5_5:
	scvtf	d0, w21
	mov	x8, #236961935654912            ; =0xd78400000000
	movk	x8, #16775, lsl #48
	fmov	d1, x8
	fdiv	d0, d1, d0
	fcvtzs	w8, d0
	cmp	w8, #1
	csinc	w8, w8, wzr, gt
	mov	w9, #38528                      ; =0x9680
	movk	w9, #152, lsl #16
	cmp	w8, w9
	csel	w23, w8, w9, lo
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d8, d0, d2, d1
	cmp	w21, #0
	b.le	LBB5_12
; %bb.6:
	cmp	w21, #16
	b.hs	LBB5_15
; %bb.7:
	mov	w8, #0                          ; =0x0
LBB5_8:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB5_9 Depth 2
	movi	d0, #0000000000000000
	mov	x9, x20
	mov	x10, x19
	mov	x11, x22
LBB5_9:                                 ;   Parent Loop BB5_8 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	s1, [x9], #4
	ldr	s2, [x10], #4
	fmadd	s0, s1, s2, s0
	subs	x11, x11, #1
	b.ne	LBB5_9
; %bb.10:                               ;   in Loop: Header=BB5_8 Depth=1
	str	s0, [sp, #28]
	add	w8, w8, #1
	cmp	w8, w23
	b.ne	LBB5_8
	b	LBB5_14
LBB5_11:
Lloh0:
	adrp	x8, ___stderrp@GOTPAGE
Lloh1:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh2:
	ldr	x0, [x8]
                                        ; kill: def $w21 killed $w21 killed $x21 def $x21
	str	x21, [sp]
Lloh3:
	adrp	x1, l_.str@PAGE
Lloh4:
	add	x1, x1, l_.str@PAGEOFF
	bl	_fprintf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #80]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #144
	b	_free
LBB5_12:
	mov	x8, x23
LBB5_13:                                ; =>This Inner Loop Header: Depth=1
	str	wzr, [sp, #28]
	subs	w8, w8, #1
	b.ne	LBB5_13
LBB5_14:
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d0, d0, d2, d1
	ldr	s1, [sp, #28]
	fsub	d0, d0, d8
	scvtf	d1, w23
	fdiv	d0, d0, d1
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fdiv	d0, d0, d1
	str	x21, [sp]
	str	d0, [sp, #8]
Lloh5:
	adrp	x0, l_.str.1@PAGE
Lloh6:
	add	x0, x0, l_.str.1@PAGEOFF
	bl	_printf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	bl	_free
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #80]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB5_15:
	mov	w8, #0                          ; =0x0
	and	x9, x22, #0xfffffff0
	add	x10, x19, #32
	add	x11, x20, #32
	sub	x12, x22, x9
	lsl	x13, x22, #2
	and	x14, x13, #0x3ffffffc0
	add	x13, x19, x14
	add	x14, x20, x14
	b	LBB5_17
LBB5_16:                                ;   in Loop: Header=BB5_17 Depth=1
	str	s0, [sp, #28]
	add	w8, w8, #1
	cmp	w8, w23
	b.eq	LBB5_14
LBB5_17:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB5_18 Depth 2
                                        ;     Child Loop BB5_21 Depth 2
	movi	d0, #0000000000000000
	mov	x15, x11
	mov	x16, x10
	mov	x17, x9
LBB5_18:                                ;   Parent Loop BB5_17 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	q1, q2, [x15, #-32]
	ldp	q3, q4, [x15], #64
	ldp	q5, q6, [x16, #-32]
	ldp	q7, q16, [x16], #64
	fmul.4s	v1, v1, v5
	mov	s5, v1[3]
	mov	s17, v1[2]
	mov	s18, v1[1]
	fmul.4s	v2, v2, v6
	mov	s6, v2[3]
	mov	s19, v2[2]
	mov	s20, v2[1]
	fmul.4s	v3, v3, v7
	mov	s7, v3[3]
	mov	s21, v3[2]
	mov	s22, v3[1]
	fmul.4s	v4, v4, v16
	mov	s16, v4[3]
	mov	s23, v4[2]
	mov	s24, v4[1]
	fadd	s0, s0, s1
	fadd	s0, s0, s18
	fadd	s0, s0, s17
	fadd	s0, s0, s5
	fadd	s0, s0, s2
	fadd	s0, s0, s20
	fadd	s0, s0, s19
	fadd	s0, s0, s6
	fadd	s0, s0, s3
	fadd	s0, s0, s22
	fadd	s0, s0, s21
	fadd	s0, s0, s7
	fadd	s0, s0, s4
	fadd	s0, s0, s24
	fadd	s0, s0, s23
	fadd	s0, s0, s16
	subs	x17, x17, #16
	b.ne	LBB5_18
; %bb.19:                               ;   in Loop: Header=BB5_17 Depth=1
	cmp	x22, x9
	b.eq	LBB5_16
; %bb.20:                               ;   in Loop: Header=BB5_17 Depth=1
	mov	x15, x14
	mov	x16, x13
	mov	x17, x12
LBB5_21:                                ;   Parent Loop BB5_17 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	s1, [x15], #4
	ldr	s2, [x16], #4
	fmadd	s0, s1, s2, s0
	subs	x17, x17, #1
	b.ne	LBB5_21
	b	LBB5_16
	.loh AdrpAdd	Lloh3, Lloh4
	.loh AdrpLdrGotLdr	Lloh0, Lloh1, Lloh2
	.loh AdrpAdd	Lloh5, Lloh6
	.cfi_endproc
                                        ; -- End function
	.globl	_BenchmarkDouble                ; -- Begin function BenchmarkDouble
	.p2align	2
_BenchmarkDouble:                       ; @BenchmarkDouble
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x26, x25, [sp, #64]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #80]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset b8, -88
	.cfi_offset b9, -96
	mov	x21, x0
	sbfiz	x19, x0, #3, #32
	mov	x0, x19
	bl	_malloc
	mov	x20, x0
	mov	x0, x19
	bl	_malloc
	mov	x19, x0
	cbz	x20, LBB6_16
; %bb.1:
	cbz	x19, LBB6_16
; %bb.2:
	mov	w23, w21
	cmp	w21, #1
	b.lt	LBB6_5
; %bb.3:
	mov	x22, #281474972516352           ; =0xffffffc00000
	movk	x22, #16863, lsl #48
	mov	x24, x23
	mov	x25, x20
	mov	x26, x19
LBB6_4:                                 ; =>This Inner Loop Header: Depth=1
	bl	_rand
	scvtf	d0, w0
	fmov	d8, x22
	fdiv	d0, d0, d8
	str	d0, [x25], #8
	bl	_rand
	scvtf	d0, w0
	fdiv	d0, d0, d8
	str	d0, [x26], #8
	subs	x24, x24, #1
	b.ne	LBB6_4
LBB6_5:
	scvtf	d0, w21
	mov	x8, #236961935654912            ; =0xd78400000000
	movk	x8, #16775, lsl #48
	fmov	d1, x8
	fdiv	d0, d1, d0
	fcvtzs	w8, d0
	cmp	w8, #1
	csinc	w8, w8, wzr, gt
	mov	w9, #38528                      ; =0x9680
	movk	w9, #152, lsl #16
	cmp	w8, w9
	csel	w22, w8, w9, lo
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d8, d0, d2, d1
	cmp	w21, #0
	b.le	LBB6_17
; %bb.6:
	cmp	w21, #8
	b.hs	LBB6_20
; %bb.7:
	ldr	d0, [x20]
	ldr	d1, [x19]
	movi	d2, #0000000000000000
	fmadd	d0, d0, d1, d2
	mov	x8, x22
	b	LBB6_9
LBB6_8:                                 ;   in Loop: Header=BB6_9 Depth=1
	str	d1, [sp, #24]
	subs	w8, w8, #1
	b.eq	LBB6_19
LBB6_9:                                 ; =>This Inner Loop Header: Depth=1
	fmov	d1, d0
	cmp	w21, #1
	b.eq	LBB6_8
; %bb.10:                               ;   in Loop: Header=BB6_9 Depth=1
	ldr	d1, [x20, #8]
	ldr	d2, [x19, #8]
	fmadd	d1, d1, d2, d0
	cmp	w21, #2
	b.eq	LBB6_8
; %bb.11:                               ;   in Loop: Header=BB6_9 Depth=1
	ldr	d2, [x20, #16]
	ldr	d3, [x19, #16]
	fmadd	d1, d2, d3, d1
	cmp	w21, #3
	b.eq	LBB6_8
; %bb.12:                               ;   in Loop: Header=BB6_9 Depth=1
	ldr	d2, [x20, #24]
	ldr	d3, [x19, #24]
	fmadd	d1, d2, d3, d1
	cmp	w21, #4
	b.eq	LBB6_8
; %bb.13:                               ;   in Loop: Header=BB6_9 Depth=1
	ldr	d2, [x20, #32]
	ldr	d3, [x19, #32]
	fmadd	d1, d2, d3, d1
	cmp	w21, #5
	b.eq	LBB6_8
; %bb.14:                               ;   in Loop: Header=BB6_9 Depth=1
	ldr	d2, [x20, #40]
	ldr	d3, [x19, #40]
	fmadd	d1, d2, d3, d1
	cmp	w21, #6
	b.eq	LBB6_8
; %bb.15:                               ;   in Loop: Header=BB6_9 Depth=1
	ldr	d2, [x20, #48]
	ldr	d3, [x19, #48]
	fmadd	d1, d2, d3, d1
	b	LBB6_8
LBB6_16:
Lloh7:
	adrp	x8, ___stderrp@GOTPAGE
Lloh8:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh9:
	ldr	x0, [x8]
                                        ; kill: def $w21 killed $w21 killed $x21 def $x21
	str	x21, [sp]
Lloh10:
	adrp	x1, l_.str@PAGE
Lloh11:
	add	x1, x1, l_.str@PAGEOFF
	bl	_fprintf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #80]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #144
	b	_free
LBB6_17:
	mov	x8, x22
LBB6_18:                                ; =>This Inner Loop Header: Depth=1
	str	xzr, [sp, #24]
	subs	w8, w8, #1
	b.ne	LBB6_18
LBB6_19:
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d0, d0, d2, d1
	ldr	d1, [sp, #24]
	fsub	d0, d0, d8
	scvtf	d1, w22
	fdiv	d0, d0, d1
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fdiv	d0, d0, d1
	str	x21, [sp]
	str	d0, [sp, #8]
Lloh12:
	adrp	x0, l_.str.2@PAGE
Lloh13:
	add	x0, x0, l_.str.2@PAGEOFF
	bl	_printf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	bl	_free
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #80]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB6_20:
	mov	w8, #0                          ; =0x0
	and	x9, x23, #0xfffffff8
	add	x10, x19, #32
	add	x11, x20, #32
	sub	x12, x23, x9
	lsl	x13, x23, #3
	and	x14, x13, #0x7ffffffc0
	add	x13, x19, x14
	add	x14, x20, x14
	b	LBB6_22
LBB6_21:                                ;   in Loop: Header=BB6_22 Depth=1
	str	d0, [sp, #24]
	add	w8, w8, #1
	cmp	w8, w22
	b.eq	LBB6_19
LBB6_22:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB6_23 Depth 2
                                        ;     Child Loop BB6_26 Depth 2
	movi	d0, #0000000000000000
	mov	x15, x11
	mov	x16, x10
	mov	x17, x9
LBB6_23:                                ;   Parent Loop BB6_22 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	q1, q2, [x15, #-32]
	ldp	q3, q4, [x15], #64
	ldp	q5, q6, [x16, #-32]
	ldp	q7, q16, [x16], #64
	fmul.2d	v1, v1, v5
	mov	d5, v1[1]
	fmul.2d	v2, v2, v6
	mov	d6, v2[1]
	fmul.2d	v3, v3, v7
	mov	d7, v3[1]
	fmul.2d	v4, v4, v16
	mov	d16, v4[1]
	fadd	d0, d0, d1
	fadd	d0, d0, d5
	fadd	d0, d0, d2
	fadd	d0, d0, d6
	fadd	d0, d0, d3
	fadd	d0, d0, d7
	fadd	d0, d0, d4
	fadd	d0, d0, d16
	subs	x17, x17, #8
	b.ne	LBB6_23
; %bb.24:                               ;   in Loop: Header=BB6_22 Depth=1
	cmp	x23, x9
	b.eq	LBB6_21
; %bb.25:                               ;   in Loop: Header=BB6_22 Depth=1
	mov	x15, x14
	mov	x16, x13
	mov	x17, x12
LBB6_26:                                ;   Parent Loop BB6_22 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	d1, [x15], #8
	ldr	d2, [x16], #8
	fmadd	d0, d1, d2, d0
	subs	x17, x17, #1
	b.ne	LBB6_26
	b	LBB6_21
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpLdrGotLdr	Lloh7, Lloh8, Lloh9
	.loh AdrpAdd	Lloh12, Lloh13
	.cfi_endproc
                                        ; -- End function
	.globl	_BenchmarkInt                   ; -- Begin function BenchmarkInt
	.p2align	2
_BenchmarkInt:                          ; @BenchmarkInt
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #160
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #80]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #96]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #112]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #128]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #144]            ; 16-byte Folded Spill
	add	x29, sp, #144
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	x21, x0
	sbfiz	x19, x0, #2, #32
	mov	x0, x19
	bl	_malloc
	mov	x20, x0
	mov	x0, x19
	bl	_malloc
	mov	x19, x0
	cbz	x20, LBB7_11
; %bb.1:
	cbz	x19, LBB7_11
; %bb.2:
	mov	w22, w21
	cmp	w21, #1
	b.lt	LBB7_5
; %bb.3:
	mov	w23, #6521                      ; =0x1979
	movk	w23, #652, lsl #16
	mov	w24, #201                       ; =0xc9
	mov	x25, x22
	mov	x26, x20
	mov	x27, x19
LBB7_4:                                 ; =>This Inner Loop Header: Depth=1
	bl	_rand
	smull	x8, w0, w23
	lsr	x9, x8, #63
	asr	x8, x8, #33
	add	w8, w8, w9
	msub	w8, w8, w24, w0
	sub	w8, w8, #100
	str	w8, [x26], #4
	bl	_rand
	smull	x8, w0, w23
	lsr	x9, x8, #63
	asr	x8, x8, #33
	add	w8, w8, w9
	msub	w8, w8, w24, w0
	sub	w8, w8, #100
	str	w8, [x27], #4
	subs	x25, x25, #1
	b.ne	LBB7_4
LBB7_5:
	scvtf	d0, w21
	mov	x8, #236961935654912            ; =0xd78400000000
	movk	x8, #16775, lsl #48
	fmov	d1, x8
	fdiv	d0, d1, d0
	fcvtzs	w8, d0
	cmp	w8, #1
	csinc	w8, w8, wzr, gt
	mov	w9, #38528                      ; =0x9680
	movk	w9, #152, lsl #16
	cmp	w8, w9
	csel	w23, w8, w9, lo
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d8, d0, d2, d1
	cmp	w21, #0
	b.le	LBB7_12
; %bb.6:
	cmp	w21, #16
	b.hs	LBB7_15
; %bb.7:
	mov	w8, #0                          ; =0x0
LBB7_8:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB7_9 Depth 2
	mov	x9, #0                          ; =0x0
	mov	x10, x20
	mov	x11, x19
	mov	x12, x22
LBB7_9:                                 ;   Parent Loop BB7_8 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrsw	x13, [x10], #4
	ldrsw	x14, [x11], #4
	smaddl	x9, w14, w13, x9
	subs	x12, x12, #1
	b.ne	LBB7_9
; %bb.10:                               ;   in Loop: Header=BB7_8 Depth=1
	str	x9, [sp, #24]
	add	w8, w8, #1
	cmp	w8, w23
	b.ne	LBB7_8
	b	LBB7_14
LBB7_11:
Lloh14:
	adrp	x8, ___stderrp@GOTPAGE
Lloh15:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh16:
	ldr	x0, [x8]
                                        ; kill: def $w21 killed $w21 killed $x21 def $x21
	str	x21, [sp]
Lloh17:
	adrp	x1, l_.str@PAGE
Lloh18:
	add	x1, x1, l_.str@PAGEOFF
	bl	_fprintf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	b	_free
LBB7_12:
	mov	x8, x23
LBB7_13:                                ; =>This Inner Loop Header: Depth=1
	str	xzr, [sp, #24]
	subs	w8, w8, #1
	b.ne	LBB7_13
LBB7_14:
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d0, d0, d2, d1
	ldr	x8, [sp, #24]
	fsub	d0, d0, d8
	scvtf	d1, w23
	fdiv	d0, d0, d1
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fdiv	d0, d0, d1
	str	x21, [sp]
	str	d0, [sp, #8]
Lloh19:
	adrp	x0, l_.str.3@PAGE
Lloh20:
	add	x0, x0, l_.str.3@PAGEOFF
	bl	_printf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	bl	_free
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	ret
LBB7_15:
	mov	w8, #0                          ; =0x0
	and	x9, x22, #0xfffffff0
	add	x10, x19, #32
	add	x11, x20, #32
	sub	x12, x22, x9
	lsl	x13, x22, #2
	and	x14, x13, #0x3ffffffc0
	add	x13, x19, x14
	add	x14, x20, x14
	b	LBB7_17
LBB7_16:                                ;   in Loop: Header=BB7_17 Depth=1
	str	x15, [sp, #24]
	add	w8, w8, #1
	cmp	w8, w23
	b.eq	LBB7_14
LBB7_17:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB7_18 Depth 2
                                        ;     Child Loop BB7_21 Depth 2
	movi.2d	v0, #0000000000000000
	mov	x15, x11
	movi.2d	v1, #0000000000000000
	mov	x16, x10
	mov	x17, x9
	movi.2d	v3, #0000000000000000
	movi.2d	v4, #0000000000000000
	movi.2d	v5, #0000000000000000
	movi.2d	v2, #0000000000000000
	movi.2d	v7, #0000000000000000
	movi.2d	v6, #0000000000000000
LBB7_18:                                ;   Parent Loop BB7_17 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	q16, q17, [x15, #-32]
	ldp	q18, q19, [x15], #64
	ldp	q20, q21, [x16, #-32]
	ldp	q22, q23, [x16], #64
	smlal2.2d	v1, v20, v16
	smlal.2d	v0, v20, v16
	smlal2.2d	v4, v21, v17
	smlal.2d	v3, v21, v17
	smlal2.2d	v2, v22, v18
	smlal.2d	v5, v22, v18
	smlal2.2d	v6, v23, v19
	smlal.2d	v7, v23, v19
	subs	x17, x17, #16
	b.ne	LBB7_18
; %bb.19:                               ;   in Loop: Header=BB7_17 Depth=1
	add.2d	v0, v3, v0
	add.2d	v1, v4, v1
	add.2d	v3, v7, v5
	add.2d	v0, v3, v0
	add.2d	v2, v6, v2
	add.2d	v1, v2, v1
	add.2d	v0, v0, v1
	addp.2d	d0, v0
	fmov	x15, d0
	cmp	x22, x9
	b.eq	LBB7_16
; %bb.20:                               ;   in Loop: Header=BB7_17 Depth=1
	mov	x16, x14
	mov	x17, x13
	mov	x0, x12
LBB7_21:                                ;   Parent Loop BB7_17 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrsw	x1, [x16], #4
	ldrsw	x2, [x17], #4
	smaddl	x15, w2, w1, x15
	subs	x0, x0, #1
	b.ne	LBB7_21
	b	LBB7_16
	.loh AdrpAdd	Lloh17, Lloh18
	.loh AdrpLdrGotLdr	Lloh14, Lloh15, Lloh16
	.loh AdrpAdd	Lloh19, Lloh20
	.cfi_endproc
                                        ; -- End function
	.globl	_BenchmarkShort                 ; -- Begin function BenchmarkShort
	.p2align	2
_BenchmarkShort:                        ; @BenchmarkShort
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #160
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #80]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #96]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #112]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #128]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #144]            ; 16-byte Folded Spill
	add	x29, sp, #144
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	x21, x0
	sbfiz	x19, x0, #1, #32
	mov	x0, x19
	bl	_malloc
	mov	x20, x0
	mov	x0, x19
	bl	_malloc
	mov	x19, x0
	cbz	x20, LBB8_9
; %bb.1:
	cbz	x19, LBB8_9
; %bb.2:
	mov	w23, w21
	cmp	w21, #1
	b.lt	LBB8_5
; %bb.3:
	mov	w22, #6521                      ; =0x1979
	movk	w22, #652, lsl #16
	mov	w24, #201                       ; =0xc9
	mov	x25, x23
	mov	x26, x20
	mov	x27, x19
LBB8_4:                                 ; =>This Inner Loop Header: Depth=1
	bl	_rand
	smull	x8, w0, w22
	lsr	x9, x8, #63
	lsr	x8, x8, #33
	add	w8, w8, w9
	msub	w8, w8, w24, w0
	sub	w8, w8, #100
	strh	w8, [x26], #2
	bl	_rand
	smull	x8, w0, w22
	lsr	x9, x8, #63
	lsr	x8, x8, #33
	add	w8, w8, w9
	msub	w8, w8, w24, w0
	sub	w8, w8, #100
	strh	w8, [x27], #2
	subs	x25, x25, #1
	b.ne	LBB8_4
LBB8_5:
	scvtf	d0, w21
	mov	x8, #236961935654912            ; =0xd78400000000
	movk	x8, #16775, lsl #48
	fmov	d1, x8
	fdiv	d0, d1, d0
	fcvtzs	w8, d0
	cmp	w8, #1
	csinc	w8, w8, wzr, gt
	mov	w9, #38528                      ; =0x9680
	movk	w9, #152, lsl #16
	cmp	w8, w9
	csel	w22, w8, w9, lo
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d8, d0, d2, d1
	cmp	w21, #0
	b.le	LBB8_10
; %bb.6:
	cmp	w21, #1
	b.ne	LBB8_12
; %bb.7:
	ldrsh	x8, [x20]
	ldrsh	x9, [x19]
	smull	x8, w9, w8
	mov	x9, x22
LBB8_8:                                 ; =>This Inner Loop Header: Depth=1
	str	x8, [sp, #24]
	subs	w9, w9, #1
	b.ne	LBB8_8
	b	LBB8_23
LBB8_9:
Lloh21:
	adrp	x8, ___stderrp@GOTPAGE
Lloh22:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh23:
	ldr	x0, [x8]
                                        ; kill: def $w21 killed $w21 killed $x21 def $x21
	str	x21, [sp]
Lloh24:
	adrp	x1, l_.str@PAGE
Lloh25:
	add	x1, x1, l_.str@PAGEOFF
	bl	_fprintf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	b	_free
LBB8_10:
	mov	x8, x22
LBB8_11:                                ; =>This Inner Loop Header: Depth=1
	str	xzr, [sp, #24]
	subs	w8, w8, #1
	b.ne	LBB8_11
	b	LBB8_23
LBB8_12:
	and	x8, x23, #0xfffffffe
	subs	x9, x23, x8
	b.ne	LBB8_17
; %bb.13:
	add	x10, x19, #2
	add	x11, x20, #2
LBB8_14:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB8_15 Depth 2
	mov	x12, #0                         ; =0x0
	mov	x13, #0                         ; =0x0
	mov	x14, x11
	mov	x15, x10
	mov	x16, x8
LBB8_15:                                ;   Parent Loop BB8_14 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldursh	x17, [x14, #-2]
	ldrsh	x0, [x14]
	ldursh	x1, [x15, #-2]
	ldrsh	x2, [x15]
	smaddl	x12, w1, w17, x12
	smaddl	x13, w2, w0, x13
	add	x15, x15, #4
	add	x14, x14, #4
	subs	x16, x16, #2
	b.ne	LBB8_15
; %bb.16:                               ;   in Loop: Header=BB8_14 Depth=1
	add	x12, x13, x12
	str	x12, [sp, #24]
	add	w9, w9, #1
	cmp	w9, w22
	b.ne	LBB8_14
	b	LBB8_23
LBB8_17:
	mov	w10, #0                         ; =0x0
	add	x11, x20, #2
	add	x12, x19, #2
	lsl	x13, x23, #1
	and	x14, x13, #0x1fffffffc
	add	x13, x19, x14
	add	x14, x20, x14
LBB8_18:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB8_19 Depth 2
                                        ;     Child Loop BB8_21 Depth 2
	mov	x15, #0                         ; =0x0
	mov	x16, #0                         ; =0x0
	mov	x17, x8
	mov	x0, x12
	mov	x1, x11
LBB8_19:                                ;   Parent Loop BB8_18 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldursh	x2, [x1, #-2]
	ldrsh	x3, [x1]
	ldursh	x4, [x0, #-2]
	ldrsh	x5, [x0]
	smaddl	x15, w4, w2, x15
	smaddl	x16, w5, w3, x16
	add	x1, x1, #4
	add	x0, x0, #4
	subs	x17, x17, #2
	b.ne	LBB8_19
; %bb.20:                               ;   in Loop: Header=BB8_18 Depth=1
	add	x15, x16, x15
	mov	x16, x14
	mov	x17, x13
	mov	x0, x9
LBB8_21:                                ;   Parent Loop BB8_18 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrsh	x1, [x16], #2
	ldrsh	x2, [x17], #2
	smaddl	x15, w2, w1, x15
	subs	x0, x0, #1
	b.ne	LBB8_21
; %bb.22:                               ;   in Loop: Header=BB8_18 Depth=1
	str	x15, [sp, #24]
	add	w10, w10, #1
	cmp	w10, w22
	b.ne	LBB8_18
LBB8_23:
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d0, d0, d2, d1
	ldr	x8, [sp, #24]
	fsub	d0, d0, d8
	scvtf	d1, w22
	fdiv	d0, d0, d1
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fdiv	d0, d0, d1
	str	x21, [sp]
	str	d0, [sp, #8]
Lloh26:
	adrp	x0, l_.str.4@PAGE
Lloh27:
	add	x0, x0, l_.str.4@PAGEOFF
	bl	_printf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	bl	_free
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	ret
	.loh AdrpAdd	Lloh24, Lloh25
	.loh AdrpLdrGotLdr	Lloh21, Lloh22, Lloh23
	.loh AdrpAdd	Lloh26, Lloh27
	.cfi_endproc
                                        ; -- End function
	.globl	_BenchmarkChar                  ; -- Begin function BenchmarkChar
	.p2align	2
_BenchmarkChar:                         ; @BenchmarkChar
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #160
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #80]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #96]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #112]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #128]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #144]            ; 16-byte Folded Spill
	add	x29, sp, #144
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	x21, x0
	sxtw	x19, w0
	mov	x0, x19
	bl	_malloc
	mov	x20, x0
	mov	x0, x19
	bl	_malloc
	mov	x19, x0
	cbz	x20, LBB9_9
; %bb.1:
	cbz	x19, LBB9_9
; %bb.2:
	mov	w23, w21
	cmp	w21, #1
	b.lt	LBB9_5
; %bb.3:
	mov	w22, #6521                      ; =0x1979
	movk	w22, #652, lsl #16
	mov	w24, #201                       ; =0xc9
	mov	x25, x23
	mov	x26, x20
	mov	x27, x19
LBB9_4:                                 ; =>This Inner Loop Header: Depth=1
	bl	_rand
	smull	x8, w0, w22
	lsr	x9, x8, #63
	lsr	x8, x8, #33
	add	w8, w8, w9
	msub	w8, w8, w24, w0
	sub	w8, w8, #100
	strb	w8, [x26], #1
	bl	_rand
	smull	x8, w0, w22
	lsr	x9, x8, #63
	lsr	x8, x8, #33
	add	w8, w8, w9
	msub	w8, w8, w24, w0
	sub	w8, w8, #100
	strb	w8, [x27], #1
	subs	x25, x25, #1
	b.ne	LBB9_4
LBB9_5:
	scvtf	d0, w21
	mov	x8, #236961935654912            ; =0xd78400000000
	movk	x8, #16775, lsl #48
	fmov	d1, x8
	fdiv	d0, d1, d0
	fcvtzs	w8, d0
	cmp	w8, #1
	csinc	w8, w8, wzr, gt
	mov	w9, #38528                      ; =0x9680
	movk	w9, #152, lsl #16
	cmp	w8, w9
	csel	w22, w8, w9, lo
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d8, d0, d2, d1
	cmp	w21, #0
	b.le	LBB9_10
; %bb.6:
	cmp	w21, #1
	b.ne	LBB9_12
; %bb.7:
	ldrsb	x8, [x20]
	ldrsb	x9, [x19]
	smull	x8, w9, w8
	mov	x9, x22
LBB9_8:                                 ; =>This Inner Loop Header: Depth=1
	str	x8, [sp, #24]
	subs	w9, w9, #1
	b.ne	LBB9_8
	b	LBB9_23
LBB9_9:
Lloh28:
	adrp	x8, ___stderrp@GOTPAGE
Lloh29:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh30:
	ldr	x0, [x8]
                                        ; kill: def $w21 killed $w21 killed $x21 def $x21
	str	x21, [sp]
Lloh31:
	adrp	x1, l_.str@PAGE
Lloh32:
	add	x1, x1, l_.str@PAGEOFF
	bl	_fprintf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	b	_free
LBB9_10:
	mov	x8, x22
LBB9_11:                                ; =>This Inner Loop Header: Depth=1
	str	xzr, [sp, #24]
	subs	w8, w8, #1
	b.ne	LBB9_11
	b	LBB9_23
LBB9_12:
	and	x8, x23, #0xfffffffe
	subs	x9, x23, x8
	b.ne	LBB9_17
; %bb.13:
	add	x10, x19, #1
	add	x11, x20, #1
LBB9_14:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB9_15 Depth 2
	mov	x12, #0                         ; =0x0
	mov	x13, #0                         ; =0x0
	mov	x14, x11
	mov	x15, x10
	mov	x16, x8
LBB9_15:                                ;   Parent Loop BB9_14 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldursb	x17, [x14, #-1]
	ldrsb	x0, [x14]
	ldursb	x1, [x15, #-1]
	ldrsb	x2, [x15]
	smaddl	x12, w1, w17, x12
	smaddl	x13, w2, w0, x13
	add	x15, x15, #2
	add	x14, x14, #2
	subs	x16, x16, #2
	b.ne	LBB9_15
; %bb.16:                               ;   in Loop: Header=BB9_14 Depth=1
	add	x12, x13, x12
	str	x12, [sp, #24]
	add	w9, w9, #1
	cmp	w9, w22
	b.ne	LBB9_14
	b	LBB9_23
LBB9_17:
	mov	w10, #0                         ; =0x0
	add	x11, x20, #1
	add	x12, x19, #1
	add	x13, x19, x8
	add	x14, x20, x8
LBB9_18:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB9_19 Depth 2
                                        ;     Child Loop BB9_21 Depth 2
	mov	x15, #0                         ; =0x0
	mov	x16, #0                         ; =0x0
	mov	x17, x8
	mov	x0, x12
	mov	x1, x11
LBB9_19:                                ;   Parent Loop BB9_18 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldursb	x2, [x1, #-1]
	ldrsb	x3, [x1]
	ldursb	x4, [x0, #-1]
	ldrsb	x5, [x0]
	smaddl	x15, w4, w2, x15
	smaddl	x16, w5, w3, x16
	add	x1, x1, #2
	add	x0, x0, #2
	subs	x17, x17, #2
	b.ne	LBB9_19
; %bb.20:                               ;   in Loop: Header=BB9_18 Depth=1
	add	x15, x16, x15
	mov	x16, x14
	mov	x17, x13
	mov	x0, x9
LBB9_21:                                ;   Parent Loop BB9_18 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrsb	x1, [x16], #1
	ldrsb	x2, [x17], #1
	smaddl	x15, w2, w1, x15
	subs	x0, x0, #1
	b.ne	LBB9_21
; %bb.22:                               ;   in Loop: Header=BB9_18 Depth=1
	str	x15, [sp, #24]
	add	w10, w10, #1
	cmp	w10, w22
	b.ne	LBB9_18
LBB9_23:
	add	x1, sp, #32
	mov	w0, #6                          ; =0x6
	bl	_clock_gettime
	ldp	d0, d1, [sp, #32]
	scvtf	d0, d0
	scvtf	d1, d1
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d2, x8
	fmadd	d0, d0, d2, d1
	ldr	x8, [sp, #24]
	fsub	d0, d0, d8
	scvtf	d1, w22
	fdiv	d0, d0, d1
	mov	x8, #145685290680320            ; =0x848000000000
	movk	x8, #16686, lsl #48
	fmov	d1, x8
	fdiv	d0, d0, d1
	str	x21, [sp]
	str	d0, [sp, #8]
Lloh33:
	adrp	x0, l_.str.5@PAGE
Lloh34:
	add	x0, x0, l_.str.5@PAGEOFF
	bl	_printf
	mov	x0, x20
	bl	_free
	mov	x0, x19
	bl	_free
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	add	sp, sp, #160
	ret
	.loh AdrpAdd	Lloh31, Lloh32
	.loh AdrpLdrGotLdr	Lloh28, Lloh29, Lloh30
	.loh AdrpAdd	Lloh33, Lloh34
	.cfi_endproc
                                        ; -- End function
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	mov	w0, #42                         ; =0x2a
	bl	_srand
Lloh35:
	adrp	x0, l_str@PAGE
Lloh36:
	add	x0, x0, l_str@PAGEOFF
	bl	_puts
	mov	w0, #128                        ; =0x80
	bl	_BenchmarkFloat
	mov	w0, #1000                       ; =0x3e8
	bl	_BenchmarkFloat
	mov	w0, #10000                      ; =0x2710
	bl	_BenchmarkFloat
	mov	w0, #34464                      ; =0x86a0
	movk	w0, #1, lsl #16
	bl	_BenchmarkFloat
	mov	w0, #16960                      ; =0x4240
	movk	w0, #15, lsl #16
	bl	_BenchmarkFloat
	mov	w0, #38528                      ; =0x9680
	movk	w0, #152, lsl #16
	bl	_BenchmarkFloat
	mov	w0, #57600                      ; =0xe100
	movk	w0, #1525, lsl #16
	bl	_BenchmarkFloat
	mov	w0, #128                        ; =0x80
	bl	_BenchmarkDouble
	mov	w0, #1000                       ; =0x3e8
	bl	_BenchmarkDouble
	mov	w0, #10000                      ; =0x2710
	bl	_BenchmarkDouble
	mov	w0, #34464                      ; =0x86a0
	movk	w0, #1, lsl #16
	bl	_BenchmarkDouble
	mov	w0, #16960                      ; =0x4240
	movk	w0, #15, lsl #16
	bl	_BenchmarkDouble
	mov	w0, #38528                      ; =0x9680
	movk	w0, #152, lsl #16
	bl	_BenchmarkDouble
	mov	w0, #57600                      ; =0xe100
	movk	w0, #1525, lsl #16
	bl	_BenchmarkDouble
	mov	w0, #128                        ; =0x80
	bl	_BenchmarkInt
	mov	w0, #1000                       ; =0x3e8
	bl	_BenchmarkInt
	mov	w0, #10000                      ; =0x2710
	bl	_BenchmarkInt
	mov	w0, #34464                      ; =0x86a0
	movk	w0, #1, lsl #16
	bl	_BenchmarkInt
	mov	w0, #16960                      ; =0x4240
	movk	w0, #15, lsl #16
	bl	_BenchmarkInt
	mov	w0, #38528                      ; =0x9680
	movk	w0, #152, lsl #16
	bl	_BenchmarkInt
	mov	w0, #57600                      ; =0xe100
	movk	w0, #1525, lsl #16
	bl	_BenchmarkInt
	mov	w0, #128                        ; =0x80
	bl	_BenchmarkShort
	mov	w0, #1000                       ; =0x3e8
	bl	_BenchmarkShort
	mov	w0, #10000                      ; =0x2710
	bl	_BenchmarkShort
	mov	w0, #34464                      ; =0x86a0
	movk	w0, #1, lsl #16
	bl	_BenchmarkShort
	mov	w0, #16960                      ; =0x4240
	movk	w0, #15, lsl #16
	bl	_BenchmarkShort
	mov	w0, #38528                      ; =0x9680
	movk	w0, #152, lsl #16
	bl	_BenchmarkShort
	mov	w0, #57600                      ; =0xe100
	movk	w0, #1525, lsl #16
	bl	_BenchmarkShort
	mov	w0, #128                        ; =0x80
	bl	_BenchmarkChar
	mov	w0, #1000                       ; =0x3e8
	bl	_BenchmarkChar
	mov	w0, #10000                      ; =0x2710
	bl	_BenchmarkChar
	mov	w0, #34464                      ; =0x86a0
	movk	w0, #1, lsl #16
	bl	_BenchmarkChar
	mov	w0, #16960                      ; =0x4240
	movk	w0, #15, lsl #16
	bl	_BenchmarkChar
	mov	w0, #38528                      ; =0x9680
	movk	w0, #152, lsl #16
	bl	_BenchmarkChar
	mov	w0, #57600                      ; =0xe100
	movk	w0, #1525, lsl #16
	bl	_BenchmarkChar
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpAdd	Lloh35, Lloh36
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Memory allocation failed for n = %d\n"

l_.str.1:                               ; @.str.1
	.asciz	"float,%d,%.8f\n"

l_.str.2:                               ; @.str.2
	.asciz	"double,%d,%.8f\n"

l_.str.3:                               ; @.str.3
	.asciz	"int,%d,%.8f\n"

l_.str.4:                               ; @.str.4
	.asciz	"short,%d,%.8f\n"

l_.str.5:                               ; @.str.5
	.asciz	"char,%d,%.8f\n"

l_str:                                  ; @str
	.asciz	"type,n,ms"

.subsections_via_symbols
