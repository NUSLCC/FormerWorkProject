/*
 * @file WriteExcel.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */
#ifndef FLEXIVRDK_WRITEEXCEL_HPP_
#define FLEXIVRDK_WRITEEXCEL_HPP_

// third-party header files
#include <eigen3/Eigen/Eigen>

// Kostal header files
#include <kostal/KostalStates.hpp>
#include <kostal/SystemParams.h>

namespace kostal {
    
    // get current time in time_t format
    std::string getTime()
    {
        time_t timep;
        time (&timep); // get current time in time_t type
        char strTime[64];
        strftime(strTime, sizeof(strTime), "_%Y-%m-%d_%H:%M:%S",localtime(&timep) );
        return strTime;
    }

    // transfer a double array of tcp quaternion to an array of euler
    std::array<double, 3> quaternionToEuler(double* tcpPose){
        double M_Pi;
        double rad = 180/M_PI;
        Eigen::Quaternion<double> q;
        std::array<double, 3> eulerAngle;
        q.w() = tcpPose[0];
        q.x() = tcpPose[1];
        q.y() = tcpPose[2];
        q.z() = tcpPose[3];
        auto euler = q.toRotationMatrix().eulerAngles(2,1,0);
        eulerAngle[0] = euler[0];
        eulerAngle[1] = euler[1];
        eulerAngle[2] = euler[2];
        return eulerAngle;
    }

    /**
     * @class WriteExcelHandler
     * @brief Base class of writing different data into an excel file
     */
    class WriteExcelHandler
    {
    public:
        WriteExcelHandler() = default;
        virtual ~WriteExcelHandler() = default;

        /**
         * @brief Write the global spi data list and robot data list into a csv file with associated name
         * @param[in] taskType the type of the task, can be NORMAL, BIAS, DUMMY
         * @param[in] taskName the name of the task
         * @param[in] robotDataListPtr robot data list's pointer
         * @param[in] spiDataListPtr spi data list's pointer
         * @param[in] logPtr robot's log pointer
         * @return Status code
         */
        Status writeDataToExcel(std::string taskType,
                                std::string taskName, 
                                std::list<RobotData>* robotDataListPtr,
                                std::list<SPIData>* spiDataListPtr, 
                                flexiv::Log* logPtr)
        {
            
            if(spiDataListPtr->size() == 0){
                logPtr->error("The collected spi data list is null, exiting...");  
                return CSV;
            }

            if(robotDataListPtr->size() == 0){
                logPtr->error("The collected robot data list is null, exiting...");  
                return CSV;
            }

            std::fstream excelFile;
            std::string filePath = UPLOADADDRESS;
            std::string excelFileName = filePath + taskType + "/" + taskName + getTime() + ".csv";
            std::cout<<"The generated file path is: "<<excelFileName<<std::endl;
            excelFile.open(excelFileName, std::ios::out);
            if(!excelFile.is_open()){
                logPtr->error("The associated excel file is not created correctly");        
                return CSV;
            }

            //Header
            excelFile << "NodeName"<<",";
            excelFile << "TCP_x"<<","<<"TCP_y"<<"," << "TCP_z"<<"," ;
            excelFile << "TCP_Rx"<<","<<"TCP_Ry"<<"," << "TCP_Rz"<<"," ;
            excelFile << "FLANGE_x"<<","<<"FLANGE_y"<<"," << "FLANGE_z"<<"," ;
            excelFile << "FLANGE_Rx"<<","<<"FLANGE_Ry"<<"," << "FLANGR_Rz"<<",";
            excelFile << "RowDataSensor0"<<","<< "RowDataSensor1"<<","<< "RowDataSensor2"<<",";
            excelFile << "RowDataSensor3"<<","<< "RowDataSensor4"<<","<< "RowDataSensor5"<<",";
            excelFile << "SPI0-0"<<","<< "SPI0-1"<<","<< "SPI0-2"<<","<< "SPI0-3"<<","<< "SPI0-4"<<",";
            excelFile << "SPI0-5"<<","<< "SPI0-6"<<","<< "SPI0-7"<<",";
            excelFile << "SPI1-0"<<","<< "SPI1-1"<<","<< "SPI1-2"<<","<< "SPI1-3"<<","<< "SPI1-4"<<",";
            excelFile << "SPI1-5"<<","<< "SPI1-6"<<","<< "SPI1-7"<<","<< std::endl;

            while(robotDataListPtr->size()>0){

                RobotData robotDataRow = robotDataListPtr->front();
                SPIData spiDataRow = spiDataListPtr->front();
                //node name
                excelFile << robotDataRow.nodeName << ",";

                //tcp xyz
                excelFile << robotDataRow.tcpPose[0] << ",";
                excelFile << robotDataRow.tcpPose[1] << ",";
                excelFile << robotDataRow.tcpPose[2] << ",";

                //tcp euler data
                double quaternionTcp[4] = {robotDataRow.tcpPose[3], robotDataRow.tcpPose[4], robotDataRow.tcpPose[5], robotDataRow.tcpPose[6]};
                auto eulerTcp = quaternionToEuler(quaternionTcp);
                excelFile << eulerTcp[0] << ",";
                excelFile << eulerTcp[1] << ",";
                excelFile << eulerTcp[2] << ",";

                //flange xyz
                excelFile << robotDataRow.flangePose[0] << ",";
                excelFile << robotDataRow.flangePose[1] << ",";
                excelFile << robotDataRow.flangePose[2] << ",";
                
                //flange euler data
                double quaternionFlange[4] = {robotDataRow.flangePose[3], robotDataRow.flangePose[4], robotDataRow.flangePose[5], robotDataRow.flangePose[6]};
                auto eulerFlange = quaternionToEuler(quaternionFlange);
                excelFile << eulerFlange[0] << ",";
                excelFile << eulerFlange[1] << ",";
                excelFile << eulerFlange[2] << ",";

                //raw sensor data
                for (int i=0; i<6; i++){
                excelFile << robotDataRow.rawDataForceSensor[i] << ",";
                }

                //spi sensor data
                for (int i = 0; i < 16; i++){
                excelFile << std::setfill('0') << std::setw(2) << std::right<<std::hex ;
                excelFile << + static_cast<uint8_t>(spiDataRow.SPISensor[i])<<",";
                }
                //finish this line
                excelFile << std::endl;
                robotDataListPtr->pop_front();
                spiDataListPtr->pop_front();
            }

            excelFile.close();
            return SUCCESS;
        }

        /**
         * @brief Write the global robot data list into a csv file with associated name
         * @param[in] taskName the serial number of the work piece in message
         * @param[in] robotDataListPtr robot data list's pointer
         * @param[in] logPtr robot's log pointer
         * @return Status code
         */
        Status writeRobotDataToExcel(std::string taskName, 
                                    std::list<RobotData>* robotDataListPtr,
                                    flexiv::Log* logPtr)
        {   
            if(robotDataListPtr->size()==0){
                logPtr->error("The robot data list is null");  
                return CSV;
            }
            
            std::fstream excelFile;
            std::string filePath = "kostal-";
            std::string excelFileName = filePath + taskName + getTime() + ".csv";
            excelFile.open(excelFileName, std::ios::out);
            if(!excelFile.is_open()){
                logPtr->error("The excel file is not create correctly");        
                return CSV;
            }

            //Header
            excelFile << "NodeName"<<",";
            excelFile << "TCP_x"<<","<<"TCP_y"<<"," << "TCP_z"<<"," ;
            excelFile << "TCP_Rx"<<","<<"TCP_Ry"<<"," << "TCP_Rz"<<"," ;
            excelFile << "FLANGE_x"<<","<<"FLANGE_y"<<"," << "FLANGE_z"<<"," ;
            excelFile << "FLANGE_Rx"<<","<<"FLANGE_Ry"<<"," << "FLANGR_Rz"<<",";
            excelFile << "RowDataSensor0"<<","<< "RowDataSensor1"<<","<< "RowDataSensor2"<<",";
            excelFile << "RowDataSensor3"<<","<< "RowDataSensor4"<<","<< "RowDataSensor5"<<","<< std::endl;

            while(robotDataListPtr->size()>0){

                RobotData dataRow = robotDataListPtr->front();
                //node name
                excelFile << dataRow.nodeName << ",";

                //tcp xyz
                excelFile << dataRow.tcpPose[0] << ",";
                excelFile << dataRow.tcpPose[1] << ",";
                excelFile << dataRow.tcpPose[2] << ",";

                //tcp euler data
                double quaternionTcp[4] = {dataRow.tcpPose[3], dataRow.tcpPose[4], dataRow.tcpPose[5], dataRow.tcpPose[6]};
                auto eulerTcp = quaternionToEuler(quaternionTcp);
                excelFile << eulerTcp[0] << ",";
                excelFile << eulerTcp[1] << ",";
                excelFile << eulerTcp[2] << ",";

                //flange xyz
                excelFile << dataRow.flangePose[0] << ",";
                excelFile << dataRow.flangePose[1] << ",";
                excelFile << dataRow.flangePose[2] << ",";
                
                //flange euler data
                double quaternionFlange[4] = {dataRow.flangePose[3], dataRow.flangePose[4], dataRow.flangePose[5], dataRow.flangePose[6]};
                auto eulerFlange = quaternionToEuler(quaternionFlange);
                excelFile << eulerFlange[0] << ",";
                excelFile << eulerFlange[1] << ",";
                excelFile << eulerFlange[2] << ",";

                //raw sensor data
                for (int i=0; i<6; i++){
                excelFile << dataRow.rawDataForceSensor[i] << ",";
                }

                //finish this line
                excelFile << std::endl;
                robotDataListPtr->pop_front();
            }

            excelFile.close();
            return SUCCESS;
        }
    };

} /* namespace kostal */

#endif /* FLEXIVRDK_WRITEEXCEL_HPP_ */

