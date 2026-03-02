// this file contains some code copied from hashcat, licensed as follows:
// The MIT License (MIT)
// 
// Copyright (c) 2015-2023 Jens Steube
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#define SHIFT_RIGHT_32(x,n) ((x) >> (n))

enum { SHA256C00=0x428a2f98U, SHA256C01=0x71374491U, SHA256C02=0xb5c0fbcfU, SHA256C03=0xe9b5dba5U, SHA256C04=0x3956c25bU, SHA256C05=0x59f111f1U, SHA256C06=0x923f82a4U, SHA256C07=0xab1c5ed5U, SHA256C08=0xd807aa98U, SHA256C09=0x12835b01U, SHA256C0a=0x243185beU, SHA256C0b=0x550c7dc3U, SHA256C0c=0x72be5d74U, SHA256C0d=0x80deb1feU, SHA256C0e=0x9bdc06a7U, SHA256C0f=0xc19bf174U, SHA256C10=0xe49b69c1U, SHA256C11=0xefbe4786U, SHA256C12=0x0fc19dc6U, SHA256C13=0x240ca1ccU, SHA256C14=0x2de92c6fU, SHA256C15=0x4a7484aaU, SHA256C16=0x5cb0a9dcU, SHA256C17=0x76f988daU, SHA256C18=0x983e5152U, SHA256C19=0xa831c66dU, SHA256C1a=0xb00327c8U, SHA256C1b=0xbf597fc7U, SHA256C1c=0xc6e00bf3U, SHA256C1d=0xd5a79147U, SHA256C1e=0x06ca6351U, SHA256C1f=0x14292967U, SHA256C20=0x27b70a85U, SHA256C21=0x2e1b2138U, SHA256C22=0x4d2c6dfcU, SHA256C23=0x53380d13U, SHA256C24=0x650a7354U, SHA256C25=0x766a0abbU, SHA256C26=0x81c2c92eU, SHA256C27=0x92722c85U, SHA256C28=0xa2bfe8a1U, SHA256C29=0xa81a664bU, SHA256C2a=0xc24b8b70U, SHA256C2b=0xc76c51a3U, SHA256C2c=0xd192e819U, SHA256C2d=0xd6990624U, SHA256C2e=0xf40e3585U, SHA256C2f=0x106aa070U, SHA256C30=0x19a4c116U, SHA256C31=0x1e376c08U, SHA256C32=0x2748774cU, SHA256C33=0x34b0bcb5U, SHA256C34=0x391c0cb3U, SHA256C35=0x4ed8aa4aU, SHA256C36=0x5b9cca4fU, SHA256C37=0x682e6ff3U, SHA256C38=0x748f82eeU, SHA256C39=0x78a5636fU, SHA256C3a=0x84c87814U, SHA256C3b=0x8cc70208U, SHA256C3c=0x90befffaU, SHA256C3d=0xa4506cebU, SHA256C3e=0xbef9a3f7U, SHA256C3f=0xc67178f2U };
__constant uint k_sha256[64] =
{
  SHA256C00, SHA256C01, SHA256C02, SHA256C03,
  SHA256C04, SHA256C05, SHA256C06, SHA256C07,
  SHA256C08, SHA256C09, SHA256C0a, SHA256C0b,
  SHA256C0c, SHA256C0d, SHA256C0e, SHA256C0f,
  SHA256C10, SHA256C11, SHA256C12, SHA256C13,
  SHA256C14, SHA256C15, SHA256C16, SHA256C17,
  SHA256C18, SHA256C19, SHA256C1a, SHA256C1b,
  SHA256C1c, SHA256C1d, SHA256C1e, SHA256C1f,
  SHA256C20, SHA256C21, SHA256C22, SHA256C23,
  SHA256C24, SHA256C25, SHA256C26, SHA256C27,
  SHA256C28, SHA256C29, SHA256C2a, SHA256C2b,
  SHA256C2c, SHA256C2d, SHA256C2e, SHA256C2f,
  SHA256C30, SHA256C31, SHA256C32, SHA256C33,
  SHA256C34, SHA256C35, SHA256C36, SHA256C37,
  SHA256C38, SHA256C39, SHA256C3a, SHA256C3b,
  SHA256C3c, SHA256C3d, SHA256C3e, SHA256C3f,
};

uint hc_rotl32(uint x, int y) {
    return (x << y) | (x >> (32 - y));
}
uint hc_rotl32_S(uint x, int y) {
    return (x << y) | (x >> (32 - y));
}
#define hc_add3_S(x,y,z) ((x)+(y)+(z))
#define SHA256_S0_S(x) (hc_rotl32_S ((x), 25u) ^ hc_rotl32_S ((x), 14u) ^ SHIFT_RIGHT_32 ((x),  3u))
#define SHA256_S1_S(x) (hc_rotl32_S ((x), 15u) ^ hc_rotl32_S ((x), 13u) ^ SHIFT_RIGHT_32 ((x), 10u))
#define SHA256_S2_S(x) (hc_rotl32_S ((x), 30u) ^ hc_rotl32_S ((x), 19u) ^ hc_rotl32_S ((x), 10u))
#define SHA256_S3_S(x) (hc_rotl32_S ((x), 26u) ^ hc_rotl32_S ((x), 21u) ^ hc_rotl32_S ((x),  7u))

#define SHA256_S0(x) (hc_rotl32 ((x), 25u) ^ hc_rotl32 ((x), 14u) ^ SHIFT_RIGHT_32 ((x),  3u))
#define SHA256_S1(x) (hc_rotl32 ((x), 15u) ^ hc_rotl32 ((x), 13u) ^ SHIFT_RIGHT_32 ((x), 10u))
#define SHA256_S2(x) (hc_rotl32 ((x), 30u) ^ hc_rotl32 ((x), 19u) ^ hc_rotl32 ((x), 10u))
#define SHA256_S3(x) (hc_rotl32 ((x), 26u) ^ hc_rotl32 ((x), 21u) ^ hc_rotl32 ((x),  7u))

#define SHA256_F0(x,y,z)  (((x) & (y)) | ((z) & ((x) ^ (y))))
#define SHA256_F1(x,y,z)  ((z) ^ ((x) & ((y) ^ (z))))

#ifdef USE_BITSELECT
#define SHA256_F0o(x,y,z) (bitselect ((x), (y), ((x) ^ (z))))
#define SHA256_F1o(x,y,z) (bitselect ((z), (y), (x)))
#else
#define SHA256_F0o(x,y,z) (SHA256_F0 ((x), (y), (z)))
#define SHA256_F1o(x,y,z) (SHA256_F1 ((x), (y), (z)))
#endif

#define SHA256_STEP_S(F0,F1,a,b,c,d,e,f,g,h,x,K)  \
{                                                 \
  h = hc_add3_S (h, K, x);                        \
  h = hc_add3_S (h, SHA256_S3_S (e), F1 (e,f,g)); \
  d += h;                                         \
  h = hc_add3_S (h, SHA256_S2_S (a), F0 (a,b,c)); \
}

#define SHA256_EXPAND_S(x,y,z,w) (SHA256_S1_S (x) + y + SHA256_S0_S (z) + w)

#define SHA256_STEP(F0,F1,a,b,c,d,e,f,g,h,x,K)    \
{                                                 \
  h = hc_add3 (h, make_u32x (K), x);              \
  h = hc_add3 (h, SHA256_S3 (e), F1 (e,f,g));     \
  d += h;                                         \
  h = hc_add3 (h, SHA256_S2 (a), F0 (a,b,c));     \
}

#define SHA256_EXPAND(x,y,z,w) (SHA256_S1 (x) + y + SHA256_S0 (z) + w)


__kernel void bruteforce(ulong off)
{
    ulong seed0 = get_global_id(0) + off;
    ulong seed1 = (seed0 * 25214903917 + 11) & 281474976710655;
    ulong seed2 = (seed1 * 25214903917 + 11) & 281474976710655;
    uint upper = seed1 >> (48 - 32);
    uint lower = seed2 >> (48 - 32);
    uint a0 = 0x6a09e667U;
    uint b0 = 0xbb67ae85U;
    uint c0 = 0x3c6ef372U;
    uint d0 = 0xa54ff53aU;
    uint e0 = 0x510e527fU;
    uint f0 = 0x9b05688cU;
    uint g0 = 0x1f83d9abU;
    uint h0 = 0x5be0cd19U;
    uint a = a0, b = b0, c = c0, d = d0, e = e0, f = f0, g = g0, h = h0;
    uint w0_t = __builtin_bswap32(lower);
    uint w1_t = __builtin_bswap32(upper);
    uint w2_t = 0x80000000;
    uint w3_t = 0;
    uint w4_t = 0;
    uint w5_t = 0;
    uint w6_t = 0;
    uint w7_t = 0;
    uint w8_t = 0;
    uint w9_t = 0;
    uint wa_t = 0;
    uint wb_t = 0;
    uint wc_t = 0;
    uint wd_t = 0;
    uint we_t = 0;
    uint wf_t = 64;

#define ROUND_EXPAND_S()                            \
  {                                                   \
    w0_t = SHA256_EXPAND_S (we_t, w9_t, w1_t, w0_t);  \
    w1_t = SHA256_EXPAND_S (wf_t, wa_t, w2_t, w1_t);  \
    w2_t = SHA256_EXPAND_S (w0_t, wb_t, w3_t, w2_t);  \
    w3_t = SHA256_EXPAND_S (w1_t, wc_t, w4_t, w3_t);  \
    w4_t = SHA256_EXPAND_S (w2_t, wd_t, w5_t, w4_t);  \
    w5_t = SHA256_EXPAND_S (w3_t, we_t, w6_t, w5_t);  \
    w6_t = SHA256_EXPAND_S (w4_t, wf_t, w7_t, w6_t);  \
    w7_t = SHA256_EXPAND_S (w5_t, w0_t, w8_t, w7_t);  \
    w8_t = SHA256_EXPAND_S (w6_t, w1_t, w9_t, w8_t);  \
    w9_t = SHA256_EXPAND_S (w7_t, w2_t, wa_t, w9_t);  \
    wa_t = SHA256_EXPAND_S (w8_t, w3_t, wb_t, wa_t);  \
    wb_t = SHA256_EXPAND_S (w9_t, w4_t, wc_t, wb_t);  \
    wc_t = SHA256_EXPAND_S (wa_t, w5_t, wd_t, wc_t);  \
    wd_t = SHA256_EXPAND_S (wb_t, w6_t, we_t, wd_t);  \
    we_t = SHA256_EXPAND_S (wc_t, w7_t, wf_t, we_t);  \
    wf_t = SHA256_EXPAND_S (wd_t, w8_t, w0_t, wf_t);  \
  }

#define ROUND_STEP_S(i)                                                                   \
  {                                                                                         \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, a, b, c, d, e, f, g, h, w0_t, k_sha256[i +  0]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, h, a, b, c, d, e, f, g, w1_t, k_sha256[i +  1]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, g, h, a, b, c, d, e, f, w2_t, k_sha256[i +  2]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, f, g, h, a, b, c, d, e, w3_t, k_sha256[i +  3]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, e, f, g, h, a, b, c, d, w4_t, k_sha256[i +  4]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, d, e, f, g, h, a, b, c, w5_t, k_sha256[i +  5]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, c, d, e, f, g, h, a, b, w6_t, k_sha256[i +  6]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, b, c, d, e, f, g, h, a, w7_t, k_sha256[i +  7]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, a, b, c, d, e, f, g, h, w8_t, k_sha256[i +  8]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, h, a, b, c, d, e, f, g, w9_t, k_sha256[i +  9]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, g, h, a, b, c, d, e, f, wa_t, k_sha256[i + 10]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, f, g, h, a, b, c, d, e, wb_t, k_sha256[i + 11]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, e, f, g, h, a, b, c, d, wc_t, k_sha256[i + 12]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, d, e, f, g, h, a, b, c, wd_t, k_sha256[i + 13]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, c, d, e, f, g, h, a, b, we_t, k_sha256[i + 14]); \
    SHA256_STEP_S (SHA256_F0o, SHA256_F1o, b, c, d, e, f, g, h, a, wf_t, k_sha256[i + 15]); \
  }
    ROUND_STEP_S (0);
    for (int i = 16; i < 64; i += 16)
    {
        ROUND_EXPAND_S (); ROUND_STEP_S (i);
    }
    a += a0; b += b0; c += c0; d += d0; e += e0; f += f0; g += g0; h += h0;

    ulong hash = (ulong)__builtin_bswap32(b) << 32 | __builtin_bswap32(a);

    if(hash % ((ulong)(1) << 34) == 16074632720) {
        ulong fullseed = (ulong)upper << 32 | lower;
        printf("WIN at=%lu fs=%lu hash=%lu\n", seed0, fullseed, hash);
    }
}

