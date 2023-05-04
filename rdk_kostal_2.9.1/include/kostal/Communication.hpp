/*
 * @file Communication.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */
#ifndef FLEXIVRDK_COMMUNICATION_HPP_
#define FLEXIVRDK_COMMUNICATION_HPP_

// Kostal header files
#include <kostal/SystemParams.h>
#include <kostal/KostalStates.hpp>
#include <kostal/SyncServer.hpp>
#include <kostal/JsonParser.hpp>
#include <kostal/RobotOperation.hpp>
#include <kostal/SPIOperation.hpp>
#include <kostal/SyncTask.hpp>
#include <kostal/WriteExcel.hpp>

using namespace boost::posix_time;

namespace kostal {
    
    // object of robot data
    kostal::RobotData robotData;
    // object of spi data
    kostal::SPIData spiData;
    // the list to store robot data
    std::list<kostal::RobotData> g_robotDataList;
    // the list to store spi data
    std::list<kostal::SPIData> g_spiDataList; 

    /**
     * @class CommHandler
     * @brief Base class for flexiv system to communicate with outside client
     */
    class CommHandler{
    private:
        std::atomic<serverStatus> flexivStatus = {INIT};
        std::atomic<bool> seriousError = {false};
        std::atomic<bool> spiStatus = {false};
        std::atomic<bool> hbSwitch = {false};
        std::atomic<bool> checkStatus = {false};
        kostal::Server m_service;
        flexiv::Log f_log;
        std::string m_queryStatus;
        std::string m_taskType;
        std::string m_taskName;
        boost::asio::thread_pool t_pool;
        // some member variable under kostal namespace
        kostal::Log k_log;
        kostal::JSONMessageHandler m_parser;
        kostal::RobotOperationHandler m_robotHandler;
        kostal::SyncTaskHandler m_stHandler;
        kostal::SPIOperationHandler m_spiHandler;
        kostal::WriteExcelHandler m_weHandler;

    public:
        CommHandler() = default;
        virtual ~CommHandler() = default;
        
        /**
         * @brief This function inits the communication between server and client,
         * the client needs to send SPI message first
         * @param[in] robotPtr Pointer to robot object
         * @return Flexiv status code
         */
        Status init(flexiv::Robot* robotPtr)
        {
            Status result;
            
            // check robot connection and set robot to plan execution mode
            result = m_robotHandler.buildRobotConnection(robotPtr, &f_log);
            if (result != SUCCESS) 
            {
                
                k_log.error("The flexiv system failed to initialize the robot!");
                k_log.error("Please recover the robot and then reboot it");
                k_log.error("===================================================");
                flexivStatus=FAULT;
                seriousError=true;
            }else{
                f_log.info("The robot connection is built successfully");
            }

            // start to initialize the server and get spi config
            result = m_service.init();
            if (result != SUCCESS)
            {
                k_log.error("The flexiv system fails to initialize the socket server");
                return result;
            }

            // check spi connection
            //result = m_spiHandler.buildSPIConnectionDummy();
            result = m_spiHandler.buildSPIConnectionSocket();
            
            if (result != SUCCESS) 
            {
                flexivStatus = FAULT;
                seriousError=true;
            }else{
                k_log.info("The spi connection is built successfully");
            }
            
            // If flexiv status has been changed from INIT
            if (flexivStatus!=INIT)
            {
                k_log.error("The flexiv system failed in initialization, please check!");
                flexivStatus==FAULT;
                return SYSTEM;
            }else{
                f_log.info("The flexiv system is initialized successfully");
                flexivStatus = IDLE;
                return SUCCESS;
            }
        }

        /**
         * @brief This function checks the received message and see whether 
         * the query status is set as yes or no
         * @return Flexiv status code
         */
        Status executeCheck()
        {
            Status result;
            std::string checkMsg = m_service.getRecvMsg();
            result = m_parser.parseJSON(&checkMsg, &m_queryStatus, &k_log);
            if (result != SUCCESS){
                flexivStatus = FAULT;
                f_log.error("The task message is failed to be parsed");   
                return result;
            }
            if (m_queryStatus=="true" || 
                m_queryStatus=="yes" || 
                m_queryStatus=="True" || 
                m_queryStatus=="Yes")
            {
                checkStatus = true;
            }else{
                checkStatus = false;
            }
            return SUCCESS;
        }

        /**
         * @brief This function executes the task after the task message is correctly received and parsed
         * @param[in] robotPtr Pointer to robot object
         * @return Flexiv status code
         */
        Status executeTask(flexiv::Robot* robotPtr)
        {
            Status result;        
            std::string taskMsg = m_service.getRecvMsg();        
            
            result = m_parser.parseJSON(&taskMsg, &m_queryStatus, &m_taskType, &m_taskName, &k_log);
            if (result != SUCCESS){
                flexivStatus = FAULT;
                f_log.error("The task message is failed to be parsed");
                f_log.error("===================================================");
                return result;
            }
            //f_log.info("The task message is parsed successfully");        
            
            result = m_stHandler.runScheduler(robotPtr, &spiData, &g_spiDataList, &robotData, &g_robotDataList, &f_log, m_taskName + "-" + m_taskType);
            if (result != SUCCESS){
                flexivStatus = FAULT;
                f_log.error("The sync task is failed to be executed");
                f_log.error("===================================================");
                return result;
            }
            std::cout<<"spi list size is "<<g_spiDataList.size()<<std::endl;
            std::cout<<"robot list size is "<<g_robotDataList.size()<<std::endl;
            
            
            result = m_weHandler.writeDataToExcel(m_taskType, m_taskName, &g_robotDataList, &g_spiDataList, &f_log);
            if (result != SUCCESS){
                flexivStatus = FAULT;
                f_log.error("The excel file is failed to be generated");
                f_log.error("===================================================");
                return result;
            }
            f_log.info("The excel file is generated successfully");
            f_log.info("****************************************************");
            f_log.info("The task is executed successfully");
            f_log.info("****************************************************");
            flexivStatus = IDLE;
            return SUCCESS;
        }

        /**
         * @brief This is a state machine to switch flexiv system from different status
         * @param[in] robotPtr Pointer to robot object
         * @return Flexiv status code
         */
        void stateMachine(flexiv::Robot* robotPtr)
        {
            Status result;
            while (true)
            {
                switch (flexivStatus)
                {
                    case IDLE:
                    {
                        result = m_robotHandler.clearTinyFault(robotPtr, &f_log);
                        if (result != SUCCESS)
                        {
                            flexivStatus = FAULT;
                            seriousError = true;
                            k_log.error("Please recover the robot and then reboot it");
                            break;
                        }
                        m_service.setReplyMsg("IDLE");
                        k_log.info("The flexiv system is in idle mode, ready to talk...");
                        // Check whether the connection is timeout
                        result = m_service.monitor();
                        if (result != SUCCESS)
                        {
                            k_log.error("The flexiv system is having an error in connection");
                            k_log.error("===================================================");
                            flexivStatus = FAULT;
                            return;
                        }
                        // Check whether the client ask status
                        result = executeCheck();
                        if (result != SUCCESS)
                        {
                            flexivStatus = FAULT;
                            k_log.error("The flexiv system is having an error in server status checking");
                            break;
                        }
                        if (checkStatus==true){
                            m_service.setReplyMsg("BUSY"); // Or you can use other words to show you received task msg
                            result = m_service.monitor();
                            if (result != SUCCESS)
                            {
                                k_log.error("The flexiv system is having an error in connection");
                                k_log.error("===================================================");
                                flexivStatus = FAULT;
                                return;
                            }
                            flexivStatus = BUSY;
                            boost::asio::post(t_pool, boost::bind(&CommHandler::executeTask, this, robotPtr));
                            break;
                        }else{
                            k_log.warn("*************************************************");
                            k_log.warn("The client is not checking the server status in socket message");
                            break;
                        }
                    }

                    case BUSY:
                    {
                        m_service.setReplyMsg("BUSY");
                        result = m_service.monitor();
                        if (result != SUCCESS)
                        {
                            k_log.error("The flexiv system is having an error in connection");
                            k_log.error("===================================================");
                            t_pool.join();
                            flexivStatus = FAULT;
                            return;
                        }
                        //std::this_thread::sleep_for(std::chrono::seconds(2));
                        break;
                    }

                    case FAULT:
                    {
                        m_service.setReplyMsg("FAULT");
                        k_log.error("The flexiv system is in fault mode...");
                        k_log.error("===================================================");
                        
                        result = m_service.monitor();
                        if (result != SUCCESS)
                        {
                            k_log.error("The flexiv system is having an error in connection");
                            k_log.error("===================================================");
                            flexivStatus = FAULT;
                            return;
                        }
                        m_service.disconnect();
                        return;
                    }
                }
                //std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }    
        
        /**
         * @brief Returen whether the flexiv system is currently in fault mode
         * @return true or false
         */
        bool checkFault(){
            if (flexivStatus == FAULT){
                return true;
            }else{
                return false;
            }
        }

        /**
         * @brief Returen whether the flexiv system is currently in serious error
         * if yes, you need to reboot
         * @return true or false
         */
        bool checkSeriousError(){
            if (seriousError == true){
                return true;
            }else{
                return false;
            }
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_COMMUNICATION_HPP_ */