#include "StorageManager.h"
bool StorageManager::begin(){return true;}
bool StorageManager::hasData(){return false;}
bool StorageManager::saveRecord(String record){return true;}
String StorageManager::getNextRecord(){return "";}
bool StorageManager::deleteNextRecord(){return true;}
bool StorageManager::clear(){return true;}
bool StorageManager::isFull(){return false;}
