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
#include "iostream"

using namespace std;

string * file_list;
double * x_coord;
double * y_coord;
string current_line;
int i,j,k;
char choix;
int nb_dest,nb_x,nb_y;
const bool in=true;
const bool out=false;
//GPIO enter(67,in);
//GPIO avance(68,in);
//GPIO preced(68,in);

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

double stod(std::string Text)
{
 double result=0.0;
 std::stringstream convert(Text);
 if (!(convert >> result))
	result=0.0;
 return(result);
}

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
	debut=complete.find(">");
	fin=complete.find("<",2);
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
    		cout << "Waiting for the move_base action server to come up" << endl;
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
	ifstream index_file ("/home/florian/index.xml");
	if (!index_file.is_open())
	{
		cout << "Critical error, could not find index file. Please refer to a specialist for help." << endl;
		return (-1);
	}
	nb_dest=0;
	nb_x=0;
	nb_y=0;
	while (getline(index_file,current_line))
	{
		if(current_line.find("<nom_dest>")!=-1)
			nb_dest+=1;
		else if(current_line.find("<x_coord>")!=-1)
			nb_x+=1;
		else if(current_line.find("<y_coord>")!=-1)
			nb_y+=1;
	}
	if (nb_dest==0)
		return(-1);
	file_list=new string [nb_dest];
	x_coord=new double [nb_x];
	y_coord=new double [nb_y];
	index_file.clear();
	index_file.seekg(0, ios::beg);
	cout << "Welcome to S.A.R.G.A.s !" << endl;
	i=0;
	j=0;
	k=0;
	while (getline(index_file,current_line))
	{
		if(current_line.find("<nom_dest>")!=-1)
		{
			file_list[i]=Get_data(current_line);
			i+=1;
		}
		else if(current_line.find("<x_coord>")!=-1)
		{
			x_coord[j]=stod(Get_data(current_line));
			j+=1;
		}
		else if(current_line.find("<y_coord>")!=-1)
		{
			y_coord[k]=stod(Get_data(current_line));
			k+=1;
		}
		cout << current_line << endl;
	}
	cout << ("cvlc"+file_list[nb_dest-3]).c_str() << endl;
	while(n.ok()){
		i=0;
		cin >> choix;
		cout << "cvlc"+file_list[i] << endl;//play destination choisie
		while(choix!='e')
		{
			cin >> choix;			
			if (choix=='d')
			{
				i=(i+1)%(nb_dest-3);
				cout << "cvlc"+file_list[i] << endl;//play destination choisie
			}
			else if(choix=='q')
			{
				if(i==0)
				{			
					i=nb_dest-4;
				}
				else
				{
					i=i-1;
				}
			cout << "cvlc"+file_list[i] << endl;//play destination choisie
			}
		}
		goal.target_pose.header.frame_id = "base_link";
	  	goal.target_pose.header.stamp = ros::Time::now();

	  	goal.target_pose.pose.position.x = x_coord[i];
		goal.target_pose.pose.position.y = y_coord[i];
	  	goal.target_pose.pose.orientation.w = 1.0;
		ac.sendGoal(goal);

  		ac.waitForResult();

  		while(ac.getState() != actionlib::SimpleClientGoalState::SUCCEEDED)
    			cout << "En route pour le but" << endl;
		cout << "cvlc"+file_list[nb_dest-2] << endl;
	}
	delete [] file_list;
	return 0;
}
