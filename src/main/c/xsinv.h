#pragma once
#include <cstdint>
void crackFP_baseline(uint64_t nextlong1, uint64_t nextlong2);
void crackFP_avx2(uint64_t nextlong1, uint64_t nextlong2);
void crackFP_avx512(uint64_t nextlong1, uint64_t nextlong2);
