#ifndef DISCOMFORT_INDEX_H
#define DISCOMFORT_INDEX_H

class DiscomfortIndex {
public:
  DiscomfortIndex();
  void Update(float temp, float hum);
  int GetValue();

private:
  int _value = 0;
  // const char* _unit;
  // float _value = 0.0;
  // struct tm _timeinfo;
};

#endif /* DISCOMFORT_INDEX_H */