// Copyright 2024 blaise
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <assert.h>
#include <util.h>
#include "types.h"
#include "core.h"
#include "debug.h"

using namespace tinyrv;

///////////////////////////////////////////////////////////////////////////////

GShare::GShare(uint32_t BTB_size, uint32_t BHR_size)
  : BTB_(BTB_size, BTB_entry_t{false, 0x0, 0x0})   //meagan: I initialized this struct to be {valid, target, tag} not sure if thats correct
  , PHT_((1 << BHR_size), 0x0)
  , BHR_(0x0)
  , BTB_shift_(log2ceil(BTB_size))
  , BTB_mask_(BTB_size-1)
  , BHR_mask_((1 << BHR_size)-1) {
  //--
}

GShare::~GShare() {
  //--
}

uint32_t GShare::predict(uint32_t PC) {
  uint32_t next_PC = PC + 4;
  bool predict_taken = false;

  // TODO: meagan: i think mostly done
  //2 bits of the PC are always 0, cuz instruction alignment bits (mentioned on campus wire) so we can shift to get the meaningful bits
  uint32_t BTB_index = (PC >> 2) & BTB_mask_;
  
  uint32_t PHT_index = (PC >> 2) ^ BHR_; //xor the PC with the BHR to get the index into the PHT
  predict_taken = PHT_[PHT_index] > 1; //if the value in the PHT is greater than 1, predict taken

  if (predit_taken && BTB_[BTB_index].valid) { //if the BTB says we should branch
    next_PC = BTB_[BTB_index].target; //then branch to the target address
    //what are the tags for????? and also what to do if not valid (zane please test and figure this out)
  }
  //it predit_taken is false, then we just go to the next PC (which is PC + 4) so don't change next PC
  


  DT(3, "*** GShare: predict PC=0x" << std::hex << PC << std::dec
        << ", next_PC=0x" << std::hex << next_PC << std::dec
        << ", predict_taken=" << predict_taken);
  return next_PC;
}

void GShare::update(uint32_t PC, uint32_t next_PC, bool taken) {
  DT(3, "*** GShare: update PC=0x" << std::hex << PC << std::dec
        << ", next_PC=0x" << std::hex << next_PC << std::dec
        << ", taken=" << taken);

  // TODO: meagan: this is not done but i think the last thing to do is just update valid, and tags, but I don't understand them
  // Update the BHR 
  BHR_ = ((BHR_ << 1) | (taken ? 1 : 0)) & BHR_mask_; //shift left and add the taken bit to the end & by 0b11111111

  // Update the PHT (if actually taken, increment by 1, if actually not taken, decrement by 1)
  uint32_t PHT_index = (PC >> 2) ^ BHR_; 
  if (taken) {
    if (PHT_[PHT_index] < 3) {
      PHT_[PHT_index]++;
    }
  } else {
    if (PHT_[PHT_index] > 0) {
      PHT_[PHT_index]--;
    }
  }

  // Update the BTB
  uint32_t BTB_index = (PC >> 2) & BTB_mask_;
  BTB_[BTB_index].valid = true; //it's been visited / used before
  BTB_[BTB_index].target = next_PC; //could be PC+4 if not taken or the branch target but we shouldn't have to handle the conditional
  //what is the tag for?????
  
}

///////////////////////////////////////////////////////////////////////////////

GSharePlus::GSharePlus(uint32_t BTB_size, uint32_t BHR_size) {
  (void) BTB_size;
  (void) BHR_size;
}

GSharePlus::~GSharePlus() {
  //--
}

uint32_t GSharePlus::predict(uint32_t PC) {
  uint32_t next_PC = PC + 4;
  bool predict_taken = false;
  (void) PC;
  (void) next_PC;
  (void) predict_taken;

  // TODO: extra credit component

  DT(3, "*** GShare+: predict PC=0x" << std::hex << PC << std::dec
        << ", next_PC=0x" << std::hex << next_PC << std::dec
        << ", predict_taken=" << predict_taken);
  return next_PC;
}

void GSharePlus::update(uint32_t PC, uint32_t next_PC, bool taken) {
  (void) PC;
  (void) next_PC;
  (void) taken;

  DT(3, "*** GShare+: update PC=0x" << std::hex << PC << std::dec
        << ", next_PC=0x" << std::hex << next_PC << std::dec
        << ", taken=" << taken);

  // TODO: extra credit component
}


