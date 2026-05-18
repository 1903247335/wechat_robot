#pragma once
#include"pch.h"
#include<iostream>
namespace MyNetwork {
	enum MsgType : int;  
	class Response;
}
bool HandleStateNotice(const MyNetwork::Response&);
bool ProcessStateNotice(const MyNetwork::Response& );
bool IsNotificationState(const MyNetwork::MsgType& );
bool IsClientInitialized();