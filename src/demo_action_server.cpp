
#include "ros/ros.h"
#include "std_msgs/Int32.h"
//implement an action server node
#include <actionlib/server/simple_action_server.h>
//generated from de stored action file , for accessing pur action definition
#include "mastering_ros_demo_pkg/Demo_actionAction.h"
#include <iostream>
#include <sstream>

class Demo_actionAction //class action server definition
{
protected:
  ros::NodeHandle nh_;
  // NodeHandle instance must be created before this line. Otherwise strange error may occur.
  
  // Create a simple action server instance with our custom action message type
  actionlib::SimpleActionServer<mastering_ros_demo_pkg::Demo_actionAction> as; 
  // create messages that are used to published feedback/result
  mastering_ros_demo_pkg::Demo_actionFeedback feedback;
  mastering_ros_demo_pkg::Demo_actionResult result;

  std::string action_name;
  int goal;
  int progress;

public:
 // action constructor. action server created taking arguments in (), executeCB is the action callback
  Demo_actionAction(std::string name) :
    as(nh_, name, boost::bind(&Demo_actionAction::executeCB, this, _1), false),
    action_name(name)
  {// register a callback when the action is preempted
	as.registerPreemptCallback(boost::bind(&Demo_actionAction::preemptCB, this));
	as.start();
  }

  ~Demo_actionAction(void)
  {
  }

  void preemptCB(){
	ROS_WARN("%s got preempted!", action_name.c_str());
	result.final_count = progress;
	as.setPreempted(result,"I got Preempted"); 

  }
  //callback definition which is executed when the action server receives a goal value
  void executeCB(const mastering_ros_demo_pkg::Demo_actionGoalConstPtr &goal)
  {
	if(!as.isActive() || as.isPreemptRequested()) return;
	ros::Rate rate(5);
	ROS_INFO("%s is processing the goal %d", action_name.c_str(), goal->count);
        // execute this until the goal value is reached, it send the current progress as feedback
	for(progress = 1 ; progress <= goal->count; progress++){
		//Check for ros
		if(!ros::ok()){
			result.final_count = progress;
			as.setAborted(result,"I failed !");
			ROS_INFO("%s Shutting down",action_name.c_str());
			break;

		}
	        //check whether the action server is active or it is preempted.
		if(!as.isActive() || as.isPreemptRequested()){
			return;
		}	
                //If the current value reaches the goal value, then it publishes the final result
		if(goal->count <= progress){
			ROS_INFO("%s Succeeded at getting to goal %d", action_name.c_str(), goal->count);
			result.final_count = progress;
			as.setSucceeded(result);

		}else{
			ROS_INFO("Setting to goal %d / %d",feedback.current_number,goal->count);
			feedback.current_number = progress;
			as.publishFeedback(feedback);
		}
		rate.sleep();
	}	
  }
};

int main(int argc, char** argv)
{

  ros::init(argc, argv, "demo_action");
  ROS_INFO("Starting Demo Action Server");
  //In main() ,we create an instance of Demo_actionAction , which will start the action server.
  Demo_actionAction demo_action_obj(ros::this_node::getName());
  ros::spin();
  return 0;
}
