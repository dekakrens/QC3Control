#include <QC3Control.h>

QC3Control quickCharge(4, 5);
const int button[]={1, 2, 3};			//mode  C--  C++
const int led[]={9, 10, 11, 12, 13};
#define NumButtons sizeof (button)

int buttonState[NumButtons];
int lastButtonState[NumButtons];
bool buttonIsPressed[NumButtons];
bool QCState = false;					//default QC2.0
long lastDebounceTime = 0;
long debounceDelay = 10;
int lastQC2State, lastQC3State;

void setup(){
	Serial.begin(9600);
	for(int i=0; i<NumButtons; i++){
		pinMode (button[i], INPUT_PULLUP);
		lastButtonState[i]=HIGH;
		buttonIsPressed[i]=false;
	}
	for (int i=0; i<5; i++){
		pinMode (led[i], OUTPUT);
	}
	
	quickCharge.set5V();
	digitalWrite(led[0], HIGH); //mode QC2.0
	digitalWrite(led[2], HIGH); //tegangan 5v
}

void loop(){
	checkbutton();
	action();
}

void checkbutton(){
	for (int currentButton=0; currentButton<NumButtons; currentButton++) {
		int reading = digitalRead(button[currentButton]);
		if (reading != lastButtonState[currentButton]) { lastDebounceTime = millis(); }
		if ((millis() - lastDebounceTime) > debounceDelay) {
			if (reading != buttonState[currentButton]) {
				buttonState[currentButton] = reading;
				if (buttonState[currentButton]==LOW) {
					buttonIsPressed[currentButton]=true;
				}
			}
		}
		lastButtonState[currentButton] = reading;
	}
}

void action(){
	while (buttonIsPressed[0] == true && QCState == false){
		QCState = !QCState;
		lastQC3State=0;
		quickCharge.setMilliVoltage(5000);
		digitalWrite(led[0], LOW);
		digitalWrite(led[1], HIGH);
		Serial.println("Mode QC3.0 Tegangan 5v");
		buttonIsPressed[0]=false;
	}
	while (buttonIsPressed[0] == true && QCState == true){
		QCState = !QCState;
		lastQC2State=0;
		quickCharge.set5V();
		digitalWrite(led[0], HIGH);
		digitalWrite(led[1], LOW);
		Serial.println("Mode QC2.0 Tegangan 5v");
		buttonIsPressed[0]=false;
	}
	
	switch(QCState){
		case 0:{
			if (buttonIsPressed[2] == true && lastQC2State<2){
				lastQC2State++;
				percabanganQC2();
				buttonIsPressed[2]=false;
			}else if (buttonIsPressed[1] == true && lastQC2State>0){
				lastQC2State--;
				percabanganQC2();
				buttonIsPressed[1]=false;
			}else{
				buttonIsPressed[1]=false;
				buttonIsPressed[2]=false;
			}
		}break;
		case 1:{
			if (buttonIsPressed[2] == true && lastQC3State<35){
				lastQC3State++;
				quickCharge.incrementVoltage();
				Serial.println(lastQC3State);
				buttonIsPressed[2]=false;
			}else if (buttonIsPressed[1] == true && lastQC3State>0){
				lastQC3State--;
				quickCharge.decrementVoltage();
				Serial.println(lastQC3State);
				buttonIsPressed[1]=false;
			}else {
				buttonIsPressed[1]=false;
				buttonIsPressed[2]=false;
			}
		}break;
	}
}

void percabanganQC2(){
	switch(lastQC2State){
		case 0:{
		quickCharge.set5V();
		digitalWrite(led[2], HIGH);
		digitalWrite(led[3], LOW);
		digitalWrite(led[4], LOW);
		Serial.println("Tegangan 5v");
		}break;
		case 1:{
		quickCharge.set9V();
		digitalWrite(led[2], LOW);
		digitalWrite(led[3], HIGH);
		digitalWrite(led[4], LOW);
		Serial.println("Tegangan 9v");
		}break;
		case 2:{
		quickCharge.set12V();
		digitalWrite(led[2], LOW);
		digitalWrite(led[3], LOW);
		digitalWrite(led[4], HIGH);
		Serial.println("Tegangan 12v");
		}break;
	}
}
