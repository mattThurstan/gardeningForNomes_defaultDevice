/*----------------------------messages - publish-----------------------*/
void meshSendSingleToBridge(String nom, String msg, bool save) {
  if (DEBUG_COMMS) { Serial.print(nom); Serial.print(" - "); }
  mesh.sendSingle(id_bridge1, msg);
  if (DEBUG_COMMS) { Serial.println(msg); }
  if (save == true) { _shouldSaveSettings = true; }
}
void publishMeshMsgSingleState(String nom, String addr, boolean state, bool save) {
  addr += ":"; //..just so we are all sure what is going on here !?
  if (state == false) {  addr += "OFF"; }
  else if (state == true) { addr += "ON"; }
  meshSendSingleToBridge(nom, addr, save);
}
void publishMeshMsgSingleString(String nom, String addr, String msg, bool save) {
  addr += ":"; //..just so we are all sure what is going on here !?
  addr += msg;
  meshSendSingleToBridge(nom, addr, save);
}
void publishMeshMsgSingleColor(String nom, String addr, uint8_t r, uint8_t g, uint8_t b, bool save) {
  addr += ":"; //..just so we are all sure what is going on here !?
  addr += String(r);
  addr += ",";
  addr += String(g);
  addr += ",";
  addr += String(b);
  meshSendSingleToBridge(nom, addr, save);
}

/*----------------------------messages - publish - main----------------*/
void publishState(bool save) {
  //bool onOff;
  //if (_state == 0 || _state == 3) { onOff = false; } 
  //else { onOff = true; }
  publishMeshMsgSingleState("publishState", "lights/light/status", _onOff, save);
}

void publishDayMode(bool save) {
  publishMeshMsgSingleState("publishDayMode", "lights/day/status", _dayMode, save);
}

void publishMode(bool save) {
  publishMeshMsgSingleString("publishMode", "lights/mode", _modeName[_modeCur], save);
}

// sub-modes
void publishColorTemp(bool save) { }
void publishEffect(bool save) { }

void publishBrightness(bool save) {
  publishMeshMsgSingleString("publishBrightness", "lights/brightness/status", String(_ledGlobalBrightnessCur), save);
}

void publishRGB(bool save) {
  RgbColor tempRGB = _colorHSL;
  publishMeshMsgSingleColor("publishRGB", "lights/rgb/status", tempRGB.R, tempRGB.G, tempRGB.B, save);
}

void publishRiseSpeed(bool save) {
  publishMeshMsgSingleString("publishRiseSpeed", "lights/risespeed/status", String(_ledRiseSpeedSaved), save);
}

void publishGHue2Cycle(bool save) {
  publishMeshMsgSingleString("publishGHue2Cycle", "lights/hue/cycle/status", String(_gHue2CycleSaved), save);
}

/*
void publishSensor(bool save) {
  bool onOff;
  if (_state == 0 || _state == 3) { onOff = false; } 
  else { onOff = true; }
  publishMeshMsgSingleState("publishSensor", "sensors/status", onOff, save);
}

void publishSensorOn(bool save) {
  publishMeshMsgSingleState("publishSensor", "sensors/status", true, save);
}
void publishSensorOff(bool save) {
  publishMeshMsgSingleState("publishSensorTop", "sensors/status", false, save);
}

*/

void publishDebugGeneralState(bool save) {
  publishMeshMsgSingleState("publishDebugGeneralState", "debug/general/status", DEBUG_GEN, save);
}

void publishDebugOverlayState(bool save) {
  publishMeshMsgSingleState("publishDebugOverlayState", "debug/overlay/status", DEBUG_OVERLAY, save);
}

void publishDebugMeshsyncState(bool save) {
  publishMeshMsgSingleState("publishDebugMeshsyncState", "debug/meshsync/status", DEBUG_MESHSYNC, save);
}

void publishDebugCommsState(bool save) {
  publishMeshMsgSingleState("publishDebugCommsState", "debug/comms/status", DEBUG_COMMS, save);
}

void publishStatusAll(bool save) {
  
  if (DEBUG_COMMS) { Serial.println("publishStatusAll "); }
  publishState(save);
  publishDayMode(save);
  publishMode(save);
  publishColorTemp(save);
  publishEffect(save);
  
  publishBrightness(save);
  publishRGB(save);
  publishRiseSpeed(save);
  publishGHue2Cycle(save);
  
  //publishSensor(save);

  publishDebugGeneralState(save);
  publishDebugOverlayState(save);
  publishDebugMeshsyncState(save);
  publishDebugCommsState(save);
}
    
