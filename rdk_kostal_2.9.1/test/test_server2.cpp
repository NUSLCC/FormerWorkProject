/**
 * @test test_kostal.cpp
 * Test kolstar connection function with flexiv Rizon
 * @copyright Copyright (C) 2016-2022 Flexiv Ltd. All Rights Reserved.
 * @author lcc@Flexiv
 */

// Kostal header files
#include <kostal/Communication.hpp>

int main(){
    flexiv::Log log;
    Status result;
    try{
        //we check robot connection and set robot to plan execution mode
        flexiv::Robot robot(ROBOTADDRESS, LOCALADDRESS);
        
        while(true)
        {
            // Instantiation the com object
            kostal::CommHandler com;
           
            // 1st Initialize the server
            result = com.init(&robot);
            if (result == SYSTEM){
                // failed to initialize, jump to stateMachine
            }else if (result == SUCCESS)
            {
                // initialize successfully, do nothing
            }else{
                // Other serious problem, need to reboot, break directly
                break;
            }
            
            // 2nd Put the system into a state machine
            com.stateMachine(&robot);
            
            //3rd Check whether system has a fault or not, if does, return 
            if (com.checkSeriousError()){
                break;
            }
        }

    } catch(const flexiv::Exception& e){
        log.error(e.what());
    }
}