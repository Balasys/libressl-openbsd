/* $OpenBSD: ecp_nist.c,v 1.26 2023/04/11 18:58:20 jsing Exp $ */
/*
 * Written by Nils Larsch for the OpenSSL project.
 */
/* ====================================================================
 * Copyright (c) 1998-2003 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */
/* ====================================================================
 * Copyright 2002 Sun Microsystems, Inc. ALL RIGHTS RESERVED.
 * Portions of this software developed by SUN MICROSYSTEMS, INC.,
 * and contributed to the OpenSSL project.
 */

#include <limits.h>

#include <openssl/err.h>
#include <openssl/objects.h>

#include "ec_local.h"

static int
ec_GFp_nist_group_copy(EC_GROUP *dest, const EC_GROUP *src)
{
	dest->field_mod_func = src->field_mod_func;

	return ec_GFp_simple_group_copy(dest, src);
}

static int
ec_GFp_nist_group_set_curve(EC_GROUP *group, const BIGNUM *p,
    const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx)
{
	if (BN_ucmp(BN_get0_nist_prime_192(), p) == 0)
		group->field_mod_func = BN_nist_mod_192;
	else if (BN_ucmp(BN_get0_nist_prime_224(), p) == 0)
		group->field_mod_func = BN_nist_mod_224;
	else if (BN_ucmp(BN_get0_nist_prime_256(), p) == 0)
		group->field_mod_func = BN_nist_mod_256;
	else if (BN_ucmp(BN_get0_nist_prime_384(), p) == 0)
		group->field_mod_func = BN_nist_mod_384;
	else if (BN_ucmp(BN_get0_nist_prime_521(), p) == 0)
		group->field_mod_func = BN_nist_mod_521;
	else {
		ECerror(EC_R_NOT_A_NIST_PRIME);
		return 0;
	}

	return ec_GFp_simple_group_set_curve(group, p, a, b, ctx);
}

static int
ec_GFp_nist_field_mul(const EC_GROUP *group, BIGNUM *r, const BIGNUM *a,
    const BIGNUM *b, BN_CTX *ctx)
{
	if (group == NULL || r == NULL || a == NULL || b == NULL) {
		ECerror(ERR_R_PASSED_NULL_PARAMETER);
		return 0;
	}

	if (!BN_mul(r, a, b, ctx))
		return 0;

	return group->field_mod_func(r, r, &group->field, ctx);
}

static int
ec_GFp_nist_field_sqr(const EC_GROUP *group, BIGNUM *r, const BIGNUM *a,
    BN_CTX *ctx)
{
	if (group == NULL || r == NULL || a == NULL) {
		ECerror(EC_R_PASSED_NULL_PARAMETER);
		return 0;
	}

	if (!BN_sqr(r, a, ctx))
		return 0;

	return group->field_mod_func(r, r, &group->field, ctx);
}

static const EC_METHOD ec_GFp_nist_method = {
	.field_type = NID_X9_62_prime_field,
	.group_init = ec_GFp_simple_group_init,
	.group_finish = ec_GFp_simple_group_finish,
	.group_copy = ec_GFp_nist_group_copy,
	.group_set_curve = ec_GFp_nist_group_set_curve,
	.group_get_curve = ec_GFp_simple_group_get_curve,
	.group_get_degree = ec_GFp_simple_group_get_degree,
	.group_order_bits = ec_group_simple_order_bits,
	.group_check_discriminant = ec_GFp_simple_group_check_discriminant,
	.point_init = ec_GFp_simple_point_init,
	.point_finish = ec_GFp_simple_point_finish,
	.point_copy = ec_GFp_simple_point_copy,
	.point_set_to_infinity = ec_GFp_simple_point_set_to_infinity,
	.point_set_Jprojective_coordinates =
	    ec_GFp_simple_set_Jprojective_coordinates,
	.point_get_Jprojective_coordinates =
	    ec_GFp_simple_get_Jprojective_coordinates,
	.point_set_affine_coordinates =
	    ec_GFp_simple_point_set_affine_coordinates,
	.point_get_affine_coordinates =
	    ec_GFp_simple_point_get_affine_coordinates,
	.point_set_compressed_coordinates =
	    ec_GFp_simple_set_compressed_coordinates,
	.point2oct = ec_GFp_simple_point2oct,
	.oct2point = ec_GFp_simple_oct2point,
	.add = ec_GFp_simple_add,
	.dbl = ec_GFp_simple_dbl,
	.invert = ec_GFp_simple_invert,
	.is_at_infinity = ec_GFp_simple_is_at_infinity,
	.is_on_curve = ec_GFp_simple_is_on_curve,
	.point_cmp = ec_GFp_simple_cmp,
	.make_affine = ec_GFp_simple_make_affine,
	.points_make_affine = ec_GFp_simple_points_make_affine,
	.mul_generator_ct = ec_GFp_simple_mul_generator_ct,
	.mul_single_ct = ec_GFp_simple_mul_single_ct,
	.mul_double_nonct = ec_GFp_simple_mul_double_nonct,
	.field_mul = ec_GFp_nist_field_mul,
	.field_sqr = ec_GFp_nist_field_sqr,
	.blind_coordinates = ec_GFp_simple_blind_coordinates,
};

const EC_METHOD *
EC_GFp_nist_method(void)
{
	return &ec_GFp_nist_method;
}
