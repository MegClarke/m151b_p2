#pragma once
#include <vector>
#include <cstdint>
#include "gshare.h"
#include "util.h"
#include "debug.h"

using namespace tinyrv;

class LocalHistoryPredictor {
    public:
        LocalHistoryPredictor(uint32_t BTB_size, uint32_t LHT_size, uint32_t LHB_size);
        ~LocalHistoryPredictor();

        uint32_t predict(uint32_t PC);

        void update(uint32_t PC, uint32_t next_PC, bool taken);

    private:
        // branch target buffer similar to GShare's
        std::vector<BTB_entry_t> BTB_;
        // local history table; one history per branch (indexed by PC bits)
        std::vector<uint32_t> LHT_;
        // localPHT: local pattern history table (2-bit saturating counters)
        std::vector<uint8_t> localPHT_;

        uint32_t BTB_mask_;
        uint32_t LHT_mask_;
        uint32_t LHB_mask_;
        uint32_t BTB_shift_;
};
