#ifndef _PUBLISH_OPERATIONS_HPP_
#define _PUBLISH_OPERATIONS_HPP_

// middleware2 headers
#include <flexiv/middleware2/fast_rtps_node.h>

#include "KostalLever.h"
#include "KostalLeverPubSubTypes.h"

/**
 * @brief Create a publisher by passing node pointer into this function
 * @param[in] node FastRTPSNode pointer
 * @param[in] topicName the name of the topic that will be published to
 * @return the shared pointer of the publisher
 */
std::shared_ptr<flexiv::middleware2::FastRTPSPublisher<
    test_msgs::msg::KostalLeverPubSubType>>
createPublisher(flexiv::middleware2::FastRTPSNode* node, std::string topicName)
{
    std::shared_ptr<flexiv::middleware2::FastRTPSPublisher<
        test_msgs::msg::KostalLeverPubSubType>>
        publisher
        = node->CreateDefaultPublisher<test_msgs::msg::KostalLeverPubSubType>(
            topicName);
    return publisher;
}

/**
 * @brief Publish
 * @param[in] publisher shared pointer of the FastRTPSPublisher
 * @param[in] pub_msg the message that will be published
 * @param[in] tcpPose_array array that store tcpPose
 * @param[in] flangePose_array array that store flangePose
 * @param[in] rawForceSensor_array array that store rawForceSensorData (Not used
 * yet)
 * @return the shared pointer of the publisher
 */
void publishTcpMsg(std::shared_ptr<flexiv::middleware2::FastRTPSPublisher<
                       test_msgs::msg::KostalLeverPubSubType>>
                       publisher,
    test_msgs::msg::KostalLever* pub_msg, std::array<double, 7>* tcpPose_array,
    std::array<double, 7>* flangePose_array,
    std::array<double, 6>* rawForceSensor_array)
{
    pub_msg->tcp_pose(*tcpPose_array);
    pub_msg->flange_pose(*flangePose_array);
    publisher->Publish((void*)pub_msg);
}

#endif