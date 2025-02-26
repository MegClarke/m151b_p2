#include "tournament_predictor.h"
#include "debug.h"
#include "core.h"

using namespace tinyrv;

TournamentPredictor::TournamentPredictor(uint32_t globalBTB_size, uint32_t globalBHR_size,
                                         uint32_t localBTB_size, uint32_t LHT_size, uint32_t LHB_size,
                                         uint32_t tournamentTableSize)
    : tournamentTableSize(tournamentTableSize)
{
    globalPredictor = new GShare(globalBTB_size, globalBHR_size); // dynamic GShare allocation
    localPredictor = new LocalHistoryPredictor(localBTB_size, LHT_size, LHB_size);
    tournamentTable.resize(tournamentTableSize, 1);
}

TournamentPredictor::~TournamentPredictor()
{
    delete globalPredictor;
    delete localPredictor;
}

uint32_t TournamentPredictor::predict(uint32_t PC)
{
    // predict for local and global
    uint32_t global_nextPC = globalPredictor->predict(PC);
    uint32_t local_nextPC = localPredictor->predict(PC);

    // find if branch is taken
    bool globalTaken = (global_nextPC != (PC + 4));
    bool localTaken = (local_nextPC != (PC + 4));

    // index into tournament table
    uint32_t tIndex = (PC >> 2) % tournamentTableSize;
    uint8_t selector = tournamentTable[tIndex];

    // If tournament counter is >1 select global else select local
    bool finalTaken;
    uint32_t final_nextPC;
    if (selector > 1)
    {
        finalTaken = globalTaken;
        final_nextPC = global_nextPC;
    }
    else
    {
        finalTaken = localTaken;
        final_nextPC = local_nextPC;
    }

    DT(3, "*** TournamentPredictor: predict PC=0x" << std::hex << PC << std::dec
                                                   << ", global_taken=" << globalTaken
                                                   << ", local_taken=" << localTaken
                                                   << ", selector=" << static_cast<uint32_t>(selector)
                                                   << ", final_taken=" << finalTaken);

    // return prediction if taken return PC+4 if not taken
    return finalTaken ? final_nextPC : (PC + 4);
}

void TournamentPredictor::update(uint32_t PC, uint32_t nextPC, bool taken)
{
    uint32_t global_nextPC = globalPredictor->predict(PC);
    uint32_t local_nextPC = localPredictor->predict(PC);
    bool globalPred = (global_nextPC != (PC + 4));
    bool localPred = (local_nextPC != (PC + 4));

    uint32_t tIndex = (PC >> 2) % tournamentTableSize;

    if (globalPred == taken && localPred != taken)
    {
        if (tournamentTable[tIndex] < 3)
            tournamentTable[tIndex]++;
    }
    else if (localPred == taken && globalPred != taken)
    {
        if (tournamentTable[tIndex] > 0)
            tournamentTable[tIndex]--;
    }

    globalPredictor->update(PC, nextPC, taken);
    localPredictor->update(PC, nextPC, taken);

    DT(3, "*** TournamentPredictor: update PC=0x" << std::hex << PC << std::dec
                                                  << ", taken=" << taken
                                                  << ", tournament counter=" << static_cast<uint32_t>(tournamentTable[tIndex]));
}
