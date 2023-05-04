/**
 * @file RobotOperations.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */
#ifndef _ROBOT_OPERATIONS_HPP_
#define _ROBOT_OPERATIONS_HPP_

// Flexiv header files
#include <flexiv/Exception.hpp>
#include <flexiv/Gripper.hpp>
#include <flexiv/Log.hpp>
#include <flexiv/Mode.hpp>
#include <flexiv/Model.hpp>
#include <flexiv/Robot.hpp>
#include <flexiv/Scheduler.hpp>
#include <flexiv/StatesData.hpp>
#include <flexiv/Utility.hpp>
#include <flexiv/Visualization.hpp>

/**
 * @brief Prepare the robot by checking any fault. If fault exists, clear them.
 * If still fault, return error
 * @param[in] robot robot's pointer
 * @param[in] log log's pointer
 * @return 1 is failed 0 is successful
 */
int prepareRobot(flexiv::Robot* robot, flexiv::Log* log)
{
    // Clear fault on robot server if any
    if (robot->isFault()) {
        log->warn("Fault occurred on robot server, trying to clear ...");
        // Try to clear the fault
        robot->clearFault();
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // Check again
        if (robot->isFault()) {
            log->error("Fault cannot be cleared, exiting ...");
            return 1;
        }
        log->info("Fault on robot server is cleared");
    }
    log->info("Enabling robot ...");
    // robot.enable();
    //  Wait for the robot to become operational
    int secondsWaited = 0;
    while (!robot->isOperational()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (++secondsWaited == 10) {
            log->warn(
                "Still waiting for robot to become operational, please "
                "check that the robot 1) has no fault, 2) is booted "
                "into Auto mode");
        }
    }
    log->info("Robot is now operational");
    return 0;
}

/**
 * @brief Fetch robot states and put them into outside arrays and sleep 1
 * seconds
 * @param[in] robot robot's pointer
 * @param[in] tcpPose_array array to store tcpPose
 * @param[in] flangePose_array array to store flangePose
 * @param[in] rawForceSensor_array array to store rawForceSensorData (Not used
 * yet)
 */
void fetchRobotStates(flexiv::Robot* robot,
    std::array<double, 7>* tcpPose_array,
    std::array<double, 7>* flangePose_array,
    std::array<double, 6>* rawForceSensor_array)
{
    // Data struct for storing robot states
    flexiv::RobotStates robotStates;
    // Get robot states and output
    robot->getRobotStates(robotStates);
    // Retrieve 7 numbers and put them into outside arrays
    std::copy_n(robotStates.tcpPose.begin(), 7, tcpPose_array->begin());
    std::copy_n(robotStates.flangePose.begin(), 7, flangePose_array->begin());
    // Sleep for 1 seconds
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return;
}

#endif // _ROBOT_OPERATIONS_HPP_