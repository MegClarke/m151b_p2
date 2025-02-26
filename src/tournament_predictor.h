#pragma once
#include <vector>
#include <cstdint>
#include "gshare.h"
#include "localHistoryPredictor.h"


using namespace tinyrv;

class TournamentPredictor {
    public:
    TournamentPredictor(uint32_t globalBTB_size, uint32_t globalBHR_size,
                uint32_t localBTB_size, uint32_t LHT_size, uint32_t LHB_size,
                uint32_t tournamentTableSize);
    ~TournamentPredictor();

    uint32_t predict(uint32_t PC);

    void update(uint32_t PC, uint32_t nextPC, bool taken);

    private:
        GShare* globalPredictor;
        LocalHistoryPredictor* localPredictor;
        std::vector<uint32_t> tournamentTable;
        uint32_t tournamentTableSize;
};