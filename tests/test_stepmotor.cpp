#include "flprogFourWireControlledStepMotor.h"
#include "flprogPulDirStepMotor.h"
#include <cassert>
#include <climits>
#include <cstdio>
#include <vector>
#include <utility>
FakeRT_HW_Base RT_HW_Base;
static uint32_t fakeUs=0;
static uint8_t values[256]={};
static std::vector<std::pair<uint8_t,int>> writes;
static unsigned dirDelayCalls=0;
uint32_t micros(){return fakeUs;}
void pinMode(uint8_t,uint8_t){}
int digitalRead(uint8_t p){return values[p];}
void digitalWrite(uint8_t p,int v){values[p]=v;writes.push_back({p,v});}
void delayMicroseconds(unsigned n){assert(n==10);dirDelayCalls++;}
template<class M> void run(M& m, unsigned n){for(unsigned i=0;i<n;i++){fakeUs+=10;m.tick();m.pool();}}
int main(){
 // 0us input no longer causes division by zero.
 {
  FLProgPulDirStepMotor m(3,4);
  assert(m.tickPeriod()==10);m.tickPeriod(0);assert(m.tickPeriod()==10);
  m.tickPeriod(100);m.pulseTime(1);m.acceleration(0);m.maxSpeed(200);
  m.mode(FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE);run(m,1500);
 }
 // Recalculates interval even if currentSpeed already equals maxSpeed;
 // checks mode status, exact step count, queued increments, and saturated position.
 {
  FLProgFourWireControlledStepMotor m(10,11,12,13);
  m.acceleration(0);m.maxSpeed(1000);m.mode(FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE);
  run(m,1000);assert(m.currentStep()==-10);
  m.tickPeriod(20);run(m,500);assert(m.currentStep()==-20);
  m.mode(FLPROG_STOP_STEP_MOTOR_MODE);assert(m.getStatus()==FLPROG_STOP_STEP_MOTOR_STATUS);
  int32_t old=m.currentStep();run(m,100);assert(m.currentStep()==old);
  m.mode(FLPROG_GO_STEP_COUNT_STEP_MOTOR_MODE);
  assert(m.getStatus()==FLPROG_GO_STEP_COUNT_STEP_WAIT_COMMAND_STEP_MOTOR_STATUS);
  m.goThroughSteps(3);m.goThroughSteps(2);run(m,250);assert(m.currentStep()==old-5);
  assert(m.getStatus()==FLPROG_GO_STEP_COUNT_STEP_WAIT_COMMAND_STEP_MOTOR_STATUS);
  m.mode(FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE);
  m.targetStep(m.currentStep());assert(m.getStatus()==FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS);
  run(m,100);assert(m.currentStep()==old-5);
  m.setCurrentStep(INT32_MIN);m.mode(FLPROG_STOP_STEP_MOTOR_MODE);m.dir(false);
  m.mode(FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE);run(m,1000);
  assert(m.currentStep()==INT32_MIN);
 }
 // Every STEP pulse ends before the next one and has a LOW gap.
 {
  FLProgPulDirStepMotor m(5,6);writes.clear();m.acceleration(0);m.maxSpeed(50000);m.pulseTime(20);
  m.mode(FLPROG_CONTINUOUS_ROTATION_STEP_MOTOR_MODE);run(m,1000);
  unsigned rises=0,falls=0;bool high=false;
  for(auto e:writes) if(e.first==5){
    if(e.second==HIGH){ assert(!high);high=true;rises++; }
    else { assert(high);high=false;falls++; }
  }
  assert(rises>=5);assert(rises==falls||rises==falls+1);
  m.mode(FLPROG_STOP_STEP_MOTOR_MODE);run(m,10);assert(values[5]==LOW);
 }
 // Inverted DIR must provide the same timing as noninverted.
 {
  FLProgPulDirStepMotor m(20,21);m.invertDirPin(true);dirDelayCalls=0;
  m.dir(true);assert(dirDelayCalls==1);
 }
 // Homing and target position (no additional moves once target is reached).
 {
  FLProgFourWireControlledStepMotor m(50,51,52,53);
  m.acceleration(0);m.maxSpeed(1000);m.mode(FLPROG_POSITION_TRANSITION_STEP_MOTOR_MODE);
  m.targetStep(-3);run(m,300);assert(m.currentStep()==-3);
  m.targetStep(m.currentStep());assert(m.getStatus()==FLPROG_POSITION_TRANSITION_WAIT_COMMAND_STEP_MOTOR_STATUS);
 }
 puts("STEPMOTOR_HOST_TESTS_PASS");
}
