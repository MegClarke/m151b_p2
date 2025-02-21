// Copyright 2024 Blaise Tine
//
// Licensed under the Apache License;
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <vector>

namespace tinyrv {

class BranchPredictor {
public:
  virtual ~BranchPredictor() {}

  virtual uint32_t predict(uint32_t PC) {
      return PC + 4;
  };

  virtual void update(uint32_t PC, uint32_t next_PC, bool taken) {
      (void) PC;
      (void) next_PC;
      (void) taken;
  };
};

//there was no TODO here but campuswire says to initialize this struct so i did
struct BTB_entry_t {
    bool valid;        // what does valid mean exactly? my guess is it means that it has been used before
    uint32_t target;   // target address
    uint32_t tag;      // tag to differentiate branches mapping to the same index (don't really understand this yet)
};

class GShare : public BranchPredictor {
public:
  GShare(uint32_t BTB_size, uint32_t BHR_size);

  ~GShare() override;

  uint32_t predict(uint32_t PC) override;
  void update(uint32_t PC, uint32_t next_PC, bool taken) override;

  // TODO: Add your own methods here (meagan: done, but added a lot of comments to explain what's going on for the .cpp file implementation)
  private:
    std::vector<BTB_entry_t> BTB_;     // Branch Target Buffer (if PHT says taken, then this stores the target address to branch to)
    // this is of size BTB_size (which I think is 256), I think this is indexed by just some 8 bits of the PC (NO XOR) - not sure which 8 (but my guess is bits 10 to 2)
    //  how to use the tag???
    std::vector<uint8_t> PHT_;         // Pattern History Table (this determines whether we predict taken or not) - initialized to 0
    //okay so this is confusing because the readme talks about having a BHT (Branch History Table) but the code has a PHT, I'm assuming they're the same thing
    //automatically of size 2^(BHR_size = 8) = 256 and initialized to 0
    //I'm assuming that the PHT is indexed by PC XOR BHR -> still figuring out which bits of the PC (but my guess is bits 10 to 2)
    //if this is a 2-bit predictor (im assuming but it's not specified) we only use last 2 bits of uint_8_t entry to make the prediction 
    //00: Strongly not taken, 01: Weakly not taken, 10: Weakly taken, 11: Strongly taken (implemented)
    //if actually taken, increment by 1, if actually not taken, decrement by 1 (implement)

    uint32_t BHR_;                     // Branch History Register (8 bits)
    uint32_t BTB_shift_;               // genuinely not sure what this is for
    uint32_t BTB_mask_;                // this is literally just 0b11111111 so that we can zero out everything except last 8 bits of the PC
    uint32_t BHR_mask_;                // this is literally just 0b11111111 so that we can zero out everything except last 8 bits of the BHR

};

class GSharePlus : public BranchPredictor {
public:
  GSharePlus(uint32_t BTB_size, uint32_t BHR_size);

  ~GSharePlus() override;

  uint32_t predict(uint32_t PC) override;
  void update(uint32_t PC, uint32_t next_PC, bool taken) override;

  // TODO: extra credit component
};

}
