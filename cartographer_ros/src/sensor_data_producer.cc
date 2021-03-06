/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sensor_data_producer.h"

#include "msg_conversion.h"
#include "time_conversion.h"

namespace cartographer_ros {

SensorDataProducer::SensorDataProducer(
    const int trajectory_id,
    ::cartographer::mapping::SensorCollator<SensorData>* sensor_collator)
    : trajectory_id_(trajectory_id), sensor_collator_(sensor_collator) {}

void SensorDataProducer::AddOdometryMessage(
    const string& topic, const nav_msgs::Odometry::ConstPtr& msg) {
  string temp;
  temp = "robot_1/base_footprint";
  //std::cout<<"header.frame_id: "<<msg->header.frame_id<<'\n';
  if (temp[0]=='/')
  {
    temp.erase(0,1);
    temp = temp.replace(temp.begin()+8,temp.begin()+12,"base_footprint");
    //std::cout<<"header.frame_id: "<<temp<<'\n';
  }
  auto sensor_data = ::cartographer::common::make_unique<SensorData>(
      temp,
      SensorData::Odometry{ToRigid3d(msg->pose.pose),
                           ToPoseCovariance(msg->pose.covariance)});
  sensor_collator_->AddSensorData(
      trajectory_id_,
      ::cartographer::common::ToUniversal(FromRos(msg->header.stamp)), topic,
      std::move(sensor_data));
}

void SensorDataProducer::AddImuMessage(const string& topic,
                                       const sensor_msgs::Imu::ConstPtr& msg) {
  auto sensor_data = ::cartographer::common::make_unique<SensorData>(
      msg->header.frame_id, ToCartographer(*msg));
  sensor_collator_->AddSensorData(
      trajectory_id_,
      ::cartographer::common::ToUniversal(FromRos(msg->header.stamp)), topic,
      std::move(sensor_data));
}

void SensorDataProducer::AddLaserScanMessage(
    const string& topic, const sensor_msgs::LaserScan::ConstPtr& msg) {
  string temp;
  temp = msg->header.frame_id;
  if (temp[0]=='/')
  {
    temp.erase(0,1);
  }
  auto sensor_data = ::cartographer::common::make_unique<SensorData>(
      temp, ToCartographer(*msg));
  sensor_collator_->AddSensorData(
      trajectory_id_,
      ::cartographer::common::ToUniversal(FromRos(msg->header.stamp)), topic,
      std::move(sensor_data));
}

void SensorDataProducer::AddMultiEchoLaserScanMessage(
    const string& topic, const sensor_msgs::MultiEchoLaserScan::ConstPtr& msg) {
  auto sensor_data = ::cartographer::common::make_unique<SensorData>(
      msg->header.frame_id, ToCartographer(*msg));
  sensor_collator_->AddSensorData(
      trajectory_id_,
      ::cartographer::common::ToUniversal(FromRos(msg->header.stamp)), topic,
      std::move(sensor_data));
}

void SensorDataProducer::AddPointCloud2Message(
    const string& topic, const sensor_msgs::PointCloud2::ConstPtr& msg) {
  pcl::PointCloud<pcl::PointXYZ> pcl_points;
  pcl::fromROSMsg(*msg, pcl_points);

  auto sensor_data = ::cartographer::common::make_unique<SensorData>(
      msg->header.frame_id, ToCartographer(pcl_points));
  sensor_collator_->AddSensorData(
      trajectory_id_,
      ::cartographer::common::ToUniversal(FromRos(msg->header.stamp)), topic,
      std::move(sensor_data));
}

}  // namespace cartographer_ros
