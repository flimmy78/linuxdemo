/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file GpioClass.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2015/11/17-12:46:18
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2015/11/17by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
#ifndef __GPIOCLASS_H_
#define __GPIOCLASS_H_
#ifdef __cplusplus

#include <iostream>
#include <string>
#include <fstream>

extern "C" {
#endif


using namespace std;

class GPIOClass {
 public:
  GPIOClass();  // create a GPIO object that controls GPIO4 (default
  GPIOClass(string x); // create a GPIO object that controls GPIOx, where x is passed to this constructor
  int export_gpio(); // exports GPIO
  int unexport_gpio(); // unexport GPIO
  int setdir_gpio(string dir); // Set GPIO Direction
  int setval_gpio(string val); // Set GPIO Value (putput pins)
  int getval_gpio(string& val); // Get GPIO Value (input/ output pins)
  string get_gpionum(); // return the GPIO number associated with the instance of an object
 private:
  string gpionum; // GPIO number associated with the instance of an object
};

GPIOClass::GPIOClass(){
    this->gpionum = "4"; //GPIO4 is default
}

GPIOClass::GPIOClass(string gnum) {
    this->gpionum = gnum;  //Instatiate GPIOClass object for GPIO pin number "gnum"
}

int GPIOClass::export_gpio() {
    string export_str = "/sys/class/gpio/export";

    // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames
    ofstream exportgpio(export_str.c_str()); 

    if (exportgpio < 0){
        cout << " OPERATION FAILED: Unable to export GPIO"<< this->gpionum <<" ."<< endl;
        return -1;
    }

    exportgpio << this->gpionum ; //write GPIO number to export
    exportgpio.close(); //close export file

    return 0;
}

int GPIOClass::unexport_gpio() {
    string unexport_str = "/sys/class/gpio/unexport";

    ofstream unexportgpio(unexport_str.c_str()); //Open unexport file

    if (unexportgpio < 0){
        cout << " OPERATION FAILED: Unable to unexport GPIO"<< this->gpionum <<" ."<< endl;
        return -1;
    }

    unexportgpio << this->gpionum ; //write GPIO number to unexport
    unexportgpio.close(); //close unexport file
    return 0;
}

int GPIOClass::setdir_gpio(string dir) {
  string setdir_str ="/sys/class/gpio/gpio" + this->gpionum + "/direction";

  ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio
  if (setdirgpio < 0) {
    cout << " OPERATION FAILED: Unable to set direction of GPIO"<< this->gpionum <<" ."<< endl;
    return -1;
  }

  setdirgpio << dir; //write direction to direction file
  setdirgpio.close(); // close direction file
  return 0;
}

int GPIOClass::setval_gpio(string val) {
  string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";

  ofstream setvalgpio(setval_str.c_str()); // open value file for gpio
  if (setvalgpio < 0) {
    cout << " OPERATION FAILED: Unable to set the value of GPIO"<< this->gpionum <<" ."<< endl;
    return -1;
  }

  setvalgpio << val ;//write value to value file
  setvalgpio.close();// close value file
  return 0;
}

int GPIOClass::getval_gpio(string& val){
  string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
  ifstream getvalgpio(getval_str.c_str());// open value file for gpio
  if (getvalgpio < 0) {
     cout << " OPERATION FAILED: Unable to get value of GPIO"<< this->gpionum <<" ."<< endl;
    return -1;
  }

  getvalgpio >> val ;  //read gpio value

  if(val != "0")
	val = "1";
  else
	val = "0";
  
  getvalgpio.close(); //close the value file

  return 0;
}

string GPIOClass::get_gpionum() {
   return this->gpionum;
}

#ifdef __cplusplus
}
#endif
#endif
