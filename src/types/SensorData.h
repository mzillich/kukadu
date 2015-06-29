#ifndef SENSORDATA
#define SENSORDATA

#include <vector>
#include <string>
#include <armadillo>
#include <utility>

#include "../utils/utils.h"

class SensorData {

private:

    std::vector<std::string> labels;
    std::vector<std::string> jointPosLabels;
    std::vector<std::string> jointFrcLabels;
    std::vector<std::string> cartPosLabels;
    std::vector<std::string> cartFrcTrqLabels;
    std::vector<std::string> cartForceAbsLabel;
    arma::mat values;

public:

    SensorData(std::vector<std::string> labels, arma::mat values);
    SensorData(std::string timeLabel, std::vector<std::string> jointPosLabels, std::vector<std::string> jointFrcLabels, std::vector<std::string> cartPosLabels,
               std::vector<std::string> cartForceAbsLabel, std::vector<std::string> cartFrcTrqLabels,
               arma::vec time, arma::mat jointPos, arma::mat jointFrc, arma::mat cartPos, arma::mat cartForceAbs, arma::mat cartFrcTrq);

    int labelExists(std::string label);
    arma::vec getDataByIdx(int idx);
    arma::vec getDataByLabel(std::string label);
    arma::vec getTime();
    arma::mat getRange(std::vector<std::string> indexes);
    arma::mat getRange(std::vector<int> indexes);
    arma::mat getRange(int startIdx, int endIdx);

    arma::vec getTimes();
    arma::mat getJointPos();
    arma::mat getJointForces();
    arma::mat cartPos();
    arma::mat cartFrcTrqs();

    double getTime(int rowIdx);
    arma::vec getJointPosRow(int rowIdx);
    arma::vec getJointForcesRow(int rowIdx);
    arma::vec cartPosRow(int rowIdx);
    arma::vec cartFrcTrqsRow(int rowIdx);

    void removeDuplicateTimes();

};

#endif
