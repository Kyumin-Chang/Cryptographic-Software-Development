#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "typedef.h"

void LtoR_Exp(MPZ *r, MPZ *input, MPZ *exp, MPZ *mod)
{
	SINT32 i, normbits, t;
	MPZ x, y, q;

	for (i = 31; i >= 0; i--)
	{
		if (exp->dat[exp->len - 1] & (1 << i))
			break;
	}
	normbits = 31 - i;

	t = exp->len * 32 - normbits;

	x.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	y.dat = (UINT32*)calloc(2*mod->len, sizeof(UINT32));
	q.dat = (UINT32*)calloc(mod->len+2, sizeof(UINT32));

	COPY_MPZ(&x, input);// x <- input
	for (i = t - 2; i >= 0; i--)
	{
		MPZ_SQR(&y, &x);	
		MPZ_UDIV(&q, &x, &y, mod); //x <- x*x mod n
		if (exp->dat[i / 32] & (1 << (i % 32)))
		{
			MPZ_MUL(&y, &x, input);
			MPZ_UDIV(&q, &x, &y, mod); //x <- x*x mod n
		}
	}
	COPY_MPZ(r, &x);// r <- x

	free(x.dat);
	free(y.dat);
	free(q.dat);
}


void LtoR_Kary_Exp(MPZ *r, MPZ *input, MPZ *exp, MPZ *mod)
{
	SINT32 i, normbits, t;
	MPZ x, y, q, S[16];

	for (i = 31; i >= 0; i--)
	{
		if (exp->dat[exp->len - 1] & (1 << i))
			break;
	}
	normbits = 31 - i;

	t = exp->len * 32 - normbits;

	x.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	y.dat = (UINT32*)calloc(2 * mod->len, sizeof(UINT32));
	q.dat = (UINT32*)calloc(mod->len + 2, sizeof(UINT32));

	for (i = 0; i < 16; i++)
	{
		S[i].sign = 1;
		S[i].len = mod->len;
		S[i].dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	}

	//S[i] = input^i mod n
	S[0].sign = 1;
	S[0].len = 1;
	S[0].dat[0] = 1;

	for (i = 1; i < 16; i++)
	{
		MPZ_MUL(&y, &S[i-1], input);  //S[i-1] = g^(i-1) mod n  -> y = S[i-1]*g = g^i  ->  S[i] = g^i mod n
		MPZ_UDIV(&q, &S[i], &y, mod); 
	}


	COPY_MPZ(&x, &S[(exp->dat[((t - 1) / 4) / 8]>> ((((t - 1) / 4) % 8) * 4)) & 15]);// x <- input^e_{t-1}
	for (i = (t+3)/4 - 2; i >= 0; i--)
	{
		MPZ_SQR(&y, &x);
		MPZ_UDIV(&q, &x, &y, mod); //x <- x*x mod n
		MPZ_SQR(&y, &x);
		MPZ_UDIV(&q, &x, &y, mod); //x <- x*x mod n
		MPZ_SQR(&y, &x);
		MPZ_UDIV(&q, &x, &y, mod); //x <- x*x mod n
		MPZ_SQR(&y, &x);
		MPZ_UDIV(&q, &x, &y, mod); //x <- x*x mod n

		MPZ_MUL(&y, &x, &S[(exp->dat[i / 8]>> ((i % 8) * 4)) & 15]);
		MPZ_UDIV(&q, &x, &y, mod); //x <- x*S[i] mod n
	}
	COPY_MPZ(r, &x);// r <- x

	free(x.dat);
	free(y.dat);
	free(q.dat);
	for (i = 0; i < 16; i++)
	{
		free(S[i].dat);
	}
}


void RtoL_Exp(MPZ *r, MPZ *input, MPZ *exp, MPZ *mod)
{
	SINT32 i, normbits, t;
	MPZ A, S, y, q;

	for (i = 31; i >= 0; i--)
	{
		if (exp->dat[exp->len - 1] & (1 << i))
			break;
	}
	normbits = 31 - i;

	t = exp->len * 32 - normbits;

	A.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	S.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	y.dat = (UINT32*)calloc(2 * mod->len, sizeof(UINT32));
	q.dat = (UINT32*)calloc(mod->len + 2, sizeof(UINT32));

	A.len = 1;
	A.dat[0] = 1;
	A.sign = 1;
	COPY_MPZ(&S, input);// x <- input
	for (i = 0; i < t; i++)
	{
		if (exp->dat[i / 32] & (1 << (i % 32)))
		{
			MPZ_MUL(&y, &A, &S);
			MPZ_UDIV(&q, &A, &y, mod); 
		}
		
		MPZ_SQR(&y, &S);
		MPZ_UDIV(&q, &S, &y, mod); //S <- S*S mod n
		
	}
	COPY_MPZ(r, &A);// r <- x

	free(A.dat);
	free(S.dat);
	free(y.dat);
	free(q.dat);
}

UINT32 minus_modinv(SINT64 m, SINT64 mod)
{
	SINT64 r1, r2, r, q;
	SINT64 t1 = 0, t2 = 1, t;

	if (mod == 1)
		return 0;

	r1 = mod;
	r2 = m;

	while (r1 > 1) {
		// q is quotient 
		q = r1 / r2;
		r = r1 % r2;
		t = t1 - q * t2;
		r1 = r2;
		r2 = r;
		t1 = t2;
		t2 = t;
	}

	t1 = (-t1) % mod;
	if (t1 < 0)t1 += mod;

	return (UINT32)t1;
}

void LtoR_MONT_Exp(MPZ *r, MPZ *input, MPZ *exp, MPZ *mod)
{
	SINT32 i, normbits, t;
	SINT64 b, m;
	UINT32 m_;
	MPZ x, x_, input_, y, q, R, R2, one;

	//R = (2^32)^(mod->len)
	R.sign = 1;
	R.len = mod->len + 1;
	R.dat = (UINT32*)calloc(R.len, sizeof(UINT32));
	R.dat[R.len - 1] = 1;

	b = (UINT64)1 << 32;
	m = mod->dat[0];
	m_= minus_modinv(m, b);
	
	for (i = 31; i >= 0; i--)
	{
		if (exp->dat[exp->len - 1] & (1 << i))
			break;
	}
	normbits = 31 - i;

	t = exp->len * 32 - normbits;

	x.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	x_.dat = (UINT32*)calloc(mod->len+1, sizeof(UINT32));
	input_.dat = (UINT32*)calloc(mod->len + 1, sizeof(UINT32));
	R2.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	y.dat = (UINT32*)calloc(2 * mod->len+2, sizeof(UINT32));
	q.dat = (UINT32*)calloc(mod->len + 4, sizeof(UINT32));

	MPZ_SQR(&y, &R);
	MPZ_UDIV(&q, &R2, &y, mod); //x <- x*x mod n

	MPZ_UDIV(&q, &x, input, mod); //x <- x*x mod n
	MONT_MUL(&x_, &x, &R2, mod, m_);
	
	COPY_MPZ(&input_, &x_);

	//void MONT_MUL(MPZ *r, MPZ *x, MPZ *y, MPZ *mod, MPZ *R, UINT32 m_)
	for (i = t - 2; i >= 0; i--)
	{
		MONT_MUL(&x_, &x_, &x_, mod, m_);
		if (exp->dat[i / 32] & (1 << (i % 32)))
		{
			MONT_MUL(&x_, &x_, &input_, mod, m_);
		}
	}

	one.dat = (UINT32*)calloc(mod->len, sizeof(UINT32));
	one.len = 1;
	one.dat[0] = 1;
	one.sign = 1;

	MONT_MUL(r, &x_, &one, mod, m_);

	free(x.dat);
	free(x_.dat);
	free(input_.dat);
	free(y.dat);
	free(q.dat);
	free(R.dat);
	free(R2.dat);
	free(one.dat);
}