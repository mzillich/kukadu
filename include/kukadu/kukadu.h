#ifndef KUKADU
#define KUKADU

#include "../../src/learning/LWRRegressor.h"
#include "../../src/learning/GeneralFitter.h"
#include "../../src/learning/GenericKernel.h"
#include "../../src/learning/TricubeKernel.h"
#include "../../src/learning/GaussianKernel.h"
#include "../../src/learning/QuadraticKernel.h"
#include "../../src/learning/KernelRegressor.h"
#include "../../src/learning/GaussianProcessRegressor.h"
#include "../../src/learning/metric_learning/Mahalanobis.h"
#include "../../src/learning/metric_learning/InfTheoConstraints.h"
#include "../../src/learning/metric_learning/MahalanobisLearner.h"
#include "../../src/learning/metric_learning/InfTheoMetricLearner.h"
#include "../../src/learning/metric_learning/TogersonMetricLearner.h"

#include "../../src/learning/projective_simulation/core/projectivesimulator.h"
#include "../../src/learning/projective_simulation/application/manualreward.h"
#include "../../src/learning/projective_simulation/visualization/treedrawer.h"

#include "../../src/learning/reinforcement_learning/PoWER.h"
#include "../../src/learning/reinforcement_learning/CostComputer.h"
#include "../../src/learning/reinforcement_learning/DMPReinforcer.h"
#include "../../src/learning/reinforcement_learning/GradientDescent.h"
#include "../../src/learning/reinforcement_learning/DmpRewardComputer.h"
#include "../../src/learning/reinforcement_learning/TerminalCostComputer.h"
#include "../../src/learning/reinforcement_learning/SampleRewardComputer.h"
#include "../../src/learning/reinforcement_learning/TrajectoryBasedReward.h"

#include "../../src/robot/ControlQueue.h"
#include "../../src/robot/SensorStorage.h"
#include "../../src/robot/VisionInterface.h"
#include "../../src/robot/mounted/RosSchunk.h"
#include "../../src/robot/mounted/GenericHand.h"
#include "../../src/robot/mounted/PlottingHand.h"
#include "../../src/robot/PlottingControlQueue.h"

#include "../../src/trajectory/DMPExecutor.h"
#include "../../src/trajectory/DMPGeneralizer.h"
#include "../../src/trajectory/JointDMPLearner.h"
#include "../../src/trajectory/TrajectoryExecutor.h"
#include "../../src/trajectory/CartesianDMPLearner.h"
#include "../../src/trajectory/TrajectoryGenerator.h"
#include "../../src/trajectory/TrajectoryComparator.h"
#include "../../src/trajectory/DMPTrajectoryGenerator.h"
#include "../../src/trajectory/PolyTrajectoryGenerator.h"

#include "../../src/types/DMP.h"
#include "../../src/types/DMPBase.h"
#include "../../src/types/jointdmp.h"
#include "../../src/types/Trajectory.h"
#include "../../src/types/SensorData.h"
#include "../../src/types/QueryPoint.h"
#include "../../src/types/KukaduTypes.h"
#include "../../src/types/CartesianDMP.h"
#include "../../src/types/DictionaryTrajectory.h"
#include "../../src/types/SingleSampleTrajectory.h"

#include "../../src/manipulation/Controller.hpp"
#include "../../src/manipulation/ComplexController.hpp"
#include "../../src/manipulation/SensingController.hpp"
#include "../../src/manipulation/haptic/HapticPlanner.h"

#include "../../src/utils/types.h"
#include "../../src/utils/utils.h"
#include "../../src/utils/tictoc.hpp"
#include "../../src/utils/CustomSet.h"
#include "../../src/utils/KukaduTokenizer.h"
#include "../../src/utils/conversion_utils.h"
#include "../../src/utils/DestroyableObject.h"

#include "../../src/vision/Kinect.hpp"
#include "../../src/vision/PCLTools.hpp"
#include "../../src/vision/openboxfilter.hpp"
#include "../../src/vision/planarcuttransformator.hpp"

#ifdef ROS_SUPPORT
#include "../../src/robot/KukieControlQueue.h"
#endif

#endif
