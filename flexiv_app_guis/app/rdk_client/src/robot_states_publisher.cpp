/**
 * @file publisher_example.cc
 * @copyright Flexiv Robotics 2021
 */
#include <flexiv/middleware2/fast_rtps_node.h>

#include "KostalLever.h"
#include "KostalLeverPubSubTypes.h"

#include "RobotOperations.hpp"
#include "PublishOperations.hpp"

const std::string robotIP = "127.0.0.1";
const std::string localIP = "127.0.0.1";
const std::string topicKostal = "topic_kostal";

int main()
{
    // Log object for printing message with timestamp and coloring
    flexiv::Log log;
    try {
        // RDK Initialization
        //=============================================================================
        // Instantiate robot interface
        flexiv::Robot robot(robotIP, localIP);

        int result = prepareRobot(&robot, &log);
        if (result == 1) {
            return 0;
        }

        // Create app-related message
        test_msgs::msg::KostalLever pub_msg;

        // Create publisher node
        std::unique_ptr<flexiv::middleware2::FastRTPSNode> pub_node;
        pub_node = std::make_unique<flexiv::middleware2::FastRTPSNode>(
            "my_publisher");
        auto publisher = createPublisher(pub_node.get(), topicKostal);

        // Create an array for storing tcpPose
        std::array<double, 7> tcpPose_array;
        // Create an array for storing flangePose
        std::array<double, 7> flangePose_array;
        // Create an array for storing rawForceSensorData
        std::array<double, 6> rawForceSensor_array;

        // Keep fetching robot states
        while (true) {
            // First, fetch data to assigned arrays
            fetchRobotStates(&robot, &tcpPose_array, &flangePose_array,
                &rawForceSensor_array);

            // Then publish these arrays
            publishTcpMsg(publisher, &pub_msg, &tcpPose_array,
                &flangePose_array, &rawForceSensor_array);
            // Print these arrays
            std::cout << "tcpPose array: ";
            for (auto it = tcpPose_array.begin(); it != tcpPose_array.end();
                 it++) {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
            std::cout << "flangePose array: ";
            for (auto it = flangePose_array.begin();
                 it != flangePose_array.end(); it++) {
                std::cout << *it << " ";
            }
            std::cout << std::endl;
        }

    } catch (const flexiv::Exception& e) {
        log.error(e.what());
        return 1;
    }
    return 0;
}