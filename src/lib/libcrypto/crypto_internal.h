/*	$OpenBSD: crypto_internal.h,v 1.2 2023/04/12 04:54:15 jsing Exp $ */
/*
 * Copyright (c) 2023 Joel Sing <jsing@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <endian.h>
#include <stddef.h>
#include <string.h>

#ifndef HEADER_CRYPTO_INTERNAL_H
#define HEADER_CRYPTO_INTERNAL_H

#ifndef HAVE_CRYPTO_STORE_HTOBE64
static inline void
crypto_store_htobe64(uint8_t *dst, uint64_t v)
{
	v = htobe64(v);
	memcpy(dst, &v, sizeof(v));
}
#endif

#ifndef HAVE_CRYPTO_ROL_U32
static inline uint32_t
crypto_rol_u32(uint32_t v, size_t shift)
{
	return (v << shift) | (v >> (32 - shift));
}
#endif

#ifndef HAVE_CRYPTO_ROR_U32
static inline uint32_t
crypto_ror_u32(uint32_t v, size_t shift)
{
	return (v << (32 - shift)) | (v >> shift);
}
#endif

#ifndef HAVE_CRYPTO_ROL_U64
static inline uint64_t
crypto_rol_u64(uint64_t v, size_t shift)
{
	return (v << shift) | (v >> (64 - shift));
}
#endif

#ifndef HAVE_CRYPTO_ROR_U64
static inline uint64_t
crypto_ror_u64(uint64_t v, size_t shift)
{
	return (v << (64 - shift)) | (v >> shift);
}
#endif

#endif
