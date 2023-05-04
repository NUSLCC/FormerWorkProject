    /*
 * @file RobotOperation.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef FLEXIVRDK_SYNCTASK_HPP_
#define FLEXIVRDK_SYNCTASK_HPP_

// Kostal header files
#include <kostal/KostalStates.hpp>
#include <kostal/SystemParams.h>
#include <kostal/SPIOperation.hpp>
#include <kostal/RobotOperation.hpp>

namespace kostal {

    /**
     * @class SyncTaskHandler
     * @brief Base class for run sync task
     */
    class SyncTaskHandler
    {
    private:
        kostal::RobotOperationHandler m_robotHandler;
        kostal::SPIOperationHandler m_spiHandler;
    public:
        SyncTaskHandler() = default;
        virtual ~SyncTaskHandler() = default;

        /**
         * @brief Run embedded scheduler to execute task in a desired frequency
         * @param[in] robotPtr robot's pointer
         * @param[in] spiDataPtr spi data's pointer
         * @param[in] spiDataListPtr spi data list's pointer
         * @param[in] robotDataPtr robot data's pointer
         * @param[in] robotDataListPtr robot data list's pointer
         * @param[in] logPtr robot's log pointer
         * @param[in] planName the name of the executing work plan
         * @return Status code
         */
        Status runScheduler(flexiv::Robot* robotPtr,
                            SPIData* spiDataPtr,
                            std::list<SPIData>* spiDataListPtr,
                            RobotData* robotDataPtr,
                            std::list<RobotData>* robotDataListPtr,
                            flexiv::Log* logPtr, 
                            std::string planName)
        {
            Status result;
            if (m_robotHandler.checkRobotPlan(robotPtr, logPtr, planName)==false){
                return ROBOT;
            }

            g_collectSwitch = true;
            flexiv::SystemStatus systemStatus;
            // Execute the plan by name, need to wait until the system response
            robotPtr->executePlanByName(planName);
            while (systemStatus.m_programRunning == false)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                robotPtr->getSystemStatus(&systemStatus);
            }
            // Create two threads to realize data collecting and inserting collectSPIDataDummy
            //boost::thread T1(boost::bind(&kostal::SPIOperationHandler::collectSPIDataDummy, m_spiHandler, spiDataPtr, logPtr));
            boost::thread T1(boost::bind(&kostal::SPIOperationHandler::collectSPIData, m_spiHandler, spiDataPtr, logPtr));
            boost::thread T2(boost::bind(&kostal::RobotOperationHandler::collectUsefulData, m_robotHandler, robotPtr, robotDataPtr, robotDataListPtr, spiDataPtr, spiDataListPtr));
            T1.detach();
            T2.detach();
            // Wait until the program is finished
            while (systemStatus.m_programRunning == true)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                robotPtr->getSystemStatus(&systemStatus);
            }
            // Close the two while loop in threads
            {
                std::lock_guard<std::mutex> lock(g_kostalDataMutex);
                g_collectSwitch = false;
            }
            
            logPtr->info("The sync task is finished by scheduler");
            return SUCCESS;
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_SYNCTASK_HPP_ */