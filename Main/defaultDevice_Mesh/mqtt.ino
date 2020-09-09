/*----------------------------MQTT----------------------------*/
void mqqtSubscribeList1() 
{
  mqttClient.subscribe("mesh/to/#");
  mqttClient.subscribe(DEVICE_SUB_BRIDGE1);
  mqttClient.subscribe(DEVICE_SUB_STAIRS1);
  mqttClient.subscribe(DEVICE_SUB_DESK1);
  mqttClient.subscribe(DEVICE_SUB_DESK2);
  mqttClient.subscribe(DEVICE_SUB_KITCHEN1);
  mqttClient.subscribe(DEVICE_SUB_LONGBOARD1);
  mqttClient.subscribe(DEVICE_SUB_LEANINGBOOKSHELVES1);
  mqttClient.subscribe(DEVICE_SUB_FUTONBED1);
  mqttClient.subscribe(DEVICE_SUB_LIVINGROOMDIVIDER1);
  mqttClient.subscribe("house/testNode/#");
  mqttClient.subscribe("house/sunrise");
  mqttClient.subscribe("house/sunset");
  mqttClient.subscribe("house/breath");
  //mqttClient.subscribe("house/reset");
  mqttClient.subscribe("house/restart");
  mqttClient.subscribe("house/lockdown");
}

void mqqtConnect()
{
    if (mqttClient.connect(HOSTNAME, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD))   // "thurstanMeshBridge"
    {
      //mqttClient.publish("mesh/from/bridge1","Ready!");
      mqttClient.publish("house/bridge1/available","online");
      mqttClient.publish("house/bridge1/status","ON");
      mqqtSubscribeList1();
      if (DEBUG_COMMS) { Serial.println("MQTT connected."); }
    }
}

boolean mqttReconnect() 
{
  if (mqttClient.connect(HOSTNAME, MQTT_BROKER_USERNAME, MQTT_BROKER_PASSWORD, WILL_TOPIC, 0, true, WILL_MESSAGE)) {
    // Once connected, publish an announcement...
    mqttClient.publish("house/bridge1/available","online");
    mqttClient.publish("house/bridge1/status","ON");
    mqqtSubscribeList1(); // ... and resubscribe
    if (DEBUG_COMMS) { Serial.println("MQTT reconnected."); }
  }
  return mqttClient.connected();
}

/*----------------------------MQTT callbacks----------------------------*/
// callback for Mesh messages
void receivedCallback( const uint32_t &from, const String &msg ) {
  if (DEBUG_COMMS) { Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str()); Serial.println(); }
  
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex+1);

  if (DEBUG_COMMS) { 
    Serial.printf("mesh msg callback msgSub: ");
    Serial.println(msgSub);
  }
  
  String topic = "house/";
  if (from == DEVICE_ID_STAIRS1)
  { topic += DEVICE_TOP_STAIRS1; } 
  else if (from == DEVICE_ID_DESK1)
  { topic += DEVICE_TOP_DESK1; } 
  else if (from == DEVICE_ID_DESK2)
  { topic += DEVICE_TOP_DESK2; } 
  else if (from == DEVICE_ID_KITCHEN1)
  { topic += DEVICE_TOP_KITCHEN1; } 
  else if (from == DEVICE_ID_LONGBOARD1)
  { topic += DEVICE_TOP_LONGBOARD1; }
  else if (from == DEVICE_ID_LEANINGBOOKSHELVES1)
  { topic += DEVICE_TOP_LEANINGBOOKSHELVES1; }
  else if (from == DEVICE_ID_FUTONBED1)
  { topic += DEVICE_TOP_FUTONBED1; }
  else if (from == DEVICE_ID_LIVINGROOMDIVIDER1)
  { topic += DEVICE_TOP_LIVINGROOMDIVIDER1; }
  
  topic += targetSub;
  mqttClient.publish(topic.c_str(), msgSub.c_str());
}

// callback for LAN messages
void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  if (DEBUG_COMMS) { Serial.printf("bridge: Received MQTT from network: msg=%s\n", topic); Serial.println(); }
  
  char* cleanPayload = (char*)malloc(length+1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length+1);
  String msg = String(cleanPayload);
  free(cleanPayload);

  //String targetStr = String(topic).substring(8);  //starting after 'mesh/to/' (removes it)
  String targetStr = String(topic);

  if(targetStr == "mesh/to/bridge")
  {
    if(msg == "getNodes")
    {
      mqttClient.publish("mesh/from/bridge", mesh.subConnectionJson().c_str());
      if (DEBUG_COMMS) { Serial.printf("bridge: Sent msg to mesh: targetStr=%s\n", topic); Serial.println(); }
    }
  }
  else if(targetStr == "mesh/to/all") 
  {
    // ..all wrong, will probably remove this..
    //uint8_t firstTargetStrIndex = targetStr.indexOf('/');
    //String targetNode = targetStr.substring(0, firstTargetStrIndex);  //get the device name - don't need
    //String targetSub = targetStr.substring(firstTargetStrIndex);      //get the rest of the address
    //String targetSub = targetStr.substring(12);      // get the rest of the address
    //add the rest of the address to the beginning of the message
    //String ts = targetSub;
    //ts += ":";
    //ts += msg;
    //mesh.sendBroadcast(ts); //send message
    stripAndBroadcastGlobalMeshMsg(targetStr, 12, msg);
  }
  else if(targetStr == "house/sunrise") { stripAndBroadcastGlobalMeshMsg(targetStr, 6, msg);                                }
  else if(targetStr == "house/sunset")  { stripAndBroadcastGlobalMeshMsg(targetStr, 6, msg);                                }
  else if(targetStr == "house/breath")  { stripAndBroadcastGlobalMeshMsg(targetStr, 6, msg);                                }
  else if(targetStr == "house/reset")   { stripAndBroadcastGlobalMeshMsg(targetStr, 6, msg); if(msg == "ON") { doReset(); } }
  else if(targetStr == "house/restart") { 
    stripAndBroadcastGlobalMeshMsg(targetStr, 6, msg); 
    uint8_t restartTime = msg.toInt();
    if (restartTime < 0 || restartTime > 255) { return; /* do nothing... */ } 
    else { doRestart(restartTime); }
  }
  else if(targetStr == "house/lockdown") 
  { 
    stripAndBroadcastGlobalMeshMsg(targetStr, 6, msg);
    
    uint8_t severity = msg.toInt();
    if (severity < 0 || severity > 255) { return; /* do nothing... */ } 
    else { doLockdown(severity); } 
  }
  else
  {
    parseMQTT(targetStr, msg);
  }
}

void stripAndBroadcastGlobalMeshMsg(String targetStr, uint8_t charactersToRemove, String msg) {
  String targetSub = targetStr.substring(charactersToRemove); // remove the ammount of characters at the start and get the rest of the address 
  String message = targetSub; // create a new string (or it has 'issues') and set the remainder of the address at the beginning of the mesh message
  message += ":";
  message += msg;
  mesh.sendBroadcast(message); // send global mesh message
}

/*----------------------------MQTT message parsing----------------------------*/
void parseMQTT(String topic, String msg)
{
  //remove 'house/' from (address) 'targetStr'
  String targetStr2 = String(topic).substring(6);  //starting after 'house/' (removes it)
  
  //at this point we are now expecting the first part of the address to be a device name
  //which we then translate into a node ID ..or not
  uint8_t firstTargetStrIndex = targetStr2.indexOf('/');
  String targetNode = targetStr2.substring(0, firstTargetStrIndex);  //get the device name
  String targetSub = targetStr2.substring(firstTargetStrIndex+1);      //get the rest of the address

  if (DEBUG_COMMS) { 
    Serial.printf("parseMQTT targetSub: ");
    Serial.println(targetSub);
    Serial.printf("parseMQTT msg: ");
    Serial.println(msg);
  }
  
  //get target by way of device name
  //uint32_t target = strtoul(targetNode.c_str(), NULL, 10);bridge1
  uint32_t target;
  if (targetNode == DEVICE_NOD_BRIDGE1)
  { target = 1; }
  else if (targetNode == DEVICE_NOD_STAIRS1)
  { target = DEVICE_ID_STAIRS1; } 
  else if (targetNode == DEVICE_NOD_DESK1)
  { target = DEVICE_ID_DESK1; } 
  else if (targetNode == DEVICE_NOD_DESK2)
  { target = DEVICE_ID_DESK2; } 
  else if (targetNode == DEVICE_NOD_KITCHEN1)
  { target = DEVICE_ID_KITCHEN1; } 
  else if (targetNode == DEVICE_NOD_LONGBOARD1)
  { target = DEVICE_ID_LONGBOARD1; }
  else if (targetNode == DEVICE_NOD_LEANINGBOOKSHELVES1)
  { target = DEVICE_ID_LEANINGBOOKSHELVES1; }
  else if (targetNode == DEVICE_NOD_FUTONBED1 )
  { target = DEVICE_ID_FUTONBED1; }
  else if (targetNode == DEVICE_NOD_LIVINGROOMDIVIDER1)
  { target = DEVICE_ID_LIVINGROOMDIVIDER1; }

  if (target == 0) { /* SYSTEM SPARE */ }
  else if (target == 1) { 
    String o1 = "ON";
    String o2 = "OFF";
    if (targetSub == "debug/general/set") 
    {
      if(msg == "ON") { 
        DEBUG_GEN = true;
        mqttClient.publish("house/bridge1/debug/general/status", o1.c_str());
      } 
      else if(msg == "OFF") { 
        DEBUG_GEN = false; 
        mqttClient.publish("house/bridge1/debug/general/status", o2.c_str()); 
      }
    }
    else if(targetSub == "debug/comms/set") 
    {
      if(msg == "ON") { 
        DEBUG_COMMS = true;
        mqttClient.publish("house/bridge1/debug/comms/status", o1.c_str()); 
      } 
      else if(msg == "OFF") { 
        DEBUG_COMMS = false;
        mqttClient.publish("house/bridge1/debug/comms/status", o2.c_str()); 
      }
    }
    else if(targetSub == "debug/available/scan") 
    {
      if(msg == "ON") { 
        //DEBUG_COMMS = true;
        checkDevicesStatus();
        // hack
        mqttClient.publish("house/bridge1/available","online");
        mqttClient.publish("house/bridge1/status","ON");
      } 
      else if(msg == "OFF") { 
        //DEBUG_COMMS = false;
        //mqttClient.publish("house/bridge1/debug/comms/status", o1.c_str()); 
      }
    }
    else if(targetSub == "debug/reset") { if(msg == "ON") { doReset(); } }
    else if(targetSub == "debug/restart") 
    {
      uint8_t restartTime = msgSub.toInt();
      if (restartTime < 0 || restartTime > 255) { return; /* do nothing... */ } 
      else { doReset(restartTime); }
    }
  }
  else if (target == 2) { /* SYSTEM SPARE */ }
  else {
    // translate addresses and re-broadcast into the mesh
    if (mesh.isConnected(target))
    {
      //add the rest of the address to the beginning of the message
      String ts = targetSub;
      ts += ":";
      ts += msg;
      //send message to target
      mesh.sendSingle(target, ts);
      //the target will decode all it's sub-parts
      if (DEBUG_COMMS) { 
        Serial.printf("bridge: Sent msg to mesh: "); 
        Serial.print(ts);
        Serial.print(",");
        Serial.println(target);
      }
    }
    else
    {
      String sm = "Node ";
      sm += targetNode;
      sm += " not connected!";
      mqttClient.publish("mesh/from/bridge", sm.c_str());
    }
  }
  if (DEBUG_COMMS) { Serial.println(); }
}

void buildMQTT() { }
void parseMessages() { }
void buildMessages() { }


/*----------------------------check if devices are online---------------------*/
void checkDevicesStatus() {
  // check the mesh to see if known devices are online
  uint32_t target;
  String cd1 = "h";
  //String ds = DEVICE_OFFLINE;
  String ds = "offline";
  for (int i = 0; i < 9; i++) 
  {
    if (i == 0) { target = DEVICE_ID_BRIDGE1; cd1 = DEVICE_CD1_BRIDGE1; }
    else if (i == 1) { target = DEVICE_ID_STAIRS1; cd1 = DEVICE_CD1_STAIRS1; }
    else if (i == 2) { target = DEVICE_ID_DESK1; cd1 = DEVICE_CD1_DESK1; }
    else if (i == 3) { target = DEVICE_ID_DESK2; cd1 = DEVICE_CD1_DESK2; }
    else if (i == 4) { target = DEVICE_ID_KITCHEN1; cd1 = DEVICE_CD1_KITCHEN1; }
    else if (i == 5) { target = DEVICE_ID_LONGBOARD1; cd1 = DEVICE_CD1_LONGBOARD1; }
    else if (i == 6) { target = DEVICE_ID_LEANINGBOOKSHELVES1; cd1 = DEVICE_CD1_LEANINGBOOKSHELVES1; }
    else if (i == 7) { target = DEVICE_ID_FUTONBED1; cd1 = DEVICE_CD1_FUTONBED1; }
    else if (i == 8) { target = DEVICE_ID_LIVINGROOMDIVIDER1; cd1 = DEVICE_CD1_LIVINGROOMDIVIDER1; }

    if (mesh.isConnected(target)) {
      // is device is online then broadcast online mqtt message
      //ds = DEVICE_ONLINE;
      ds = "online";

      //send status request message to target
      String ts = "status/request";
      ts += ":";
      ts += "ON";
      mesh.sendSingle(target, ts);
      
    } else {
      // is device is online then broadcast offline mqtt message
      //ds = DEVICE_OFFLINE;
      ds = "offline";
    }
    if (i == 0) { ds = "online"; }  // hack cos we know this (bridge) is online
    mqttClient.publish(cd1.c_str(), ds.c_str());
    
    if (DEBUG_COMMS) { 
      Serial.print("bridge: Check and publish device online status, "); 
      Serial.print(cd1);
      Serial.print(" - ");
      Serial.print(ds); 
      Serial.println(); 
    }
    
  }
  //if (DEBUG_COMMS) { Serial.printf("bridge: Checked devices online status."); Serial.println(); }
}
