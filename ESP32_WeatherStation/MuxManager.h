#pragma once
class MuxManager{
public:
bool begin();
bool selectChannel(unsigned char channel);
float readChannel(unsigned char channel);
bool readAllChannels();
};
