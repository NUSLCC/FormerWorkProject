/*
 * @file KostalStates.hpp
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 */

#ifndef KOSTALSTATES_HPP_
#define KOSTALSTATES_HPP_

// Kostal header files
#include <kostal/SystemParams.h>

namespace kostal {

    /**
     * @class RobotData
     * @brief The class of the robot data type
     */
    class RobotData
    {
        public:
            RobotData(){}
            virtual ~RobotData(){}
            std::string nodeName;
            std::vector<double> tcpPose; // [7]
            std::vector<double> flangePose; // [7]
            std::vector<double> rawDataForceSensor; // [6]
            
        public:
            RobotData(const RobotData& r){
                nodeName = r.nodeName;
                tcpPose  = r.tcpPose;
                flangePose = r.flangePose;
                rawDataForceSensor = r.rawDataForceSensor;
            }

    };

    /**
     * @class SPIData
     * @brief The class of the spi data type
     */
    class SPIData
    {
        public:
            SPIData(){}
            virtual ~SPIData(){}
            uint8_t SPISensor[16]= {0};
            SPIData& operator= (const uint8_t* buffer) {
                memcpy(SPISensor, buffer, sizeof(uint8_t) * 16);
                return *this;
            }
            
        public:
            SPIData(const SPIData& s){
                for (int i=0; i<16; i++){
                    SPISensor[i] = s.SPISensor[i];
                }
            }
    };

    /**
     * @class KostalData
     * @brief The class of the kostal data type (Not used yet)
     * It is the combination of the robot data and spi data
     */
    class KostalData
    {
        public:
            KostalData(){}
            virtual ~KostalData(){}
            std::string nodeName;
            std::vector<double> tcpPose; // [7]
            std::vector<double> flangePose; // [7]
            std::vector<double> rawDataForceSensor; // [6]
            uint8_t SPISensor[16]= {0};
            
        public:
            KostalData(const KostalData& k){
                nodeName = k.nodeName;
                tcpPose = k.tcpPose;
                flangePose = k.flangePose;
                rawDataForceSensor = k.rawDataForceSensor;
                for (int i=0; i<16; i++){
                    SPISensor[i] = k.SPISensor[i];
                }
            }
    };
} /* namespace kostal */

#endif