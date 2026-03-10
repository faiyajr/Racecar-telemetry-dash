#pragma once
#include "powertrain.h"
#include "chassis.h"

struct CanBus {
    // Powertrain DBC
    Pedals    pedals{};
    Wheel     wheel{};

    AmkActVal1Fl amk_actval_1_fl{};
    AmkActVal2Fl amk_actval_2_fl{};
    AmkActVal1Fr amk_actval_1_fr{};
    AmkActVal2Fr amk_actval_2_fr{};
    AmkActVal1Rl amk_actval_1_rl{};
    AmkActVal2Rl amk_actval_2_rl{};
    AmkActVal1Rr amk_actval_1_rr{};
    AmkActVal2Rr amk_actval_2_rr{};

    AmkSetptFl amk_setpt_fl{};

    AmsStatus ams_status{};
    AmsStatusPackState ams_status_pack_state{};
    AmsVoltSummary ams_volt_summary{};
    AmsTabTempSummary ams_tab_temp_summary{};
    AmsCellTempSummary ams_cell_temp_summary{};
    AmsFaults ams_faults{};
    AmsMask ams_mask{};
    AmsEnergetics ams_energetics{};
    
    // Chassis DBC
    RSns1    rsns1{};
    RSns2    rsns2{};
    RCurrent    rcurrent{};
    LvPwrOverride lv_pwr_override{};
    LvPwrCtrl lv_pwr_ctrl{};
    Front1 front1{};
    Front2 front2{};
    Front3 front3{};
    
};
