﻿#pragma once

#define TEXTSERVICE_NAME	L"tsf-tutcode"
#ifndef _DEBUG
#define TEXTSERVICE_DESC	TEXTSERVICE_NAME
#else
#define TEXTSERVICE_DESC	TEXTSERVICE_NAME L"_DEBUG"
#endif
#define TEXTSERVICE_DIR		L"IMTSFTUTCODE"
#define TEXTSERVICE_VER		L"0.8.1"

//for resource
#define RC_AUTHOR			"KIHARA Hideto"
#define RC_PRODUCT			"tsf-tutcode"
#define RC_VERSION			"0.8.1"
#define RC_VERSION_D		0,8,1,0
