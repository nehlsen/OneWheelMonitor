My OneWheel
ow082157
90 e2 02 2b 21 63

----


https://github.com/TaraHoleInIt/tarablessd1306


----

https://github.com/Nanoux/Onewave/blob/master/app/src/main/java/com/nanowheel/nanoux/nanowheel/model/OWDevice.java#L227

/*
0x0000 = e659F301-ea98-11e3-ac10-0800200c9a66 (OnewheelServiceUUID)
0x001a = e659F301-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicSerialNumber)
0x001d = e659f302-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicRidingMode) y
0x0021 = e659f303-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicBatteryRemaining) y
0x0025 = e659f304-ea98-11e3-ac10-0800200c9a66  batt 5
0x0029 = e659f305-ea98-11e3-ac10-0800200c9a66  batt 20
0x003d = e659f306-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicBatterySerial)
0x0041 = 659f307-ea98-11e3-ac10-0800200c9a66  (OnewheelCharacteristicTiltAnglePitch) y
0x0045 = e659f308-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicTiltAngleRoll)  y
0x0049 = e659f309-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicTiltAngleYaw)
0x003e = e659f30a-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicOdometer) y
0x0041 = e659f30b-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicSpeed) y
0x0045 = e659f30c-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicLightingMode) y
0x0049 = e659f30d-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicLightsFront)
0x004d = e659f30e-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicLightsBack)
0x0051 = e659f30f-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicStatusError) y
^ is misnamed, is an array containing riderDetected, frontPad1, frontPad2, icsuFault, icsvFault, isCharging, bmsCtrlComms, brokenCapacitor in that order
0x0055 = e659f310-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicTemperature)
0x0059 = e659f311-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicFirmwareRevision)
0x005d = e659f312-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicCurrentAmps) y
0x0061 = e659f313-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicTripTotalAmpHours) y
0x0065 = e659f314-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicTripRegenAmpHours) y
0x0069 = e659f315-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicBatteryTemp)
0x006d = e659f316-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicBatteryVoltage) y
0x0071 = e659f317-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicSafetyHeadroom)
0x0075 = e659f318-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicHardwareRevision)
0x0079 = e659f319-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicLifetimeOdometer) y
0x007d = e659f31a-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicLifetimeAmpHours)
0x0081 = e659f31b-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicBatteryCells) ys
0x0085 = e659f31c-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicLastErrorCode) ys
0x0089 = e659f31d-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicUNKNOWN1)
0x009d = e659f31e-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicUNKNOWN2) y/n
0x0101 = e659f31f-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicUNKNOWN3)
0x0105 = e659f320-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicUNKNOWN4)

e659f3fd-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicCustomName)
e659f3fe-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicUartSerialRead)
e659f3ff-ea98-11e3-ac10-0800200c9a66 (OnewheelCharacteristicUartSerialWrite)


0x0045=00 then lights are OFF
0x0045=01 is default lights
0x0045=02 is manual mode for lights
In manual mode (0x0045=02) 0x0049 is front lights and 0x004d is back lights
For both, the first byte is the level of light for white and second byte for red. Levels are 00 (off) to 75 (super bright)
SETS FRONT TO BRIGHT RED AND BACK TO BRIGHT WHITE:
gatttool --device=D0:39:72:BE:0A:32 --char-write-req --value=0002 --handle=0x0045
gatttool --device=D0:39:72:BE:0A:32 --char-write-req --value=0075 --handle=0x0049
gatttool --device=D0:39:72:BE:0A:32 --char-write-req --value=7500 --handle=0x004d
*/


this comment provides a "btsnoop_hci.log" https://github.com/ponewheel/android-ponewheel/issues/109#issuecomment-562357395
from what i can tell it includes the following challenge-response, made by the original OneWheel App

challenge
09 8e 56
32 04 df 7a  1b 9e 5a 0f  5a 8c bf 33  84 0e 4d dd
d2


response
09 8e 56
c8 c5 95 bc  94 23 ce 87  ae a3 bc 3a  45 73 8c 42
78


