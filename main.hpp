// #pragma once

// #include "calibratables.h"
// #include "Simulink_Helper.h"

// #include "chassis.h"
// #include "powertrain.h"
// #include "candriver.hpp"


// #include "State_Machine.hpp"
// #include "Torque_Path.hpp"
// #include "State_Estimation.hpp"

// struct MainLoopInputs {
//     const Powertrain_Parameters& cpp;
//     const Chassis_Parameters& ccp;
//     const Enable_Parameters& cep;
// };


// class Main_Loop {


//     public:


//     void init();

//     CAN_Handler can_handler;
//     State_Estimation state_estimation;
//     // StateMachine state_machine;
//     TorquePath torque_path;

    

//   void run(const MainLoopInputs& inputs) {
//     // 0) Tick snapshot for old values
//     can_handler.snapshot_tick();

//     // 1) Update RX (blocking one frame, or replace with drain loop later)
//     can_handler.read_and_decode_once();

//     // 2) Run estimators/controllers using RX snapshot
//     state_estimation.run(can_handler.bus(), inputs.cpp, inputs.ccp);

//     torque_path.run(can_handler.bus(), state_estimation, inputs.cpp, inputs.cep);

//     // 3) Write outputs into TX snapshot (needs mutable bus)
//     torque_path.write(can_handler.bus_mut());

//     // 4) Pack + send TX frames explicitly
//     can_handler.send_messages();

//     // state_machine.run(can_handler.bus(), torque_path);
// }


// };






