/*
Muscles class:
- define state of each muscle (inflated y/n)
- assign pin + output 
- store millis counter 
*/

class Muscle{

private:
    bool isInflated;
    int pin; 
    const unsigned long max_inflate = 7000; 
    int proprioceptionPin; 
    const unsigned long stretchThreshold=50; 
    const unsigned long inflateStart; 
    
public:
    //define constructer of class 
    Muscles(int mpin, int ppin):
        isInflated(false), 
        pin(mpin),
        proprioceptionPin(ppin)
        {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW); 
        pinMode(proprioceptionPin, INPUT);
        }

    void inflate() {
        if (!isInflated) {
            digitalWrite(pin, HIGH);
            isInflated = true;
            inflateStart = millis();
        }
    }

    void deflate() {
        if (isInflated) {
            digitalWrite(pin, LOW);
            isInflated = false;
            inflateStop = millis();
        }
    }

    void contract(){
        inflate();
        if ((millis()-inflateStart())>max_inflate) deflate(); 
    }

    //can adjust to give inflate % state as analog input
    bool stretch(){
        return analogRead(proprioceptionPin)>stretchThreshold; 
    }

    bool checkState() {
        return (isInflated&&stretch()); 
    }
}

/*
- define muscles set in module (one circular, two diagonal, one pull back)
*/

class Module{

private:
    Muscle& circular; 
    Muscle& diagL;
    Muscle& diagR;
    Muscle& pullBack; 

public:
//initialise constructor with muscles inc. proprioception pin
    Module(Muscle& circ, Muscle& dl, Muscle& dr, Muscle& pb) :  // Fixed constructor
        circular(circ),
        diagL(dl),
        diagR(dr),
        pullBack(pb)
    {}


void activateCircle(){
    circular.contract(); 
}

void activatePullBack(){
    pullBack.contract();
}

void stop(){
    circular.deflate();
    diagL.deflate();
    diagR.deflate();
    pullBack.deflate(); 
}

void turnLeft(){
    diagL.contract();
}

void turnRight(){
    diagR.contract();
}

}

/*
-define behaviour states
*/
class wormController{
//initialise modules into array
Module wormbody[]; 


void forward()   {
    int pause=0;
    int start = millis();
   for (int i =0; i<wormbody.length(); i++){
       wormbody[i].activateCircle();
        curr=millis();
        while(millis()-start<pause);
        start=millis();
   }

   for (int i =0; i<wormbody.length(); i++){
    for (int i =0; i<wormbody.length(); i++){
       wormbody[i].activatePullBack();
        curr=millis();
        while(millis()-start<pause);
        start=millis();
   }
}
   }

//instantiate in a similiar manner 
void turnLeft();
void turnRight();
voi stop(); 
}

/*
- contain sensor array and distance threshold for left and right turn
*/


class Senses {
private:
//ideally have an array of ultrasonic sensors 
    const int TRIG_PIN_LEFT = 24;
    const int ECHO_PIN_LEFT = 25;
    const int TRIG_PIN_RIGHT = 26;
    const int ECHO_PIN_RIGHT = 27;
    const unsigned long OBSTACLE_THRESHOLD = 100; // Distance in mm

    unsigned long getDistance(int trigPin, int echoPin) {
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        return pulseIn(echoPin, HIGH) * 0.34 / 2; // Distance in mm
    }

public:
    Senses() {
        pinMode(TRIG_PIN_LEFT, OUTPUT);
        pinMode(ECHO_PIN_LEFT, INPUT);
        pinMode(TRIG_PIN_RIGHT, OUTPUT);
        pinMode(ECHO_PIN_RIGHT, INPUT);
    }

    bool obstacleLeft() {
        return getDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT) < OBSTACLE_THRESHOLD;
    }

    bool obstacleRight() {
        return getDistance(TRIG_PIN_RIGHT, ECHO_PIN_RIGHT) < OBSTACLE_THRESHOLD;
    }

    char updateSenses() {
        if (obstacleLeft()) return 'l';
        if (obstacleRight()) return 'r';
        if (!obstacleLeft() && !obstacleRight()) return 'f';
        return 's'; // stop if obstacles on both sides
    }
};


//global variables: muscle pointers and 
const int NUM_MODULES = 3;
// arrays for ea. muscle type 
Muscle* circularMuscles[NUM_MODULES];
Muscle* diagLMuscles[NUM_MODULES];
Muscle* diagRMuscles[NUM_MODULES];
Muscle* pullMuscles[NUM_MODULES];
//array for modules 
Module* modules[NUM_MODULES];
WormController* controller;
Senses* sensors;


/*
- switch between states using Finate state machine 
*/

void setup(){

    Muscle* circularMuscles[NUM_MODULES] = {
        new Muscle(2, 3),
        new Muscle(21, 31),
        new Muscle(22, 32)
    };
    
    Muscle* diagLMuscles[NUM_MODULES] = {
        new Muscle(4, 5),
        new Muscle(41, 51),
        new Muscle(42, 52)
    };
    
    Muscle* diagRMuscles[NUM_MODULES] = {
        new Muscle(6, 7),  
        new Muscle(61, 71),
        new Muscle(62, 72)
    };
    
    Muscle* pullBackMuscles[NUM_MODULES] = {
        new Muscle(8, 9),  
        new Muscle(81, 91),
        new Muscle(82, 92)
    };

    // Create Module objects
    Module** modules = new Module*[NUM_MODULES];
    for (int i = 0; i < NUM_MODULES; i++) {
        modules[i] = new Module(
            *circularMuscles[i],
            *diagLMuscles[i],
            *diagRMuscles[i],
            *pullBackMuscles[i]
        );

    //initialise controller and sensor object 
    wormController worm = new wormController(modules, NUM_MODULES);
    Senses* sensors = new Senses();

    Serial.begin(9600);
}

void loop(
//switch case to select behviour with sensor changes 
 
switch (sensors->updateSenses()) {
        case 'l':
            worm->turnRight(); // Turn right when obstacle on left
            break;
        case 'r':
            worm->turnLeft();  // Turn left when obstacle on right
            break;
        case 'f':
            worm->forward();   // Go forward when no obstacles
            break;
        default:
            worm->stop();      // Stop if uncertain
            break;
    }
    
    delay(10);
)


