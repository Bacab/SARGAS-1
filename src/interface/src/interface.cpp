#include "ros/ros.h"
#include "ros/time.h"
#include <fstream>
#include <string>
#include <new>
#include <exception>
#include "GPIO.hpp"
#include <move_base_msgs/MoveBaseAction.h>
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <actionlib/client/simple_action_client.h>

using namespace std;

string * file_list;
double * x_coord;
double * y_coord;
string current_line;
int i,j,k;
char choix;
int taille_index;
const bool in=true;
const bool out=false;
GPIO enter(67,in);
GPIO avance(68,in);
GPIO preced(68,in);

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int stoi(std::string Text)
{
 int result=0;
 std::stringstream convert(Text);
 if (!(convert >> result))
	result=0;
 return(result);
}

string Get_data(string complete)
{
	int debut,fin,taille;
	char * data;
	string res;
	debut=complete.find("<");
	fin=complete.find(">");
	taille=fin-debut-1;
	data=new char[taille+1];
	if (taille<=0)
		return("error, invalid data");
	taille=complete.copy(data,taille,debut+1);
	data[taille]='\0';
	res=data;
	delete [] data;
	return (res);
}

int main(int argc, char** argv){
	ros::init(argc, argv, "interface");
	ros::NodeHandle n;
	ros::Publisher goal_pub = n.advertise<move_base_msgs::MoveBaseAction>("goal", 50);
	ros::Publisher init_pub = n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initial_pose",50);
	MoveBaseClient ac("move_base", true);
 	while(!ac.waitForServer(ros::Duration(5.0))){
    		ROS_INFO("Waiting for the move_base action server to come up");
  	}
	move_base_msgs::MoveBaseGoal goal;
	geometry_msgs::PoseWithCovarianceStamped initial_pose;
	initial_pose.header.stamp=ros::Time::now();
	initial_pose.header.frame_id="/map";
	initial_pose.pose.pose.position.x=16.006;
	initial_pose.pose.pose.position.y=41.333;
	initial_pose.pose.pose.position.z=0;
	initial_pose.pose.pose.orientation.x=0;
	initial_pose.pose.pose.orientation.y=0;
	initial_pose.pose.pose.orientation.z=0;
	initial_pose.pose.pose.orientation.w=1;
	ifstream index_file ("C:/Users/Florian/Documents/Florian/PC/PFE/index.xml");
	if (!index_file.is_open())
	{
		ROS_INFO("Critical error, could not find index file. Please refer to a specialist for help.");
		return (-1);
	}
	taille_index=0;
	while (getline(index_file,current_line))
	{
		if(!current_line.compare("<nom_dest>"))
			taille_index=taille_index+1;
	}
	file_list=new string [taille_index];
	x_coord=new double [taille_index];
	y_coord=new double [taille_index];
	index_file.clear();
	index_file.seekg(0, ios::beg);
	ROS_INFO("Welcome to S.A.R.G.A.s !");
	system(("cvlc"+file_list[taille_index-3]).c_str());
	while (getline(index_file,current_line))
	{
		if(!current_line.compare("<nom_dest>"))
		{
			getline(index_file,current_line);
			file_list[i]=Get_data(current_line);
			i+=1;
			getline(index_file,current_line);
			x_coord[i]=stoi(Get_data(current_line));
			i+=1;
			getline(index_file,current_line);
			y_coord[i]=stoi(Get_data(current_line));
			i+=1;
		}
	}
	while(n.ok()){
		i=0;
		while(!enter.getValue())
		{
			if (avance.getValue())
			{
				while(avance.getValue())
				{}
				i=i+1%(taille_index-4);
			system(("cvlc"+file_list[i]).c_str());//play destination choisie
			}
			else if(preced.getValue())
			{
				while(preced.getValue())
				{}
				if(i=0)
				{			
					i=taille_index-4;
				}
				else
				{
					i=i-1;
				}
			system(("cvlc"+file_list[i]).c_str());//play destination choisie
			}
		}
		while(enter.getValue())
		{}
		goal.target_pose.header.frame_id = "base_link";
	  	goal.target_pose.header.stamp = ros::Time::now();

	  	goal.target_pose.pose.position.x = x_coord[i];
		goal.target_pose.pose.position.y = y_coord[i];
	  	goal.target_pose.pose.orientation.w = 1.0;
		ac.sendGoal(goal);

  		ac.waitForResult();

  		while(ac.getState() != actionlib::SimpleClientGoalState::SUCCEEDED)
    			ROS_INFO("En route pour le but");
		system(("cvlc"+file_list[taille_index-2]).c_str());
	}
	delete [] file_list;
	return 0;
}
