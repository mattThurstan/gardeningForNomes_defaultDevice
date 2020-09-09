/*----------------------------comms----------------------------*/
void setupMesh()
{
  mesh.setDebugMsgTypes( ERROR | STARTUP );       // set before init() so that you can see startup messages

  mesh.init(MESH_NAME, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL, 1, 4); // hidden, 4 max connections
  
  mesh.setContainsRoot(true);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void turnOffComms() 
{
  publisshDeviceOffline();
  DEBUG_COMMS = false;
  turnOffMesh();
  turnOffWifi();
  DEBUG_GEN = false;
  turnOffSerial();
}
