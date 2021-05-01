#include <mrs_msgs/ReferenceStampedSrv.h>
#include <ros/ros.h>
#include <std_msgs/String.h>

#include <sstream>
#include <cstdio>

int main(int argc, char **argv) {
    ros::init(argc, argv, "test_contol");

    if (argc < 2) {
        ROS_ERROR("UAV not specified");
        return -1;
    }

    ros::NodeHandle n;

    const std::string uav_name = argv[1];
    std::string frame_id = uav_name + "/fcu_untilted";

    ROS_INFO_STREAM("Started custom control for " << uav_name);

    ros::ServiceClient reference_client = n.serviceClient<mrs_msgs::ReferenceStampedSrv>("/" + uav_name + "/control_manager/reference");
    
    ros::Rate rate(100);

    int requestCounter = 0;
    double change_x, change_y, change_z, change_heading;
    char c;

    while (ros::ok()) {
        change_x = 0;
        change_y = 0;
        change_z = 0;
        change_heading = 0;
        system("stty raw"); 
        c = getchar();
        switch (c) {
            
            case 3: // Contol + C
                system("stty cooked");
                return 0;

            case 'w':
                change_y = 2;
                break;
            case 's':
                change_y = -2;
                break;
            case 'd':
                change_x = 2;
                break;
            case 'a':
                change_x = -2;
                break;
            case 'q':
                change_heading = 0.7;
                break;
            case 'e':
                change_heading = -0.7;
                break;
            case 'r':
                change_z = 2;
                break;
            case 'f':
                change_z = -2;
            default:
                break;

        }
        mrs_msgs::ReferenceStampedSrv srv;

        srv.request.header.frame_id = frame_id;
        srv.request.header.seq = requestCounter++;
        srv.request.header.stamp = ros::Time::now();

        srv.request.reference.position.x = change_x;
        srv.request.reference.position.y = change_y;
        srv.request.reference.position.z = change_z;
        srv.request.reference.heading = change_heading;

        if (reference_client.call(srv)) {
            if (!srv.response.success) {
                ROS_WARN_STREAM("Service call successfull. Success: " << srv.response.success + '0' << ". Message: " << srv.response.message;);
            }
        } else {
            ROS_ERROR("Service call error...");
        }

        ros::spinOnce();
        rate.sleep();
    }
}
