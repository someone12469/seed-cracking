#include <cassert>
#include <cstdint>
#include <cstdio>
#include <map>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <unordered_set>
#include <vector>
struct point3i {
	int x, y, z;
	std::strong_ordering operator<=>(const point3i& rhs) const = default;
	bool operator<(const point3i& rhs) const = default;
};
struct point3 {
	double x, y, z;
};
constexpr uint64_t lcg(uint64_t seed, uint64_t c) {
	return seed * (seed * 6364136223846793005ull + 1442695040888963407ull) + c;
}
constexpr double fiddle1(uint64_t seed) {
	seed >>= 24;
	seed %= 1024;
	return (seed / 1024.0 - 0.5) * 0.9;
}
constexpr point3 fiddle(uint64_t seed, int x, int y, int z) {
	uint64_t rval = lcg(lcg(lcg(lcg(lcg(lcg(seed, x), y), z), x), y), z);
	double fx = fiddle1(rval);
	rval = lcg(rval, seed);
	double fy = fiddle1(rval);
	rval = lcg(rval, seed);
	double fz = fiddle1(rval);
	return {fx, fy, fz};
}
int getBiome(uint64_t seed, int x, int y, int z) {
	int absX = x - 2, absY = y - 2, absZ = z - 2;
	int parentX = absX >> 2, parentY = absY >> 2, parentZ = absZ >> 2;
	double fractX = (absX & 3) / 4.0;
	double fractY = (absY & 3) / 4.0;
	double fractZ = (absZ & 3) / 4.0;
	int minI = 0;
	double minFiddledDistance = 1e308;
	for(int i = 0; i < 8; i++) {
		bool xEven = (i & 4) == 0;
		bool yEven = (i & 2) == 0;
		bool zEven = (i & 1) == 0;
		int cx = xEven ? parentX : parentX + 1;
		int cy = yEven ? parentY : parentY + 1;
		int cz = zEven ? parentZ : parentZ + 1;
		double dx = xEven ? fractX : fractX - 1.0;
		double dy = yEven ? fractY : fractY - 1.0;
		double dz = zEven ? fractZ : fractZ - 1.0;
		auto[fx, fy, fz] = fiddle(seed, cx, cy, cz);
		double next = (dz+fz)*(dz+fz) + (dy+fy)*(dy+fy) + (dx+fx)*(dx+fx);
		if(minFiddledDistance > next) {
			minI = i;
			minFiddledDistance = next;
		}
	}
	return minI;
}
struct block_info { 
	int x, y, z;
	int srcs[8];
	std::vector<int> ctr_inds;
};
struct ctr_info { 
	int max0, max1;
	int x, y, z, range;
};
int getBiome(const block_info& bi, std::span<const point3> fiddles) {
	int x = bi.x, y = bi.y, z = bi.z;
	int absX = x - 2, absY = y - 2, absZ = z - 2;
	double fractX = (absX & 3) / 4.0;
	double fractY = (absY & 3) / 4.0;
	double fractZ = (absZ & 3) / 4.0;
	int minI = -1;
	double minFiddledDistance = 1e308;
	for(int i = 0; i < 8; i++) {
		int si = bi.srcs[i];
		bool xEven = (i & 4) == 0;
		bool yEven = (i & 2) == 0;
		bool zEven = (i & 1) == 0;
		double dx = xEven ? fractX : fractX - 1.0;
		double dy = yEven ? fractY : fractY - 1.0;
		double dz = zEven ? fractZ : fractZ - 1.0;
		auto[fx, fy, fz] = fiddles[si];
		double next = (dz+fz)*(dz+fz) + (dy+fy)*(dy+fy) + (dx+fx)*(dx+fx);
		if(minFiddledDistance > next) {
			minI = si;
			minFiddledDistance = next;
		}
	}
	return minI;
}
int main()
{
	setlinebuf(stdout);
	std::map<point3i, std::vector<int>> uniq_blocks; // the quarts that we need fiddles for
	std::vector<block_info> in_blocks;
	std::vector<ctr_info> in_ctrs;
	size_t N; scanf("%zu", &N);
	for(size_t i = 0; i < N; i++) {
		int x, y, z, range, mincnt, maxcnt;
		scanf("%d%d%d%d%d%d", &x, &y, &z, &range, &mincnt, &maxcnt);
		assert(range % 2 == 1);
		for(int dx = -range / 2; dx <= range / 2; dx++)
		for(int dz = -range / 2; dz <= range / 2; dz++)
		{
			uniq_blocks[{x+dx, y, z+dz}].push_back(i);
		}
		in_ctrs.push_back({range * range - mincnt, maxcnt});
	}
	for(const auto&[b, to] : uniq_blocks) {
		auto[x,y,z] = b;
		in_blocks.push_back({x, y, z, {}, to});
	}
	std::set<point3i> uniq_rel; // the quarts that we need fiddles for
	std::set<point3i> uniq_rel_2d; // the xz of them
	for(auto[x,y,z,_,_2] : in_blocks) {
		int px = (x - 2) >> 2, py = (y - 2) >> 2, pz = (z - 2) >> 2;
		for(int dx = 0; dx < 2; dx++)
		for(int dy = 0; dy < 2; dy++)
		for(int dz = 0; dz < 2; dz++)
		{
			uniq_rel.insert({px + dx, py + dy, pz + dz});
			uniq_rel_2d.insert({px + dx, pz + dz});
		}
	}
	printf("blocks %zu flat %zu\n", uniq_rel.size(), uniq_rel_2d.size());
	std::vector<point3i> rel_quarts(uniq_rel.begin(), uniq_rel.end());
	std::vector<point3i> rel_quarts_2d(uniq_rel_2d.begin(), uniq_rel_2d.end());
	std::vector<int> flat_idx(rel_quarts.size());
	for(size_t i = 0; i < rel_quarts.size(); i++) {
		auto[x,y,z] = rel_quarts[i];
		auto it = std::ranges::find(rel_quarts_2d, {x,z});
		assert(it != rel_quarts_2d.end());
		flat_idx[i] = it - rel_quarts_2d.begin();
		//printf("flat_idx[%d = %d %d %d] = %d\n", i, x, y, z, flat_idx[i]);
	}
	uniq_rel = {};
	// assign block srcs precisely
	for(auto&[x,y,z,srcs,_] : in_blocks) {
		int px = (x - 2) >> 2, py = (y - 2) >> 2, pz = (z - 2) >> 2;
		for(int i = 0; i < 8; i++) {
			bool xEven = (i & 4) == 0;
			bool yEven = (i & 2) == 0;
			bool zEven = (i & 1) == 0;
			int cx = xEven ? px : px + 1;
			int cy = yEven ? py : py + 1;
			int cz = zEven ? pz : pz + 1;
			auto it = std::ranges::find(rel_quarts, {cx,cy,cz});
			assert(it != rel_quarts.end());
			srcs[i] = it - rel_quarts.begin();
			//printf("%d %d %d %d %d: %d <- %d (%d)\n", x,y,z,r,c, i, srcs[i], flat_idx[srcs[i]]);
		}
	}

//	uint64_t oklist [] {
//#include "oklist.txt"
//	};
//	for(uint64_t si : oklist)
#pragma omp parallel for schedule(dynamic, 64)
	for(uint64_t si = 0; si < 1ull<<34; si++)
	{
		if(si % 1048576 == 0) {
			printf("Si %zu\n", si);
		}
		std::vector<point3> fiddles(rel_quarts.size());
		uint64_t seed = si;
		for(size_t i = 0; i < fiddles.size(); i++)
			fiddles[i] = fiddle(seed, rel_quarts[i].x, rel_quarts[i].y, rel_quarts[i].z);
		// given fiddles, compute srcs for each block
		std::vector<int> srcs(in_blocks.size());
		std::vector<std::vector<int>> inv_src(rel_quarts_2d.size());
		for(size_t i = 0; i < srcs.size(); i++) {
			srcs[i] = flat_idx[getBiome(in_blocks[i], fiddles)];
			for(int j : in_blocks[i].ctr_inds)
				inv_src[srcs[i]].push_back(j);
		}

		
		std::vector<int> cur0(in_ctrs.size()), cur1(in_ctrs.size());
		std::vector<int> tmpbufs(rel_quarts_2d.size() * 2 * in_ctrs.size());
		bool is_ok = false;
		uint64_t ok_mask = 0;
		auto dfs = [&](this auto&& self, size_t i, size_t mask, const int* cur0, const int* cur1) -> void {
			if(i == rel_quarts_2d.size()) {
				//printf("OK %zu %zu\n", seed, mask);
				is_ok = true;
				ok_mask = mask;
				return;
			}
			auto oc0 = cur0, oc1 = cur1;
			auto nc0 = &tmpbufs[2 * i * in_ctrs.size()];
			auto nc1 = &tmpbufs[(2 * i + 1) * in_ctrs.size()];
			for(size_t i = 0; i < in_ctrs.size(); i++) {
				nc0[i] = cur0[i];
				nc1[i] = cur1[i];
			}
			// guess bit is 0
			bool ok0 = true, ok1 = true;
			for(int to : inv_src[i]) {
				nc0[to] += 1;
				nc1[to] += 1;
				ok0 &= nc0[to] <= in_ctrs[to].max0;
				ok1 &= nc1[to] <= in_ctrs[to].max1;
			}
			if(ok0) {
				self(i + 1, mask, nc0, oc1);
			}
			if(ok1) {
				self(i + 1, mask | 1zu << i, oc0, nc1);
			}
		};
		dfs(0, 0, cur0.data(), cur1.data());
		if(is_ok)
			printf("OK %zu %zu\n", seed, ok_mask);
	}
}
