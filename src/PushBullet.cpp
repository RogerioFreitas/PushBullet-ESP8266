#include <PushBullet.h>


PushBullet::PushBullet(const String api_token, WiFiClientSecure * secure_client, uint16_t port){
	this->secure_client = secure_client;
	this->api_token = api_token;
	this->port = port;
	
}

bool PushBullet::openConnection(){
  if (!this->secure_client->connect(this->push_bullet_host.c_str(), this->port)) {
    return false;
  }
   return true;
}

bool PushBullet::closeConnection(){
	this->secure_client->stop();
	if(this->secure_client->connected() == 1){
		return false;
	}
	else{
		return true;
	}
}

bool PushBullet::checkConnection(){
	
	
}

String PushBullet::buildRequest(String url, String body, String getOrPost){
	String request = getOrPost + url +" HTTP/1.1\r\n" +
                   "Host: " + this->push_bullet_host + "\r\n" +
                   "User-Agent: ESP8266/NodeMCU 0.9\r\n" +
                   "Accept: */*\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: "+ body.length() +"\r\n" +
                   "Access-Token: "+ this->api_token +"\r\n\r\n" +
                   body;
	return request;
}

String PushBullet::sendRequest(String req){

#ifdef DEBUGGING
    Serial.println("Request string: ");
	Serial.println(req);
#endif

	
	if (this->secure_client->available()) {
		this->secure_client->print(req);
		delay(10);
		return this->secure_client->readString();
	} else{
		return "FAILED TO SEND";
		
	}
	
}


void PushBullet::sendNotePush(const String message, const String title){
	String req = buildRequest("/v2/pushes", "{\"body\":\""+message+"\",\"title\":\""+title+"\",\"type\":\"note\"}");
	sendRequest(req);

}

void PushBullet::sendLinkPush(const String message, const String title, const String url){
	String req = buildRequest("/v2/pushes", "{\"body\":\""+message+"\",\"title\":\""+title+"\",\"url\":\""+url+"\",\"type\":\"link\"}");		   
	sendRequest(req);
}

void PushBullet::sendSMSPush(const String message, const String phoneNumber, const String source_device, const String source_user){
	String req = buildRequest("/v2/ephemerals", "{ \"push\": {    \"conversation_iden\": \""+phoneNumber+"\",    \"message\": \""+message+"\",    \"package_name\": \"com.pushbullet.android\",    \"source_user_iden\": \""+source_user+"\",    \"target_device_iden\": \""+source_device+"\",    \"type\": \"messaging_extension_reply\"  },  \"type\": \"push\"}			");
	sendRequest(req);
	
}

void PushBullet::copyToClipboard(const String contents, const String source_device, const String source_user){
 	String req = buildRequest("/v2/ephemerals", "{\"push\":{\"body\":\""+contents+"\",\"source_device_iden\":\""+source_device+"\",\"source_user_iden\":\""+source_user+"\",\"type\":\"clip\"},\"type\":\"push\"}");
	sendRequest(req);
}

void PushBullet::registerThisDevice(const String nickName, const String pushToken = ""){
	String req = buildRequest("/v2/devices", "{\"app_version\":8623,\"manufacturer\":\"ESP\",\"model\":\"ESP8266\",\"nickname\":\""+nickName+"\",\"push_token\":\"production:"+pushToken+"\"}");
	sendRequest(req);
}

String PushBullet::getLatestPushed(const String modified_after, int limit){
	String req = buildRequest("/v2/pushes", "?active=true&modified_after="+modified_after+"&limit="+limit+"");
	return sendRequest(req);

}
