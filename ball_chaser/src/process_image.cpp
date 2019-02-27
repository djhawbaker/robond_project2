#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot

    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if ( !client.call(srv) )
    {
        ROS_ERROR( "Failed to call service ball_chaser" );
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int i, j;
    // Divide image into 3 equal width sections. These are the left and right dividing lines
    int left = img.width / 3;
    int right = left * 2;
    bool ball_found = false;
    float forward_speed = 5;
    float stop = 0.0;
    // Turn 5 degrees / second in radians
    float turn_left = -0.0872665;
    float turn_right = 0.0872665;


    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    for ( i = 0; i < img.height(); ++i)
    {
        for ( j = 0; j < img.width(); ++j)
        {
            if ( white_pixel == img.data[i][j] )
            {
                ball_found = true;
                if ( img.data[i][j] < left )
                {
                    // Ball is to the left, turn left
                    drive_robot( forward_speed, turn_left );
                }
                else if (img.data[i][j] > right )
                {
                    // Ball is to the right, turn right 
                    drive_robot( forward_speed, turn_right );
                }
                else
                {
                    // Ball is straight ahead, chase it!
                    drive_robot( forward_speed, stop );
                }
                break;
            }
        }
        if ( ball_found == true )
        {
            break;
        }
    }

    if ( ball_found == false )
    {
        // Stop the bot
        drive_robot( stop, stop );
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
