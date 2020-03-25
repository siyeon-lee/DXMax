// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
// Precompile

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>



// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <maxtypes.h>
#include <utilapi.h>
#include <max.h>
#include <stdmat.h>
#include <decomp.h>
#include <bipexp.h>	//캐릭터
#include <phyexp.h>
#include <iskin.h>
#include <algorithm>

#define CTL_CHARS		31 // ( )
#define SINGLE_QUOTE	39 // ( ' )
#define ALMOST_ZERO		1.0e-3f