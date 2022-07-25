#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 float prevIdle_ = 0.f;
 float prevNonIdle_ = 0.f;
};

#endif