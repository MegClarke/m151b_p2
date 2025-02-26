#include "localHistoryPredictor.h"
#include "debug.h"
#include <iostream>
#include <assert.h>
#include <util.h>
#include "types.h"
#include "core.h"

using namespace tinyrv;

LocalHistoryPredictor::LocalHistoryPredictor(uint32_t BTB_size, uint32_t LHT_size, uint32_t LHB_size)
    : BTB_(BTB_size, BTB_entry_t{false, 0x0, 0x0}),
      LHT_(LHT_size, 0),               // Initialize each branch's local history to 0
      localPHT_((1 << LHB_size), 1),   // Initialize localPHT counters to weakly taken (1)
      BTB_mask_(BTB_size - 1),
      LHT_mask_(LHT_size - 1),
      LHB_mask_((1 << LHB_size) - 1),
      BTB_shift_(log2ceil(BTB_size))
{
}

LocalHistoryPredictor::~LocalHistoryPredictor() {
}

// This is very similar to Gshare except I do not use a BHR to keep track of the global record and instead
// completely rely upon the PC to index intop the BTB and then a LHT to index into using LHT and then index into PHT using this table  

uint32_t LocalHistoryPredictor::predict(uint32_t PC)
{
    uint32_t next_PC = PC + 4;
    bool predict_taken = false;

    uint32_t BTB_index = (PC >> 2) & BTB_mask_;

    uint32_t LHT_index = (PC >> 2) & LHT_mask_;
    uint32_t local_history = LHT_[LHT_index];

    uint32_t PHT_index = local_history & LHB_mask_;
    predict_taken = localPHT_[PHT_index] > 1;

    uint32_t tag = (PC >> (BTB_shift_ + 2));
    if (predict_taken && BTB_[BTB_index].valid && BTB_[BTB_index].tag == tag){
        next_PC = BTB_[BTB_index].target;
    }

   DT(3, "*** LocalHistoryPredictor: predict PC=0x" << std::hex << PC << std::dec
       << ", next_PC=0x" << std::hex << next_PC << std::dec
       << ", predict_taken=" << predict_taken);
    return next_PC;
}

void LocalHistoryPredictor::update(uint32_t PC, uint32_t next_PC, bool taken)
{
    DT(3, "*** LocalHistoryPredictor: update PC=0x" << std::hex << PC << std::dec
       << ", next_PC=0x" << std::hex << next_PC << std::dec
       << ", taken=" << taken);

    uint32_t BTB_index = (PC >> 2) & BTB_mask_;
    uint32_t LHT_index = (PC >> 2) & LHT_mask_;
    uint32_t local_history = LHT_[LHT_index];
    uint32_t PHT_index = local_history & LHB_mask_;

    if (taken) {
        if (localPHT_[PHT_index] < 3)
            localPHT_[PHT_index]++;
    } else {
        if (localPHT_[PHT_index] > 0)
            localPHT_[PHT_index]--;
    }

    local_history = ((local_history << 1) | (taken ? 1 : 0)) & LHB_mask_;
    LHT_[LHT_index] = local_history;

    if (taken) {
        uint32_t tag = (PC >> (BTB_shift_ + 2));
        BTB_[BTB_index].valid = true;
        BTB_[BTB_index].target = next_PC;
        BTB_[BTB_index].tag = tag;
    }
}