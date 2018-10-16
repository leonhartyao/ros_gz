/*
 * Copyright (C) 2018 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <string>
#include <thread>
#include <ignition/common/Image.hh>
#include <ignition/msgs.hh>
#include <ignition/transport.hh>

/// \brief Flag used to break the publisher loop and terminate the program.
static std::atomic<bool> g_terminatePub(false);

//////////////////////////////////////////////////
/// \brief Function callback executed when a SIGINT or SIGTERM signals are
/// captured. This is used to break the infinite loop that publishes messages
/// and exit the program smoothly.
void signal_handler(int _signal)
{
  if (_signal == SIGINT || _signal == SIGTERM)
    g_terminatePub = true;
}

//////////////////////////////////////////////////
int main(int /*argc*/, char **/*argv*/)
{
  // Install a signal handler for SIGINT and SIGTERM.
  std::signal(SIGINT,  signal_handler);
  std::signal(SIGTERM, signal_handler);

  // Create a transport node and advertise a topic.
  ignition::transport::Node node;

  // ignition::msgs::Header.
  auto header_pub = node.Advertise<ignition::msgs::Header>("header");
  ignition::msgs::Header header_msg;
  header_msg.mutable_stamp()->set_sec(2);
  header_msg.mutable_stamp()->set_nsec(3);

  // ignition::msgs::StringMsg.
  auto string_pub = node.Advertise<ignition::msgs::StringMsg>("string");
  ignition::msgs::StringMsg string_msg;
  string_msg.set_data("string");

  // ignition::msgs::Quaternion.
  auto quaternion_pub =
    node.Advertise<ignition::msgs::Quaternion>("quaternion");
  ignition::msgs::Quaternion quaternion_msg;
  quaternion_msg.set_x(1.0);
  quaternion_msg.set_y(2.0);
  quaternion_msg.set_z(3.0);
  quaternion_msg.set_w(4.0);

  // ignition::msgs::Vector3d.
  auto vector3_pub = node.Advertise<ignition::msgs::Vector3d>("vector3");
  ignition::msgs::Vector3d vector3_msg;
  vector3_msg.set_x(1.0);
  vector3_msg.set_y(2.0);
  vector3_msg.set_z(3.0);

  // ignition::msgs::Image.
  auto image_pub = node.Advertise<ignition::msgs::Image>("image");
  ignition::msgs::Image image_msg;
  image_msg.mutable_header()->CopyFrom(header_msg);
  image_msg.set_width(320);
  image_msg.set_height(240);
  image_msg.set_pixel_format(
    ignition::common::Image::PixelFormatType::RGB_INT8);
  image_msg.set_step(image_msg.width() * 3);
  image_msg.set_data(std::string(image_msg.height() * image_msg.step(), '0'));

  // ignition::msgs::IMU.
  auto imu_pub = node.Advertise<ignition::msgs::IMU>("imu");
  ignition::msgs::IMU imu_msg;
  imu_msg.mutable_header()->CopyFrom(header_msg);
  imu_msg.mutable_orientation()->CopyFrom(quaternion_msg);
  imu_msg.mutable_angular_velocity()->CopyFrom(vector3_msg);
  imu_msg.mutable_linear_acceleration()->CopyFrom(vector3_msg);

  // ignition::msgs::LaserScan.
  auto laserscan_pub = node.Advertise<ignition::msgs::LaserScan>("laserscan");
  const unsigned int num_readings = 100u;
  ignition::msgs::LaserScan laserscan_msg;
  laserscan_msg.mutable_header()->CopyFrom(header_msg);
  laserscan_msg.set_angle_min(-1.57);
  laserscan_msg.set_angle_max(1.57);
  laserscan_msg.set_angle_step(3.14 / num_readings);
  laserscan_msg.set_range_min(1);
  laserscan_msg.set_range_max(2);
  laserscan_msg.set_count(num_readings);
  laserscan_msg.set_vertical_angle_min(0);
  laserscan_msg.set_vertical_angle_max(0);
  laserscan_msg.set_vertical_angle_step(0);
  laserscan_msg.set_vertical_count(0);

  for (auto i = 0u; i < laserscan_msg.count(); ++i)
  {
    laserscan_msg.add_ranges(0);
    laserscan_msg.add_intensities(1);
  }

  // ignition::msgs::Magnetometer.
  auto magnetic_pub = node.Advertise<ignition::msgs::Magnetometer>("magnetic");
  ignition::msgs::Magnetometer magnetic_msg;
  magnetic_msg.mutable_header()->CopyFrom(header_msg);
  magnetic_msg.mutable_field_tesla()->CopyFrom(vector3_msg);

  // Publish messages at 1Hz.
  while (!g_terminatePub)
  {
    header_pub.Publish(header_msg);
    string_pub.Publish(string_msg);
    quaternion_pub.Publish(quaternion_msg);
    vector3_pub.Publish(vector3_msg);
    image_pub.Publish(image_msg);
    imu_pub.Publish(imu_msg);
    laserscan_pub.Publish(laserscan_msg);
    magnetic_pub.Publish(magnetic_msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  return 0;
}