#ifndef KUKADU_KUKIECONTROLQUEUE_H
#define KUKADU_KUKIECONTROLQUEUE_H

#include <queue>
#include <deque>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <kukadu/utils/types.hpp>
#include <kukadu/utils/utils.hpp>
#include <kukadu/types/kukadutypes.hpp>
#include <kukadu/robot/arm/controlqueue.hpp>
#include <kukadu/kinematics/kinematics.hpp>
#include <kukadu/kinematics/pathplanner.hpp>
#include <kukadu/utils/destroyableobject.hpp>

#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Wrench.h>
#include <sensor_msgs/JointState.h>
#include <iis_robot_dep/KukieError.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <iis_robot_dep/FriRobotData.h>
#include <iis_robot_dep/FriJointState.h>
#include <std_msgs/MultiArrayDimension.h>
#include <iis_robot_dep/FriJointCommand.h>
#include <iis_robot_dep/FriRobotJntData.h>
#include <iis_robot_dep/FriJointImpedance.h>
#include <iis_robot_dep/CartesianImpedance.h>

namespace kukadu {

    /** \brief The KukieControlQueue provides control capabilities for the Kuka LWR 4+ robotic arm
     *
     * This class implements the abstract ControlQueue class for the usage with the iisKukie system. It provides basic functionalities such as command mode control
     * in joint space as well as point to point movement in cartesian and joint space. To use it, the additionally provided KRL script has to be selected on the robot
     * controller side. For further information how to use it, please see the sample programs and the kuka documentation
     * \ingroup RobotFramework
     */
    class KukieControlQueue : public ControlQueue {

    private:

        bool loadCycleTimeFromServer;
        bool loadMaxDistPerCycleFromServer;

        bool firstModeReceived;
        bool firstJointsReceived;
        bool firstControllerCycletimeReceived;
        bool firstMaxDistPerCycleReceived;

        int impMode;
        int cartesianPtpReached;

        bool ptpReached;
        bool isRealRobot;
        bool acceptCollisions;
        bool plannerInitialized;
        bool kinematicsInitialized;

        double maxDistPerCycle;

        arma::vec currJoints;
        arma::vec currentJntFrqTrq;
        arma::vec currentCartFrqTrq;

        kukadu_mutex komoMutex;
        kukadu_mutex forwadKinMutex;
        kukadu_mutex cartFrcTrqMutex;
        kukadu_mutex currentJointsMutex;

        geometry_msgs::Pose currCarts;
        geometry_msgs::Pose currentCartPose;
        geometry_msgs::Pose currentCartPoseRf;

        std::string ptpTopic;
        std::string armPrefix;
        std::string deviceType;
        std::string commandTopic;
        std::string addLoadTopic;
        std::string cartPtpTopic;
        std::string stiffnessTopic;
        std::string jntFrcTrqTopic;
        std::string cartFrcTrqTopic;
        std::string retCartPosTopic;
        std::string switchModeTopic;
        std::string ptpReachedTopic;
        std::string cartPoseRfTopic;
        std::string retJointPosTopic;
        std::string jntStiffnessTopic;
        std::string commandStateTopic;
        std::string cartPtpReachedTopic;
        std::string cartMoveRfQueueTopic;
        std::string cartMoveWfQueueTopic;
        std::string jntSetPtpThreshTopic;

        std::string clockCycleTopic;
        std::string maxDistPerCycleTopic;

        ros::NodeHandle node;

        KUKADU_SHARED_PTR<ros::Rate> loop_rate;

        ros::Publisher pubPtp;
        ros::Publisher pubCommand;
        ros::Publisher pubCartPtp;
        ros::Publisher pubAddLoad;
        ros::Publisher pubSwitchMode;
        ros::Publisher pubCartMoveRfQueue;
        ros::Publisher pubCartMoveWfQueue;
        ros::Publisher pub_set_ptp_thresh;
        ros::Publisher pub_set_cart_stiffness;
        ros::Publisher pub_set_joint_stiffness;

        ros::Subscriber subJntPos;
        ros::Subscriber subComState;
        ros::Subscriber subCycleTime;
        ros::Subscriber subjntFrcTrq;
        ros::Subscriber subPtpReached;
        ros::Subscriber subCartFrqTrq;
        ros::Subscriber subCartPoseRf;
        ros::Subscriber subCartPtpReached;
        ros::Subscriber subMaxDistPerCycle;

        kukadu_thread cartPoseThr;

        KUKADU_SHARED_PTR<Kinematics> kin;
        KUKADU_SHARED_PTR<PathPlanner> planner;

        /* Kukie callback functions */
        void cartPosRfCallback(const geometry_msgs::Pose msg);
        void cartFrcTrqCallback(const geometry_msgs::Wrench& msg);
        void jntMoveCallback(const std_msgs::Float64MultiArray& msg);
        void ptpReachedCallback(const std_msgs::Int32MultiArray& msg);
        void robotJointPosCallback(const sensor_msgs::JointState& msg);
        void jntFrcTrqCallback(const std_msgs::Float64MultiArray& msg);
        void commandStateCallback(const std_msgs::Float32MultiArray& msg);
        void cartPtpReachedCallback(const std_msgs::Int32MultiArray& msg);
        void maxDistPerCycleCallback(const std_msgs::Float64& msg);
        void cycleTimeCallback(const std_msgs::Float64& msg);

        void computeCurrentCartPose();

        double computeDistance(float* a1, float* a2, int size);

    protected:

        virtual void setInitValues();
        virtual void startQueueThreadHook();
        virtual void jointPtpInternal(arma::vec joints);
        virtual void submitNextJointMove(arma::vec joints);
        virtual void cartPtpInternal(geometry_msgs::Pose pos, double maxForce);
        virtual void submitNextCartMove(geometry_msgs::Pose pose);
        virtual void setCurrentControlTypeInternal(int controlType);

        virtual bool stopQueueWhilePtp();

    public:

        KukieControlQueue(std::string deviceType, std::string armPrefix, ros::NodeHandle node,
                          bool acceptCollisions = false,
                          KUKADU_SHARED_PTR<Kinematics> kin = KUKADU_SHARED_PTR<Kinematics>(), KUKADU_SHARED_PTR<PathPlanner> planner = KUKADU_SHARED_PTR<PathPlanner>(),
                          double sleepTime = -1.0, double maxDistPerCycle = -1.0);

        void constructQueue(std::string commandTopic, std::string retPosTopic, std::string switchModeTopic, std::string retCartPosTopic,
                            std::string cartStiffnessTopic, std::string jntStiffnessTopic, std::string ptpTopic,
                            std::string commandStateTopic, std::string ptpReachedTopic, std::string addLoadTopic, std::string jntFrcTrqTopic, std::string cartFrcTrqTopic,
                            std::string cartPtpTopic, std::string cartPtpReachedTopic, std::string cartMoveRfQueueTopic, std::string cartMoveWfQueueTopic, std::string cartPoseRfTopic,
                            std::string setPtpThresh, std::string clockCycleTopic, std::string maxDistancePerCycleTopic, bool acceptCollisions, ros::NodeHandle node,
                            KUKADU_SHARED_PTR<Kinematics> kin, KUKADU_SHARED_PTR<PathPlanner> planner, double sleepTime, double maxDistPerCycle
                        );

        void safelyDestroy();

        void setJntPtpThresh(double thresh);

        void setAdditionalLoad(float loadMass, float loadPos);
        void setStiffness(float cpstiffnessxyz, float cpstiffnessabc, float cpdamping, float cpmaxdelta, float maxforce, float axismaxdeltatrq);

        int getCurrentControlType();

        std::string getRobotName();
        std::string getRobotFileName();
        std::string getRobotSidePrefix();
        std::string getRobotDeviceType();

        std::vector<std::string> getJointNames();

        mes_result getCurrentJoints();
        mes_result getCurrentJntFrcTrq();
        mes_result getCurrentCartesianFrcTrq();

        geometry_msgs::Pose getCurrentCartesianPose();
        geometry_msgs::Pose getCurrentCartesianPoseRf();
        geometry_msgs::Pose moveCartesianRelativeWf(geometry_msgs::Pose basePoseRf, geometry_msgs::Pose offset);

        arma::vec getFrcTrqCart();

        static const int KUKA_STOP_MODE = 0;
        static const int KUKA_JNT_POS_MODE = 10;
        static const int KUKA_CART_IMP_MODE = 20;
        static const int KUKA_JNT_IMP_MODE = 30;

        static const int KUKA_STD_XYZ_STIFF = 250;
        static const int KUKA_STD_ABC_STIFF = 100;
        static const int KUKA_STD_CPDAMPING = 0.3;

    };

}

#endif
