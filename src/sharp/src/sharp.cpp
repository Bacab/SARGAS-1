#include "ros/ros.h"
#include "ros/time.h"
#include "ADC.hpp"
#include <iostream>
#include <fstream>
#include <sensor_msgs/LaserScan.h>
#include <cmath>
#include <string>

int main(int argc, char **argv)
{
  try {
    /*ADC adc_right(4); 
    ADC adc_middle(5); 
    ADC adc_left(6); */

    ros::init(argc, argv, "sharp");

    ros::NodeHandle n;
  
    const double distance_min=0.093;//in meters
    const double distance_max=0.108;
    const double pi=4*atan(1);
    double offset[3]={0.106,0.093,0.108};
    double raw[3];
    float lookup[3][1024];
    
    ros::Publisher sharp_pub= n.advertise<sensor_msgs::LaserScan>("scan", 50);
    
    ros::Rate loop_rate(1);
    sensor_msgs::LaserScan sharp;
    
    sharp.header.frame_id="base_link";
    sharp.angle_min = -pi/4.0;
    sharp.angle_max = pi/4.0;
    sharp.angle_increment = pi/4.0;
    sharp.time_increment = 0;
    sharp.range_min = 0.03+distance_min;
    sharp.range_max = 0.4+distance_max;
    sharp.ranges.resize(3);

    for(int i=1;i<=1024;i++)
    {
     lookup[0][i-1]=(3.00*1000.00)*pow(i,(-1.00));
     lookup[1][i-1]=(3.08*1000.00)*pow(i,(-1.03));
     lookup[2][i-1]=(3.57*1000.00)*pow(i,(-1.06));
    }
 
    while (ros::ok())
    {
      for (int k=0;k<10;k++)
      {
       raw[0] += 90;//adc_right.getValue()/4.0;
       raw[1] += 90;//adc_middle.getValue()/4.0;
       raw[2] += 90;//adc_left.getValue()/4.0; 
      }
      raw[0]=raw[0]/10.0;
      raw[1]=raw[1]/10.0;
      raw[2]=raw[2]/10.0;
      ROS_DEBUG("ADC values read: %u %u %u", raw[0], raw[1], raw[2]);
      sharp.header.stamp = ros::Time::now();
      for(int i=0;i<3;i++)
      {
 	if(raw[i]<85)//a determiner
  	{
  	  raw[i]=1;
  	}
      }
      sharp.ranges[i]=lookup[i][raw[i]-1]+offset[i];
      }
      sharp_pub.publish(sharp);
      ros::spinOnce();
  
      loop_rate.sleep();
    }
  }
  catch (std::exception &e) {
  	ROS_ERROR("Exception : %s", e.what());
  }
  
  return 0;
}
