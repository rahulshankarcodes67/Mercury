#pragma once
#include <string>
#include "../engine/MatchingEngine.h"

namespace mercury {

class SnapshotManager {
public:
    explicit SnapshotManager(const std::string& snapshot_dir);
    
    void takeSnapshot(MatchingEngine& engine, uint64_t sequence_number);
    void loadSnapshot(MatchingEngine& engine, const std::string& filename);

private:
    std::string dir_;
};

} // namespace mercury
