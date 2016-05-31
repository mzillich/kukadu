#ifndef KUKADU_CONTROLQUEUE
#define KUKADU_CONTROLQUEUE

/**
 * @file   controlqueue.hpp
 * @Author Simon Hangl (simon.hangl@uibk.ac.at)
 * @date   May, 2016
 * @brief  Contains the control queue interface. This interface is responsible for all communication
 * the kukadu software stack and the robotic arm
 *
 */

#include <kukadu/utils/types.hpp>
#include <kukadu/utils/tictoc.hpp>
#include <kukadu/types/kukadutypes.hpp>
#include <kukadu/utils/destroyableobject.hpp>
#include <kukadu/kinematics/constraints/constraints.hpp>

#include <queue>
#include <armadillo>
#include <ros/ros.h>
#include <geometry_msgs/Pose.h>

namespace kukadu {

    /**
     * \defgroup Robot
     * The control module provides control interfaces to robots and certain control
     * methods such as DMPs
     */

    /**
     * \class ControlQueue
     *
     * \brief Interface between the kukadu software stack and the robot hardware. If
     * kukadu shall support another robot, the abstract methods need to be implemented
     * according to the documentation. Then the kukadu software stack is available
     * for usage with the robot.
     *
     * The ControlQueue interface is the bridge between all implemented control methods (DMPs, ...) and
     * the robot hardware. It supports functionlity such as trajectory following, simple point to point
     * planning or security mechanisms such as maximum force values for execution.
     * \ingroup Robot
     */
    class ControlQueue : public DestroyableObject, public KUKADU_ENABLE_SHARED_FROM_THIS<ControlQueue> {

    private:

        bool isInit;
        bool finish;
        bool isShutUpFlag;
        bool rollbackMode;

        bool continueCollecting;

        bool jointPtpRunning;
        bool cartesianPtpRunning;

        int degOfFreedom;
        int rollBackQueueSize;

        double sleepTime;
        double currentTime;
        double rollBackTime;
        double lastDuration;
        double desiredCycleTime;

        arma::vec currentJoints;
        arma::vec startingJoints;
        arma::vec internalJointPasser;

        kukadu_mutex loadMaxDistMutex;
        kukadu_mutex loadCycleTimeMutex;

        geometry_msgs::Pose currentCartPose;
        geometry_msgs::Pose internalPosePasser;

        std::vector<mes_result> collectedJoints;

        std::queue<arma::vec> movementQueue;
        std::queue<geometry_msgs::Pose> cartesianMovementQueue;

        std::deque<arma::vec> rollBackQueue;

        TicToc t;

        KUKADU_SHARED_PTR<kukadu_thread> thr;
        KUKADU_SHARED_PTR<kukadu_thread> cartPtpThr;
        KUKADU_SHARED_PTR<kukadu_thread> jointPtpThr;
        KUKADU_SHARED_PTR<kukadu_thread> jointsColletorThr;

        void setInitValuesInternal();
        void internalCartPtpCaller();
        void internalJointPtpCaller();

        void jointsCollector();

        /**
         * \brief run() is started by startQueue() in a separate thread and takes care
         * of the queue execution (submits and collects data packets to and from the robot
         * per cycle)
         */
        virtual void run();

    protected:

        int currentControlType;
        
        /**
         * @brief Can be used to set the number of degrees of freedom.
         */
        virtual void setDegOfFreedom(int degOfFreedom);

        /**
         * @brief This method can be used to initialize custom parts of the
         * ControlQueue implementation before the queue is started by
         * startQueue().
         */
        virtual void setInitValues() = 0;

        /**
         * @brief The control queue collects the data about the actually executed trajectory
         * during a point to point movement. This is automatically done by jointPtp().
         * jointPtp() calls the jointPtpInternal() function that actually plans the
         * movement in joint space and executes it.
         */
        virtual void jointPtpInternal(arma::vec joints) = 0;

        /**
         * @brief This method is called by the run() method once per clock
         * cycle and actually submits the command about the next desired
         * joint pose to the robot.
         */
        virtual void submitNextJointMove(arma::vec joints) = 0;

        /**
         * @brief This method is called by the run() method once per clock
         * cycle and actually submits the command about the next desired
         * Cartesian pose to the robot.
         */
        virtual void submitNextCartMove(geometry_msgs::Pose pose) = 0;

        /**
         * @brief This method is called by switchMode() and actually sends the control
         * mode command to the robot.
         */
        virtual void setCurrentControlTypeInternal(int controlType) = 0;

        /**
         * @brief The control queue collects the data about the actually executed trajectory
         * during a point to point movement. This is automatically done by cartesianPtp().
         * cartesianPtp() calls the cartPtpInternal() function that actually plans the
         * movement in Cartesian space and executes it.
         */
        virtual void cartPtpInternal(geometry_msgs::Pose pose, double maxForce) = 0;

        /**
         * @brief This method can be used to add some additional initialization behavior
         * when the queue is started. If no additional initialization is required, the
         * method can be left empty.
         */
        virtual void startQueueHook() = 0;

        /**
         * @brief This method determines, if the queue execetion should be stopped while ptp commands are executed
         * (this is typically the case when ptp is done outside of the control queue implementation). If two different
         * controls interfere it can result in dangerous behaviour
         * @return true if the queue should be stopped during ptp, false otherwise
         */
        virtual bool stopQueueWhilePtp() = 0;

    public:

        /**
         * \brief Constructor taking the robot dependent degrees of freedom
         * \param degOfFreedom number of robots degrees of freedom
         */
        ControlQueue(int degOfFreedom, double desiredCycleTime);

        /**
         * \brief Returns number of the robots degrees of freedom
         * @return number of degrees of freedom
         */
        int getMovementDegreesOfFreedom();

        /**
         * \brief Sets the queue cycle time
         * \param cycleTime time to wait between two cycles
         */
        void setCycleTime(double cycleTime);

        /**
         * \brief Starts new thread to control the robot with real time capability
         */
        KUKADU_SHARED_PTR<kukadu_thread> startQueue();

        /**
         * \brief Returns the time between two cycles
         * @return cycle time
         */
        virtual double getCycleTime();

        /**
         * \brief Returns whether or not the queue is currently running
         * @return true if the queue is started, false otherwise
         */
        virtual bool getQueueRunning();

        /**
         * \brief Stops the queue
         */
        virtual void stopQueue();

        /**
         * \brief With setNextTrajectory() a complete trajectory can be added to the queue.
         * The trajectory will be executed after all other added commands (already contained
         * in the queue) were executed
         */
        virtual void setNextTrajectory(std::vector<arma::vec> jointTrajectory);

        /**
         * \brief Adds next joint position to the end of the queue (can be used for trajectory following)
         * \param joints joints to add
         */
        virtual void move(arma::vec joints);

        /**
         * \brief Adds next cartesian position to the end of the queue (can be used for trajectory following)
         * \param pose end-effector pose to add
         */
        virtual void move(geometry_msgs::Pose pose);

        /**
         * \brief Switches robot modes. A state might be a real time command mode or monitoring mode
         * \param mode mode id
         */
        virtual void switchMode(int mode);

        /**
         * \brief Stops current mode and switches back to default mode (e.g. monitoring mode)
         */
        virtual void stopCurrentMode();

        /**
         * \brief Blocks, if more than the defined maximum element count is in the queue
         * \param maxNumJointsInQueue maximum number of joints in queue
         */
        virtual void synchronizeToQueue(int maxNumJointsInQueue);

        /**
         * \brief If this is set, the robot moves to the desired position by jointPtp()
         * before the queue is started
         * \param joints array of joint positions
         */
        virtual void setStartingJoints(arma::vec joints);

        /**
         * \brief Implements simple point to point movement in joint space (blocks until target reached)
         * \param joints array of joint positions
         */
        virtual std::vector<mes_result> jointPtp(arma::vec joints);

        /**
         * \brief Implements simple point to point movement in joint space (not blocking)
         * \param joints array of joint positions
         */
        KUKADU_SHARED_PTR<kukadu_thread> jointPtpNb(arma::vec joints);

        /**
         * \brief Implements simple point to point movement in cartesian space
         * \param pose of end-effector
         */
        virtual std::vector<mes_result> cartesianPtp(geometry_msgs::Pose pos);

        /**
         * \brief Implements simple point to point movement in cartesian space
         * \param pose of end-effector
         * \param maxForce the point to point movement is stopped if the observed force
         * exceeds this value (in Newton)
         */
        virtual std::vector<mes_result> cartesianPtp(geometry_msgs::Pose pos, double maxForce);

        /**
         * \brief Implements simple point to point movement in cartesian space (does not block until the position is reached)
         * \param pose of end-effector
         */
        virtual KUKADU_SHARED_PTR<kukadu_thread> cartesianPtpNb(geometry_msgs::Pose pos);

        /**
         * \brief Changes the expected load data of the robot (e.g. can be used whenever robot picks up an object).
         * Some robots might require this information in order to avoid control problems
         * \param loadMass mass of the picked up object
         * \param loadPos position of the objects center of gravity relative to the manipulator
         */
        virtual void setAdditionalLoad(float loadMass, float loadPos) = 0;

        /**
         * \brief Sets certain stiffness parameters in cartesian space (if the robot supports this) according to a mass spring damper system model
         * \param cpstiffnessxyz stiffness of the robot in the cartesian space
         * \param cpstiffnessabc stiffness of the rotational axis of the tool mounting point in cartesian space
         * \param cpdamping damping of the robots axis in cartesian space
         * \param cpmaxdelta maximum allows deviation in cartesian space
         * \param maxforce maximum allowed applied force
         * \param axismaxdeltatrq maximum allowed applied torque
         */
        virtual void setStiffness(float cpstiffnessxyz, float cpstiffnessabc, float cpdamping, float cpmaxdelta, float maxforce, float axismaxdeltatrq) = 0;

        /**
         * \brief Returns the current robot position in cartesian space
         * \return current pose of the robot in Cartesian space
         */
        virtual mes_result getCurrentCartesianPos();

        /**
         * \brief Returns current robot position in cartesian space
         * \return current pose of the robot in Cartesian space
         */
        virtual geometry_msgs::Pose getCurrentCartesianPose() = 0;

        /**
         * \brief Returns the robot joints the robot has been directly before starting command mode
         * \return The starting joints set int setStartingJoints()
         */
        virtual arma::vec getStartingJoints();

        /**
         * \brief Returns joints if the robot is in command mode
         * \return The current joint pose of the robot
         */
        virtual mes_result getCurrentJoints() = 0;

        /**
         * \brief Returns the forces measured for the robot joints
         * \return The current forces measured at the robot joints (in Newton)
         */
        virtual mes_result getCurrentJntFrc() = 0;

        /**
         * \brief Returns the forces and torques measured at the robot end-effector
         * \return The current forces and torques (in Newton and Newton / meter)
         */
        virtual mes_result getCurrentCartesianFrcTrq() = 0;

        /**
         * \brief Sets the maximum deviation in joint space that is tolerated
         * for a point to point movement
         * \param thresh joint space threshold
         */
        virtual void setJntPtpThresh(double thresh) = 0;

        /**
         * \brief Returns true if the command mode initialization is done
         * \return Initialization status
         */
        virtual bool isInitialized();

        /**
         * \brief Returns the robot name
         * \return robot name
         */
        virtual std::string getRobotName() = 0;

        /**
         * \brief Returns the robot name with escaped special characaters (e.g. white spaces)
         * \return robot name
         */
        virtual std::string getRobotFileName() = 0;

        /**
         * \brief Returns the robot joint names
         * \return robot joint names
         */
        virtual std::vector<std::string> getJointNames() = 0;

        /**
         * \brief Returns currently measured absolute force on the end-effector
         * \return absolute end-effector force
         */
        virtual double getAbsoluteCartForce();

        /**
         * \brief Prevents the queue from printing debug information to the terminal
         */
        virtual void shutUp();

        /**
         * \brief Makes the queue printing debug information to the terminal
         */
        virtual void startTalking();

        /**
         * \brief If the rollback mode is started by startRollBackMode()
         * the robot can stop the current execution and move backwards the same
         * trajectory it came. Therefore it can move back in time for a given
         * time (in seconds)
         * \param time Time to move back
         */
        virtual void rollBack(double time);

        /**
         * \brief Stops the rollback mode (see startRollBackMode())
         */
        virtual void stopJointRollBackMode();

        /**
         * \brief Starts the rollback mode. When the function rollBack() is called
         * the robot can roll back the current trajectory for at most a the given
         * time (in seconds). This mode is only available in joint space execution
         * \param possibleTimeReach Maximum rollback time
         */
        virtual void startRollBackMode(double possibleTimeReach);

        /**
         * \brief An intrinsic clock is started with startQueue(). This method returns
         * the current time according to this clock.
         * \return current time
         */
        virtual double getCurrentTime();

        /**
         * \brief Indicates whether or not the queue is printing debug information to the terminal
         * (see shutUp()).
         * \return false if the queue is printing debug information to the terminal, true otherwise
         */
        bool isShutUp();

        /**
         * \brief Returns the current robot control mode (see switchMode())
         * \return Current robot control mode
         */
        virtual int getCurrentMode() = 0;

        static const int CONTROLQUEUE_STOP_MODE = 0;
        static const int CONTROLQUEUE_JNT_POS_MODE = 10;
        static const int CONTROLQUEUE_CART_IMP_MODE = 20;
        static const int CONTROLQUEUE_JNT_IMP_MODE = 30;

    };

}

#endif
