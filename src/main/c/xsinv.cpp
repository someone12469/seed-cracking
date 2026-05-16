#include "java_helper.h"
#include <bit>
#include <experimental/simd>
#include <cstdint>
#include <cstdio>
#include <cstddef>
#include <utility>

template<size_t... Inds>
constexpr void foreach(auto&& f, std::index_sequence<Inds...>) {
	(f.template operator()<Inds>(), ...);
}

namespace stdx = std::experimental;
using simd = stdx::native_simd<uint64_t>;
constexpr size_t SIMD_BITS = 64 * simd::size();

static uint64_t unstafford13(uint64_t x)
{
	x ^= x >> 31;
	x ^= x >> 62;
	x *= 3573116690164977347ull;
	x ^= x >> 27;
	x ^= x >> 54;
	x *= 10871156337175269513ull;
	x ^= x >> 30;
	x ^= x >> 60;
	return x;
}

static uint64_t nextlong1, nextlong2;
static void check(uint64_t lo, uint64_t hi, uint64_t guess)
{
	// compute testOut1 = rotl64(lo+hi,17) + lo
	uint64_t sum0	  = lo + hi;
	uint64_t testOut1 = std::rotl(sum0, 17) + lo;

	// t = hi ^ lo
	uint64_t t = hi ^ lo;

	// lo' = rotl64(lo,49) ^ t ^ (t << 21)
	uint64_t lo2 = std::rotl(lo, 49) ^ t ^ (t << 21);

	// hi' = rotl64(t,28)
	uint64_t hi2 = std::rotl(t, 28);

	// testOut2 = rotl64(lo'+hi',17) + lo'
	uint64_t sum1	  = lo2 + hi2;
	uint64_t testOut2 = std::rotl(sum1, 17) + lo2;

	// only valid if both match
	if (testOut1 == nextlong1 && testOut2 == nextlong2) {
		sendUpdate("commands.bedrockcrack.foundCandidate", true, lo, hi, guess);
		if(unstafford13(hi) - unstafford13(lo) == 0x9e3779b97f4a7c15) {
			sendUpdate("commands.bedrockcrack.foundWorldSeed", false, unstafford13(lo) ^ 7640891576956012809);
		}
	}
}

template<bool FULL>
[[gnu::flatten, gnu::noinline]]
static bool bruteKernel(uint64_t guess)
{
	// ----------------------------------------------------------------------------
	//  1.   Expand the two 64-bit seeds into bit arrays
	// ----------------------------------------------------------------------------
	
	simd S1_lo[64];
	simd S1_hi[64];
	simd S1_ca[64];
	simd S1_is[64];
	simd S1_cb[64];
	simd S1_out[64];
	
	simd S2_lo[64];
	simd S2_hi[64];
	simd S2_ca[64];
	simd S2_is[64];
	simd S2_cb[64];
	simd S2_out[64];
	foreach([&]<size_t i> {
			S1_out[i] = uint64_t(nextlong1) >> i & 1 ? ~simd{} : simd{};
			S2_out[i] = uint64_t(nextlong2) >> i & 1 ? ~simd{} : simd{};
			}, std::make_index_sequence<64>{});

	// ----------------------------------------------------------------------------
	//  2.   Expand the BRUTEFORCE_SIZE-bit guess into individual bits
	// ----------------------------------------------------------------------------
	simd guess_bits[64];
	foreach([&]<size_t i> {
		if constexpr(i >= 6 + std::countr_zero(simd::size())) {
			guess_bits[i] = guess >> i & 1 ? ~simd{} : simd{};
		} else if constexpr(i >= 6) {
			guess_bits[i] = simd([]<size_t j>(std::integral_constant<size_t, j>) {
				return j >> (i - 6) & 1 ? ~uint64_t{} : uint64_t{};
			});
		} else {
			guess_bits[i] = [] consteval {
				uint64_t x = 0;
				for(size_t j = 0; j < 64; j++)
					x |= (j >> i & 1) << j;
				return x;
			}();
		}
	}, std::make_index_sequence<64>{});

	//  source: https://github.com/pseudogravity/xoroshiro/blob/main/from2nextlongs.cu
	// ----------------------------------------------------------------------------
	//  3.   INSERT GENERATED LOGIC HERE
	//	   –––––––––––––––––––––––––––
	//  Hundreds of lines like:
	//
	//		uint8_t S1_ca_46 = S1_hxl_47 ^ S1_is_47;
	//		uint8_t S1_ca_47;
	//		uint8_t tmpSum   = carry(S1_ca_46, S1_lo_47, S1_hi_47, &S1_ca_47);
	//
	//  Please paste the auto-generated section replacing the whole placeholder.
	// ----------------------------------------------------------------------------
	//  BEGIN ▼▼▼▼▼ (placeholder block)
	S1_lo[51] = guess_bits[0];
	S1_lo[19] = guess_bits[1];
	S1_hi[47] = guess_bits[2];
	S1_hi[36] = guess_bits[3];
	S1_hi[33] = guess_bits[4];
	S1_hi[1] = guess_bits[5];
	S2_lo[50] = guess_bits[6];
	S2_lo[40] = guess_bits[7];
	S2_lo[39] = guess_bits[8];
	S2_lo[33] = guess_bits[9];
	S2_lo[32] = guess_bits[10];
	S2_lo[15] = guess_bits[11];
	S2_hi[60] = guess_bits[12];
	S2_hi[58] = guess_bits[13];
	S2_hi[45] = guess_bits[14];
	S2_hi[43] = guess_bits[15];
	S2_hi[41] = guess_bits[16];
	S2_hi[18] = guess_bits[17];
	S2_hi[16] = guess_bits[18];
	S2_hi[12] = guess_bits[19];
	S2_hi[0] = guess_bits[20];
	S1_ca[46] = guess_bits[21];
	S1_ca[29] = guess_bits[22];
	S1_ca[14] = guess_bits[23];
	S1_ca[12] = guess_bits[24];
	S1_is[63] = guess_bits[25];
	S1_is[50] = guess_bits[26];
	S1_is[49] = guess_bits[27];
	S1_is[47] = guess_bits[28];
	S1_is[35] = guess_bits[29];
	S1_is[31] = guess_bits[30];
	S1_is[14] = guess_bits[31];
	S1_is[0] = guess_bits[32];
	S2_ca[32] = guess_bits[33];
	S2_ca[14] = guess_bits[34];
	S2_is[48] = guess_bits[35];
	S2_is[18] = guess_bits[36];
	S2_cb[49] = guess_bits[37];
	S2_cb[31] = guess_bits[38];
	S2_cb[14] = guess_bits[39];


	S2_hi[28] = S1_is[0];
	S1_lo[36] = S1_hi[36] ^ S2_hi[0];
	S1_lo[30] = S2_lo[15] ^ S2_hi[43];
	S1_lo[0] = S1_is[47] ^ S1_out[0];
	S1_hi[30] = S1_lo[30] ^ S2_hi[58];
	S1_hi[0] = S1_lo[0] ^ S1_is[0];
	S2_hi[11] = S1_ca[46] ^ S1_is[47];
	S1_lo[47] = S1_hi[47] ^ S2_hi[11];
	S1_is[30] = S2_hi[58] ^ S1_ca[29];
	S1_is[15] = S2_hi[43] ^ S1_ca[14];
	S1_is[13] = S2_hi[41] ^ S1_ca[12];
	S2_lo[36] = S1_lo[51] ^ S2_hi[43] ^ S2_hi[0];
	S2_hi[39] = S1_lo[47] ^ S2_lo[32] ^ S2_hi[60];
	S1_cb[46] = S1_lo[47] ^ S1_is[30] ^ S1_out[47];
	S1_cb[29] = S1_lo[30] ^ S1_is[13] ^ S1_out[30];
	S2_is[62] = S2_lo[15] ^ S2_cb[14] ^ S2_out[15];
	S2_is[33] = S2_lo[50] ^ S2_cb[49] ^ S2_out[50];
	S2_hi[33] = S2_lo[33] ^ S2_ca[32] ^ S2_is[33];
	S2_is[15] = S2_lo[32] ^ S2_cb[31] ^ S2_out[32];
	S2_hi[15] = S2_lo[15] ^ S2_ca[14] ^ S2_is[15];
	S1_hi[51] = S1_lo[51] ^ S2_hi[15];
	S1_ca[0] = S1_lo[0] & S1_hi[0];
	S1_ca[30] = ((S1_ca[29] & S1_lo[30]) | (S1_lo[30] & S1_hi[30]) | (S1_ca[29] & S1_hi[30]));
	S2_hi[59] = S1_ca[30] ^ S1_is[31];
	S1_ca[47] = ((S1_ca[46] & S1_lo[47]) | (S1_lo[47] & S1_hi[47]) | (S1_ca[46] & S1_hi[47]));
	S1_is[48] = S2_hi[12] ^ S1_ca[47];
	S1_cb[0] = S1_is[47] & S1_lo[0];
	S1_lo[1] = S1_is[48] ^ S1_cb[0] ^ S1_out[1];
	S2_hi[29] = S1_lo[1] ^ S1_hi[1];
	S1_is[1] = S2_hi[29] ^ S1_ca[0];
	S1_ca[1] = ((S1_ca[0] & S1_lo[1]) | (S1_lo[1] & S1_hi[1]) | (S1_ca[0] & S1_hi[1]));
	S1_cb[1] = ((S1_cb[0] & S1_is[48]) | (S1_is[48] & S1_lo[1]) | (S1_cb[0] & S1_lo[1]));
	S1_lo[2] = S1_is[49] ^ S1_cb[1] ^ S1_out[2];
	S2_lo[51] = S1_lo[2] ^ S2_hi[58] ^ S2_hi[15];
	S1_cb[2] = ((S1_cb[1] & S1_is[49]) | (S1_is[49] & S1_lo[2]) | (S1_cb[1] & S1_lo[2]));
	S1_lo[3] = S1_is[50] ^ S1_cb[2] ^ S1_out[3];
	S2_lo[52] = S1_lo[3] ^ S2_hi[59] ^ S2_hi[16];
	S1_cb[3] = ((S1_cb[2] & S1_is[50]) | (S1_is[50] & S1_lo[3]) | (S1_cb[2] & S1_lo[3]));
	S1_cb[30] = ((S1_cb[29] & S1_is[13]) | (S1_is[13] & S1_lo[30]) | (S1_cb[29] & S1_lo[30]));
	S1_lo[31] = S1_is[14] ^ S1_cb[30] ^ S1_out[31];
	S1_hi[31] = S1_lo[31] ^ S2_hi[59];
	S1_ca[31] = ((S1_ca[30] & S1_lo[31]) | (S1_lo[31] & S1_hi[31]) | (S1_ca[30] & S1_hi[31]));
	S1_is[32] = S2_hi[60] ^ S1_ca[31];
	S1_cb[31] = ((S1_cb[30] & S1_is[14]) | (S1_is[14] & S1_lo[31]) | (S1_cb[30] & S1_lo[31]));
	S1_lo[32] = S1_is[15] ^ S1_cb[31] ^ S1_out[32];
	S1_hi[32] = S1_lo[32] ^ S2_hi[60];
	S2_lo[17] = S1_lo[32] ^ S2_hi[45];
	S1_ca[32] = ((S1_ca[31] & S1_lo[32]) | (S1_lo[32] & S1_hi[32]) | (S1_ca[31] & S1_hi[32]));
	S1_cb[32] = ((S1_cb[31] & S1_is[15]) | (S1_is[15] & S1_lo[32]) | (S1_cb[31] & S1_lo[32]));
	S1_cb[47] = ((S1_cb[46] & S1_is[30]) | (S1_is[30] & S1_lo[47]) | (S1_cb[46] & S1_lo[47]));
	S1_lo[48] = S1_is[31] ^ S1_cb[47] ^ S1_out[48];
	S1_hi[48] = S1_lo[48] ^ S2_hi[12];
	S1_ca[48] = ((S1_ca[47] & S1_lo[48]) | (S1_lo[48] & S1_hi[48]) | (S1_ca[47] & S1_hi[48]));
	S2_hi[13] = S1_ca[48] ^ S1_is[49];
	S1_cb[48] = ((S1_cb[47] & S1_is[31]) | (S1_is[31] & S1_lo[48]) | (S1_cb[47] & S1_lo[48]));
	S1_lo[49] = S1_is[32] ^ S1_cb[48] ^ S1_out[49];
	S1_hi[49] = S1_lo[49] ^ S2_hi[13];
	S1_ca[49] = ((S1_ca[48] & S1_lo[49]) | (S1_lo[49] & S1_hi[49]) | (S1_ca[48] & S1_hi[49]));
	S2_hi[14] = S1_ca[49] ^ S1_is[50];
	S2_hi[57] = S1_lo[1] ^ S2_lo[50] ^ S2_hi[14];
	S1_cb[49] = ((S1_cb[48] & S1_is[32]) | (S1_is[32] & S1_lo[49]) | (S1_cb[48] & S1_lo[49]));
	S2_ca[15] = ((S2_ca[14] & S2_lo[15]) | (S2_lo[15] & S2_hi[15]) | (S2_ca[14] & S2_hi[15]));
	S2_ca[33] = ((S2_ca[32] & S2_lo[33]) | (S2_lo[33] & S2_hi[33]) | (S2_ca[32] & S2_hi[33]));
	S2_cb[15] = ((S2_cb[14] & S2_is[62]) | (S2_is[62] & S2_lo[15]) | (S2_cb[14] & S2_lo[15]));
	S2_cb[32] = ((S2_cb[31] & S2_is[15]) | (S2_is[15] & S2_lo[32]) | (S2_cb[31] & S2_lo[32]));
	S2_is[16] = S2_lo[33] ^ S2_cb[32] ^ S2_out[33];
	S2_lo[16] = S2_hi[16] ^ S2_ca[15] ^ S2_is[16];
	S2_hi[44] = S1_lo[31] ^ S2_lo[16];
	S2_is[63] = S2_lo[16] ^ S2_cb[15] ^ S2_out[16];
	S2_ca[16] = ((S2_ca[15] & S2_lo[16]) | (S2_lo[16] & S2_hi[16]) | (S2_ca[15] & S2_hi[16]));
	S2_cb[16] = ((S2_cb[15] & S2_is[63]) | (S2_is[63] & S2_lo[16]) | (S2_cb[15] & S2_lo[16]));
	S2_is[0] = S2_lo[17] ^ S2_cb[16] ^ S2_out[17];
	S2_lo[0] = S2_hi[0] ^ S2_is[0];
	S1_lo[15] = S2_lo[0] ^ S2_hi[28];
	S1_hi[15] = S1_lo[15] ^ S2_hi[43];
	S2_is[47] = S2_lo[0] ^ S2_out[0];
	S1_ca[15] = ((S1_ca[14] & S1_lo[15]) | (S1_lo[15] & S1_hi[15]) | (S1_ca[14] & S1_hi[15]));
	S1_is[16] = S2_hi[44] ^ S1_ca[15];
	S1_lo[33] = S1_is[16] ^ S1_cb[32] ^ S1_out[33];
	S2_hi[61] = S1_lo[33] ^ S1_hi[33];
	S1_is[33] = S2_hi[61] ^ S1_ca[32];
	S1_lo[50] = S1_is[33] ^ S1_cb[49] ^ S1_out[50];
	S1_hi[50] = S1_lo[50] ^ S2_hi[14];
	S2_hi[40] = S1_lo[48] ^ S2_lo[33] ^ S2_hi[61];
	S1_ca[33] = ((S1_ca[32] & S1_lo[33]) | (S1_lo[33] & S1_hi[33]) | (S1_ca[32] & S1_hi[33]));
	S1_ca[50] = ((S1_ca[49] & S1_lo[50]) | (S1_lo[50] & S1_hi[50]) | (S1_ca[49] & S1_hi[50]));
	S1_lo[4] = S2_hi[15] ^ S1_ca[50] ^ S1_cb[3] ^ S1_out[4];
	S1_is[51] = S1_lo[4] ^ S1_cb[3] ^ S1_out[4];
	S1_ca[51] = ((S1_ca[50] & S1_lo[51]) | (S1_lo[51] & S1_hi[51]) | (S1_ca[50] & S1_hi[51]));
	S1_is[52] = S2_hi[16] ^ S1_ca[51];
	S1_cb[4] = ((S1_cb[3] & S1_is[51]) | (S1_is[51] & S1_lo[4]) | (S1_cb[3] & S1_lo[4]));
	S1_lo[5] = S1_is[52] ^ S1_cb[4] ^ S1_out[5];
	S1_hi[5] = S1_lo[5] ^ S2_hi[33];
	S2_lo[54] = S1_lo[5] ^ S2_hi[61] ^ S2_hi[18];
	S1_cb[5] = ((S1_cb[4] & S1_is[52]) | (S1_is[52] & S1_lo[5]) | (S1_cb[4] & S1_lo[5]));
	S1_cb[33] = ((S1_cb[32] & S1_is[16]) | (S1_is[16] & S1_lo[33]) | (S1_cb[32] & S1_lo[33]));
	S1_cb[50] = ((S1_cb[49] & S1_is[33]) | (S1_is[33] & S1_lo[50]) | (S1_cb[49] & S1_lo[50]));
	S1_is[34] = S1_lo[51] ^ S1_cb[50] ^ S1_out[51];
	S2_hi[62] = S1_ca[33] ^ S1_is[34];
	S2_lo[34] = S1_lo[49] ^ S2_hi[62] ^ S2_hi[41];
	S1_cb[51] = ((S1_cb[50] & S1_is[34]) | (S1_is[34] & S1_lo[51]) | (S1_cb[50] & S1_lo[51]));
	S1_lo[52] = S1_is[35] ^ S1_cb[51] ^ S1_out[52];
	S1_hi[52] = S1_lo[52] ^ S2_hi[16];
	S1_ca[52] = ((S1_ca[51] & S1_lo[52]) | (S1_lo[52] & S1_hi[52]) | (S1_ca[51] & S1_hi[52]));
	S1_cb[52] = ((S1_cb[51] & S1_is[35]) | (S1_is[35] & S1_lo[52]) | (S1_cb[51] & S1_lo[52]));
	S2_ca[0] = S2_lo[0] & S2_hi[0];
	S2_cb[0] = S2_is[47] & S2_lo[0];
	S2_lo[1] = S2_is[48] ^ S2_cb[0] ^ S2_out[1];
	S1_lo[16] = S2_lo[1] ^ S2_hi[29];
	S1_hi[16] = S1_lo[16] ^ S2_hi[44];
	S1_cb[15] = S1_lo[16] ^ S1_is[63] ^ S1_out[16];
	S1_ca[16] = ((S1_ca[15] & S1_lo[16]) | (S1_lo[16] & S1_hi[16]) | (S1_ca[15] & S1_hi[16]));
	S1_is[17] = S2_hi[45] ^ S1_ca[16];
	S1_lo[34] = S1_is[17] ^ S1_cb[33] ^ S1_out[34];
	S1_hi[34] = S1_lo[34] ^ S2_hi[62];
	S1_ca[34] = ((S1_ca[33] & S1_lo[34]) | (S1_lo[34] & S1_hi[34]) | (S1_ca[33] & S1_hi[34]));
	S2_hi[63] = S1_ca[34] ^ S1_is[35];
	S1_cb[16] = ((S1_cb[15] & S1_is[63]) | (S1_is[63] & S1_lo[16]) | (S1_cb[15] & S1_lo[16]));
	S1_lo[17] = S1_is[0] ^ S1_cb[16] ^ S1_out[17];
	S1_hi[17] = S1_lo[17] ^ S2_hi[45];
	S1_ca[17] = ((S1_ca[16] & S1_lo[17]) | (S1_lo[17] & S1_hi[17]) | (S1_ca[16] & S1_hi[17]));
	S1_cb[17] = ((S1_cb[16] & S1_is[0]) | (S1_is[0] & S1_lo[17]) | (S1_cb[16] & S1_lo[17]));
	S1_lo[18] = S1_is[1] ^ S1_cb[17] ^ S1_out[18];
	S1_cb[18] = ((S1_cb[17] & S1_is[1]) | (S1_is[1] & S1_lo[18]) | (S1_cb[17] & S1_lo[18]));
	S1_is[2] = S1_lo[19] ^ S1_cb[18] ^ S1_out[19];
	S2_hi[30] = S1_ca[1] ^ S1_is[2];
	S1_hi[2] = S1_lo[2] ^ S2_hi[30];
	S2_lo[2] = S1_lo[17] ^ S2_hi[30];
	S1_ca[2] = ((S1_ca[1] & S1_lo[2]) | (S1_lo[2] & S1_hi[2]) | (S1_ca[1] & S1_hi[2]));
	S1_cb[19] = ((S1_cb[18] & S1_is[2]) | (S1_is[2] & S1_lo[19]) | (S1_cb[18] & S1_lo[19]));
	S1_cb[34] = ((S1_cb[33] & S1_is[17]) | (S1_is[17] & S1_lo[34]) | (S1_cb[33] & S1_lo[34]));
	S2_cb[1] = ((S2_cb[0] & S2_is[48]) | (S2_is[48] & S2_lo[1]) | (S2_cb[0] & S2_lo[1]));
	S2_is[49] = S2_lo[2] ^ S2_cb[1] ^ S2_out[2];
	S2_cb[2] = ((S2_cb[1] & S2_is[49]) | (S2_is[49] & S2_lo[2]) | (S2_cb[1] & S2_lo[2]));
	S2_cb[17] = ((S2_cb[16] & S2_is[0]) | (S2_is[0] & S2_lo[17]) | (S2_cb[16] & S2_lo[17]));
	S2_cb[33] = ((S2_cb[32] & S2_is[16]) | (S2_is[16] & S2_lo[33]) | (S2_cb[32] & S2_lo[33]));
	S2_is[17] = S2_lo[34] ^ S2_cb[33] ^ S2_out[34];
	S2_hi[17] = S2_lo[17] ^ S2_ca[16] ^ S2_is[17];
	S1_is[53] = S2_hi[17] ^ S1_ca[52];
	S1_lo[6] = S1_is[53] ^ S1_cb[5] ^ S1_out[6];
	S2_lo[53] = S1_lo[32] ^ S1_lo[4] ^ S1_hi[32] ^ S2_hi[17];
	S1_cb[6] = ((S1_cb[5] & S1_is[53]) | (S1_is[53] & S1_lo[6]) | (S1_cb[5] & S1_lo[6]));
	S2_ca[17] = ((S2_ca[16] & S2_lo[17]) | (S2_lo[17] & S2_hi[17]) | (S2_ca[16] & S2_hi[17]));
	S2_lo[18] = S2_hi[18] ^ S2_ca[17] ^ S2_is[18];
	S2_hi[46] = S1_lo[33] ^ S2_lo[18];
	S1_hi[18] = S1_lo[18] ^ S2_hi[46];
	S1_is[18] = S2_hi[46] ^ S1_ca[17];
	S1_lo[35] = S1_is[18] ^ S1_cb[34] ^ S1_out[35];
	S1_hi[35] = S1_lo[35] ^ S2_hi[63];
	S2_is[1] = S2_lo[18] ^ S2_cb[17] ^ S2_out[18];
	S2_hi[1] = S2_lo[1] ^ S2_ca[0] ^ S2_is[1];
	S2_lo[37] = S1_lo[52] ^ S2_hi[44] ^ S2_hi[1];
	S1_ca[18] = ((S1_ca[17] & S1_lo[18]) | (S1_lo[18] & S1_hi[18]) | (S1_ca[17] & S1_hi[18]));
	S1_ca[35] = ((S1_ca[34] & S1_lo[35]) | (S1_lo[35] & S1_hi[35]) | (S1_ca[34] & S1_hi[35]));
	S1_is[36] = S2_hi[0] ^ S1_ca[35];
	S1_lo[53] = S1_is[36] ^ S1_cb[52] ^ S1_out[53];
	S1_hi[53] = S1_lo[53] ^ S2_hi[17];
	S1_ca[36] = ((S1_ca[35] & S1_lo[36]) | (S1_lo[36] & S1_hi[36]) | (S1_ca[35] & S1_hi[36]));
	S1_is[37] = S2_hi[1] ^ S1_ca[36];
	S1_ca[53] = ((S1_ca[52] & S1_lo[53]) | (S1_lo[53] & S1_hi[53]) | (S1_ca[52] & S1_hi[53]));
	S1_is[54] = S1_lo[5] ^ S2_lo[54] ^ S2_hi[61] ^ S1_ca[53];
	S1_lo[7] = S1_is[54] ^ S1_cb[6] ^ S1_out[7];
	S1_cb[7] = ((S1_cb[6] & S1_is[54]) | (S1_is[54] & S1_lo[7]) | (S1_cb[6] & S1_lo[7]));
	S1_cb[35] = ((S1_cb[34] & S1_is[18]) | (S1_is[18] & S1_lo[35]) | (S1_cb[34] & S1_lo[35]));
	S1_is[19] = S1_lo[36] ^ S1_cb[35] ^ S1_out[36];
	S2_hi[47] = S1_ca[18] ^ S1_is[19];
	S1_hi[19] = S1_lo[19] ^ S2_hi[47];
	S2_lo[19] = S1_lo[34] ^ S2_hi[47];
	S1_ca[19] = ((S1_ca[18] & S1_lo[19]) | (S1_lo[19] & S1_hi[19]) | (S1_ca[18] & S1_hi[19]));
	S1_cb[36] = ((S1_cb[35] & S1_is[19]) | (S1_is[19] & S1_lo[36]) | (S1_cb[35] & S1_lo[36]));
	S1_cb[53] = ((S1_cb[52] & S1_is[36]) | (S1_is[36] & S1_lo[53]) | (S1_cb[52] & S1_lo[53]));
	S1_lo[54] = S1_is[37] ^ S1_cb[53] ^ S1_out[54];
	S2_hi[3] = S1_lo[54] ^ S2_lo[39] ^ S2_hi[46];
	S1_hi[54] = S1_lo[54] ^ S2_hi[18];
	S1_ca[54] = ((S1_ca[53] & S1_lo[54]) | (S1_lo[54] & S1_hi[54]) | (S1_ca[53] & S1_hi[54]));
	S1_cb[54] = ((S1_cb[53] & S1_is[37]) | (S1_is[37] & S1_lo[54]) | (S1_cb[53] & S1_lo[54]));
	S2_ca[1] = ((S2_ca[0] & S2_lo[1]) | (S2_lo[1] & S2_hi[1]) | (S2_ca[0] & S2_hi[1]));
	S2_ca[18] = ((S2_ca[17] & S2_lo[18]) | (S2_lo[18] & S2_hi[18]) | (S2_ca[17] & S2_hi[18]));
	S2_cb[18] = ((S2_cb[17] & S2_is[1]) | (S2_is[1] & S2_lo[18]) | (S2_cb[17] & S2_lo[18]));
	S2_is[2] = S2_lo[19] ^ S2_cb[18] ^ S2_out[19];
	S2_hi[2] = S2_lo[2] ^ S2_ca[1] ^ S2_is[2];
	S2_lo[38] = S1_lo[53] ^ S2_hi[45] ^ S2_hi[2];
	S2_ca[2] = ((S2_ca[1] & S2_lo[2]) | (S2_lo[2] & S2_hi[2]) | (S2_ca[1] & S2_hi[2]));
	S2_cb[19] = ((S2_cb[18] & S2_is[2]) | (S2_is[2] & S2_lo[19]) | (S2_cb[18] & S2_lo[19]));
	S1_hi[20] = S1_lo[54] ^ S1_lo[35] ^ S1_hi[18] ^ S2_lo[39] ^ S1_ca[2] ^ S1_cb[19] ^ S1_out[20] ^ S2_ca[2] ^ S2_cb[19] ^ S2_out[20];
	S2_cb[34] = ((S2_cb[33] & S2_is[17]) | (S2_is[17] & S2_lo[34]) | (S2_cb[33] & S2_lo[34]));
	S2_lo[35] = S1_lo[54] ^ S1_hi[54] ^ S2_lo[18] ^ S2_ca[17] ^ S2_cb[34] ^ S2_out[35];
	S2_hi[42] = S1_lo[50] ^ S2_lo[35] ^ S2_hi[63];
	S1_ca[13] = S2_hi[42] ^ S1_is[14];
	S2_cb[35] = ((S2_cb[34] & S2_is[18]) | (S2_is[18] & S2_lo[35]) | (S2_cb[34] & S2_lo[35]));
	S2_is[19] = S2_lo[36] ^ S2_cb[35] ^ S2_out[36];
	S2_hi[19] = S2_lo[19] ^ S2_ca[18] ^ S2_is[19];
	S1_is[55] = S2_hi[19] ^ S1_ca[54];
	S1_lo[8] = S1_is[55] ^ S1_cb[7] ^ S1_out[8];
	S2_lo[55] = S1_lo[34] ^ S1_lo[6] ^ S1_hi[34] ^ S2_hi[19];
	S1_cb[8] = ((S1_cb[7] & S1_is[55]) | (S1_is[55] & S1_lo[8]) | (S1_cb[7] & S1_lo[8]));
	S2_ca[19] = ((S2_ca[18] & S2_lo[19]) | (S2_lo[19] & S2_hi[19]) | (S2_ca[18] & S2_hi[19]));
	S2_cb[36] = ((S2_cb[35] & S2_is[19]) | (S2_is[19] & S2_lo[36]) | (S2_cb[35] & S2_lo[36]));
	S2_is[20] = S2_lo[37] ^ S2_cb[36] ^ S2_out[37];
	S2_cb[37] = ((S2_cb[36] & S2_is[20]) | (S2_is[20] & S2_lo[37]) | (S2_cb[36] & S2_lo[37]));
	S2_is[21] = S2_lo[38] ^ S2_cb[37] ^ S2_out[38];
	S2_cb[38] = ((S2_cb[37] & S2_is[21]) | (S2_is[21] & S2_lo[38]) | (S2_cb[37] & S2_lo[38]));
	S2_is[22] = S2_lo[39] ^ S2_cb[38] ^ S2_out[39];
	S2_cb[39] = ((S2_cb[38] & S2_is[22]) | (S2_is[22] & S2_lo[39]) | (S2_cb[38] & S2_lo[39]));
	S2_is[23] = S2_lo[40] ^ S2_cb[39] ^ S2_out[40];
	S2_cb[40] = ((S2_cb[39] & S2_is[23]) | (S2_is[23] & S2_lo[40]) | (S2_cb[39] & S2_lo[40]));
	S2_cb[50] = ((S2_cb[49] & S2_is[33]) | (S2_is[33] & S2_lo[50]) | (S2_cb[49] & S2_lo[50]));
	S2_is[34] = S2_lo[51] ^ S2_cb[50] ^ S2_out[51];
	S2_hi[34] = S2_lo[34] ^ S2_ca[33] ^ S2_is[34];
	S1_hi[6] = S1_lo[6] ^ S2_hi[34];
	S2_ca[34] = ((S2_ca[33] & S2_lo[34]) | (S2_lo[34] & S2_hi[34]) | (S2_ca[33] & S2_hi[34]));
	S2_cb[51] = ((S2_cb[50] & S2_is[34]) | (S2_is[34] & S2_lo[51]) | (S2_cb[50] & S2_lo[51]));
	S2_is[35] = S2_lo[52] ^ S2_cb[51] ^ S2_out[52];
	S2_hi[35] = S2_lo[35] ^ S2_ca[34] ^ S2_is[35];
	S1_hi[7] = S1_lo[7] ^ S2_hi[35];
	S2_ca[35] = ((S2_ca[34] & S2_lo[35]) | (S2_lo[35] & S2_hi[35]) | (S2_ca[34] & S2_hi[35]));
	S2_cb[52] = ((S2_cb[51] & S2_is[35]) | (S2_is[35] & S2_lo[52]) | (S2_cb[51] & S2_lo[52]));
	S2_is[36] = S2_lo[53] ^ S2_cb[52] ^ S2_out[53];
	S2_hi[36] = S2_lo[36] ^ S2_ca[35] ^ S2_is[36];
	S1_hi[8] = S1_lo[8] ^ S2_hi[36];
	S2_ca[36] = ((S2_ca[35] & S2_lo[36]) | (S2_lo[36] & S2_hi[36]) | (S2_ca[35] & S2_hi[36]));
	S2_cb[53] = ((S2_cb[52] & S2_is[36]) | (S2_is[36] & S2_lo[53]) | (S2_cb[52] & S2_lo[53]));
	S2_is[37] = S2_lo[54] ^ S2_cb[53] ^ S2_out[54];
	S2_hi[37] = S2_lo[37] ^ S2_ca[36] ^ S2_is[37];
	S2_ca[37] = ((S2_ca[36] & S2_lo[37]) | (S2_lo[37] & S2_hi[37]) | (S2_ca[36] & S2_hi[37]));
	S2_cb[54] = ((S2_cb[53] & S2_is[37]) | (S2_is[37] & S2_lo[54]) | (S2_cb[53] & S2_lo[54]));
	S2_is[38] = S2_lo[55] ^ S2_cb[54] ^ S2_out[55];
	S2_hi[38] = S2_lo[38] ^ S2_ca[37] ^ S2_is[38];
	S2_ca[38] = ((S2_ca[37] & S2_lo[38]) | (S2_lo[38] & S2_hi[38]) | (S2_ca[37] & S2_hi[38]));
	S2_is[39] = S2_lo[39] ^ S2_hi[39] ^ S2_ca[38];
	S2_ca[39] = ((S2_ca[38] & S2_lo[39]) | (S2_lo[39] & S2_hi[39]) | (S2_ca[38] & S2_hi[39]));
	S2_is[40] = S2_lo[40] ^ S2_hi[40] ^ S2_ca[39];
	S2_ca[40] = ((S2_ca[39] & S2_lo[40]) | (S2_lo[40] & S2_hi[40]) | (S2_ca[39] & S2_hi[40]));
	S2_cb[55] = ((S2_cb[54] & S2_is[38]) | (S2_is[38] & S2_lo[55]) | (S2_cb[54] & S2_lo[55]));
	S2_lo[56] = S2_is[39] ^ S2_cb[55] ^ S2_out[56];
	S2_hi[20] = S1_lo[7] ^ S2_lo[56] ^ S2_hi[63];
	S2_lo[20] = S2_hi[20] ^ S2_ca[19] ^ S2_is[20];
	S2_hi[48] = S1_lo[35] ^ S2_lo[20];
	S1_lo[20] = S1_hi[20] ^ S2_hi[48];
	S2_lo[5] = S1_lo[20] ^ S2_hi[33];
	S1_is[20] = S2_hi[48] ^ S1_ca[19];
	S1_lo[37] = S1_is[20] ^ S1_cb[36] ^ S1_out[37];
	S1_hi[37] = S1_lo[37] ^ S2_hi[1];
	S1_is[3] = S1_lo[20] ^ S1_cb[19] ^ S1_out[20];
	S2_is[3] = S2_lo[20] ^ S2_cb[19] ^ S2_out[20];
	S2_lo[3] = S2_hi[3] ^ S2_ca[2] ^ S2_is[3];
	S2_hi[31] = S1_lo[18] ^ S2_lo[3];
	S1_hi[3] = S1_lo[3] ^ S2_hi[31];
	S2_is[50] = S2_hi[3] ^ S2_ca[2] ^ S2_is[3] ^ S2_cb[2] ^ S2_out[3];
	S1_ca[3] = ((S1_ca[2] & S1_lo[3]) | (S1_lo[3] & S1_hi[3]) | (S1_ca[2] & S1_hi[3]));
	S1_ca[20] = ((S1_ca[19] & S1_lo[20]) | (S1_lo[20] & S1_hi[20]) | (S1_ca[19] & S1_hi[20]));
	S1_ca[37] = ((S1_ca[36] & S1_lo[37]) | (S1_lo[37] & S1_hi[37]) | (S1_ca[36] & S1_hi[37]));
	S1_is[38] = S2_hi[2] ^ S1_ca[37];
	S1_lo[55] = S1_is[38] ^ S1_cb[54] ^ S1_out[55];
	S1_hi[55] = S1_lo[55] ^ S2_hi[19];
	S2_hi[4] = S1_lo[55] ^ S2_lo[40] ^ S2_hi[47];
	S1_ca[55] = ((S1_ca[54] & S1_lo[55]) | (S1_lo[55] & S1_hi[55]) | (S1_ca[54] & S1_hi[55]));
	S1_is[56] = S2_hi[20] ^ S1_ca[55];
	S1_lo[9] = S1_is[56] ^ S1_cb[8] ^ S1_out[9];
	S1_hi[9] = S1_lo[9] ^ S2_hi[37];
	S1_cb[9] = ((S1_cb[8] & S1_is[56]) | (S1_is[56] & S1_lo[9]) | (S1_cb[8] & S1_lo[9]));
	S1_cb[20] = ((S1_cb[19] & S1_is[3]) | (S1_is[3] & S1_lo[20]) | (S1_cb[19] & S1_lo[20]));
	S1_cb[37] = ((S1_cb[36] & S1_is[20]) | (S1_is[20] & S1_lo[37]) | (S1_cb[36] & S1_lo[37]));
	S1_cb[55] = ((S1_cb[54] & S1_is[38]) | (S1_is[38] & S1_lo[55]) | (S1_cb[54] & S1_lo[55]));
	S2_ca[3] = ((S2_ca[2] & S2_lo[3]) | (S2_lo[3] & S2_hi[3]) | (S2_ca[2] & S2_hi[3]));
	S2_ca[20] = ((S2_ca[19] & S2_lo[20]) | (S2_lo[20] & S2_hi[20]) | (S2_ca[19] & S2_hi[20]));
	S2_cb[3] = ((S2_cb[2] & S2_is[50]) | (S2_is[50] & S2_lo[3]) | (S2_cb[2] & S2_lo[3]));
	S2_cb[20] = ((S2_cb[19] & S2_is[3]) | (S2_is[3] & S2_lo[20]) | (S2_cb[19] & S2_lo[20]));
	S1_hi[21] = S1_lo[36] ^ S1_lo[19] ^ S2_hi[28] ^ S2_hi[4] ^ S1_ca[3] ^ S1_cb[20] ^ S1_out[21] ^ S2_ca[3] ^ S2_cb[20] ^ S2_out[21];
	S2_cb[56] = ((S2_cb[55] & S2_is[39]) | (S2_is[39] & S2_lo[56]) | (S2_cb[55] & S2_lo[56]));
	S2_lo[57] = S2_is[40] ^ S2_cb[56] ^ S2_out[57];
	S2_hi[21] = S1_lo[8] ^ S2_lo[57] ^ S2_hi[0];
	S2_lo[21] = S2_hi[21] ^ S2_ca[20] ^ S2_is[21];
	S2_hi[49] = S1_lo[36] ^ S2_lo[21] ^ S2_hi[28];
	S1_lo[21] = S1_hi[21] ^ S2_hi[49];
	S2_lo[6] = S1_lo[21] ^ S2_hi[34];
	S1_is[21] = S2_hi[49] ^ S1_ca[20];
	S1_lo[38] = S1_is[21] ^ S1_cb[37] ^ S1_out[38];
	S1_hi[38] = S1_lo[38] ^ S2_hi[2];
	S1_is[4] = S1_hi[21] ^ S2_hi[49] ^ S1_cb[20] ^ S1_out[21];
	S2_hi[32] = S1_ca[3] ^ S1_is[4];
	S1_hi[4] = S1_lo[4] ^ S2_hi[32];
	S2_lo[4] = S1_lo[19] ^ S2_hi[32];
	S2_is[51] = S2_lo[4] ^ S2_cb[3] ^ S2_out[4];
	S2_is[4] = S2_lo[4] ^ S2_hi[4] ^ S2_ca[3];
	S1_ca[4] = ((S1_ca[3] & S1_lo[4]) | (S1_lo[4] & S1_hi[4]) | (S1_ca[3] & S1_hi[4]));
	S1_is[5] = S2_hi[33] ^ S1_ca[4];
	S1_ca[5] = ((S1_ca[4] & S1_lo[5]) | (S1_lo[5] & S1_hi[5]) | (S1_ca[4] & S1_hi[5]));
	S1_is[6] = S2_hi[34] ^ S1_ca[5];
	S1_ca[6] = ((S1_ca[5] & S1_lo[6]) | (S1_lo[6] & S1_hi[6]) | (S1_ca[5] & S1_hi[6]));
	S1_is[7] = S2_hi[35] ^ S1_ca[6];
	S1_ca[7] = ((S1_ca[6] & S1_lo[7]) | (S1_lo[7] & S1_hi[7]) | (S1_ca[6] & S1_hi[7]));
	S1_is[8] = S2_hi[36] ^ S1_ca[7];
	S1_ca[8] = ((S1_ca[7] & S1_lo[8]) | (S1_lo[8] & S1_hi[8]) | (S1_ca[7] & S1_hi[8]));
	S1_is[9] = S2_hi[37] ^ S1_ca[8];
	S1_ca[9] = ((S1_ca[8] & S1_lo[9]) | (S1_lo[9] & S1_hi[9]) | (S1_ca[8] & S1_hi[9]));
	S1_is[10] = S2_hi[38] ^ S1_ca[9];
	S1_ca[21] = ((S1_ca[20] & S1_lo[21]) | (S1_lo[21] & S1_hi[21]) | (S1_ca[20] & S1_hi[21]));
	S1_ca[38] = ((S1_ca[37] & S1_lo[38]) | (S1_lo[38] & S1_hi[38]) | (S1_ca[37] & S1_hi[38]));
	S1_is[39] = S2_hi[3] ^ S1_ca[38];
	S1_lo[56] = S1_is[39] ^ S1_cb[55] ^ S1_out[56];
	S1_hi[56] = S1_lo[56] ^ S2_lo[20] ^ S2_ca[19] ^ S2_is[20];
	S1_ca[56] = ((S1_ca[55] & S1_lo[56]) | (S1_lo[56] & S1_hi[56]) | (S1_ca[55] & S1_hi[56]));
	S1_is[57] = S2_lo[21] ^ S1_ca[56] ^ S2_ca[20] ^ S2_is[21];
	S1_lo[10] = S1_is[57] ^ S1_cb[9] ^ S1_out[10];
	S1_hi[10] = S1_lo[10] ^ S2_hi[38];
	S1_ca[10] = ((S1_ca[9] & S1_lo[10]) | (S1_lo[10] & S1_hi[10]) | (S1_ca[9] & S1_hi[10]));
	S1_is[11] = S2_hi[39] ^ S1_ca[10];
	S1_cb[10] = ((S1_cb[9] & S1_is[57]) | (S1_is[57] & S1_lo[10]) | (S1_cb[9] & S1_lo[10]));
	S1_cb[21] = ((S1_cb[20] & S1_is[4]) | (S1_is[4] & S1_lo[21]) | (S1_cb[20] & S1_lo[21]));
	S1_lo[22] = S1_is[5] ^ S1_cb[21] ^ S1_out[22];
	S2_lo[7] = S1_lo[22] ^ S2_hi[35];
	S1_cb[22] = ((S1_cb[21] & S1_is[5]) | (S1_is[5] & S1_lo[22]) | (S1_cb[21] & S1_lo[22]));
	S1_lo[23] = S1_is[6] ^ S1_cb[22] ^ S1_out[23];
	S2_lo[8] = S1_lo[23] ^ S2_hi[36];
	S1_cb[23] = ((S1_cb[22] & S1_is[6]) | (S1_is[6] & S1_lo[23]) | (S1_cb[22] & S1_lo[23]));
	S1_lo[24] = S1_is[7] ^ S1_cb[23] ^ S1_out[24];
	S2_lo[9] = S1_lo[24] ^ S2_hi[37];
	S1_cb[24] = ((S1_cb[23] & S1_is[7]) | (S1_is[7] & S1_lo[24]) | (S1_cb[23] & S1_lo[24]));
	S1_lo[25] = S1_is[8] ^ S1_cb[24] ^ S1_out[25];
	S2_lo[10] = S1_lo[25] ^ S2_hi[38];
	S1_cb[25] = ((S1_cb[24] & S1_is[8]) | (S1_is[8] & S1_lo[25]) | (S1_cb[24] & S1_lo[25]));
	S1_lo[26] = S1_is[9] ^ S1_cb[25] ^ S1_out[26];
	S2_lo[11] = S1_lo[26] ^ S2_lo[39] ^ S2_ca[38] ^ S2_is[39];
	S1_cb[26] = ((S1_cb[25] & S1_is[9]) | (S1_is[9] & S1_lo[26]) | (S1_cb[25] & S1_lo[26]));
	S1_lo[27] = S1_is[10] ^ S1_cb[26] ^ S1_out[27];
	S2_lo[12] = S1_lo[27] ^ S2_lo[40] ^ S2_ca[39] ^ S2_is[40];
	S1_cb[27] = ((S1_cb[26] & S1_is[10]) | (S1_is[10] & S1_lo[27]) | (S1_cb[26] & S1_lo[27]));
	S1_lo[28] = S1_lo[26] ^ S2_lo[11] ^ S1_ca[10] ^ S1_cb[27] ^ S1_out[28];
	S2_lo[13] = S1_lo[28] ^ S2_hi[41];
	S1_cb[28] = ((S1_cb[27] & S1_is[11]) | (S1_is[11] & S1_lo[28]) | (S1_cb[27] & S1_lo[28]));
	S1_cb[38] = ((S1_cb[37] & S1_is[21]) | (S1_is[21] & S1_lo[38]) | (S1_cb[37] & S1_lo[38]));
	S1_cb[56] = ((S1_cb[55] & S1_is[39]) | (S1_is[39] & S1_lo[56]) | (S1_cb[55] & S1_lo[56]));
	S2_ca[4] = ((S2_ca[3] & S2_lo[4]) | (S2_lo[4] & S2_hi[4]) | (S2_ca[3] & S2_hi[4]));
	S2_ca[21] = ((S2_ca[20] & S2_lo[21]) | (S2_lo[21] & S2_hi[21]) | (S2_ca[20] & S2_hi[21]));
	S2_cb[4] = ((S2_cb[3] & S2_is[51]) | (S2_is[51] & S2_lo[4]) | (S2_cb[3] & S2_lo[4]));
	S2_is[52] = S1_lo[20] ^ S2_hi[33] ^ S2_cb[4] ^ S2_out[5];
	S2_cb[5] = ((S2_cb[4] & S2_is[52]) | (S2_is[52] & S2_lo[5]) | (S2_cb[4] & S2_lo[5]));
	S2_is[53] = S2_lo[6] ^ S2_cb[5] ^ S2_out[6];
	S2_cb[6] = ((S2_cb[5] & S2_is[53]) | (S2_is[53] & S2_lo[6]) | (S2_cb[5] & S2_lo[6]));
	S2_is[54] = S2_lo[7] ^ S2_cb[6] ^ S2_out[7];
	S2_cb[7] = ((S2_cb[6] & S2_is[54]) | (S2_is[54] & S2_lo[7]) | (S2_cb[6] & S2_lo[7]));
	S2_is[55] = S2_lo[8] ^ S2_cb[7] ^ S2_out[8];
	S2_cb[8] = ((S2_cb[7] & S2_is[55]) | (S2_is[55] & S2_lo[8]) | (S2_cb[7] & S2_lo[8]));
	S2_is[56] = S2_lo[9] ^ S2_cb[8] ^ S2_out[9];
	S2_cb[9] = ((S2_cb[8] & S2_is[56]) | (S2_is[56] & S2_lo[9]) | (S2_cb[8] & S2_lo[9]));
	S2_is[57] = S2_lo[10] ^ S2_cb[9] ^ S2_out[10];
	S2_ca[56] = S1_lo[1] ^ S2_lo[57] ^ S2_lo[50] ^ S2_hi[14] ^ S2_is[57];
	S2_ca[57] = ((S2_ca[56] & S2_lo[57]) | (S2_lo[57] & S2_hi[57]) | (S2_ca[56] & S2_hi[57]));
	S2_cb[10] = ((S2_cb[9] & S2_is[57]) | (S2_is[57] & S2_lo[10]) | (S2_cb[9] & S2_lo[10]));
	S2_is[58] = S2_lo[11] ^ S2_cb[10] ^ S2_out[11];
	S2_lo[58] = S2_hi[58] ^ S2_ca[57] ^ S2_is[58];
	S2_hi[22] = S1_lo[9] ^ S2_lo[58] ^ S2_hi[1];
	S2_lo[22] = S1_lo[9] ^ S2_lo[58] ^ S2_hi[1] ^ S2_ca[21] ^ S2_is[22];
	S2_hi[50] = S1_lo[37] ^ S2_lo[22] ^ S2_hi[29];
	S1_hi[22] = S1_lo[22] ^ S2_hi[50];
	S1_is[22] = S2_hi[50] ^ S1_ca[21];
	S1_lo[39] = S1_is[22] ^ S1_cb[38] ^ S1_out[39];
	S2_ca[49] = S2_lo[50] ^ S2_hi[50] ^ S2_is[50];
	S1_hi[39] = S1_lo[54] ^ S1_lo[39] ^ S2_lo[39] ^ S2_hi[46];
	S1_ca[22] = ((S1_ca[21] & S1_lo[22]) | (S1_lo[22] & S1_hi[22]) | (S1_ca[21] & S1_hi[22]));
	S1_ca[39] = ((S1_ca[38] & S1_lo[39]) | (S1_lo[39] & S1_hi[39]) | (S1_ca[38] & S1_hi[39]));
	S1_is[40] = S1_lo[55] ^ S2_lo[40] ^ S2_hi[47] ^ S1_ca[39];
	S1_lo[57] = S1_lo[55] ^ S2_lo[40] ^ S2_hi[47] ^ S1_ca[39] ^ S1_cb[56] ^ S1_out[57];
	S1_hi[57] = S1_lo[57] ^ S1_lo[8] ^ S2_lo[57] ^ S1_ca[35] ^ S1_is[36];
	S1_ca[57] = ((S1_ca[56] & S1_lo[57]) | (S1_lo[57] & S1_hi[57]) | (S1_ca[56] & S1_hi[57]));
	S1_is[58] = S2_hi[22] ^ S1_ca[57];
	S1_lo[11] = S2_hi[22] ^ S1_ca[57] ^ S1_cb[10] ^ S1_out[11];
	S1_hi[11] = S1_lo[11] ^ S1_ca[10] ^ S1_is[11];
	S1_ca[11] = ((S1_ca[10] & S1_lo[11]) | (S1_lo[11] & S1_hi[11]) | (S1_ca[10] & S1_hi[11]));
	S1_is[12] = S1_lo[27] ^ S2_lo[12] ^ S1_ca[11];
	S1_lo[29] = S1_lo[27] ^ S2_lo[12] ^ S1_ca[11] ^ S1_cb[28] ^ S1_out[29];
	S2_lo[14] = S1_lo[29] ^ S1_ca[13] ^ S1_is[14];
	S1_hi[29] = S1_lo[29] ^ S2_lo[57] ^ S2_ca[56] ^ S2_is[57];
	S1_cb[11] = ((S1_cb[10] & S1_is[58]) | (S1_is[58] & S1_lo[11]) | (S1_cb[10] & S1_lo[11]));
	S1_cb[39] = ((S1_cb[38] & S1_is[22]) | (S1_is[22] & S1_lo[39]) | (S1_cb[38] & S1_lo[39]));
	S1_cb[57] = ((S1_cb[56] & S1_is[40]) | (S1_is[40] & S1_lo[57]) | (S1_cb[56] & S1_lo[57]));
	S2_ca[22] = ((S2_ca[21] & S2_lo[22]) | (S2_lo[22] & S2_hi[22]) | (S2_ca[21] & S2_hi[22]));
	S2_ca[50] = ((S2_ca[49] & S2_lo[50]) | (S2_lo[50] & S2_hi[50]) | (S2_ca[49] & S2_hi[50]));
	S2_hi[51] = S2_lo[51] ^ S2_ca[50] ^ S2_is[51];
	S1_hi[23] = S1_lo[23] ^ S2_hi[51];
	S1_is[23] = S2_hi[51] ^ S1_ca[22];
	S2_lo[23] = S1_lo[38] ^ S2_hi[51] ^ S2_hi[30];
	S1_lo[40] = S1_is[23] ^ S1_cb[39] ^ S1_out[40];
	S2_hi[23] = S2_lo[23] ^ S2_ca[22] ^ S2_is[23];
	S2_lo[59] = S1_lo[10] ^ S2_hi[23] ^ S2_hi[2];
	S1_hi[40] = S1_lo[40] ^ S2_hi[62] ^ S2_hi[4] ^ S1_ca[33] ^ S1_is[34];
	S1_ca[23] = ((S1_ca[22] & S1_lo[23]) | (S1_lo[23] & S1_hi[23]) | (S1_ca[22] & S1_hi[23]));
	S1_ca[40] = ((S1_ca[39] & S1_lo[40]) | (S1_lo[40] & S1_hi[40]) | (S1_ca[39] & S1_hi[40]));
	S1_cb[40] = ((S1_cb[39] & S1_is[23]) | (S1_is[23] & S1_lo[40]) | (S1_cb[39] & S1_lo[40]));
	S2_ca[23] = ((S2_ca[22] & S2_lo[23]) | (S2_lo[23] & S2_hi[23]) | (S2_ca[22] & S2_hi[23]));
	S2_ca[51] = ((S2_ca[50] & S2_lo[51]) | (S2_lo[51] & S2_hi[51]) | (S2_ca[50] & S2_hi[51]));
	S2_hi[52] = S2_lo[52] ^ S2_ca[51] ^ S2_is[52];
	S1_hi[24] = S1_lo[24] ^ S2_hi[52];
	S1_is[24] = S2_hi[52] ^ S1_ca[23];
	S1_lo[41] = S1_is[24] ^ S1_cb[40] ^ S1_out[41];
	S2_lo[24] = S1_lo[39] ^ S2_hi[52] ^ S2_hi[31];
	S1_ca[24] = ((S1_ca[23] & S1_lo[24]) | (S1_lo[24] & S1_hi[24]) | (S1_ca[23] & S1_hi[24]));
	S1_cb[41] = ((S1_cb[40] & S1_is[24]) | (S1_is[24] & S1_lo[41]) | (S1_cb[40] & S1_lo[41]));
	S2_ca[52] = ((S2_ca[51] & S2_lo[52]) | (S2_lo[52] & S2_hi[52]) | (S2_ca[51] & S2_hi[52]));
	S2_hi[53] = S2_lo[53] ^ S2_ca[52] ^ S2_is[53];
	S1_hi[25] = S1_lo[25] ^ S2_hi[53];
	S1_is[25] = S2_hi[53] ^ S1_ca[24];
	S1_lo[42] = S1_is[25] ^ S1_cb[41] ^ S1_out[42];
	S2_lo[25] = S1_lo[40] ^ S2_hi[53] ^ S2_hi[32];
	S1_ca[25] = ((S1_ca[24] & S1_lo[25]) | (S1_lo[25] & S1_hi[25]) | (S1_ca[24] & S1_hi[25]));
	S1_cb[42] = ((S1_cb[41] & S1_is[25]) | (S1_is[25] & S1_lo[42]) | (S1_cb[41] & S1_lo[42]));
	S2_ca[53] = ((S2_ca[52] & S2_lo[53]) | (S2_lo[53] & S2_hi[53]) | (S2_ca[52] & S2_hi[53]));
	S2_hi[54] = S2_lo[54] ^ S2_ca[53] ^ S2_is[54];
	S1_hi[26] = S1_lo[26] ^ S2_hi[54];
	S1_is[26] = S2_hi[54] ^ S1_ca[25];
	S1_lo[43] = S1_is[26] ^ S1_cb[42] ^ S1_out[43];
	S2_lo[26] = S1_lo[41] ^ S2_hi[54] ^ S2_hi[33];
	S1_ca[26] = ((S1_ca[25] & S1_lo[26]) | (S1_lo[26] & S1_hi[26]) | (S1_ca[25] & S1_hi[26]));
	S1_cb[43] = ((S1_cb[42] & S1_is[26]) | (S1_is[26] & S1_lo[43]) | (S1_cb[42] & S1_lo[43]));
	S2_ca[54] = ((S2_ca[53] & S2_lo[54]) | (S2_lo[54] & S2_hi[54]) | (S2_ca[53] & S2_hi[54]));
	S2_hi[55] = S2_lo[55] ^ S2_ca[54] ^ S2_is[55];
	S1_hi[27] = S1_lo[27] ^ S2_hi[55];
	S1_is[27] = S2_hi[55] ^ S1_ca[26];
	S1_lo[44] = S1_is[27] ^ S1_cb[43] ^ S1_out[44];
	S2_lo[29] = S1_lo[44] ^ S2_hi[57] ^ S2_hi[36];
	S2_lo[27] = S1_lo[42] ^ S2_hi[55] ^ S2_hi[34];
	S1_ca[27] = ((S1_ca[26] & S1_lo[27]) | (S1_lo[27] & S1_hi[27]) | (S1_ca[26] & S1_hi[27]));
	S1_cb[44] = ((S1_cb[43] & S1_is[27]) | (S1_is[27] & S1_lo[44]) | (S1_cb[43] & S1_lo[44]));
	S2_ca[55] = ((S2_ca[54] & S2_lo[55]) | (S2_lo[55] & S2_hi[55]) | (S2_ca[54] & S2_hi[55]));
	S2_hi[56] = S2_lo[56] ^ S2_ca[55] ^ S2_is[56];
	S1_hi[28] = S1_lo[28] ^ S2_hi[56];
	S1_is[28] = S2_hi[56] ^ S1_ca[27];
	S2_lo[49] = S1_lo[0] ^ S2_hi[56] ^ S2_hi[13];
	S1_lo[45] = S1_is[28] ^ S1_cb[44] ^ S1_out[45];
	S2_lo[30] = S1_lo[45] ^ S2_hi[58] ^ S2_hi[37];
	S2_lo[28] = S1_lo[43] ^ S2_hi[56] ^ S2_hi[35];
	S2_ca[48] = S1_lo[0] ^ S2_hi[56] ^ S2_hi[49] ^ S2_hi[13] ^ S2_is[49];
	S1_ca[28] = ((S1_ca[27] & S1_lo[28]) | (S1_lo[28] & S1_hi[28]) | (S1_ca[27] & S1_hi[28]));
	S1_is[29] = S2_hi[57] ^ S1_ca[28];
	S1_cb[45] = ((S1_cb[44] & S1_is[28]) | (S1_is[28] & S1_lo[45]) | (S1_cb[44] & S1_lo[45]));
	S1_lo[46] = S2_hi[57] ^ S1_ca[28] ^ S1_cb[45] ^ S1_out[46];
	S2_lo[31] = S1_lo[46] ^ S2_hi[59] ^ S2_hi[38];
	S2_ca[58] = ((S2_ca[57] & S2_lo[58]) | (S2_lo[58] & S2_hi[58]) | (S2_ca[57] & S2_hi[58]));
	S2_is[59] = S2_lo[59] ^ S2_hi[59] ^ S2_ca[58];
	S2_cb[11] = S2_lo[59] ^ S2_lo[12] ^ S2_hi[59] ^ S2_ca[58] ^ S2_out[12];
	S2_ca[59] = ((S2_ca[58] & S2_lo[59]) | (S2_lo[59] & S2_hi[59]) | (S2_ca[58] & S2_hi[59]));
	S2_cb[12] = ((S2_cb[11] & S2_is[59]) | (S2_is[59] & S2_lo[12]) | (S2_cb[11] & S2_lo[12]));
	S2_is[60] = S2_lo[13] ^ S2_cb[12] ^ S2_out[13];
	S2_lo[60] = S2_hi[60] ^ S2_ca[59] ^ S2_is[60];
	S2_hi[24] = S1_lo[11] ^ S2_lo[60] ^ S2_hi[3];
	S2_is[24] = S1_lo[11] ^ S2_lo[60] ^ S2_lo[24] ^ S2_hi[3] ^ S2_ca[23];
	S2_lo[41] = S2_is[24] ^ S2_cb[40] ^ S2_out[41];
	S2_hi[5] = S1_lo[56] ^ S2_lo[41] ^ S2_hi[48];
	S1_hi[41] = S1_lo[41] ^ S2_hi[5];
	S1_is[41] = S2_hi[5] ^ S1_ca[40];
	S2_is[5] = S2_lo[5] ^ S2_hi[5] ^ S2_ca[4];
	S1_lo[58] = S2_hi[5] ^ S1_ca[40] ^ S1_cb[57] ^ S1_out[58];
	S1_hi[58] = S1_lo[58] ^ S1_lo[9] ^ S2_lo[58] ^ S2_hi[1];
	S2_is[41] = S1_lo[49] ^ S2_lo[41] ^ S2_lo[34] ^ S2_hi[62] ^ S2_ca[40];
	S2_cb[57] = S2_lo[58] ^ S2_is[41] ^ S2_out[58];
	S2_cb[21] = S2_lo[22] ^ S2_lo[5] ^ S2_hi[5] ^ S2_ca[4] ^ S2_out[22];
	S1_ca[41] = ((S1_ca[40] & S1_lo[41]) | (S1_lo[41] & S1_hi[41]) | (S1_ca[40] & S1_hi[41]));
	S1_ca[58] = ((S1_ca[57] & S1_lo[58]) | (S1_lo[58] & S1_hi[58]) | (S1_ca[57] & S1_hi[58]));
	S1_is[59] = S1_lo[10] ^ S2_lo[59] ^ S2_hi[2] ^ S1_ca[58];
	S1_lo[12] = S1_lo[10] ^ S2_lo[59] ^ S2_hi[2] ^ S1_ca[58] ^ S1_cb[11] ^ S1_out[12];
	S1_hi[12] = S1_lo[12] ^ S2_hi[40];
	S1_cb[12] = ((S1_cb[11] & S1_is[59]) | (S1_is[59] & S1_lo[12]) | (S1_cb[11] & S1_lo[12]));
	S1_cb[58] = ((S1_cb[57] & S1_is[41]) | (S1_is[41] & S1_lo[58]) | (S1_cb[57] & S1_lo[58]));
	S2_ca[5] = ((S2_ca[4] & S2_lo[5]) | (S2_lo[5] & S2_hi[5]) | (S2_ca[4] & S2_hi[5]));
	S2_ca[24] = ((S2_ca[23] & S2_lo[24]) | (S2_lo[24] & S2_hi[24]) | (S2_ca[23] & S2_hi[24]));
	S2_ca[41] = ((S2_ca[40] & S2_lo[41]) | (S2_lo[41] & S2_hi[41]) | (S2_ca[40] & S2_hi[41]));
	S2_ca[60] = ((S2_ca[59] & S2_lo[60]) | (S2_lo[60] & S2_hi[60]) | (S2_ca[59] & S2_hi[60]));
	S2_cb[13] = ((S2_cb[12] & S2_is[60]) | (S2_is[60] & S2_lo[13]) | (S2_cb[12] & S2_lo[13]));
	S2_is[61] = S1_lo[50] ^ S1_lo[29] ^ S2_lo[35] ^ S2_hi[63] ^ S2_cb[13] ^ S2_out[14];
	S2_lo[61] = S1_lo[5] ^ S2_lo[54] ^ S2_hi[18] ^ S2_ca[60] ^ S2_is[61];
	S2_hi[25] = S1_lo[12] ^ S2_lo[61] ^ S2_hi[4];
	S2_is[25] = S1_lo[12] ^ S2_lo[61] ^ S2_lo[25] ^ S2_hi[4] ^ S2_ca[24];
	S2_ca[25] = ((S2_ca[24] & S2_lo[25]) | (S2_lo[25] & S2_hi[25]) | (S2_ca[24] & S2_hi[25]));
	S2_ca[61] = ((S2_ca[60] & S2_lo[61]) | (S2_lo[61] & S2_hi[61]) | (S2_ca[60] & S2_hi[61]));
	S2_lo[62] = S2_hi[62] ^ S2_ca[61] ^ S2_is[62];
	S2_ca[62] = ((S2_ca[61] & S2_lo[62]) | (S2_lo[62] & S2_hi[62]) | (S2_ca[61] & S2_hi[62]));
	S2_lo[63] = S2_hi[63] ^ S2_ca[62] ^ S2_is[63];
	S2_ca[63] = ((S2_ca[62] & S2_lo[63]) | (S2_lo[63] & S2_hi[63]) | (S2_ca[62] & S2_hi[63]));
	S2_cb[22] = ((S2_cb[21] & S2_is[5]) | (S2_is[5] & S2_lo[22]) | (S2_cb[21] & S2_lo[22]));
	S2_is[6] = S2_lo[23] ^ S2_cb[22] ^ S2_out[23];
	S2_hi[6] = S2_lo[6] ^ S2_ca[5] ^ S2_is[6];
	S1_hi[42] = S1_lo[42] ^ S2_hi[6];
	S1_is[42] = S2_hi[6] ^ S1_ca[41];
	S2_lo[42] = S1_lo[57] ^ S2_hi[49] ^ S2_hi[6];
	S1_lo[59] = S2_hi[6] ^ S1_ca[41] ^ S1_cb[58] ^ S1_out[59];
	S1_hi[59] = S1_lo[59] ^ S2_hi[23];
	S2_is[42] = S1_lo[57] ^ S1_lo[29] ^ S2_lo[14] ^ S2_hi[49] ^ S2_hi[6] ^ S2_ca[41];
	S2_cb[58] = S1_lo[57] ^ S1_lo[29] ^ S2_lo[59] ^ S2_lo[14] ^ S2_hi[49] ^ S2_hi[6] ^ S2_ca[41] ^ S2_out[59];
	S2_cb[41] = S1_lo[57] ^ S1_lo[40] ^ S2_hi[53] ^ S2_hi[49] ^ S2_hi[32] ^ S2_hi[25] ^ S2_hi[6] ^ S2_ca[24] ^ S2_out[42];
	S1_ca[42] = ((S1_ca[41] & S1_lo[42]) | (S1_lo[42] & S1_hi[42]) | (S1_ca[41] & S1_hi[42]));
	S1_ca[59] = ((S1_ca[58] & S1_lo[59]) | (S1_lo[59] & S1_hi[59]) | (S1_ca[58] & S1_hi[59]));
	S1_is[60] = S2_hi[24] ^ S1_ca[59];
	S1_lo[13] = S2_hi[24] ^ S1_ca[59] ^ S1_cb[12] ^ S1_out[13];
	S1_hi[13] = S1_lo[28] ^ S1_lo[13] ^ S2_lo[13];
	S2_hi[26] = S1_lo[13] ^ S2_lo[62] ^ S2_hi[5];
	S2_is[26] = S1_lo[41] ^ S2_hi[54] ^ S2_hi[33] ^ S2_hi[26] ^ S2_ca[25];
	S1_cb[13] = ((S1_cb[12] & S1_is[60]) | (S1_is[60] & S1_lo[13]) | (S1_cb[12] & S1_lo[13]));
	S1_cb[59] = ((S1_cb[58] & S1_is[42]) | (S1_is[42] & S1_lo[59]) | (S1_cb[58] & S1_lo[59]));
	S2_ca[6] = ((S2_ca[5] & S2_lo[6]) | (S2_lo[6] & S2_hi[6]) | (S2_ca[5] & S2_hi[6]));
	S2_ca[26] = ((S2_ca[25] & S2_lo[26]) | (S2_lo[26] & S2_hi[26]) | (S2_ca[25] & S2_hi[26]));
	S2_ca[42] = ((S2_ca[41] & S2_lo[42]) | (S2_lo[42] & S2_hi[42]) | (S2_ca[41] & S2_hi[42]));
	S2_cb[23] = ((S2_cb[22] & S2_is[6]) | (S2_is[6] & S2_lo[23]) | (S2_cb[22] & S2_lo[23]));
	S2_is[7] = S1_lo[39] ^ S2_hi[52] ^ S2_hi[31] ^ S2_cb[23] ^ S2_out[24];
	S2_hi[7] = S2_lo[7] ^ S2_ca[6] ^ S2_is[7];
	S1_hi[43] = S1_lo[43] ^ S2_hi[7];
	S1_is[43] = S2_hi[7] ^ S1_ca[42];
	S2_lo[43] = S1_lo[58] ^ S2_hi[50] ^ S2_hi[7];
	S1_lo[60] = S2_hi[7] ^ S1_ca[42] ^ S1_cb[59] ^ S1_out[60];
	S1_hi[60] = S1_lo[60] ^ S2_hi[24];
	S2_is[43] = S1_lo[58] ^ S2_hi[50] ^ S2_hi[43] ^ S2_hi[7] ^ S2_ca[42];
	S2_cb[59] = S1_lo[58] ^ S2_lo[60] ^ S2_hi[50] ^ S2_hi[43] ^ S2_hi[7] ^ S2_ca[42] ^ S2_out[60];
	S2_cb[42] = S1_lo[58] ^ S1_lo[41] ^ S2_hi[54] ^ S2_hi[50] ^ S2_hi[33] ^ S2_hi[26] ^ S2_hi[7] ^ S2_ca[25] ^ S2_out[43];
	S1_ca[43] = ((S1_ca[42] & S1_lo[43]) | (S1_lo[43] & S1_hi[43]) | (S1_ca[42] & S1_hi[43]));
	S1_ca[60] = ((S1_ca[59] & S1_lo[60]) | (S1_lo[60] & S1_hi[60]) | (S1_ca[59] & S1_hi[60]));
	S1_is[61] = S2_hi[25] ^ S1_ca[60];
	S1_lo[14] = S2_hi[25] ^ S1_ca[60] ^ S1_cb[13] ^ S1_out[14];
	S1_hi[14] = S1_lo[14] ^ S2_hi[42];
	S2_hi[27] = S1_lo[14] ^ S2_lo[63] ^ S2_hi[6];
	S1_ca[62] = S2_hi[27] ^ S1_is[63];
	S2_is[27] = S1_lo[42] ^ S2_hi[55] ^ S2_hi[34] ^ S2_hi[27] ^ S2_ca[26];
	S1_cb[14] = ((S1_cb[13] & S1_is[61]) | (S1_is[61] & S1_lo[14]) | (S1_cb[13] & S1_lo[14]));
	S1_is[62] = S1_lo[15] ^ S1_cb[14] ^ S1_out[15];
	S1_ca[61] = S2_hi[26] ^ S1_is[62];
	S1_cb[60] = ((S1_cb[59] & S1_is[43]) | (S1_is[43] & S1_lo[60]) | (S1_cb[59] & S1_lo[60]));
	S2_ca[7] = ((S2_ca[6] & S2_lo[7]) | (S2_lo[7] & S2_hi[7]) | (S2_ca[6] & S2_hi[7]));
	S2_ca[27] = ((S2_ca[26] & S2_lo[27]) | (S2_lo[27] & S2_hi[27]) | (S2_ca[26] & S2_hi[27]));
	S2_is[28] = S1_lo[43] ^ S2_hi[56] ^ S2_hi[35] ^ S2_hi[28] ^ S2_ca[27];
	S2_ca[28] = ((S2_ca[27] & S2_lo[28]) | (S2_lo[28] & S2_hi[28]) | (S2_ca[27] & S2_hi[28]));
	S2_is[29] = S1_lo[44] ^ S2_hi[57] ^ S2_hi[36] ^ S2_hi[29] ^ S2_ca[28];
	S2_ca[29] = ((S2_ca[28] & S2_lo[29]) | (S2_lo[29] & S2_hi[29]) | (S2_ca[28] & S2_hi[29]));
	S2_is[30] = S1_lo[45] ^ S2_hi[58] ^ S2_hi[37] ^ S2_hi[30] ^ S2_ca[29];
	S2_ca[30] = ((S2_ca[29] & S2_lo[30]) | (S2_lo[30] & S2_hi[30]) | (S2_ca[29] & S2_hi[30]));
	S2_is[31] = S1_lo[46] ^ S2_hi[59] ^ S2_hi[38] ^ S2_hi[31] ^ S2_ca[30];
	S2_ca[31] = ((S2_ca[30] & S2_lo[31]) | (S2_lo[31] & S2_hi[31]) | (S2_ca[30] & S2_hi[31]));
	S2_is[32] = S1_lo[47] ^ S2_hi[60] ^ S2_hi[39] ^ S2_hi[32] ^ S2_ca[31];
	S2_cb[48] = S1_lo[47] ^ S1_lo[0] ^ S2_hi[60] ^ S2_hi[56] ^ S2_hi[39] ^ S2_hi[32] ^ S2_hi[13] ^ S2_ca[31] ^ S2_out[49];
	S2_ca[43] = ((S2_ca[42] & S2_lo[43]) | (S2_lo[43] & S2_hi[43]) | (S2_ca[42] & S2_hi[43]));
	S2_cb[24] = ((S2_cb[23] & S2_is[7]) | (S2_is[7] & S2_lo[24]) | (S2_cb[23] & S2_lo[24]));
	S2_is[8] = S1_lo[40] ^ S2_hi[53] ^ S2_hi[32] ^ S2_cb[24] ^ S2_out[25];
	S2_hi[8] = S2_lo[8] ^ S2_ca[7] ^ S2_is[8];
	S1_hi[44] = S1_lo[44] ^ S2_hi[8];
	S1_is[44] = S2_hi[8] ^ S1_ca[43];
	S2_lo[44] = S1_lo[59] ^ S2_hi[51] ^ S2_hi[8];
	S1_lo[61] = S2_hi[8] ^ S1_ca[43] ^ S1_cb[60] ^ S1_out[61];
	S1_hi[61] = S1_lo[61] ^ S2_hi[25];
	S2_is[44] = S1_lo[59] ^ S2_hi[51] ^ S2_hi[44] ^ S2_hi[8] ^ S2_ca[43];
	S2_cb[60] = S1_lo[59] ^ S2_lo[61] ^ S2_hi[51] ^ S2_hi[44] ^ S2_hi[8] ^ S2_ca[43] ^ S2_out[61];
	S2_cb[43] = S1_lo[59] ^ S1_lo[42] ^ S2_hi[55] ^ S2_hi[51] ^ S2_hi[34] ^ S2_hi[27] ^ S2_hi[8] ^ S2_ca[26] ^ S2_out[44];
	S1_ca[44] = ((S1_ca[43] & S1_lo[44]) | (S1_lo[44] & S1_hi[44]) | (S1_ca[43] & S1_hi[44]));
	S1_cb[61] = ((S1_cb[60] & S1_is[44]) | (S1_is[44] & S1_lo[61]) | (S1_cb[60] & S1_lo[61]));
	S2_ca[8] = ((S2_ca[7] & S2_lo[8]) | (S2_lo[8] & S2_hi[8]) | (S2_ca[7] & S2_hi[8]));
	S2_ca[44] = ((S2_ca[43] & S2_lo[44]) | (S2_lo[44] & S2_hi[44]) | (S2_ca[43] & S2_hi[44]));
	S2_cb[25] = ((S2_cb[24] & S2_is[8]) | (S2_is[8] & S2_lo[25]) | (S2_cb[24] & S2_lo[25]));
	S2_is[9] = S1_lo[41] ^ S2_hi[54] ^ S2_hi[33] ^ S2_cb[25] ^ S2_out[26];
	S2_hi[9] = S1_lo[24] ^ S2_hi[37] ^ S2_ca[8] ^ S2_is[9];
	S1_hi[45] = S1_lo[45] ^ S2_hi[9];
	S1_is[45] = S2_hi[9] ^ S1_ca[44];
	S2_lo[45] = S1_lo[60] ^ S2_hi[52] ^ S2_hi[9];
	S1_lo[62] = S2_hi[9] ^ S1_ca[44] ^ S1_cb[61] ^ S1_out[62];
	S1_hi[62] = S1_lo[62] ^ S2_hi[26];
	S2_lo[47] = S1_lo[62] ^ S2_hi[54] ^ S2_hi[11];
	S2_ca[46] = S1_lo[62] ^ S2_hi[54] ^ S2_hi[47] ^ S2_hi[11] ^ S2_is[47];
	S2_is[45] = S1_lo[60] ^ S2_hi[52] ^ S2_hi[45] ^ S2_hi[9] ^ S2_ca[44];
	S2_cb[61] = S1_lo[60] ^ S2_lo[62] ^ S2_hi[52] ^ S2_hi[45] ^ S2_hi[9] ^ S2_ca[44] ^ S2_out[62];
	S2_cb[46] = S1_lo[62] ^ S1_lo[45] ^ S2_hi[58] ^ S2_hi[54] ^ S2_hi[37] ^ S2_hi[30] ^ S2_hi[11] ^ S2_ca[29] ^ S2_out[47];
	S2_cb[44] = S1_lo[60] ^ S1_lo[43] ^ S2_hi[56] ^ S2_hi[52] ^ S2_hi[35] ^ S2_hi[9] ^ S1_is[0] ^ S2_ca[27] ^ S2_out[45];
	S1_ca[45] = ((S1_ca[44] & S1_lo[45]) | (S1_lo[45] & S1_hi[45]) | (S1_ca[44] & S1_hi[45]));
	S1_cb[62] = ((S1_cb[61] & S1_is[45]) | (S1_is[45] & S1_lo[62]) | (S1_cb[61] & S1_lo[62]));
	S2_ca[9] = ((S2_ca[8] & S2_lo[9]) | (S2_lo[9] & S2_hi[9]) | (S2_ca[8] & S2_hi[9]));
	S2_ca[45] = ((S2_ca[44] & S2_lo[45]) | (S2_lo[45] & S2_hi[45]) | (S2_ca[44] & S2_hi[45]));
	S2_ca[47] = ((S2_ca[46] & S2_lo[47]) | (S2_lo[47] & S2_hi[47]) | (S2_ca[46] & S2_hi[47]));
	S2_lo[48] = S2_hi[48] ^ S2_ca[47] ^ S2_is[48];
	S1_lo[63] = S2_lo[48] ^ S2_hi[55] ^ S2_hi[12];
	S1_hi[63] = S1_lo[63] ^ S2_hi[27];
	S1_is[46] = S1_lo[63] ^ S1_cb[62] ^ S1_out[63];
	S2_hi[10] = S1_ca[45] ^ S1_is[46];
	S1_hi[46] = S1_lo[46] ^ S2_hi[10];
	S2_lo[46] = S1_lo[61] ^ S2_hi[53] ^ S2_hi[10];
	S2_is[10] = S1_lo[25] ^ S2_hi[38] ^ S2_hi[10] ^ S2_ca[9];
	S2_is[46] = S1_lo[61] ^ S2_hi[53] ^ S2_hi[46] ^ S2_hi[10] ^ S2_ca[45];
	S2_cb[26] = S1_lo[42] ^ S1_lo[25] ^ S2_hi[55] ^ S2_hi[38] ^ S2_hi[34] ^ S2_hi[10] ^ S2_ca[9] ^ S2_out[27];
	S2_cb[62] = S1_lo[61] ^ S1_lo[14] ^ S2_hi[53] ^ S2_hi[46] ^ S2_hi[27] ^ S2_hi[10] ^ S2_hi[6] ^ S2_ca[45] ^ S2_out[63];
	S2_cb[47] = S1_lo[63] ^ S1_lo[46] ^ S2_hi[59] ^ S2_hi[55] ^ S2_hi[38] ^ S2_hi[31] ^ S2_hi[12] ^ S2_ca[30] ^ S2_out[48];
	S2_cb[45] = S1_lo[61] ^ S1_lo[44] ^ S2_hi[57] ^ S2_hi[53] ^ S2_hi[36] ^ S2_hi[29] ^ S2_hi[10] ^ S2_ca[28] ^ S2_out[46];
	S1_ca[63] = ((S1_ca[62] & S1_lo[63]) | (S1_lo[63] & S1_hi[63]) | (S1_ca[62] & S1_hi[63]));
	S1_cb[63] = ((S1_cb[62] & S1_is[46]) | (S1_is[46] & S1_lo[63]) | (S1_cb[62] & S1_lo[63]));
	S2_ca[10] = ((S2_ca[9] & S2_lo[10]) | (S2_lo[10] & S2_hi[10]) | (S2_ca[9] & S2_hi[10]));
	S2_is[11] = S1_lo[26] ^ S2_hi[39] ^ S2_hi[11] ^ S2_ca[10];
	S2_cb[27] = S1_lo[43] ^ S1_lo[26] ^ S2_hi[56] ^ S2_hi[39] ^ S2_hi[35] ^ S2_hi[11] ^ S2_ca[10] ^ S2_out[28];
	S2_ca[11] = ((S2_ca[10] & S2_lo[11]) | (S2_lo[11] & S2_hi[11]) | (S2_ca[10] & S2_hi[11]));
	S2_is[12] = S1_lo[27] ^ S2_hi[40] ^ S2_hi[12] ^ S2_ca[11];
	S2_cb[28] = S1_lo[44] ^ S1_lo[27] ^ S2_hi[57] ^ S2_hi[40] ^ S2_hi[36] ^ S2_hi[12] ^ S2_ca[11] ^ S2_out[29];
	S2_ca[12] = ((S2_ca[11] & S2_lo[12]) | (S2_lo[12] & S2_hi[12]) | (S2_ca[11] & S2_hi[12]));
	S2_is[13] = S1_lo[28] ^ S2_hi[41] ^ S2_hi[13] ^ S2_ca[12];
	S2_cb[29] = S1_lo[45] ^ S1_lo[28] ^ S2_hi[58] ^ S2_hi[41] ^ S2_hi[37] ^ S2_hi[13] ^ S2_ca[12] ^ S2_out[30];
	S2_ca[13] = ((S2_ca[12] & S2_lo[13]) | (S2_lo[13] & S2_hi[13]) | (S2_ca[12] & S2_hi[13]));
	S2_is[14] = S1_lo[29] ^ S2_hi[42] ^ S2_hi[14] ^ S2_ca[13];
	S2_cb[30] = S1_lo[46] ^ S1_lo[29] ^ S2_hi[59] ^ S2_hi[42] ^ S2_hi[38] ^ S2_hi[14] ^ S2_ca[13] ^ S2_out[31];
	S2_cb[63] = ((S2_cb[62] & S2_is[46]) | (S2_is[46] & S2_lo[63]) | (S2_cb[62] & S2_lo[63]));
	//  END   ▲▲▲▲▲
	// ----------------------------------------------------------------------------

	simd Sum1[64];
	simd carry;
#define ADD(sumi, xi, yi) carry = 0; foreach([&]<size_t i> { \
		auto x = xi, y = yi; \
		sumi = x ^ y ^ carry; \
		carry = (x & y) | (x & carry) | (y & carry); }, std::make_index_sequence<64>{});
	simd Sum2[64];
	ADD(Sum1[i], S1_lo[i], S1_hi[i]);
	ADD(Sum2[i], Sum1[(i + 64 - 17) % 64], S1_lo[i]);
	simd ok = ~uint64_t{};
	foreach([&]<size_t i> {
			ok &= ~(Sum2[i] ^ S1_out[i]);
			}, std::make_index_sequence<64>{});
	if(stdx::reduce(ok, std::bit_or<>{}) == 0)
		return false;
	simd Sum3[64];
	simd Sum4[64];
	ADD(Sum3[i], S2_lo[i], S2_hi[i]);
	ADD(Sum4[i], Sum3[(i + 64 - 17) % 64], S2_lo[i]);
	ok = ~uint64_t{};
	foreach([&]<size_t i> {
			ok &= ~(Sum4[i] ^ S2_out[i]);
			}, std::make_index_sequence<64>{});
	if(stdx::reduce(ok, std::bit_or<>{}) == 0)
		return false;
	if constexpr(FULL) {
		uint64_t result_lo[SIMD_BITS] {};
		uint64_t result_hi[SIMD_BITS] {};
		
		for(int j = 0; j < SIMD_BITS; ++j)
		for (int i = 0; i < 64; ++i) {
			result_lo[j] |= (static_cast<uint64_t>(S1_lo[i][j / 64]) >> (j % 64) & 1ULL) << i;
			result_hi[j] |= (static_cast<uint64_t>(S1_hi[i][j / 64]) >> (j % 64) & 1ULL) << i;
		}
	
		for(int j = 0; j < SIMD_BITS; ++j)
			check(result_lo[j], result_hi[j], guess + j);
	}
	return true;
}

void KERNEL_NAME(uint64_t nextlong1, uint64_t nextlong2)
{
	::nextlong1 = nextlong1;
	::nextlong2 = nextlong2;
#pragma omp parallel for schedule(dynamic, 32768)
	for(uint64_t i = 0; i < uint64_t{1} << 40; i += SIMD_BITS) {
		bool x = bruteKernel<false>(i);
		if(x) [[unlikely]] {
			bruteKernel<true>(i);
		}
	}
}
