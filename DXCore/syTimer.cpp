#include "syTimer.h"
//__imp_timeGetTime 외부 기호 : winmm.lib
float g_fSecondPerFrame = 0.0f;
float g_fGameTimer = 0.0f;
bool       syTimer::Init()
{
	QueryPerformanceCounter(&m_BeforeTick);

	// 고해상도 타이머 지원여부 판단
	if (m_BeforeTick.QuadPart == 0)
	{
		return false;
	}
	m_FPS = m_BeforeTick;
	m_iFramePerSecond = 0;
	m_iFrameCount = 0;
	m_fSecondPerFrame = 0;

	Start();
	return true;
};
bool       syTimer::Frame()
{
	//1프레임의 시간 체크
	//현재 시간을 찾는다
	QueryPerformanceCounter(&m_Current);
	m_fSecondPerFrame = static_cast<float>(m_Current.QuadPart - m_BeforeTick.QuadPart) / static_cast<float>(m_Frequency.QuadPart);

	/*프레임 제한
		*DWORD dwInterval = 1000 / 60;*/
	//if (dwDelay > 0)
	//{

	//		return false;
	//}

	g_fSecondPerFrame = m_fSecondPerFrame;
	m_fGameTimer += m_fSecondPerFrame;
	g_fGameTimer = m_fGameTimer;
	
	//1초당 프레임 체크( 0보다 크면 정수이기 때문에 1이 된다)
	if (((m_Current.QuadPart - m_FPS.QuadPart) / m_Frequency.QuadPart) >= 1)
	{
		m_iFramePerSecond = m_iFrameCount;
		m_iFrameCount = 0;
		m_FPS = m_Current;		//왜??
		_stprintf_s(m_csBuffer, _T("Timer:[%10.4f], FPS:[%d]"),
			m_fGameTimer, m_iFramePerSecond);
	}

	
	m_iFrameCount++;

	m_BeforeTick = m_Current;


	return true;
};
bool       syTimer::Render() {

	return true;
};
bool       syTimer::Release() {
	return true;
};

int	syTimer::GetFPS()
{
	return m_iFramePerSecond;
}
float syTimer::GetSPF()
{
	return m_fSecondPerFrame;
}


void syTimer::Start()
{
	m_bStarted = true;
	m_fEventTime = 0.0f;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_Start);
}

void syTimer::Stop()
{
	GetElapsedTime();
	m_bStarted = false;
}


bool syTimer::IsStarted() const
{
	return m_bStarted;
}


float syTimer::GetElapsedTime()
{
	if (m_bStarted)
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_Elapse);
		m_fEventTime = static_cast<float>(m_Elapse.QuadPart - m_Start.QuadPart) / static_cast<float>(m_Frequency.QuadPart);
	}
	return m_fEventTime;
}



syTimer::syTimer()
{
	m_bStarted = false;
	m_iFramePerSecond = 0;
	m_iFrameCount = 0;
	m_fSecondPerFrame = 0.0f; // 1프레임 경과시간
	m_fGameTimer = 0.0f; // 실행된 경과 시간
	m_fEventTime = 0.0f;
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_Frequency);

}


syTimer::~syTimer()
{
}
