#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "typedef.h"

void Gen_BigNum_FILE(SINT8* buf, SINT32 bytes, SINT32 num)
{
	FILE* wfp;
	SINT32 err, i, j;

	if ((err = fopen_s(&wfp, buf, "w"))) {
		printf("error at opening file\n");
	}

	for (i = 0; i < num; i++)
	{
		for (j = 0; j < bytes; j++)
			fprintf(wfp, "%02x", rand() & 0xff);
		fprintf(wfp, "\n\n");
	}
	fclose(wfp);

}

void Read_MPZ_FILE(SINT8* rbuf, MPZ* a, SINT32 num)
{
	FILE* rfp;
	SINT32 err, i, j, k, bytes, pos;
	SINT8 buf[512], x;

	if ((err = fopen_s(&rfp, rbuf, "r"))) {
		printf("error at opening file\n");
	}

	for (i = 0; i < num; i++)
	{
		a[i].sign = 1;
		fgets(buf, 512, rfp);
		bytes = strlen(buf) / 2;
		a[i].len = (bytes + 3) / 4;              //bytes : 128 --> len : 32, (129, 130, 131, 132) --> 33
		a[i].dat = (UINT32*)calloc(a[i].len, sizeof(UINT32));
		pos = bytes * 2 - 1;
		for (j = 0; j < a[i].len; j++)
		{
			for (k = 0; k < 8; k++)
			{
				x = buf[pos--];
				if (x >= 'A' && x <= 'F')x = x - 'A' + 10;  // '0'~'9', 'A'~'F','a'~'f'
				else if (x >= 'a' && x <= 'f')x = x - 'a' + 10;
				else if (x >= '0' && x <= '9')x -= '0';
				a[i].dat[j] = (x << 28) ^ (a[i].dat[j] >> 4);
				if (pos == -1)break;
			}
		}
		k++;
		for (; k < 8; k++)
			a[i].dat[a[i].len - 1] >>= 4;

		for (k = a[i].len - 1; k >= 0; k--)
			if (a[i].dat[k] != 0)
				break;

		a[i].len = k + 1;

		fgets(buf, 512, rfp);
	}
	fclose(rfp);
}

void Write_MPZ_FILE(SINT8* wbuf, MPZ* a, SINT32 num)
{
	FILE* wfp;
	int err, i, j;

	if ((err = fopen_s(&wfp, wbuf, "w"))) {
		printf("error!!\n");
	}

	for (i = 0; i < num; i++)
	{
		for (j = a[i].len - 1; j >= 0; j--) {
			fprintf(wfp, "%08x", a[i].dat[j]);
		}
		fprintf(wfp, "\n\n");
	}
	fclose(wfp);
}

void Write_MPZ_FILE2(SINT8* wbuf, MPZ* a, SINT32 num)
{
	FILE* wfp;
	int err, i, j;

	if ((err = fopen_s(&wfp, wbuf, "w"))) {
		printf("error!!\n");
	}

	for (i = 0; i < num; i++)
	{
		fprintf(wfp, "%x ", a[i].sign);
		for (j = a[i].len - 1; j >= 0; j--) {
			fprintf(wfp, "%08x", a[i].dat[j]);
		}
		fprintf(wfp, "\n\n");
	}
	fclose(wfp);
}

void Write_MPZ_FILE3(SINT8* wbuf, MPZ* a, MPZ* b, SINT32 num)
{
	FILE* wfp;
	int err, i, j;

	if ((err = fopen_s(&wfp, wbuf, "w"))) {
		printf("error!!\n");
	}

	for (i = 0; i < num; i++)
	{
		fprintf(wfp, "Bignum A : ");
		fprintf(wfp, "%x ", a[i].sign);
		for (j = a[i].len - 1; j >= 0; j--) {
			fprintf(wfp, "%08x", a[i].dat[j]);
		}
		fprintf(wfp, "\nBignum B : ");
		fprintf(wfp, "%x ", b[i].sign);
		for (j = b[i].len - 1; j >= 0; j--) {
			fprintf(wfp, "%08x", b[i].dat[j]);
		}

		fprintf(wfp, "\n\n");
	}
	fclose(wfp);
}

int Comp_MPZ(MPZ* a, MPZ* b)  // if a>=b, return 1, else 0.  //a->sign = b->sign = 1�� �� ����...
{
	int i;

	if (a->len > b->len) return 1;
	else if (a->len < b->len) return 0;
	else
	{
		for (i = a->len - 1; i >= 0; i--)
		{
			if (a->dat[i] > b->dat[i])return 1;
			else if (a->dat[i] < b->dat[i])return 0;
		}
		return 1;
	}
}

void COPY_MPZ(MPZ* c, MPZ* a)
{
	SINT32 i;
	c->sign = a->sign;
	c->len = a->len;
	for (i = 0; i < a->len; i++) {
		c->dat[i] = a->dat[i];
	}
}


void Swap_MPZ(MPZ* a, MPZ* b)
{
	int i;
	MPZ tmp;

	tmp.dat = (UINT32*)calloc(a->len, sizeof(UINT32));

	tmp.sign = a->sign;
	tmp.len = a->len;
	for (i = 0; i < a->len; i++)
		tmp.dat[i] = a->dat[i];

	a->sign = b->sign;
	a->len = b->len;
	for (i = 0; i < b->len; i++)
		a->dat[i] = b->dat[i];

	b->sign = tmp.sign;
	b->len = tmp.len;
	for (i = 0; i < tmp.len; i++)
		b->dat[i] = tmp.dat[i];

	free(tmp.dat);
}

void Sort_MPZ_FILE(SINT8* rbuf, SINT8* wbuf, SINT32 num)
{
	SINT32 i, j;
	MPZ* a;

	a = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_FILE(rbuf, a, num);

	for (i = 0; i < num - 1; i++)
		for (j = i + 1; j < num; j++)
		{
			if (Comp_MPZ(&a[i], &a[j]))Swap_MPZ(&a[i], &a[j]);
		}

	Write_MPZ_FILE(wbuf, a, num);

	free(a);

}

void MPZ_UADD(MPZ* c, MPZ* a, MPZ* b)  //a>=0, b>=0
{
	SINT32 i, maxlen, minlen;
	SINT8 carry = 0;
	UINT32 temp = 0;

	if ((a->sign == 0) && (b->sign == 0))
		c->sign = 0;
	else if ((a->sign == 0) || (b->sign == 0))
	{
		if (a->sign == 0)
			COPY_MPZ(c, b);
		else
			COPY_MPZ(c, a);
	}
	else
	{
		c->sign = 1;
		if (a->len >= b->len) {
			for (i = 0; i < b->len; i++)
			{
				if ((temp = a->dat[i] + carry) < carry) // ai+bi+carry --> ai+carry = 100000000 --> carry = 1, temp 0
					temp = b->dat[i]; // ai+bi+carry  --> carry =1 temp = bi
				else if ((temp += b->dat[i]) < b->dat[i])
					carry = 1;
				else
					carry = 0;
				c->dat[i] = temp;
				//c->dat[i] = a->dat[i] + b->dat[i] + carry;
				// carry = 1 or 0?
			}
			for (i = b->len; i < a->len; i++)
			{
				if ((temp = a->dat[i] + carry) >= carry) // ai+bi+carry --> ai+carry = 100000000 --> carry = 1, temp 0
					carry = 0;
				c->dat[i] = temp;
				//c->dat[i] = a->dat[i] + b->dat[i] + carry;
				// carry = 1 or 0?
			}
			if (carry == 1)
			{
				c->len = a->len + 1;
				c->dat[c->len - 1] = 1;
			}
			else
				c->len = a->len;
		}
		else
		{
			for (i = 0; i < a->len; i++)
			{
				if ((temp = b->dat[i] + carry) < carry) // ai+bi+carry --> ai+carry = 100000000 --> carry = 1, temp 0
					temp = a->dat[i]; // ai+bi+carry  --> carry =1 temp = bi
				else if ((temp += a->dat[i]) < a->dat[i])
					carry = 1;
				else
					carry = 0;
				c->dat[i] = temp;
				//c->dat[i] = a->dat[i] + b->dat[i] + carry;
				// carry = 1 or 0?
			}
			for (i = a->len; i < b->len; i++)
			{
				if ((temp = b->dat[i] + carry) >= carry) // ai+bi+carry --> ai+carry = 100000000 --> carry = 1, temp 0
					carry = 0;
				c->dat[i] = temp;
				//c->dat[i] = a->dat[i] + b->dat[i] + carry;
				// carry = 1 or 0?
			}
			if (carry == 1)
			{
				c->len = b->len + 1;
				c->dat[c->len - 1] = 1;
			}
			else
				c->len = b->len;
		}
	}
}




void MPZ_USUB(MPZ* c, MPZ* a, MPZ* b)  
{
	SINT32 i;
	SINT8 borrow = 0;
	UINT32 temp;

	if (Comp_MPZ(a, b) == 1)  //a-b
	{
		c->sign = 1;
		for (i = 0; i < b->len; i++)  //ci=ai-bi-borrow = (ai-borrow)-bi
		{
			if (a->dat[i] >= borrow)
			{
				temp = a->dat[i] - borrow;  
				if ((c->dat[i] = temp - b->dat[i]) > temp)
					borrow = 1;
				else
					borrow = 0;
			}
			else  //a->dat[i] = 0, borrow =1
				c->dat[i] = 0xFFFFFFFF - b->dat[i]; //borrow = 1;
		}
		for (i = b->len; i < a->len; i++)
		{
			if (a->dat[i] >= borrow)
			{
				c->dat[i] = a->dat[i] - borrow;
				borrow = 0;
			}
			else  //a->dat[i] = 0, borrow =1
				c->dat[i] = 0xFFFFFFFF; //borrow = 1;
		}
		c->len = a->len;
		for (i = c->len - 1; i >= 0; i--)
		{
			if (c->dat[i] == 0)
				c->len--;
			else
				break;
		}
		if (c->len == 0)c->sign = 0;

	}
	else   //a-b = -(b-a)
	{
		c->sign = -1;
		for (i = 0; i < a->len; i++)  //ci=bi-ai-borrow = (bi-borrow)-ai
		{
			if (b->dat[i] >= borrow)
			{
				temp = b->dat[i] - borrow;
				if ((c->dat[i] = temp - a->dat[i]) > temp)
					borrow = 1;
				else
					borrow = 0;
			}
			else  //b->dat[i] = 0, borrow =1
				c->dat[i] = 0xFFFFFFFF - a->dat[i]; //borrow = 1;
		}
		for (i = a->len; i < b->len; i++)
		{
			if (b->dat[i] >= borrow)
			{
				c->dat[i] = b->dat[i] - borrow;
				borrow = 0;
			}
			else  //b->dat[i] = 0, borrow =1
				c->dat[i] = 0xFFFFFFFF; //borrow = 1;
		}
		c->len = b->len;
		for (i = c->len - 1; i >= 0; i--)
		{
			if (c->dat[i] == 0)
				c->len--;
			else
				break;
		}
	}

}

void MPZ_UMUL(MPZ* c, MPZ* a, MPZ* b)  //c = a*b , a>=0, b>=0
{
	SINT32 i, j;
	UINT32 U, V;
	UINT64 UV;

	if ((a->sign == 0) || (b->sign == 0))
		c->sign = 0;
	else
	{
		for (i = 0; i < b->len; i++)
			c->dat[i] = 0;
		for (i = 0; i < a->len; i++)
		{
			U = 0;
			for (j = 0; j < b->len; j++)
			{
				UV = c->dat[i + j] + (UINT64)a->dat[i] * b->dat[j] + U;
				U = UV >> 32;
				V = UV & 0xffffffff;
				c->dat[i + j] = V;
			}
			c->dat[i + b->len] = U;
		}

		c->len = a->len + b->len;
		for (i = c->len - 1; i >= 0; i--)
		{
			if (c->dat[i] == 0)
				c->len--;
			else
				break;
		}
	}
}

void MPZ_MUL(MPZ* c, MPZ* a, MPZ* b)  //c = a*b
{
	if ((a->sign == 0 || b->sign == 0))
		c->sign = 0;
	else if ((a->sign == 1 && b->sign == 1) || (a->sign == -1 && b->sign == -1))
	{
		c->sign == 1;
		MPZ_UMUL(c, a, b);
	}
	else
	{
		c->sign == -1;
		MPZ_UMUL(c, a, b);
	}
}

void MPZ_SQR(MPZ* c, MPZ* a)  //c = a^2
{
	UINT32 R0, R1, R2, U, V, temp;
	UINT8 carry;
	SINT32 i, j, k;
	UINT64 UV;

	if (a->sign == 0)
		c->sign = 0;
	else
	{
		c->sign = 1;

		R0 = 0;
		R1 = 0;
		R2 = 0;
		for (k = 0; k <= (2 * a->len - 2); k++)
		{
			for (i = 0; i <= k / 2; i++)  //0<=i<=j<=t-1,  j = k-i
			{
				j = k - i;
				if (j < a->len)
				{
					UV = (UINT64)a->dat[i] * a->dat[j];
					if (j > i)
					{
						carry = UV >> 63;
						UV <<= 1;
						R2 = R2 + carry;
					}
					temp = UV & 0xFFFFFFFF;
					R0 += temp; //r0 = r0+temp
					if (R0 < temp)carry = 1;
					else carry = 0;

					temp = UV >> 32;
					R1 += carry;
					if (R1 < carry) R1 = temp;  //r1=r1+carry+U
					else if ((R1 += temp) < temp)carry = 1;
					else carry = 0;

					R2 = R2 + carry;
				}

			}
			c->dat[k] = R0;
			R0 = R1;
			R1 = R2;
			R2 = 0;
		}
		c->dat[2 * a->len - 1] = R0;

		c->len = 2 * a->len;
		for (i = c->len - 1; i >= 0; i--)
		{
			if (c->dat[i] == 0)
				c->len--;
			else
				break;
		}
	}

}


void MPZ_ADD(MPZ* c, MPZ* a, MPZ* b)
{
	if ((a->sign == 0) || (b->sign == 0))
	{
		if (a->sign == 0)
			COPY_MPZ(c, b);
		else
			COPY_MPZ(c, a);
	}
	else if (a->sign == 1 && b->sign == 1)
	{
		c->sign = 1;
		MPZ_UADD(c, a, b);
	}
	else if (a->sign == 1 && b->sign == -1)
	{
		MPZ_USUB(c, a, b);
	}
	else if (a->sign == -1 && b->sign == 1)
	{
		MPZ_USUB(c, b, a);
	}
	else
	{
		// -a-b=-(a+b)
		MPZ_UADD(c, a, b);
		c->sign = -1;
	}
}

void MPZ_MOD_ADD(MPZ* c, MPZ* a, MPZ* b, MPZ* modulus) // 0<= a, b < n
{
	MPZ_UADD(c, a, b);
	if (Comp_MPZ(c, modulus) == 1)MPZ_USUB(c, c, modulus);
}

void MPZ_MOD_SUB(MPZ* c, MPZ* a, MPZ* b, MPZ* modulus) // 0<= a, b < n
{
	MPZ_USUB(c, a, b);
	if (c->sign == -1)MPZ_ADD(c, c, modulus);
}

void MPZ_SUB(MPZ* c, MPZ* a, MPZ* b)
{
	if ((a->sign == 0) || (b->sign == 0))
	{
		if (b->sign == 0)
			COPY_MPZ(c, a);
		else
		{
			COPY_MPZ(c, b);
			c->sign = -c->sign;
		}
	}
	if (a->sign == 1 && b->sign == 1)
	{
		MPZ_USUB(c, a, b);
	}
	else if (a->sign == 1 && b->sign == -1) //a-b
	{
		MPZ_UADD(c, a, b);
		c->sign = 1;
	}
	else if (a->sign == -1 && b->sign == 1) //a-b = -(b-a)
	{
		MPZ_UADD(c, a, b);
		c->sign = -1;
	}
	else
	{
		MPZ_USUB(c, b, a);  // a-b = (-b)-(-a)  b=-data2, a=-data1 ==> b-a= data1-data2
		c->sign = -c->sign;
	}
}

void MPZ_BIT_SHIFT(MPZ* r, MPZ* a, SINT32 shift)
{
	SINT32 i, j;

	if (shift == 0) {
		COPY_MPZ(r, a);
	}
	else if (shift > 0)
	{
		for (i = 31; i > 0; i--) 
		{
			if (a->dat[a->len - 1] & (1 << i))
				break;
		}
		if ((i + shift) >= 32) r->len = a->len + 1;
		else r->len = a->len;

		r->dat[0] = a->dat[0] << shift;
		for (j = 1; j < a->len; j++)
		{
			r->dat[j] = (a->dat[j] << shift) | (a->dat[j - 1] >> (32 - shift));
		}

		if ((i + shift) >= 32) r->dat[r->len - 1] = a->dat[a->len - 1] >> (32 - shift);
	}
	else
	{
		for (i = 31; i > 0; i--)
		{
			if (a->dat[a->len - 1] & (1 << i))
				break;
		}
		if (i <= -shift) r->len = a->len - 1;
		else r->len = a->len;

		for (j = 0; j < a->len - 1; j++)
		{
			r->dat[j] = (a->dat[j] >> -shift) | (a->dat[j+1] << (32+shift));
		}
		if (r->len == a->len) r->dat[r->len - 1] = (a->dat[a->len - 1] >> -shift);

	}
}

void MPZ_WORD_SHIFT(MPZ* r, MPZ* a, SINT32 shift)
{
	SINT32 i, j;
	r->sign = a->sign;
	r->len = a->len + shift;

	for (i = 0; i < a->len; i++) {
		r->dat[i + shift] = a->dat[i];
	}

	for (i = 0; i < shift; i++) {
		r->dat[i] = 0;
	}
}

void MUL_WORD_MPZ(MPZ* r, UINT32 c, MPZ* a) 
{
	SINT32 i;
	UINT32 U, V;
	UINT64 UV;

	if (c == 0)
	{
		r->sign = 0;
		r->len = 0;
	}
	else
	{
		r->sign = a->sign;
		U = 0;
		for (i = 0; i < a->len; i++)
		{
			UV = (UINT64)a->dat[i] * c + U;
			U = UV >> 32;
			r->dat[i] = UV & 0xffffffff;
		}
		if (U != 0)
		{
			r->dat[a->len] = U;
			r->len = a->len + 1;
		}
		else
			r->len = a->len;
	}

	

}

void MPZ_UDIV(MPZ* q, MPZ* r, MPZ* a, MPZ* b) //a= bq+r, a, b>0
{
	SINT32 i, normalbits, m;
	UINT64 x_, y_;
	UINT32 x0, x1, x2;
	MPZ x, y, tmp, tmp2;

	if (Comp_MPZ(a, b) == 0)
	{
		q->sign = 0;
		COPY_MPZ(r, a);
	}
	else
	{
		//beginning of normalization
		for (i = 31; i >= 0; i--)
		{
			if (b->dat[b->len - 1] & (1 << i))
				break;
		}
		normalbits = 31 - i;

		x.dat = (UINT32*)calloc(a->len + 1, sizeof(UINT32));
		y.dat = (UINT32*)calloc(b->len + 1, sizeof(UINT32));

		MPZ_BIT_SHIFT(&x, a, normalbits);
		MPZ_BIT_SHIFT(&y, b, normalbits);

		m = x.len - y.len;

		for (i = 0; i < m; i++) //step2
		{
			q->dat[i] = 0;
		}
		q->len = m + 1;
	
		tmp.dat = (UINT32*)calloc(x.len, sizeof(UINT32));
		tmp2.dat = (UINT32*)calloc(x.len+1, sizeof(UINT32));

		MPZ_WORD_SHIFT(&tmp, &y, m);
		//end of normalization

		while (Comp_MPZ(&x, &tmp) == 1)
		{
			q->dat[m]++;
			MPZ_USUB(&x, &x, &tmp);
		
		}

		for (i = x.len - 1; i >= y.len; i--) {
			if (x.dat[i] == y.dat[y.len - 1])
				q->dat[i - y.len] = 0xffffffff;
			else
				q->dat[i - y.len] = ((UINT64)x.dat[i] << 32 | x.dat[i - 1]) / y.dat[y.len - 1]; //step 3.1

			for (;;)
			{
				x_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 2];
				y_ = (UINT64)q->dat[i - y.len] * y.dat[y.len - 1];
				x0 = x_ & 0xffffffff;
				y_ += (x_ >> 32);
				x1 = y_ & 0xffffffff;
				x2 = y_ >> 32;
				if (x2 < x.dat[i]) break;
				if (x2 == x.dat[i])
				{
					if (x1 < x.dat[i - 1]) break;
					if (x1 == x.dat[i - 1])
					{
						if (x0 <= x.dat[i - 2]) break;
					}
				}

				q->dat[i - y.len]--;  //step 3.2

			}

			MPZ_WORD_SHIFT(&tmp, &y, i - y.len);
			MUL_WORD_MPZ(&tmp2, q->dat[i - y.len], &tmp);
			MPZ_USUB(&x, &x, &tmp2); //stemp 3.3

			if (x.sign == -1)
			{
				MPZ_ADD(&x, &x, &tmp);
				q->dat[i - y.len]--;
			}
		}
		for (i = q->len-1; i >= 0; i--)
		{
			if (q->dat[i] == 0)
				q->len--;
			else
				break;
		}
		MPZ_BIT_SHIFT(r, &x, -normalbits);

	}

	free(x.dat);
	free(y.dat);
	free(tmp.dat);
	free(tmp2.dat);
}


void MPZ_ADD_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_FILE(rbuf1, a, num);
	Read_MPZ_FILE(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		j = (a[i].len > b[i].len) ? (a[i].len + 1) : (b[i].len + 1);
		c[i].dat = (UINT32*)calloc(j, sizeof(UINT32));
		MPZ_ADD(&c[i], &a[i], &b[i]);
	}

	Write_MPZ_FILE(wbuf, c, num);

	free(a);
	free(b);
	free(c);
}

void MPZ_USUB_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_FILE(rbuf1, a, num);
	Read_MPZ_FILE(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		j = (a[i].len > b[i].len) ? (a[i].len + 1) : (b[i].len + 1);
		c[i].dat = (UINT32*)calloc(j, sizeof(UINT32));
		MPZ_USUB(&c[i], &a[i], &b[i]);
	}

	Write_MPZ_FILE2(wbuf, c, num);

	free(a);
	free(b);
	free(c);

}


void MPZ_UMUL_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b, * c;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_FILE(rbuf1, a, num);
	Read_MPZ_FILE(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		j = a[i].len + b[i].len;
		c[i].sign = 1;
		c[i].dat = (UINT32*)calloc(j, sizeof(UINT32));
		MPZ_UMUL(&c[i], &a[i], &b[i]);
	}

	Write_MPZ_FILE2(wbuf, c, num);

	free(a);
	free(b);
	free(c);
}

void MPZ_SQR_TEST(SINT8* rbuf1, SINT8* wbuf, SINT32 num)
{
	MPZ* a, * b;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_FILE(rbuf1, a, num);

	for (i = 0; i < num; i++)
	{
		j = 2 * a[i].len;
		b[i].dat = (UINT32*)calloc(j, sizeof(UINT32));
		MPZ_SQR(&b[i], &a[i]);
	}

	Write_MPZ_FILE2(wbuf, b, num);

	free(a);
	free(b);
	
}

void MPZ_UDIV_TEST(SINT8* rbuf1, SINT8* rbuf2, SINT8* wbuf, SINT32 num) {

	MPZ* a, * b, * c, *d;
	SINT32 i, j;

	a = (MPZ*)malloc(sizeof(MPZ) * num);
	b = (MPZ*)malloc(sizeof(MPZ) * num);
	c = (MPZ*)malloc(sizeof(MPZ) * num);
	d = (MPZ*)malloc(sizeof(MPZ) * num);

	Read_MPZ_FILE(rbuf1, a, num);
	Read_MPZ_FILE(rbuf2, b, num);

	for (i = 0; i < num; i++)
	{
		c[i].sign = d[i].sign = 1;
		c[i].dat = (UINT32*)calloc(a[i].len-b[i].len+1, sizeof(UINT32));
		d[i].dat = (UINT32*)calloc(b[i].len, sizeof(UINT32));
		MPZ_UDIV(&c[i], &d[i], &a[i], &b[i]);
	}

	Write_MPZ_FILE3(wbuf, c, d, num);

	free(a);
	free(b);
	free(c);
	free(d);
}