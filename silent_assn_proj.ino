#include <Servo.h>

// Servo object declaration
Servo marbleReleaseServo1;
Servo marbleReleaseServo2;
Servo marbleReleaseServo3;
Servo marbleReleaseServo4;
Servo marbleReleaseServo5;
Servo marbleReleaseServo6;

Servo dummyReleaseServo;

Servo marbleLoaderGate1;
Servo marbleLoaderGate2;
Servo marbleLoaderGate3;
Servo marbleLoaderGate4;
Servo marbleLoaderGate5;
Servo marbleLoaderGate6;
Servo marbleLoaderGate7;
Servo marbleLoaderGate8;

Servo loaderDirectionServo;

// Global variable declarations
bool robotTurn = false;
bool firstTimeSetup = true;

const int button1Pin = 2; // Pin connected to button 1
const int button2Pin = 3; // Pin connected to button 2
const int button3Pin = 4; // Pin connected to button 3
const int button4Pin = 5; // Pin connected to button 4
const int button5Pin = 6; // Pin connected to button 5
const int button6Pin = 7; // Pin connected to button 6

const int liftMotorA = 8;
const int liftMotorB = 9;
const int liftMotorSpeed = A0;

const int conveyorMotorA = 10;
const int conveyorMotorB = 11;
const int conveyorMotorSpeed = A1;

const int liftIRSensor = 40;
const int gateOneIRSensor = 41;
const int gateTwoIRSensor = 42;
const int gateThreeIRSensor = 43;
const int gateFourIRSensor = 44;
const int gateFiveIRSensor = 45;
const int gateSixIRSensor = 46;
const int gateSevenIRSensor = 47;
const int gateEightIRSensor = 48;

// Circular LinkedList structure definition. 

/// Node structure for linked list.
struct Node{
    int numberOfMarbles;
    bool leftSide;
    Servo gateServo;
    Servo releaseServo;
    int gateIRSensor;
    Node* next;
    
    /// Constructor to initialize a node with data.
    Node(int numberOfMarbles, bool leftSide, Servo gateServo, Servo releaseServo, int gateIRSensor)
    {
        this->numberOfMarbles = numberOfMarbles;
        this->leftSide = leftSide;
        this->gateServo = gateServo;
        this->gateIRSensor = gateIRSensor;
        this->releaseServo = releaseServo;
        next = nullptr;
    }

    /// Overloaded constructor to intialize a node with data, without the release servo. (Home nodes)
    Node(int numberOfMarbles, bool leftSide, Servo gateServo, int gateIRSensor)
    {
        this->numberOfMarbles = numberOfMarbles;
        this->leftSide = leftSide;
        this->gateServo = gateServo;
        this->gateIRSensor = gateIRSensor;
        this->releaseServo = dummyReleaseServo;
        next = nullptr;
    }
};

/// Function to insert a node at the end of a circular linked list.
Node* insertEnd(Node *tail, int value, bool leftSide, Servo gateServo, Servo releaseServo, int gateIRSensor) {
    Node *newNode = new Node(value, leftSide, gateServo, releaseServo, gateIRSensor);
    if (tail == nullptr){
        // If the list is empty, initialize it with the new node
        tail = newNode;

        // Point to itself to form a circular structure
        newNode->next = newNode;
    }
    else{
        // Insert new node after the current tail
        // and update the tail pointer.
        // New node points to the head node
        newNode->next = tail->next;

        // Tail node points to the new node
        tail->next = newNode;

        // Update tail to be the new node
        tail = newNode;
    }
    return tail;
}

/// Overloaded function to insert a node at the end of a circular linked list.
Node* insertEnd(Node *tail, int value, bool leftSide, Servo gateServo, int gateIRSensor) {
    Node *newNode = new Node(value, leftSide, gateServo, gateIRSensor);
    if (tail == nullptr){
        // If the list is empty, initialize it with the new node
        tail = newNode;

        // Point to itself to form a circular structure
        newNode->next = newNode;
    }
    else{
        // Insert new node after the current tail
        // and update the tail pointer.
        // New node points to the head node
        newNode->next = tail->next;

        // Tail node points to the new node
        tail->next = newNode;

        // Update tail to be the new node
        tail = newNode;
    }
    return tail;
}

/// Function to find the node position within the Linked List.
Node* findNodePosition(Node* last, int pos) {
  Node* curr = last->next;
  // If position is at 1, straight away return the current pointer 
  if (pos == 1) {
    return curr;
  } else {
    // Find the position of the node that has been played.
    for (int i=1; i<pos; i++) {
      curr = curr->next;
    }
  }
  return curr;
}

/// Function to update data at a certain position in the congkak. 
int getNumberOfMarbles(Node* last, int pos) {
  if (last == nullptr) {
    // If the list is empty
    Serial.println("Something went wrong, the position of the marble is not defined.");
  }

  Node* curr = findNodePosition(last, pos);
  int originalNumberOfMarbles = curr->numberOfMarbles;
  return originalNumberOfMarbles;
}

/// Prints the number of marbles within each node.
void printList(Node* last){
  if(last == NULL) return;
  
    // Start from the head node
    Node *head = last->next;
    while (true){
        Serial.println(head->numberOfMarbles);
        head = head->next;
        if (head == last->next)
            break;
    }
}

/// This function will control all the rearranging of balls into their respective holes.
void rearrangeBall(int numberOfBalls, Node* last, int pos, Servo releaseServo) {
  // Determine the node where the ball was played.
  Node* curr = findNodePosition(last, pos);
  int counter = 0;

  // Clear the marbles in the particular hole first.
  curr->numberOfMarbles = 0;

  // Rearrange the ball until all the balls are finished.
  while(counter < numberOfBalls) {
    curr = curr->next;

    // Determine the direction of the loader gate
    if (curr->leftSide) {
      loaderDirectionServo.write(110);
    } else {
      loaderDirectionServo.write(20);
    }

    // Open the gate
    delay(500);
    curr->gateServo.write(90);

    // Lift the ball up to the loader, until the ball passes the lift IR Sensor.
    delay(500);
    while(digitalRead(liftIRSensor)) {
      analogWrite(liftMotorSpeed, 150);
      digitalWrite(liftMotorA, HIGH);
    }
    digitalWrite(liftMotorA, LOW);

    delay(200);
    releaseServo.write(0);

    // Wait until the gate IR sensor, senses the ball.
    // Then close the gate.
    while(digitalRead(curr->gateIRSensor)) {
      delay(1);
    }
    curr->gateServo.write(0);
    curr->numberOfMarbles++;

    counter++;
  }

  if (curr->numberOfMarbles > 0) {
    /*
    Handle for case 1.

    After the last ball is placed, if the hole have more balls, then we will
    repeat the process.
    */
    rearrangeBall(curr->numberOfMarbles, curr, 8, curr->releaseServo);
  }
}

void setup() {
  // put your setup code here, to run once:
  marbleReleaseServo1.attach(22); // Attach servo1 to pin 8
  marbleReleaseServo2.attach(23); // Attach servo2 to pin 9
  marbleReleaseServo3.attach(24); // Attach servo3 to pin 10
  marbleReleaseServo4.attach(25); // Attach servo4 to pin 11
  marbleReleaseServo5.attach(26); // Attach servo5 to pin 12
  marbleReleaseServo6.attach(27); // Attach servo6 to pin 13
  marbleLoaderGate1.attach(28); // Attach servo1 to pin 8
  marbleLoaderGate2.attach(29); // Attach servo2 to pin 9
  marbleLoaderGate3.attach(30); // Attach servo3 to pin 10
  marbleLoaderGate4.attach(31); // Attach servo4 to pin 11
  marbleLoaderGate5.attach(32); // Attach servo5 to pin 12
  marbleLoaderGate6.attach(33); // Attach servo6 to pin 13
  marbleLoaderGate7.attach(34); // Attach servo4 to pin 11
  marbleLoaderGate8.attach(35); // Attach servo5 to pin 12
  loaderDirectionServo.attach(36); // Attach servo6 to pin 13

  marbleReleaseServo1.write(0); // Initialize servo1 position
  marbleReleaseServo2.write(0); // Initialize servo2 position
  marbleReleaseServo3.write(0); // Initialize servo3 position
  marbleReleaseServo4.write(0); // Initialize servo4 position
  marbleReleaseServo5.write(0); // Initialize servo5 position
  marbleReleaseServo6.write(0); // Initialize servo6 position
  marbleLoaderGate1.write(0); // Initialize servo2 position
  marbleLoaderGate2.write(0); // Initialize servo3 position
  marbleLoaderGate3.write(0); // Initialize servo4 position
  marbleLoaderGate4.write(0); // Initialize servo5 position
  marbleLoaderGate5.write(0); // Initialize servo6 position
  marbleLoaderGate6.write(0); // Initialize servo3 position
  marbleLoaderGate7.write(0); // Initialize servo4 position
  marbleLoaderGate8.write(0); // Initialize servo5 position
  loaderDirectionServo.write(20); // Initialize servo6 position

  pinMode(button1Pin, INPUT_PULLUP); // Set button1 pin as input with pull-up resistor
  pinMode(button2Pin, INPUT_PULLUP); // Set button2 pin as input with pull-up resistor
  pinMode(button3Pin, INPUT_PULLUP); // Set button3 pin as input with pull-up resistor
  pinMode(button4Pin, INPUT_PULLUP); // Set button4 pin as input with pull-up resistor
  pinMode(button5Pin, INPUT_PULLUP); // Set button5 pin as input with pull-up resistor
  pinMode(button6Pin, INPUT_PULLUP); // Set button6 pin as input with pull-up resistor

  pinMode(liftMotorA, OUTPUT);        // Motor driver pin 1 as output
  pinMode(liftMotorB, OUTPUT); 

  pinMode(liftIRSensor, INPUT);

  Serial.begin(9600);
}

Node* first;
Node* last;
void loop() {
  int numberOfMarbles;

  if (firstTimeSetup) {
    // Initiate the number of marbles in the first three holes
    first = new Node(8, true, marbleLoaderGate1, marbleReleaseServo1, gateOneIRSensor);
    first->next = new Node(7, true, marbleLoaderGate2, marbleReleaseServo2, gateTwoIRSensor);
    first->next->next = new Node(7, true, marbleLoaderGate3, marbleReleaseServo3, gateThreeIRSensor);

    last = first->next->next;
    last->next = first;

    // Initiate the number of numbers in home area, then the next three holes, and the last home area.
    last = insertEnd(last, 0, false, marbleLoaderGate7, gateEightIRSensor);
    last = insertEnd(last, 7, false, marbleLoaderGate4, marbleReleaseServo4, gateFourIRSensor);
    last = insertEnd(last, 7, false, marbleLoaderGate5, marbleReleaseServo5, gateFiveIRSensor);
    last = insertEnd(last, 7, false, marbleLoaderGate6, marbleReleaseServo6, gateSixIRSensor);
    last = insertEnd(last, 0, true, marbleLoaderGate8, gateSevenIRSensor);

    firstTimeSetup = false;
  }

  if (robotTurn) {
    // Whatever robot will do

  } else {
    // Human's turn to play the game

    // Button press to release balls from the hole.
    // The button will control each hole respectively.
    bool humanTurnFinished = false;

    if (!digitalRead(button1Pin)) {
      // Buffer time to release button
      delay(500);
      marbleReleaseServo1.write(90);

      // Buffer time for the servo to run before the DC motor
      // This is to prevent insufficient supply of current.
      delay(500);
      // Update the number of marbles in the hole to 0. Since the marbles have been taken out and spread to other holes.
      numberOfMarbles = getNumberOfMarbles(last, 1);

      rearrangeBall(numberOfMarbles, last, 1, marbleReleaseServo1);
      
      printList(last);

    } else if (!digitalRead(button2Pin)) {
      delay(500);
      marbleReleaseServo2.write(50);
      
      // Update the number of marbles in the hole to 0. Since the marbles have been taken out and spread to other holes.
      getNumberOfMarbles(last, 2);

      printList(last);
    } else if (!digitalRead(button3Pin)) {
      marbleReleaseServo3.write(50);
    } else if (!digitalRead(button4Pin)) {
      marbleReleaseServo4.write(50);
    } else if (!digitalRead(button5Pin)) {
      marbleReleaseServo5.write(50);
    } else if (!digitalRead(button6Pin)) {
      marbleReleaseServo6.write(50);
    } 

  }
}
