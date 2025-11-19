#include "Metrics.h"

std::vector<MetricData> Metrics::data;

void Metrics::RecordPathfinding(int agents, int gridW, int gridH, 
                              double time, int pathLen, const std::string& dist) {
    data.push_back({agents, gridW, gridH, time, pathLen, dist});
}

void Metrics::SaveToCSV(const std::string& filename) {
    std::ofstream file(filename);
    file << "agents,grid_width,grid_height,time_ms,path_length,distribution\n";
    
    for (const auto& metric : data) {
        file << metric.agentCount << ","
             << metric.gridWidth << ","
             << metric.gridHeight << ","
             << metric.pathfindingTime * 1000 << ","
             << metric.pathLength << ","
             << metric.distributionType << "\n";
    }
    file.close();
}

void Metrics::Clear() { data.clear(); }