#include "DashboardUI.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

// helper function to format float to 3 decimal places
std::string formatFloat(float value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(3) << value;
    return ss.str();
}

DashboardUI::DashboardUI() {
    fontSpeed = TTF_OpenFont(
        "../assets/fonts/DejaVuSans-Bold.ttf", 72
    );
    fontData = TTF_OpenFont(
        "../assets/fonts/DejaVuSans-Bold.ttf", 56
    );
    fontLabel = TTF_OpenFont(
        "../assets/fonts/DejaVuSans-Bold.ttf", 24
    );
    
    // check if fonts loaded successfully
    if (!fontSpeed || !fontData || !fontLabel) {
        std::cerr << "Failed to load fonts: " << TTF_GetError() << std::endl;
        std::cerr << "Make sure font file exists at: ../assets/fonts/DejaVuSans-Bold.ttf" << std::endl;
        throw std::runtime_error("Font loading failed");
    }
}

DashboardUI::~DashboardUI() {
    // only close fonts if they were successfully opened
    if (fontSpeed) TTF_CloseFont(fontSpeed);
    if (fontData) TTF_CloseFont(fontData);
    if (fontLabel) TTF_CloseFont(fontLabel);
}

// changing enum to string
const char* packStateToString(AmsStatusPackState state) {
    switch(state) {
        case UNDEFINED: return "UNDEF";
        case IDLE: return "IDLE";
        case PRECHARGING: return "PRECHG";
        case PRECHARGE_FAILED: return "PRECHG_FAIL";
        case DISCHARGE_FAILED: return "DISCHG_FAIL";
        case DISCHARGING: return "DISCHG";
        case ENERGIZED: return "ENERGZD";
        case IMD_FAULT: return "IMD_FLT";
        case BMS_FAULT: return "BMS_FLT";
        default: return "UNKNOWN";
    }
}

const char* vehicleStateToString(StateMachineVehicleState state){
    switch(state) {
        case UNDEFINED: return "UNDEFINED";
        case IDLE: return "IDLE";
        case ENERGIZED: return "ENERGZD";
        case READY: return "RETURN";
        case ENABLED: return "ENABLED";
        case DRIVE: return "DRIVE";
        case RESET: return "RESET";
        case FAULT: return "FAULT";
        default: return "UNKNOWN";
    }
}

void DashboardUI::drawPageOne(DisplayManager& d, const CanBus& b){
    int margin = 7;
    int gap = 5;

    int leftLeftEdge = margin;

    int sideBoxWidth = 195;
    int sideBoxHeight = 112;

    int middleLeftEdge = 207;
    int middleWidth = 385;

    int rightLeftEdge = 597;

    int topTopEdge = 7;
    int secondTopEdge = 124;
    int thirdTopEdge = 241;
    int fourthTopEdge = 358;

    // Check for errors ONLY if we have valid CAN data
    bool hasValidCANData = (b.ams_status.pack_voltage > 0.5);  // If voltage > 0.5V, we have real data
    bool hasAMSErrors = hasValidCANData && (b.ams_faults.pack_ov || b.ams_faults.pack_uv || 
                         b.ams_faults.pack_ot || b.ams_faults.pack_oc);

    d.drawRect(leftLeftEdge, topTopEdge, sideBoxWidth, sideBoxHeight);
    
    // mean cell volt box - flash if AMS errors
    if (hasAMSErrors) {
        Uint32 currentTime = SDL_GetTicks();
        bool flashOn = ((currentTime / 500) % 2) == 0;
        if (flashOn) {
            d.drawColoredRect(leftLeftEdge, secondTopEdge, sideBoxWidth, sideBoxHeight, 255, 0, 0);  // Bright red
        } else {
            d.drawColoredRect(leftLeftEdge, secondTopEdge, sideBoxWidth, sideBoxHeight, 180, 0, 0);  // Dark red
        }
    } else {
        d.drawRect(leftLeftEdge, secondTopEdge, sideBoxWidth, sideBoxHeight);
    }
    
    // mean cell temp box - flash if AMS errors
    if (hasAMSErrors) {
        Uint32 currentTime = SDL_GetTicks();
        bool flashOn = ((currentTime / 500) % 2) == 0;
        if (flashOn) {
            d.drawColoredRect(leftLeftEdge, thirdTopEdge, sideBoxWidth, sideBoxHeight, 255, 0, 0);  // Bright red
        } else {
            d.drawColoredRect(leftLeftEdge, thirdTopEdge, sideBoxWidth, sideBoxHeight, 180, 0, 0);  // Dark red
        }
    } else {
        d.drawRect(leftLeftEdge, thirdTopEdge, sideBoxWidth, sideBoxHeight);
    }
    
    // sdc stat box, green if closed, red if opened, gray if no CAN data
    if (hasValidCANData) {
        if (b.ams_status.pack_sdc_stat) {
            d.drawColoredRect(leftLeftEdge, fourthTopEdge, sideBoxWidth, sideBoxHeight, 0, 200, 0);  // green
        } else {
            d.drawColoredRect(leftLeftEdge, fourthTopEdge, sideBoxWidth, sideBoxHeight, 255, 0, 0);  // red
        }
    } else {
        d.drawRect(leftLeftEdge, fourthTopEdge, sideBoxWidth, sideBoxHeight);  // gray (no data)
    }

    d.drawRect(middleLeftEdge, topTopEdge, middleWidth, 2 * sideBoxHeight + 5);
    d.drawRect(middleLeftEdge, thirdTopEdge, middleWidth, sideBoxHeight);
    d.drawRect(middleLeftEdge, fourthTopEdge, middleWidth, sideBoxHeight);

    d.drawRect(rightLeftEdge, topTopEdge, sideBoxWidth, sideBoxHeight);
    d.drawRect(rightLeftEdge, secondTopEdge, sideBoxWidth, sideBoxHeight);
    d.drawRect(rightLeftEdge, thirdTopEdge, sideBoxWidth, sideBoxHeight);
    d.drawRect(rightLeftEdge, fourthTopEdge, sideBoxWidth, sideBoxHeight);

    int sideLeftOffset = sideBoxWidth / 10;
    int sideTopOffset = sideBoxHeight / 3;

    int middleLeftOffset = middleWidth / 2.7;
    int middleTopOffset = sideTopOffset / 2;

    int textOffset = 45;
    int textHeightOffset = 50;

    //form:
    // labels
    // field values from can

    d.drawText("SPEED MPH", middleLeftEdge + middleLeftOffset, topTopEdge + middleTopOffset, fontLabel);
    d.drawText(formatFloat(b.speed.vehiclespeed_mph), 
        middleLeftEdge + middleLeftOffset + 
        textOffset, topTopEdge + middleTopOffset + 
        textHeightOffset, fontData); // chassis.h float vehiclespeed_mph line 309

    d.drawText("CAR STATE", middleLeftEdge + middleLeftOffset, thirdTopEdge + middleTopOffset, fontLabel);
    d.drawText(vehicleStateToString(b.statemachine.vehicle_state), 
        middleLeftEdge + middleLeftOffset + 
        textOffset, thirdTopEdge + 
        middleTopOffset + textHeightOffset, fontLabel); // not sure where to get car state from
   
    d.drawText("PACK STATE", middleLeftEdge + middleLeftOffset, fourthTopEdge + middleTopOffset, fontLabel);
    d.drawText(packStateToString(b.ams_status.pack_state), 
        middleLeftEdge + middleLeftOffset + 
        textOffset, fourthTopEdge + 
        middleTopOffset + textHeightOffset, fontLabel); // powertrain.h AmsStatusPackState pack_state line 261

    d.drawText("PV", leftLeftEdge + sideLeftOffset, topTopEdge + sideTopOffset, fontLabel);
    d.drawText(formatFloat(b.ams_status.pack_voltage), 
        leftLeftEdge + sideLeftOffset + textOffset, 
        topTopEdge + sideTopOffset, fontLabel); // powertrain.h  float pack_voltage line 248
    
    d.drawText("MCV", leftLeftEdge + sideLeftOffset, secondTopEdge + sideTopOffset, fontLabel);
    d.drawText(formatFloat(b.ams_energetics.pack_mean_cell_ocv), 
        leftLeftEdge + sideLeftOffset + textOffset + 10, 
        secondTopEdge + sideTopOffset, fontLabel); // powertrain.h float pack_mean_cell_ocv line 334
   
    d.drawText("MCT", leftLeftEdge + sideLeftOffset, thirdTopEdge + sideTopOffset, fontLabel);
    d.drawText(formatFloat(b.ams_cell_temp_summary.pack_cell_mean_temp), 
        leftLeftEdge + sideLeftOffset + textOffset + 10, 
        thirdTopEdge + sideTopOffset, fontLabel); // powertrain.h float pack_cell_max_temp line 294
   
    d.drawText("SDCS", leftLeftEdge + sideLeftOffset, fourthTopEdge + sideTopOffset, fontLabel);
    d.drawText(b.ams_status.pack_sdc_stat ? "true" : "false", 
        leftLeftEdge + sideLeftOffset + textOffset + 20, 
        fourthTopEdge + sideTopOffset, fontLabel); // powertrain.h bool_pack_stat line 250

    d.drawText("TIME", rightLeftEdge + sideLeftOffset, topTopEdge + sideTopOffset, fontLabel);
    d.drawText("????", rightLeftEdge + sideLeftOffset + 
        textOffset + 10, topTopEdge + 
        sideTopOffset, fontLabel); // i think in chassis.h its telemtime but not sure
   
    d.drawText("BRAKE BIAS", rightLeftEdge + sideLeftOffset, secondTopEdge + sideTopOffset, fontLabel);
    d.drawText(formatFloat(b.driverinputs.brakebias), 
        rightLeftEdge + sideLeftOffset + 
        textOffset + 10, secondTopEdge + 
        sideTopOffset + 20, fontLabel); // this is in chassis.h for driverinputs but not in the canbus 
   
    d.drawText("LV BAT", rightLeftEdge + sideLeftOffset, thirdTopEdge + sideTopOffset, fontLabel);
    d.drawText(formatFloat(b.rsns2.lv_batt_voltage), 
    rightLeftEdge + sideLeftOffset + textOffset + 10, 
    thirdTopEdge + sideTopOffset + 20, fontLabel); // believe this is correct in chassis.h line 47 float lv_batt_voltage

    // error popup
    bool criticalError = false;
    std::string errorMessage = "";

    // only check for errors if we have valid CAN data (pack voltage > 0 means we're getting data)
    bool hasCANData = (b.ams_status.pack_voltage > 0.5);  // If voltage > 0.5V, we have real data

    if (b.ams_faults.pack_ov_fault && hasCANData) {
        criticalError = true;
        errorMessage = "OVERVOLTAGE";
    } else if (b.ams_faults.pack_ot_fault && hasCANData) {
        criticalError = true;
        errorMessage = "OVERTEMP";
    } else if (!b.ams_status.pack_sdc_stat && hasCANData) {
        criticalError = true;
        errorMessage = "SDC OPEN";
    }

    // popup creation - bigger size
    if (criticalError) {
        int popupW = 400;  // bigger width
        int popupH = 150;  // bigger height
        int popupX = (800 - popupW) / 2;  // center horiz
        int popupY = (480 - popupH) / 2;  // center vert
        
        d.drawErrorPopup(errorMessage, popupX, popupY, popupW, popupH, true);
        d.drawText(errorMessage, popupX + 80, popupY + 60, fontLabel);  // adjusted position for bigger popup
    }
}

void DashboardUI::drawPageTwo(DisplayManager& d, const CanBus& b) {
    int margin = 7;
    int gap = 5;

    int leftLeftEdge = margin;
    int leftWidth = 385;                         
    int rightLeftEdge = leftLeftEdge + leftWidth + gap;
    int rightWidth = 800 - margin - rightLeftEdge;

    int topEdge = margin;

    // lft side layout
    int voltBoxH = 80;
    int tempBoxH = 80;
    int amsErrBoxH = 60;
    int appsErrBoxH = 60;
    int sdcBoxH = 60;
    int sdcDetailBoxH = (480 - 2 * margin) - (voltBoxH + tempBoxH + amsErrBoxH + appsErrBoxH + sdcBoxH + 5 * gap);

    int row1Y = topEdge;
    int row2Y = row1Y + voltBoxH + gap;
    int row3Y = row2Y + tempBoxH + gap;
    int row4Y = row3Y + amsErrBoxH + gap;
    int row5Y = row4Y + appsErrBoxH + gap;
    int row6Y = row5Y + sdcBoxH + gap;

    // check for errors ONLY if we have valid CAN data
    bool hasValidCANData = (b.ams_status.pack_voltage > 0.5);  // if voltage > 0.5V, we have real data
    bool hasAMSErrors = hasValidCANData && (b.ams_faults.pack_ov || b.ams_faults.pack_uv || 
                         b.ams_faults.pack_ot || b.ams_faults.pack_oc);

    // draw left side boxes with conditional colors
    d.drawRect(leftLeftEdge, row1Y, leftWidth, voltBoxH);      // cell volt
    d.drawRect(leftLeftEdge, row2Y, leftWidth, tempBoxH);      // temp
    
    // ams errors box, flash if errors detected
    if (hasAMSErrors) {
        Uint32 currentTime = SDL_GetTicks();
        bool flashOn = ((currentTime / 500) % 2) == 0;
        if (flashOn) {
            d.drawColoredRect(leftLeftEdge, row3Y, leftWidth, amsErrBoxH, 255, 0, 0);  // bright red
        } else {
            d.drawColoredRect(leftLeftEdge, row3Y, leftWidth, amsErrBoxH, 180, 0, 0);  // dark red
        }
    } else {
        d.drawRect(leftLeftEdge, row3Y, leftWidth, amsErrBoxH);  // normal 
    }
    
    d.drawRect(leftLeftEdge, row4Y, leftWidth, appsErrBoxH);   // apps error
    
    // SDC status - green if closed, red if open, gray if no CAN data
    if (hasValidCANData) {
        if (b.ams_status.pack_sdc_stat) {
            d.drawColoredRect(leftLeftEdge, row5Y, leftWidth, sdcBoxH, 0, 255, 0);  // green
        } else {
            d.drawColoredRect(leftLeftEdge, row5Y, leftWidth, sdcBoxH, 255, 0, 0);  // red
        }
    } else {
        d.drawRect(leftLeftEdge, row5Y, leftWidth, sdcBoxH);  // gray (no data)
    }
    
    d.drawRect(leftLeftEdge, row6Y, leftWidth, sdcDetailBoxH); // SDC details

    // right side
    int motorBoxH = 140;
    int socBoxH = 120;
    int motorBoxHBottom = (480 - 2 * margin) - (motorBoxH + socBoxH + 2 * gap);

    int rightTopY = topEdge;
    int socY = rightTopY + motorBoxH + gap;
    int rightBotY = socY + socBoxH + gap;

    int rightHalfW = (rightWidth - gap) / 2;
    int rightCol1X = rightLeftEdge;
    int rightCol2X = rightLeftEdge + rightHalfW + gap;

    // Draw right side boxes
    d.drawRect(rightCol1X, rightTopY, rightHalfW, motorBoxH);     // fl motor
    d.drawRect(rightCol2X, rightTopY, rightHalfW, motorBoxH);     // fr motor
    d.drawRect(rightLeftEdge, socY, rightWidth, socBoxH);         // soc
    d.drawRect(rightCol1X, rightBotY, rightHalfW, motorBoxHBottom); // rl motor
    d.drawRect(rightCol2X, rightBotY, rightHalfW, motorBoxHBottom); // rr motor

    int padX = 15;
    int padY = 15;
    
    // cell volt box
    d.drawText("Cell Voltage", leftLeftEdge + padX, row1Y + padY, fontLabel);
    std::string voltText = "Min: " + formatFloat(b.ams_volt_summary.pack_min_volt) + "V  " +
                          "Mean: " + formatFloat(b.ams_volt_summary.pack_mean_volt) + "V";
    d.drawText(voltText, leftLeftEdge + padX, row1Y + padY + 25, fontLabel);

    // temp box
    d.drawText("Temperature", leftLeftEdge + padX, row2Y + padY, fontLabel);
    std::string cellTempText = "Cell: " + formatFloat(b.ams_cell_temp_summary.pack_cell_mean_temp) + "/" + 
                               formatFloat(b.ams_cell_temp_summary.pack_cell_max_temp) + "C";
    d.drawText(cellTempText, leftLeftEdge + padX, row2Y + padY + 20, fontLabel);
    
    std::string tabTempText = "Tab: " + formatFloat(b.ams_tab_temp_summary.pack_tab_mean_temp) + "/" + 
                              formatFloat(b.ams_tab_temp_summary.pack_tab_max_temp) + "C";
    d.drawText(tabTempText, leftLeftEdge + padX, row2Y + padY + 40, fontLabel);

    // ams errors bbox
    d.drawText("AMS Errors", leftLeftEdge + padX, row3Y + padY, fontLabel);
    std::string amsErrors = "";
    if (b.ams_faults.pack_ov) {
        amsErrors += "OV ";
    }

    if (b.ams_faults.pack_uv) {
        amsErrors += "UV ";
    }

    if (b.ams_faults.pack_ot) {
        amsErrors += "OT ";
    }

    if (b.ams_faults.pack_oc) {
        amsErrors += "OC ";
    }

    if (amsErrors.empty()) {
        amsErrors = "None";
    }

    d.drawText(amsErrors, leftLeftEdge + padX, row3Y + padY + 25, fontLabel);

    // apps error box NEED TO IMPLEMENT AFTER MESSAGE IS CREATED
    d.drawText("APPS Status", leftLeftEdge + padX, row4Y + padY, fontLabel);
    d.drawText("???", leftLeftEdge + padX + 150, row4Y + padY, fontLabel);

    // shutdown circuit status
    d.drawText("Shutdown Circuit", leftLeftEdge + padX, row5Y + padY, fontLabel);
    d.drawText(b.ams_status.pack_sdc_stat ? "CLOSED" : "OPEN", 
               leftLeftEdge + padX + 180, row5Y + padY, fontLabel);
 
    // which segments are open
    d.drawText("Open Segments:", leftLeftEdge + padX, row6Y + padY, fontLabel);
    
    std::string openSegments = "";
    int yOffset = padY + 25;
    
    if (b.ams_status.pack_rr_stat) {
        d.drawText("- Rear Right", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_rl_stat) {
        d.drawText("- Rear Left", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_fr_stat) {
        d.drawText("- Front Right", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_fl_stat) {
        d.drawText("- Front Left", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_mc_stat) {
        d.drawText("- Motor Controller", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_ac_stat) {
        d.drawText("- AIRs Closed", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_hvd_msd_stat) {
        d.drawText("- HVD/MSD", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_bspd_stat) {
        d.drawText("- BSPD", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_imd_stat) {
        d.drawText("- IMD", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    if (b.ams_status.pack_bms_stat) {
        d.drawText("- BMS", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
        yOffset += 20;
    }
    
    // if nothing open, then none-all closed displayed
    if (!b.ams_status.pack_rr_stat && !b.ams_status.pack_rl_stat && 
        !b.ams_status.pack_fr_stat && !b.ams_status.pack_fl_stat &&
        !b.ams_status.pack_mc_stat && !b.ams_status.pack_ac_stat &&
        !b.ams_status.pack_hvd_msd_stat && !b.ams_status.pack_bspd_stat &&
        !b.ams_status.pack_imd_stat && !b.ams_status.pack_bms_stat) {
        d.drawText("None - All Closed", leftLeftEdge + padX + 10, row6Y + yOffset, fontLabel);
    }

    int motorLabelY = 15;
    int motorTempY = 35;
    int invLabelY = 65;
    int invTempY = 85;

    // fl motor
    d.drawText("FL Motor", rightCol1X + padX, rightTopY + motorLabelY, fontLabel);
    d.drawText("MT: " + formatFloat(b.amk_actval_2_fl.amk_tempmotor) + "C", 
               rightCol1X + padX, rightTopY + motorTempY, fontLabel);
    d.drawText("INVT: " + formatFloat(b.amk_actval_2_fl.amk_tempinverter) + "C", 
               rightCol1X + padX, rightTopY + invTempY, fontLabel);

    // fr motor
    d.drawText("FR Motor", rightCol2X + padX, rightTopY + motorLabelY, fontLabel);
    d.drawText("MT: " + formatFloat(b.amk_actval_2_fr.amk_tempmotor) + "C", 
               rightCol2X + padX, rightTopY + motorTempY, fontLabel);
    d.drawText("INVT: " + formatFloat(b.amk_actval_2_fr.amk_tempinverter) + "C", 
               rightCol2X + padX, rightTopY + invTempY, fontLabel);

    // SOC 
    d.drawText("State of Charge", rightLeftEdge + padX, socY + 20, fontLabel);
    d.drawText(formatFloat(b.ams_energetics.pack_soc) + "%", 
               rightLeftEdge + 140, socY + 45, fontData);
    int barX = rightLeftEdge + padX;
    int barY = socY + 85;
    int barWidth = rightWidth - 2 * padX;
    int barHeight = 25; 
    d.drawFilledBar(barX, barY, barWidth, barHeight, b.ams_energetics.pack_soc);

    // rl motor
    d.drawText("RL Motor", rightCol1X + padX, rightBotY + motorLabelY, fontLabel);
    d.drawText("MT: " + formatFloat(b.amk_actval_2_rl.amk_tempmotor) + "C", 
               rightCol1X + padX, rightBotY + motorTempY, fontLabel);
    d.drawText("INVT: " + formatFloat(b.amk_actval_2_rl.amk_tempinverter) + "C", 
               rightCol1X + padX, rightBotY + invTempY, fontLabel);

    // rr motor
    d.drawText("RR Motor", rightCol2X + padX, rightBotY + motorLabelY, fontLabel);
    d.drawText("MT: " + formatFloat(b.amk_actval_2_rr.amk_tempmotor) + "C", 
               rightCol2X + padX, rightBotY + motorTempY, fontLabel);
    d.drawText("INVT: " + formatFloat(b.amk_actval_2_rr.amk_tempinverter) + "C", 
               rightCol2X + padX, rightBotY + invTempY, fontLabel);

    // error pop up
    bool criticalError = false;
    std::string errorMessage = "";

    // only check for errors if we have valid CAN data (pack voltage > 0 means we're getting data)
    bool hasCANData = (b.ams_status.pack_voltage > 0.5);  // if voltage > 0.5V, we have real data

    if (b.ams_faults.pack_ov_fault && hasCANData) {
        criticalError = true;
        errorMessage = "OVERVOLTAGE";
    } else if (b.ams_faults.pack_ot_fault && hasCANData) {
        criticalError = true;
        errorMessage = "OVERTEMP";
    } else if (!b.ams_status.pack_sdc_stat && hasCANData) {
        criticalError = true;
        errorMessage = "SDC OPEN";
    }

    // popup creation - BIGGER size
    if (criticalError) {
        int popupW = 400;  // bigger width
        int popupH = 150;  // bigger height
        int popupX = (800 - popupW) / 2;  // center horiz
        int popupY = (480 - popupH) / 2;  // center vert
        
        d.drawErrorPopup(errorMessage, popupX, popupY, popupW, popupH, true);
        d.drawText(errorMessage, popupX + 80, popupY + 60, fontLabel);  // Adjusted position for bigger popup
    }
}