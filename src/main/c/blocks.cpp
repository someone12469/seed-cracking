#include "blocks.h"
#include "xsinv.h"
#include "java_helper.h"
#include <array>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <execution>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <bit>
#include <map>
#include <set>
#include <span>
#include <utility>
#include <z3++.h>

uint64_t getSeed(int x, int y, int z)
{
	// obviously the first multiplication is 32-bit while the second is 64-bit
		uint64_t seed = int64_t(int32_t(uint32_t(x) * 3129871u) ^ int64_t(uint64_t(z) * 116129781u) ^ int32_t(y));
		return int64_t(((seed * seed) * 42317861) + (seed * 11)) >> 16; // signed right shift
}

template<int ITER>
bool get(uint64_t seedlo, uint64_t seedhi, uint64_t xv, double p)
{
	uint64_t s0 = xv, s1 = 0;
	for(size_t i = 0; i < ITER; i++) {
		uint64_t s12 = s0 ^ s1;
		s0 = std::rotl(s0, 49) ^ s12 ^ s12 << 21;
		s1 = std::rotl(s12, 28);
	}
	seedlo ^= s0;
	seedhi ^= s1;
	uint64_t out = std::rotl(seedlo + seedhi, 17) + seedlo;
	float r = (out >> 40) * 0x1.0p-24;
	return double(r) < p;
}

struct beam_entry {
	size_t nbits;
	uint64_t w1;
	uint64_t w2;
	uint64_t w3;
	int64_t score;
	std::vector<beam_entry> next() {
		std::vector<beam_entry> ret;
		if(nbits < 17)
			for(size_t i = 0; i < 8; i++)
				ret.push_back({nbits + 1, 2 * w1 + i / 4, 2 * w2 + i / 2 % 2, 2 * w3 + i % 2, 0});
		else
			for(size_t i = 0; i < 4; i++)
				ret.push_back({nbits + 1, 2 * w1, 2 * w2 + i / 2 % 2, 2 * w3 + i % 2, 0});
		return ret;
	}
};

std::vector<std::bitset<128>> solve(std::vector<std::tuple<uint64_t, double, bool>> xvs, int mode)
{
	if(xvs.size() == 0)
		return {};
	std::vector<beam_entry> cur { {0, 0, 0, 0, 0} };
	std::vector<beam_entry> next;
	size_t iters = 0;
	for(; iters < 21; iters++)
	{
		for(beam_entry e : cur) {
			// printf("At: %zu %zu %zu %zu %zd\n", e.nbits, e.w1, e.w2, e.w3, e.score);
			for(auto to : e.next()) {
				next.push_back(to);
			}
		}
#pragma omp parallel for
		for(beam_entry& to : next) {
			int64_t score = 0;
			uint64_t seedlo = to.w1 << (64-to.nbits) | to.w2 << (64-17-to.nbits);
			uint64_t seedhi = to.w3 << (64-17-to.nbits);
			if(mode <= 3)
				for(auto[xv,p,ret] : xvs)
					score += get<0>(seedlo, seedhi, xv, p) != ret;
			else if(mode == 4)
				for(auto[xv,p,ret] : xvs)
					score += get<1>(seedlo, seedhi, xv, p) != ret;
			else if(mode == 5)
				for(auto[xv,p,ret] : xvs)
					score += (get<2>(seedlo, seedhi, xv, p) != ret) * (ret ? 50 : 1); // increase weight of rare class
			to.score = score;
			//printf("To: %zu %zu %zu %zu %zd\n", to.nbits, to.w1, to.w2, to.w3, to.score);
		};
		std::ranges::stable_sort(next.begin(), next.end(), std::less<>{}, &beam_entry::score); // stability appears to be important
		size_t keep = 1000;
		if(next.size() > keep)
			next.erase(next.begin() + keep, next.end());
		if(iters > 6 && next.front().score >= next.back().score * 0.95) { // no point in continuing
			break; // use cur, do not proceed to next
		}
		cur = std::exchange(next, {});
	}
	std::vector<std::bitset<128>> ret;
	auto push = [&ret](uint64_t a, uint64_t b) {
		std::bitset<128> x;
		for(size_t i = 0; i < 64; i++) // of course this does not get optimized
			x[i] = a >> i & 1;
		for(size_t i = 0; i < 64; i++)
			x[64 + i] = b >> i & 1;
		ret.push_back(x);
	};
	// first returned element = mask where something is known
	// all others = candidates
	uint64_t itermask = (1ull << iters) - 1;
	uint64_t km0 = itermask << (64 - iters) | itermask << (64 - iters - 17), km1 = itermask << (64 - iters - 17);
	push(km0, km1);
	for(auto[nb,w1,w2,w3,score] : cur) {
		uint64_t c0 = w1 << (64 - iters) | w2 << (64 - iters - 17), c1 = w3 << (64 - iters - 17);
		assert((c0 &~ km0) == 0);
		assert((c1 &~ km1) == 0);
		push(c0, c1);
	}
	return ret;
}

using point3 = std::tuple<int,int,int>;
z3::expr Rnd(z3::expr S0, z3::expr S1)
{
	return (S0 + S1).rotate_left(17) + S0;
}
std::pair<z3::expr, z3::expr> Step(z3::expr S0, z3::expr S1)
{
	auto S12 = S0 ^ S1;
	return {S0.rotate_left(49) ^ S12 ^ z3::shl(S12, 21), S12.rotate_left(28)};
}

void bedrockseed$crack(int size, block_entry* entries, int mode)
{
	assert(1 <= mode && mode <= 31);
	std::map<point3, block_type> blocks;
	std::vector<std::tuple<uint64_t, double, bool>> xvs1, xvs2, xvs3, xvs4, xvs5;
	std::set<point3> maybe_veins;
	for(uint64_t entry_idx = 0; entry_idx < size; ++entry_idx) {
		auto entry = entries[entry_idx];
		blocks[{entry.x, entry.y, entry.z}] = (block_type)entry.typ;
	}
	for(auto[coord, typ] : blocks) {
		auto[x,y,z] = coord;
		uint64_t xv = getSeed(x, y, z);
		if(mode & 1) {
			if(-63 <= y && y <= -60)
			{
				float thrs[] {0.8, 0.6, 0.4, 0.19999999999999996 };
				float thr = thrs[y + 63];
				if(typ == BEDROCK) {
					// printf("%d %d %d: bedrock xv=%lld\n", x, y, z, xv);
					xvs1.push_back({xv, thr, 1});
				} else if(typ == DEEPSLATE) {
					// printf("%d %d %d: deepslate xv=%lld\n", x, y, z, xv);
					xvs1.push_back({xv, thr, 0});
				}
			}
		}
		if(mode & 2) {
			if(1 <= y && y <= 7)
			{
				float thrs[] {0.875, 0.75, 0.625, 0.5, 0.375, 0.25, 0.125};
				float thr = thrs[y - 1];
				if(typ == DEEPSLATE) {
					// printf("%d %d %d: deepslate xv=%lld\n", x, y, z, xv);
					xvs2.push_back({xv, thr, 1});
				} else if(typ == STONE) {
					// printf("%d %d %d: stone xv=%lld\n", x, y, z, xv);
					xvs2.push_back({xv, thr, 0});
				}
			}
		}
		// identify veins. heuristic 1: investigate only 7x7x7 boxes around raw ore blocks for being parts of veins
		if(typ == RAW) {
			// printf("Raw: %d %d %d\n", x, y, z);
			for(int dx = -3; dx <= 3; dx++)
			for(int dy = -3; dy <= 3; dy++)
			for(int dz = -3; dz <= 3; dz++)
				maybe_veins.insert({x + dx, y + dy, z + dz});
		}
	}
	for(auto[x,y,z] : maybe_veins) {
		// check if the block is actually part of a vein, based on what it is adjacent to
		int adj_ore = 0, adj_filler = 0, adj_raw = 0;
		for(int dx = -1; dx <= 1; dx++)
		for(int dy = -1; dy <= 1; dy++)
		for(int dz = -1; dz <= 1; dz++)
		{
			auto it = blocks.find({x+dx, y+dy, z+dz});
			if(it == blocks.end())
				continue;
			block_type typ = it->second;
			adj_raw += typ == RAW;
			adj_ore += typ == ORE;
			adj_filler += typ == FILL;
		}
		if(adj_ore + adj_filler + 3 * adj_raw < 18)
			continue;
		if(adj_filler == 0 || (adj_ore == 0 && adj_raw == 0)) // if we passed above but not this, we're probably in nearby normal ore / filler
			continue;
		// vein confirmed. 
		uint64_t xv = getSeed(x, y, z);
		auto it = blocks.find({x, y, z});
		if(mode & 4) { // identify vein vs skipped. 70% is replaced by vein (filler / ore / raw), 30% is left as is
			bool vein = false;
			if(it != blocks.end()) {
				block_type typ = it->second;
				vein |= typ == RAW || typ == ORE || typ == FILL;
			}
			// printf("%d %d %d: vein (%d %d %d), replaced=%d xv=%lld\n", x, y, z, adj_ore, adj_filler, adj_raw, (int)vein, xv);
			xvs3.push_back({xv, 0.7, vein});
		}
		if(mode & 8) { // identify filler vs ore among those that passed previous step
			// vein richness varies from 0.1 to 0.3 so assume the worst
			// ore is placed if rand() < richness
			if(it == blocks.end())
				continue; // not needed by next either
			if(it->second == RAW || it->second == ORE) {
				// printf("%d %d %d: vein (%d %d %d), ore xv=%lld\n", x, y, z, adj_ore, adj_filler, adj_raw, xv);
				xvs4.push_back({xv, 0.3, true});
			}
			else if(it->second == FILL) {
				// printf("%d %d %d: vein (%d %d %d), fill xv=%lld\n", x, y, z, adj_ore, adj_filler, adj_raw, xv);
				xvs4.push_back({xv, 0.1, false});
			}
		}
		if(mode & 16) { // identify ore vs raw block
			if(it == blocks.end())
				continue;
			if(it->second == RAW || it->second == ORE) {
				// printf("%d %d %d: vein (%d %d %d), raw=%d xv=%lld\n", x, y, z, adj_ore, adj_filler, adj_raw, int(it->second == RAW), xv);
				xvs5.push_back({xv, 0.02, it->second == RAW});
			}
		}
	}
	auto sol1 = solve(xvs1, 1);
	sendUpdate("commands.bedrockcrack.completedBitSearch1", false);
	auto sol2 = solve(xvs2, 2);
	sendUpdate("commands.bedrockcrack.completedBitSearch2", false);
	auto sol3 = solve(xvs3, 3);
	sendUpdate("commands.bedrockcrack.completedBitSearch3", false);
	auto sol4 = solve(xvs4, 4);
	sendUpdate("commands.bedrockcrack.completedBitSearch4", false);
	auto sol5 = solve(xvs5, 5);
	sendUpdate("commands.bedrockcrack.completedBitSearch5", false);
	// I = unknown
	// S = I ^ BEDROCK, T = I ^ DEEPSLATE, V = I ^ ORE
	// O0 = forkPositional(S), O1 = forkPositional(T), O2 = forkPositional(V)
	// O3 = step(O2), O4 = step(O3)
	// our solutions above leak data from Os
	z3::context ctx;
	z3::solver solver(ctx);

	auto I0 = ctx.bv_const("I0", 64);
	auto I1 = ctx.bv_const("I1", 64);
	auto S0 = I0 ^ ctx.bv_val("13544455532117611141", 64u);
	auto S1 = I1 ^ ctx.bv_val("14185350335435586452", 64u);
	auto T0 = I0 ^ ctx.bv_val("10411719568726253007", 64u);
	auto T1 = I1 ^ ctx.bv_val("14964796469053385315", 64u);
	auto V0 = I0 ^ ctx.bv_val("11207227798492025197", 64u);
	auto V1 = I1 ^ ctx.bv_val("3091299299654006625", 64u);
	auto O0 = Rnd(S0, S1);
	auto[S2, S3] = Step(S0, S1);
	auto O1 = Rnd(S2, S3);
	auto O2 = Rnd(T0, T1);
	auto[T2, T3] = Step(T0, T1);
	auto O3 = Rnd(T2, T3);
	auto O4 = Rnd(V0, V1);
	auto[V2, V3] = Step(V0, V1);
	auto O5 = Rnd(V2, V3);
	auto[O6, O7] = Step(O4, O5);
	auto[O8, O9] = Step(O6, O7);

	auto constrain = [&](std::span<const std::bitset<128>> cons, z3::expr v0, z3::expr v1) {
		const std::bitset<128>& mask = cons[0];
		uint64_t mask_low = 0;
		uint64_t mask_high = 0;
		for (int i = 0; i < 64; ++i) {
			if (mask[i]) {
				mask_low |= (uint64_t(1) << i);
			}
		}
		for (int i = 64; i < 128; ++i) {
			if (mask[i]) {
				mask_high |= (uint64_t(1) << (i - 64));
			}
		}
		z3::expr v0_masked = v0 & ctx.bv_val(mask_low, 64u);
		z3::expr v1_masked = v1 & ctx.bv_val(mask_high, 64u);

		z3::expr clause = ctx.bool_val(false);
		for (int i = 0; i < cons.size() - 1; i++) {
			const std::bitset<128>& cur = cons[i + 1];
			const std::bitset<128> masked = cur & mask;

			uint64_t low = 0;
			uint64_t high = 0;
			for (int i = 0; i < 64; ++i) {
				if (masked[i]) {
					low |= (uint64_t(1) << i);
				}
			}
			for (int i = 64; i < 128; ++i) {
				if (masked[i]) {
					high |= (uint64_t(1) << (i - 64));
				}
			}

			clause = clause || (v0_masked == ctx.bv_val(low, 64u) && v1_masked == ctx.bv_val(high, 64u));
		}
		solver.add(clause);
	};
	if(sol1.size()) constrain(sol1, O0, O1);
	if(sol2.size()) constrain(sol2, O2, O3);
	if(sol3.size()) constrain(sol3, O4, O5);
	if(sol4.size()) constrain(sol4, O6, O7);
	if(sol5.size()) constrain(sol5, O8, O9);
	sendUpdate("commands.bedrockcrack.startingZ3Search", false);
	z3::check_result res = solver.check();
	if (res == z3::unknown) {
		sendUpdate("commands.bedrockcrack.Z3Failed", false);
		return;
	}
	if (res == z3::unsat) {
		sendUpdate("commands.bedrockcrack.Z3Unsat", false);
		return;
	}
	assert(res == z3::sat);
	z3::model m = solver.get_model();
	uint64_t i0 = m.eval(I0).get_numeral_uint64(), i1 = m.eval(I1).get_numeral_uint64();
	sendUpdate("commands.bedrockcrack.crackedRandomStateRandom", false, i0, i1);
	crackFP(i0, i1);
}
