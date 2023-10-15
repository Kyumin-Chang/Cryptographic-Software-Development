#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "typedef.h"

void MPZ_LtoR_EXP_TEST(SINT8 *rbuf1, SINT8 *rbuf2, SINT8 *rbuf3, SINT8 *wbuf,  SINT32 num)
{
	MPZ *g, *e, *mod, *r;
	SINT32 i, j;

	g = (MPZ*)malloc(sizeof(MPZ)*num);
	e = (MPZ*)malloc(sizeof(MPZ)*num);
	mod = (MPZ*)malloc(sizeof(MPZ)*num);
	r = (MPZ*)malloc(sizeof(MPZ)*num);

	Read_MPZ_FILE(rbuf1, g, num);
	Read_MPZ_FILE(rbuf2, e, num);
	Read_MPZ_FILE(rbuf3, mod, num);

	for (i = 0; i < num; i++)
	{
		r[i].dat = (UINT32*)calloc(mod[i].len, sizeof(UINT32));
		r[i].sign = 1;
		LtoR_Exp(&r[i], &g[i], &e[i], &mod[i]);
	}

	Write_MPZ_FILE(wbuf, r, num);

	for (i = 0; i < num; i++)
	{
		free(r[i].dat);
	}

	free(g);
	free(e);
	free(mod);
	free(r);

}

void MPZ_LtoR_MONT_EXP_TEST(SINT8 *rbuf1, SINT8 *rbuf2, SINT8 *rbuf3, SINT8 *wbuf, SINT32 num)
{
	MPZ *g, *e, *mod, *r;
	SINT32 i, j;

	g = (MPZ*)malloc(sizeof(MPZ)*num);
	e = (MPZ*)malloc(sizeof(MPZ)*num);
	mod = (MPZ*)malloc(sizeof(MPZ)*num);
	r = (MPZ*)malloc(sizeof(MPZ)*num);

	Read_MPZ_FILE(rbuf1, g, num);
	Read_MPZ_FILE(rbuf2, e, num);
	Read_MPZ_FILE(rbuf3, mod, num);

	for (i = 0; i < num; i++)
	{
		r[i].dat = (UINT32*)calloc(mod[i].len, sizeof(UINT32));
		r[i].sign = 1;
		LtoR_MONT_Exp(&r[i], &g[i], &e[i], &mod[i]);
	}

	Write_MPZ_FILE(wbuf, r, num);

	for (i = 0; i < num; i++)
	{
		free(r[i].dat);
	}

	free(g);
	free(e);
	free(mod);
	free(r);

}

void MPZ_LtoR_Kary_EXP_TEST(SINT8 *rbuf1, SINT8 *rbuf2, SINT8 *rbuf3, SINT8 *wbuf, SINT32 num)
{
	MPZ *g, *e, *mod, *r;
	SINT32 i, j;

	g = (MPZ*)malloc(sizeof(MPZ)*num);
	e = (MPZ*)malloc(sizeof(MPZ)*num);
	mod = (MPZ*)malloc(sizeof(MPZ)*num);
	r = (MPZ*)malloc(sizeof(MPZ)*num);

	Read_MPZ_FILE(rbuf1, g, num);
	Read_MPZ_FILE(rbuf2, e, num);
	Read_MPZ_FILE(rbuf3, mod, num);

	for (i = 0; i < num; i++)
	{
		r[i].dat = (UINT32*)calloc(mod[i].len, sizeof(UINT32));
		r[i].sign = 1;
		LtoR_Kary_Exp(&r[i], &g[i], &e[i], &mod[i]);
	}

	Write_MPZ_FILE(wbuf, r, num);

	for (i = 0; i < num; i++)
	{
		free(r[i].dat);
	}

	free(g);
	free(e);
	free(mod);
	free(r);

}

void MPZ_RtoL_EXP_TEST(SINT8 *rbuf1, SINT8 *rbuf2, SINT8 *rbuf3, SINT8 *wbuf, SINT32 num)
{
	MPZ *g, *e, *mod, *r;
	SINT32 i, j;

	g = (MPZ*)malloc(sizeof(MPZ)*num);
	e = (MPZ*)malloc(sizeof(MPZ)*num);
	mod = (MPZ*)malloc(sizeof(MPZ)*num);
	r = (MPZ*)malloc(sizeof(MPZ)*num);

	Read_MPZ_FILE(rbuf1, g, num);
	Read_MPZ_FILE(rbuf2, e, num);
	Read_MPZ_FILE(rbuf3, mod, num);

	for (i = 0; i < num; i++)
	{
		r[i].dat = (UINT32*)calloc(mod[i].len, sizeof(UINT32));
		r[i].sign = 1;
		RtoL_Exp(&r[i], &g[i], &e[i], &mod[i]);
	}

	Write_MPZ_FILE(wbuf, r, num);

	for (i = 0; i < num; i++)
	{
		free(r[i].dat);
	}

	free(g);
	free(e);
	free(mod);
	free(r);

}


int main()
{
	LARGE_INTEGER Frequency;
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER Endtime;
	__int64 elapsed;
	double duringtime;
	UINT32 temp;
	UINT64 temp1;
	int b;

	MPZ *a;

	srand(time(NULL));

	a = (MPZ*)malloc(sizeof(MPZ) * 1000);

	QueryPerformanceFrequency(&Frequency);
	
	QueryPerformanceCounter(&BeginTime);

	//Gen_BigNum_FILE("rand_A.txt", 128, 1000);
	//Gen_BigNum_FILE("rand_B.txt", 64, 1000);

	//Read_MPZ_FILE("rand_A.txt", a, 1000);
	
	//Sort_MPZ_FILE("rand_A.txt", "Sorted_A.txt", 1000);

	//MPZ_ADD_TEST("rand_A.txt", "rand_B.txt", "rand_C.txt", 1000);
	//MPZ_USUB_TEST("rand_A.txt", "rand_B.txt", "USUB_result.txt", 1000);
	//MPZ_UMUL_TEST("rand_A.txt", "rand_B.txt", "UMUL_result.txt", 1000);
	//MPZ_SQR_TEST("rand_A.txt", "SQR_result.txt", 1000);
	//MPZ_UDIV_TEST("rand_A.txt", "rand_B.txt", "Quotient_result.txt", "Remainder_result.txt", 1000);

	//Gen_BigNum_FILE("g.txt", 128, 1000);
	//Gen_BigNum_FILE("e.txt", 128, 1000);
	//Gen_BigNum_FILE("mod.txt", 128, 1000);
	//Gen_BigNum_Odd_FILE("mod.txt", 128, 1000);

	//MPZ_LtoR_EXP_TEST("g.txt", "e.txt", "mod.txt", "r.txt", 1000);
	//MPZ_RtoL_EXP_TEST("g.txt", "e.txt", "mod.txt", "r1.txt", 1000);
	//MPZ_LtoR_Kary_EXP_TEST("g.txt", "e.txt", "mod.txt", "r2.txt", 1000);
	MPZ_LtoR_MONT_EXP_TEST("g.txt", "e.txt", "mod.txt", "r3.txt", 1000);

	QueryPerformanceCounter(&Endtime);
	elapsed = Endtime.QuadPart - BeginTime.QuadPart;
	duringtime = (double)elapsed / (double)Frequency.QuadPart;
	printf("elapsed time: %lf\n", duringtime);

	free(a);

	return 0;
}