#pragma once
#include <vector>
#include <string>
#include <fstream>

struct MetricData {
    int agentCount;
    int gridWidth;
    int gridHeight;
    double pathfindingTime;
    int pathLength;
    std::string distributionType;
};

class Metrics {
private:
    static std::vector<MetricData> data;
    
public:
    static void RecordPathfinding(int agents, int gridW, int gridH, 
                                double time, int pathLen, const std::string& dist);
    static void SaveToCSV(const std::string& filename);
    static void Clear();
};