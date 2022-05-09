This project is contains sample code for pycom fipy and TTGO lora32 v2, this is sample code used to send a message to the things network.

Pycom fipy set up <br/>
1) First load the id python class this is used to get the DEVEUI of the devices. <br/>
2) On the things network webside create an application and add a node, use the DEVEUI optained from the id output, and let the things network create a random joinEUI and APPkey
3) Added the DEVEUI, joinEUI and AppKey in the main class. 
4) when running the main class a message should be sent to the things network

TTGO lora32 v2 <br/>
1) On the things network create a application, and add a node. On the website generate a DEVEUI, joinEUI and appKey
2) add all the files from the TTGO lora32 v2 folder to a ardinio project. 
3) in Arduino choose the board TTGO LORA32-OLED v2.16
4) in the class TTGO_lora32_v2_example. insert the DEVEUI, joinEUI and Appkey
5) run the project and it is going to send message to the things network
