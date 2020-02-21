# FanMonitor
Advanced controller for industrial fan

This repository contains all the files used for the creation of a custom industrial fan controller.

## Context

__Two spaces__ have to be maintained at the lowest temperature possible (to a certain extent).
For that purpose, an __industrial fan__ (controlled by a 220V relay) is used. The fan is __physically moved__ from one space to the other depending on the needs (the air intake is outside).

The goal is to control the fan automatically with 3 temperature sensors, one in each space and one near the air intake. The 3 current tempratures have to be display for the user to check things if needed. The environnement is also very dusty so the system has to be contained in a closed box.

## System

The system is made off three main part : 
  - a generic usb charger for the voltage supply
  - a main board build aroud a __STM32F103__ microprocessor
  - a screen based on the __HD44780__ driver
  
The box's plans are not depicted here since they are very simple (a few holes, no more, no less).
Appart form that, you will find here all the code - created from scratch since I didn't want to use ST's HAL and anyways it's funnier this way - and the 3D models used. All the documentation is also there. You may also find (if I didnt forget to put it there) EasyEDA files for the main board PCB.

## How to compile

The code uses the __GNU toolchain__, whith __gcc__ and __gdb__ for AMR. __OpenOCD__ is used for the SWD handling, through a __STLinkv2__. Don't forget to install __arm-none-eabi-newlib__ if you want things to work.

Finaly, there is a RELEASE tag in the Makefile if you want to optimise and remove debug.
