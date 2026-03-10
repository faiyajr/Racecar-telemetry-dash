#include "DashboardUI.h"
#include <string>
#include <iostream>  // ADDED for error messages

DashboardUI::DashboardUI() {
    fontSpeed = TTF_OpenFont(
        "../assets/fonts/DejaVuSans-Bold.ttf", 48
    );
    fontData = TTF_OpenFont(
        "../assets/fonts/DejaVuSans-Bold.ttf", 48
    );
    fontLabel = TTF_OpenFont(
        "../assets/fonts/DejaVuSans-Bold.ttf", 18
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

    d.drawRect(leftLeftEdge, topTopEdge, sideBoxWidth, sideBoxHeight);
    d.drawRect(leftLeftEdge, secondTopEdge, sideBoxWidth, sideBoxHeight);
    d.drawRect(leftLeftEdge, thirdTopEdge, sideBoxWidth, sideBoxHeight);
    d.drawRect(leftLeftEdge, fourthTopEdge, sideBoxWidth, sideBoxHeight);

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

    // labels
    // field values from can

    d.drawText("SPEED MPH", middleLeftEdge + middleLeftOffset, topTopEdge + middleTopOffset, fontLabel);
    d.drawText(std::to_string(b.amk_actval_1_fl.amk_actualvelocity), 
        middleLeftEdge + middleLeftOffset + 
        textOffset, topTopEdge + middleTopOffset + 
        textHeightOffset, fontData); // chassis.h float vehiclespeed_mph line 309

    d.drawText("CAR STATE", middleLeftEdge + middleLeftOffset, thirdTopEdge + middleTopOffset, fontLabel);
    d.drawText("?????", 
        middleLeftEdge + middleLeftOffset + 
        textOffset, thirdTopEdge + 
        middleTopOffset + textHeightOffset, fontLabel); // not sure where to get car state from
   
    d.drawText("PACK STATE", middleLeftEdge + middleLeftOffset, fourthTopEdge + middleTopOffset, fontLabel);
    d.drawText(packStateToString(b.ams_status.pack_state), 
        middleLeftEdge + middleLeftOffset + 
        textOffset, fourthTopEdge + 
        middleTopOffset + textHeightOffset, fontLabel); // powertrain.h AmsStatusPackState pack_state line 261

    d.drawText("PV", leftLeftEdge + sideLeftOffset, topTopEdge + sideTopOffset, fontLabel);
    d.drawText(std::to_string(b.ams_status.pack_voltage), 
        leftLeftEdge + sideLeftOffset + textOffset, 
        topTopEdge + sideTopOffset, fontLabel); // powertrain.h  float pack_voltage line 248
    
    d.drawText("MCV", leftLeftEdge + sideLeftOffset, secondTopEdge + sideTopOffset, fontLabel);
    d.drawText(std::to_string(b.ams_energetics.pack_mean_cell_ocv), 
        leftLeftEdge + sideLeftOffset + textOffset + 10, 
        secondTopEdge + sideTopOffset, fontLabel); // powertrain.h float pack_mean_cell_ocv line 334
   
    d.drawText("MCT", leftLeftEdge + sideLeftOffset, thirdTopEdge + sideTopOffset, fontLabel);
    d.drawText(std::to_string(b.ams_cell_temp_summary.pack_cell_mean_temp), 
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
    d.drawText("?????", rightLeftEdge + sideLeftOffset + 
        textOffset + 10, secondTopEdge + 
        sideTopOffset + 20, fontLabel); // this is in chassis.h for driverinputs but not in the canbus 
   
    d.drawText("LV BAT", rightLeftEdge + sideLeftOffset, thirdTopEdge + sideTopOffset, fontLabel);
    d.drawText(std::to_string(b.rsns2.lv_batt_voltage), 
    rightLeftEdge + sideLeftOffset + textOffset + 10, 
    thirdTopEdge + sideTopOffset + 20, fontLabel); // believe this is correct in chassis.h line 47 float lv_batt_voltage
}

/*void DashboardUI::drawPageTwo(DisplayManager& d, int speed, int rpm, int gear) {
    // ... (no changes to this function)
    int margin = 7;
    int gap = 5;

    int leftLeftEdge = margin;
    int leftWidth = 385;                         
    int rightLeftEdge = leftLeftEdge + leftWidth + gap;
    int rightWidth = 800 - margin - rightLeftEdge;

    int topEdge = margin;

    int topRowH = 80;
    int errRowH = 60;   
    int sdcRowH = 60;   
    int bottomRowH = (480 - 2 * margin) - (topRowH + 3 * errRowH + sdcRowH + 5 * gap);

    int row1Y = topEdge;
    int row2Y = row1Y + topRowH + gap;
    int row3Y = row2Y + errRowH + gap;
    int row4Y = row3Y + errRowH + gap;
    int row5Y = row4Y + errRowH + gap;
    int row6Y = row5Y + sdcRowH + gap;

    int halfW = (leftWidth - gap) / 2;
    int leftBox1X = leftLeftEdge;
    int leftBox2X = leftLeftEdge + halfW + gap;

    d.drawRect(leftBox1X, row1Y, halfW, topRowH);
    d.drawRect(leftBox2X, row1Y, halfW, topRowH);
    d.drawRect(leftLeftEdge, row2Y, leftWidth, errRowH);
    d.drawRect(leftLeftEdge, row3Y, leftWidth, errRowH);
    d.drawRect(leftLeftEdge, row4Y, leftWidth, errRowH);
    d.drawRect(leftLeftEdge, row5Y, leftWidth, sdcRowH);
    d.drawRect(leftLeftEdge, row6Y, leftWidth, bottomRowH);

    int motorBoxHTop = 140;
    int socBoxH = 120;
    int motorBoxHBottom = (480 - 2 * margin) - (motorBoxHTop + socBoxH + 2 * gap);

    int rightTopY = topEdge;
    int socY = rightTopY + motorBoxHTop + gap;
    int rightBotY = socY + socBoxH + gap;

    int rightHalfW = (rightWidth - gap) / 2;
    int rightCol1X = rightLeftEdge;
    int rightCol2X = rightLeftEdge + rightHalfW + gap;

    d.drawRect(rightCol1X, rightTopY, rightHalfW, motorBoxHTop);
    d.drawRect(rightCol2X, rightTopY, rightHalfW, motorBoxHTop);
    d.drawRect(rightLeftEdge, socY, rightWidth, socBoxH);
    d.drawRect(rightCol1X, rightBotY, rightHalfW, motorBoxHBottom);
    d.drawRect(rightCol2X, rightBotY, rightHalfW, motorBoxHBottom);

    int padX = 18;
    int padY = 22;

    d.drawText("Min/Max V", leftBox1X + padX, row1Y + padY, fontLabel);
    d.drawText("Min/Max T", leftBox2X + padX, row1Y + padY, fontLabel);

    d.drawText("IMD Error", leftLeftEdge + padX, row2Y + padY, fontLabel);
    d.drawText("AMS Error", leftLeftEdge + padX, row3Y + padY, fontLabel);
    d.drawText("APPS Error", leftLeftEdge + padX, row4Y + padY, fontLabel);

    d.drawText("Shutdown Circuit Open/Closed", leftLeftEdge + padX, row5Y + padY, fontLabel);
    d.drawText("If Open, list where here", leftLeftEdge + padX, row6Y + padY, fontLabel);

    int motorLine1Y = padY + 10;
    int motorLine2Y = motorLine1Y + 40;

    d.drawText("MT1",   rightCol1X + padX, rightTopY + motorLine1Y, fontLabel);
    d.drawText("INVT1", rightCol1X + padX, rightTopY + motorLine2Y, fontLabel);

    d.drawText("MT2",   rightCol2X + padX, rightTopY + motorLine1Y, fontLabel);
    d.drawText("INVT2", rightCol2X + padX, rightTopY + motorLine2Y, fontLabel);

    d.drawText("SOC %", rightLeftEdge + padX, socY + (socBoxH / 2) - 20, fontLabel);

    d.drawText("MT3",   rightCol1X + padX, rightBotY + motorLine1Y, fontLabel);
    d.drawText("INVT3", rightCol1X + padX, rightBotY + motorLine2Y, fontLabel);

    d.drawText("MT4",   rightCol2X + padX, rightBotY + motorLine1Y, fontLabel);
    d.drawText("INVT4", rightCol2X + padX, rightBotY + motorLine2Y, fontLabel);
} */