/*
 * @file RobotOperation.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_ROBOTOPERATION_HPP_
#define FLEXIVRDK_ROBOTOPERATION_HPP_

#include <kostal/KostalStates.hpp>
#include <kostal/SystemParams.h>
#include <kostal/SPIOperation.hpp>

namespace kostal {

    /**
     * @class RobotOperationHandler
     * @brief Base class for robot operations, dealt by a handler
     */
    class RobotOperationHandler
    {
    private:
        kostal::Log k_log;
    public:
        RobotOperationHandler() = default;
        virtual ~RobotOperationHandler() = default;

        /**
         * @brief Check robot connection and set robot to plan execution mode
         * @param[in] robotPtr robot's pointer
         * @param[in] logPtr robot's log pointer
         * @return Status code
         */
        Status buildRobotConnection(flexiv::Robot* robotPtr, flexiv::Log* logPtr)
        {   
            // Check whether the robot is connected or not
            if(robotPtr->isConnected()!=true){
                logPtr->error("The robot is not connected");
                return ROBOT;
            }else logPtr->info("The robot is now connected");
                
            // Clear fault on robot server if any
            if (robotPtr->isFault()) {
                logPtr->warn("Robot is facing a fault, trying to clear the fault...");

                // Try to clear the fault
                robotPtr->clearFault();
                std::this_thread::sleep_for(std::chrono::seconds(2));

                // Check fault again
                if (robotPtr->isFault()) 
                {
                    logPtr->error("===================================================");
                    logPtr->error("Robot's fault cannot be cleared, exiting ...");

                    return ROBOT;
                }
                logPtr->info("Fault on robot server is cleared");
            }

            try {
                //robot.enable();
            } catch (const flexiv::Exception& e) {
                logPtr->error(e.what());
                logPtr->error("The robot can not be enabled");
                return ROBOT;
            }

            // Wait for the robot to become operational
            while (!robotPtr->isOperational()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            logPtr->info("The robot is now operational");

            // Set mode after robot is operational
            robotPtr->setMode(flexiv::MODE_PLAN_EXECUTION);
            
            // Wait for the mode to be switched
            while (robotPtr->getMode() != flexiv::MODE_PLAN_EXECUTION) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            logPtr->info("The robot is now in plan execution mode");
            return SUCCESS;
        }

        /**
         * @brief This function helps to clear some tiny software fault that users trigger
         * if return is not success, it means the error can not be eliminated
         * @param[in] robotPtr Pointer to robot object
         * @param[in] logPtr Pointer to flexiv log object
         * @return Flexiv status code
         */
        Status clearTinyFault(flexiv::Robot* robotPtr, flexiv::Log* logPtr)
        {
            // Clear fault on robot server if any
            if (robotPtr->isFault()) 
            {
                logPtr->warn("Robot is facing a fault, trying to clear the fault...");
                // Try to clear the fault
                robotPtr->clearFault();
                std::this_thread::sleep_for(std::chrono::seconds(2));
                // Check again
                if (robotPtr->isFault()) {
                    logPtr->error("===================================================");
                    logPtr->error("Robot's fault cannot be cleared, exiting ...");
                    return ROBOT;
                }else{logPtr->info("Fault on robot server is cleared");} 
            }
            return SUCCESS;
        }

        /**
         * @brief Access the current robot data and store it
         * @param[in] robotPtr robot's pointer
         * @param[out] robotDataPtr robot data's pointer
         * @param[out] robotDataListPtr robot data list's pointer
         * @return Status code
         */
        Status collectRobotData(flexiv::Robot* robotPtr, 
                                RobotData* robotDataPtr, 
                                std::list<RobotData>* robotDataListPtr)
        {
        while (g_collectSwitch)
        {
            // get plan info and put it into instance pointer
            flexiv::PlanInfo planInfo;
            robotPtr->getPlanInfo(&planInfo); 
            if (planInfo.m_nodeName == "Start")
            {
                g_dataCollectFlag = true;
            }
            if (planInfo.m_nodeName == "Stop")
            {
                g_dataCollectFlag = false;
                //k_log.info("The node is now Stop");
            }
            if (g_dataCollectFlag == true)
            {
                std::lock_guard<std::mutex> lock(g_kostalDataMutex);
                robotDataPtr->nodeName = planInfo.m_nodeName;   
                // get robot states and put it into instance pointer
                flexiv::RobotStates robotStates;
                robotPtr->getRobotStates(&robotStates);
                {
                    // use mutex to lock robot data, store robot data to kostal data 
                    
                    robotDataPtr->tcpPose = robotStates.m_tcpPose;
                    robotDataPtr->rawDataForceSensor = robotStates.m_rawExtForceInTcpFrame;
                    robotDataPtr->flangePose = robotStates.m_flangePose;
                    robotDataListPtr->push_back(*robotDataPtr);
                }
            }
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        return SUCCESS;
        }

        /**
         * @brief Access the current robot and spi data and store it into each list
         * @param[in]  robotPtr robot's pointer
         * @param[out] robotDataPtr robot data's pointer
         * @param[out] robotDataListPtr robot data list's pointer
         * @param[out] spiDataPtr spi data's pointer
         * @param[out] spiDataListPtr spi data list's pointer
         * @return Status code
         */
        Status collectUsefulData(flexiv::Robot* robotPtr, 
                                 RobotData* robotDataPtr, 
                                 std::list<RobotData>* robotDataListPtr,
                                 SPIData* spiDataPtr,
                                 std::list<SPIData>* spiDataListPtr)
        {
            while (g_collectSwitch)
            {
                // get plan info and put it into instance pointer
                flexiv::PlanInfo planInfo;
                robotPtr->getPlanInfo(&planInfo); 
                if (planInfo.m_nodeName == "Start")
                {
                    g_dataCollectFlag = true;
                }
                if (planInfo.m_nodeName == "Stop")
                {
                    g_dataCollectFlag = false;
                }
                if (g_dataCollectFlag == true)
                {
                    robotDataPtr->nodeName = planInfo.m_nodeName;   
                    // get robot states and put it into instance pointer
                    flexiv::RobotStates robotStates;
                    {
                        std::lock_guard<std::mutex> lock(g_kostalDataMutex);
                        robotPtr->getRobotStates(&robotStates);

                        robotDataPtr->tcpPose = robotStates.m_tcpPose;
                        robotDataPtr->rawDataForceSensor = robotStates.m_rawExtForceInTcpFrame;
                        robotDataPtr->flangePose = robotStates.m_flangePose;
                        robotDataListPtr->push_back(*robotDataPtr);
                        spiDataListPtr->push_back(*spiDataPtr);
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return SUCCESS;
        }


        /**
         * @brief Check whether robot has this plan in list
         * @param[in] robotPtr robot's pointer
         * @param[in] logPtr robot's log pointer
         * @param[in] planName the name of the executing work plan
         * @return yes for do have, no for do not have
         */
        bool checkRobotPlan(flexiv::Robot* robotPtr, 
                              flexiv::Log* logPtr,
                              std::string planName)
        {
            auto planList = robotPtr->getPlanNameList();
                
            if (std::find(planList.begin(), planList.end(), planName) == planList.end()){
                logPtr->error("=================================================");
                logPtr->error("The robot does not have planName: " + planName);
                return false;
            }else{
                //logPtr->info("The robot has planName: " + planName);
                return true;
            }
        }

        /**
         * @brief Execute the plan in robot by name
         * @param[in] robotPtr robot's pointer
         * @param[in] logPtr robot's log pointer
         * @param[in] planName the name of the executing work plan
         * @return Status code
         */
        Status executeRobotPlan(flexiv::Robot* robotPtr, 
                                flexiv::Log* logPtr,
                                std::string planName)
        {      
            flexiv::SystemStatus systemStatus;
            robotPtr->executePlanByName(planName);
            while (systemStatus.m_programRunning == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                robotPtr->getSystemStatus(&systemStatus);
            }
            while (systemStatus.m_programRunning == true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                robotPtr->getSystemStatus(&systemStatus);
            }
            //logPtr->info("The robot has executed plan: " + planName);
            return SUCCESS;
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_ROBOTOPERATION_HPP_ */