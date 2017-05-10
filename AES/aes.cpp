﻿#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>

using namespace std;

// func
void show(const uint8_t *print, int len);
void expandkey(uint8_t *key, uint8_t *w);					/* 密钥扩展		*/

void gfunc(uint8_t *w, int j);								/* g函数		*/
void leftshift(uint8_t *w, int np);							/* 字节循环左移 */
void sub_word(uint8_t *w);									/* S盒映射		*/
void add(uint8_t *w, int j);								/* 矩阵相加		*/

void subbytes(uint8_t *);									/* 字节替代		*/
void inv_subbytes(uint8_t *);								/* 逆字节替代	*/


uint8_t gmult(uint8_t, uint8_t);
void mult(uint8_t *w, uint8_t *a, uint8_t *out);			/* 矩阵相乘		*/

void addroundkey(uint8_t *state, const uint8_t *w, uint8_t r);	/* 轮密钥相加	*/
void shiftrows(uint8_t *in);								/* 行移位		*/
void inv_shiftrows(uint8_t *);								/* 逆向行移位	*/

void mixcolumns(uint8_t *in);								/* 列混淆		*/
void inv_mixcolumns(uint8_t *);								/* 逆列混淆		*/

void ace(uint8_t *in, uint8_t *out, const uint8_t *w);			/* ACE加密		*/
void inv_ace(uint8_t *in, uint8_t *out, const uint8_t *w);		/* inv_ACE解密	*/
																// func

																/* s_box */
static uint8_t s_box[256] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, // 0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, // 1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, // 2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, // 3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, // 4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, // 5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, // 6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, // 7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, // 8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, // 9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, // a
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, // b
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, // c
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, // d
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, // e
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16  // f
};

/* inv_s_box */
static uint8_t inv_s_box[256] = {
	// 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, // 0
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, // 1
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, // 2
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, // 3
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, // 4
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, // 5
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, // 6
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, // 7
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, // 8
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, // 9
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, // a
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, // b
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, // c
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, // d
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, // e
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d  // f
};

/* RC */
static uint8_t Rc[4] = { 0x00, 0x00, 0x00,0x00 };

/* key */
uint8_t key[16] = {
	0x22, 0x23, 0x56, 0x9f,
	0x31, 0xac, 0xaa, 0xdd,
	0xca, 0x13, 0x10, 0x00,
	0xff, 0xcc, 0x77, 0xbe
};

/* 秘钥扩展 */
void expandkey(uint8_t *key, uint8_t *w)
{
	int wlen = 0;
	wlen = 4 * (10 + 1);

	// w[0] ~ w[3] 32bit 字长
	for (int j = 0;j < 4;j++) {
		w[4 * j + 0] = key[4 * j + 0];
		w[4 * j + 1] = key[4 * j + 1];
		w[4 * j + 2] = key[4 * j + 2];
		w[4 * j + 3] = key[4 * j + 3];
		//printf("%-7d: ", j); show(w, 22);
	}

	uint8_t gtemp[4] = { 0x00 };
	for (int j = 4; j < wlen; j++) {
		gtemp[0] = w[4 * (j - 1) + 0];
		gtemp[1] = w[4 * (j - 1) + 1];
		gtemp[2] = w[4 * (j - 1) + 2];
		gtemp[3] = w[4 * (j - 1) + 3];

		if (j % 4 == 0) {
			gfunc(gtemp, j);
		}
		else { ; }

		w[4 * j + 0] = w[4 * (j - 4) + 0] ^ gtemp[0];
		w[4 * j + 1] = w[4 * (j - 4) + 1] ^ gtemp[1];
		w[4 * j + 2] = w[4 * (j - 4) + 2] ^ gtemp[2];
		w[4 * j + 3] = w[4 * (j - 4) + 3] ^ gtemp[3];
		//printf("%-7d: ", j); show(w, 22);
	}
	//	

}

/* gfunc */
void gfunc(uint8_t *w, int j)
{
	/* 将w循环左移一个字节 */
	leftshift(w, 1);

	/* 分别对每一个字节按S盒进行映射 */
	sub_word(w);

	/*  与32 bits的常量（RC[j/4],0,0,0）进行异或 */
	add(w, j);
}

/* 字节循环左移 */
void leftshift(uint8_t *w, int np)
{
	uint8_t temp;

	for (int n = 0; n < np;n++) {
		temp = w[0];
		for (int i = 0;i < 3;i++)
		{
			w[i] = w[i + 1];
		}

		w[3] = temp;
	}
}

/* 密钥每个字节进行S盒映射 */
void sub_word(uint8_t *w) {

	int p = 0;

	for (int i = 0; i < 4; i++) {
		p = 16 * ((w[i] & 0xf0) >> 4) + (w[i] & 0x0f);
		w[i] = s_box[p];
	}
}

/* add Rc异或 4bytes + 4bytes */
void add(uint8_t *w, int j)
{
	j = j / 4;
	if (j == 1) {
		Rc[0] = 0x01; // x^(1-1) = x^0 = 1
	}
	else if (j > 1) {
		Rc[0] = 0x02;
		j--;
		while (j - 1 > 0) {
			Rc[0] = gmult(Rc[0], 0x02);
			j--;
		}
	}

	for (int i = 0;i < 4;i++)
	{
		w[i] = w[i] ^ Rc[i];
	}
}

/* SubBytes 字节替代 */
void subbytes(uint8_t *w)
{
	/* s position */
	int row, col = 0;
	int p = 0;
	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			/*
			* case:0x3c
			*     0x3c & 0xf0 = 0x30, 0x30 >> 4 = 0x03
			*     0x3c & 0x0f = 0x0c
			*/
			row = (w[4 * i + j] & 0xf0) >> 4;
			col = w[4 * i + j] & 0x0f;
			p = 16 * row + col;
			w[4 * i + j] = s_box[p];
		}
	}
}

/* Inv_SubBytes 逆向字节替代 */
void inv_subbytes(uint8_t *w)
{
	/* s position */
	int row, col = 0;
	int p = 0;
	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			/*
			* case:0x3c
			*     0x3c & 0xf0 = 0x30, 0x30 >> 4 = 0x03
			*     0x3c & 0x0f = 0x0c
			*/
			row = (w[4 * i + j] & 0xf0) >> 4;
			col = w[4 * i + j] & 0x0f;
			p = 16 * row + col;
			w[4 * i + j] = inv_s_box[p];
		}
	}
}


/* gmult 辅助mult进行矩阵乘法运算 */
uint8_t gmult(uint8_t a, uint8_t b) {

	uint8_t p = 0, i = 0, hbs = 0;

	for (i = 0; i < 8; i++) {
		if (b & 1) {
			p ^= a;
		}

		hbs = a & 0x80;
		a <<= 1;
		if (hbs) a ^= 0x1b; // 0000 0001 0001 1011	
		b >>= 1;
	}

	return (uint8_t)p;
}

/* mult 乘法 4bytes * 4bytes*/
void mult(uint8_t *w, uint8_t *a, uint8_t *out)
{
	out[0] = gmult(w[0], a[0]) ^ gmult(w[3], a[1]) ^ gmult(w[2], a[2]) ^ gmult(w[1], a[3]);
	out[1] = gmult(w[1], a[0]) ^ gmult(w[0], a[1]) ^ gmult(w[3], a[2]) ^ gmult(w[2], a[3]);
	out[2] = gmult(w[2], a[0]) ^ gmult(w[1], a[1]) ^ gmult(w[0], a[2]) ^ gmult(w[3], a[3]);
	out[3] = gmult(w[3], a[0]) ^ gmult(w[2], a[1]) ^ gmult(w[1], a[2]) ^ gmult(w[0], a[3]);
}

/* ShiftRows 行移位 */
void shiftrows(uint8_t *in)
{
	uint8_t *temp[4];
	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			temp[j] = &in[4 * i + j];
		}
		leftshift(*temp, i);
	}
}

/* inv_ShiftRows 逆向行移位 */
void inv_shiftrows(uint8_t *in)
{
	uint8_t *temp[4];
	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			temp[j] = &in[4 * i + j];
		}
		// 右移i位，相当于左移 4 - i位
		leftshift(*temp, 4 - i);
	}
}

/* MixColumns 列混淆 */
void mixcolumns(uint8_t *in)
{
	// 混合向量
	/*
	* 0x22  0x03  0x01  0x01
	* 0x11  0x02  0x03  0x01
	* 0x01  0x01  0x02  0x03
	* 0x03  0x01  0x01  0x02
	*/
	uint8_t arr[4] = { 0x02, 0x01, 0x01, 0x03 };

	/* 列向量 列 */
	uint8_t cols[4], out[4] = { 0x00 };

	for (int i = 0; i < 4;i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cols[j] = in[4 * j + i];
		}

		mult(arr, cols, out);

		for (int k = 0;k < 4;k++)
		{
			in[4 * k + i] = out[k];
		}

	}

}

/* InvMixColumns 逆向列混淆 */
void inv_mixcolumns(uint8_t *in)
{
	// 混合向量
	/*
	* 0x0E  0x0B  0x0D  0x09
	* 0x09  0x0E  0x0B  0x0D
	* 0x0D  0x09  0x0E  0x0B
	* 0x0B  0x0D  0x09  0x0E
	*/
	uint8_t inv_arr[4] = { 0x0e, 0x09, 0x0d, 0x0b };

	/* 列向量 列 */
	uint8_t cols[4], out[4] = { 0x00 };

	for (int i = 0; i < 4;i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cols[j] = in[4 * j + i];
		}

		mult(inv_arr, cols, out);

		for (int k = 0;k < 4;k++)
		{
			in[4 * k + i] = out[k];
		}

	}

}

/*
* AddRoundKey 轮秘钥加
*
* state: 待加密字节 w: 秘钥  r: 轮数
*/
void addroundkey(uint8_t *state, const uint8_t *w, uint8_t r)
{
	for (int i = 0;i < 4;i++)
	{
		state[4 * 0 + i] = state[4 * 0 + i] ^ w[4 * 4 * r + 4 * i + 0];
		state[4 * 1 + i] = state[4 * 1 + i] ^ w[4 * 4 * r + 4 * i + 1];
		state[4 * 2 + i] = state[4 * 2 + i] ^ w[4 * 4 * r + 4 * i + 2];
		state[4 * 3 + i] = state[4 * 3 + i] ^ w[4 * 4 * r + 4 * i + 3];
	}
}

/* ACE 加密 */
void ace(uint8_t *in, uint8_t *out, const uint8_t *w)
{
	uint8_t state[4 * 4]; // 16 字节

	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			state[4 * i + j] = in[i + 4 * j];
		}
	}

	/* 第0轮 轮密钥加 */
	addroundkey(state, w, 0);

	/* 第1 ~ 9轮 轮密钥加 */
	for (int r = 1;r < 10;r++)
	{
		subbytes(state);				/* 字节代替 */
		shiftrows(state);				/* 行移位	*/
		mixcolumns(state);				/* 列混淆	*/
		addroundkey(state, w, r);		/* r轮密钥加*/
	}

	/* 第 10 轮 密钥加 */
	subbytes(state);					/* 字节代替 */
	shiftrows(state);					/* 行移位	*/
	addroundkey(state, w, 10);			/* r轮密钥加*/

										/* real value */
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out[i + 4 * j] = state[4 * i + j];
		}
	}

}

/* ACE 解密 */
void inv_ace(uint8_t *in, uint8_t *out, const uint8_t *w)
{
	uint8_t state[4 * 4]; // 16 字节

	for (int i = 0;i < 4;i++)
	{
		for (int j = 0;j < 4;j++)
		{
			state[4 * i + j] = in[i + 4 * j];
		}
	}

	/* 第0轮 轮密钥加 */
	addroundkey(state, w, 10);

	/* 第1 ~ 9轮 轮密钥加 */
	for (int r = 9;r >= 1;r--)
	{
		inv_shiftrows(state);			/* 逆向行移位	*/
		inv_subbytes(state);			/* 逆向字节代替 */
		addroundkey(state, w, r);		/* r轮密钥加	*/
		inv_mixcolumns(state);			/* 逆向列混淆	*/

	}

	/* 第 10 轮 密钥加 */
	inv_shiftrows(state);			/* 逆向行移位	*/
	inv_subbytes(state);			/* 逆向字节代替 */
	addroundkey(state, w, 0);		/* r轮密钥加	*/


									/* real value */
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out[i + 4 * j] = state[4 * i + j];
		}
	}

}

/* 输出数据 */
void show(const uint8_t *print, int len)
{
	uint8_t t = 0x00;
	for (int i = 0;i < len;i++)
	{
		t = print[i];
		printf("%4x ", t);
	}
	cout << endl;

}
int main(int argc, char **argv)
{
	/* input */
	uint8_t in[16] = {
		0x11, 0x12, 0x36, 0x59,
		0x3f, 0xf5, 0x05, 0x00,
		0xab, 0xff, 0x46, 0xee,
		0xcc, 0x57, 0xde, 0x3a
	};
	cout << "原文 : "; show(in, 16);

	/* output */
	uint8_t out[16] = { 0x00 };

	/* key */
	uint8_t key[16] = {
		0x11, 0x23, 0x56, 0x9f,
		0x31, 0xac, 0xaa, 0xdd,
		0xca, 0x13, 0x10, 0x00,
		0xff, 0xcc, 0x77, 0xbe
	};

	/* key扩展 */
	uint8_t *w;
	w = (uint8_t*)malloc(4 * (1 + 10) * 4 * sizeof(uint8_t));
	cout << endl;
	cout << "密钥 : "; show(key, 16);
	expandkey(key, w);
	cout << endl;

	/* 加密 */
	ace(in, out, w);
	cout << "加密 : "; show(out, 16);

	/* 解密 */
	uint8_t inv_out[16] = { 0x00 };
	inv_ace(out, inv_out, w);
	cout << "解密 : "; show(inv_out, 16);

	getchar();
	return 0;
}