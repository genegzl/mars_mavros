/**
 * @brief Airspeed sensor plugin
 * @file px4airspeed.cpp
 * @author Yixi CAI <ixicai@connect.hku.hk>
 *
 * @addtogroup plugin
 * @{
 */
/*
 * Copyright 2021 Yixi CAI
 *
 * This file is part of the mavros package and subject to the license terms
 * in the top-level LICENSE file of the mavros repository.
 * https://github.com/mavlink/mavros/tree/master/LICENSE.md
 */

#include <mavros/mavros_plugin.h>
#include <mavros_msgs/Airspeed.h>


namespace mavros {
namespace extra_plugins {
/**
 * @brief PX4 Airspeed Sensor plugin
 *
 * This plugin allows publishing airspeed sensor data from PX4 FCU to ROS.
 */
class PX4AirspeedPlugin : public plugin::PluginBase {
public:
	PX4AirspeedPlugin() : PluginBase(),
		airspeed_nh("~px4airspeed")
	{}

	void initialize(UAS &uas_) override
	{
		PluginBase::initialize(uas_);

		airspeed_nh.param<std::string>("frame_id", frame_id, "px4airspeed");

		airspeed_pub = airspeed_nh.advertise<mavros_msgs::Airspeed>("airspeed", 100);

	}

	Subscriptions get_subscriptions() override
	{
		return {
			make_handler(&PX4AirspeedPlugin::handle_airspeed_m_s)
		};
	}

private:
	ros::NodeHandle airspeed_nh;

	std::string frame_id;

	ros::Publisher airspeed_pub;

	void handle_airspeed_m_s(const mavlink::mavlink_message_t *msg, mavlink::common::msg::AIRSPEED_M_S &airspeed_m_s)
	{
		auto header = m_uas->synchronized_header(frame_id, airspeed_m_s.time_usec);

		auto airspeed_msg = boost::make_shared<mavros_msgs::Airspeed>();

        airspeed_msg->header = header;
        airspeed_msg->indicated_airspeed_m_s = airspeed_m_s.indicated_airspeed_m_s;
        airspeed_msg->true_airspeed_m_s = airspeed_m_s.true_airspeed_m_s;
        airspeed_msg->air_temperature_celsius = airspeed_m_s.air_temperature_celsius;
        airspeed_msg->confidence = airspeed_m_s.confidence;
		airspeed_pub.publish(airspeed_msg);

	}

};
}	// namespace extra_plugins
}	// namespace mavros

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(mavros::extra_plugins::PX4AirspeedPlugin, mavros::plugin::PluginBase)


