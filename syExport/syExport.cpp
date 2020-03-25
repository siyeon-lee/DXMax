// syExport.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "syWriter.h"
//#include "syDraw.h"
#define syExport_CLASS_ID Class_ID(0x907008e, 0x2af660f8);		//맥스에서는 이 인덱스로 구별한다

class syExport : public SceneExport
{
	//SceneExport : 맥스에 떠있는 화면의 정보를 얻어오는 기능 -> 확장해서 화면 출력

public:
	syWriter m_syWriter;
	//syDraw m_syDraw;
	syExport() {};
	~syExport() {};

	int				ExtCount();
	const MCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
	const MCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
	const MCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
	const MCHAR *	AuthorName();				// ASCII Author name
	const MCHAR *	CopyrightMessage();			// ASCII Copyright message
	const MCHAR *	OtherMessage1();			// Other message #1
	const MCHAR *	OtherMessage2();			// Other message #2
	unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
	void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box
	int				DoExport(const MCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts = FALSE, DWORD options = 0);	// Export file

};

class syExportClassDesc : public ClassDesc2
{
	//max sdk가 돌아가는 기본 파이프라인 
public:
	virtual int IsPublic()
	{
		return true;
	};
	virtual void* Create(BOOL /*loading = FALSE*/) 
	{ 
		//제일중요 
		return new syExport();
	}
	virtual const TCHAR *	ClassName()
	{ 
		return _T("syExport100"); 
	}
	virtual SClass_ID SuperClassID() 
	{
		//중요
		return SCENE_EXPORT_CLASS_ID; //무조건 이거(상속받은 부모가 SceneExport임을 알림)
	}
	virtual Class_ID ClassID() 
	{ 
		//멕스 플러그인의 고유한 아이디를 만들어 냄. 유틸리티로 발급받아야 함
		return syExport_CLASS_ID; 
	}
	virtual const TCHAR* Category()
	{ 
		return _T("syExporter");
	}

	virtual const TCHAR* InternalName()
	{
		return _T("syExportClassDesc");
	}	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 
	{ 
		return hInstance;		//HINSTANCE hInstance; dllmain에서 만들어
	}					// returns owning module handle

};

ClassDesc2* GetExportDesc()
{
	static syExportClassDesc syExportDesc;
	return &syExportDesc;
}


int	syExport::ExtCount()
{
	//확장자 중요
	return 1;

}
const MCHAR * syExport::Ext(int n) 
{
	//맥스에서 파일 뽑으면 확장자를 뭐로 붙일래?
	return _T("sy");
}
const MCHAR * syExport::LongDesc()
{
	//
	return _T("sy exporter 2.0");
}
const MCHAR * syExport::ShortDesc() 
{
	return _T("syexporter");
}
const MCHAR * syExport::AuthorName()
{
	//개발자 이름
	return _T("sy");
}
const MCHAR * syExport::CopyrightMessage() 
{
	//저작권
	return _T("");
}
const MCHAR * syExport::OtherMessage1() 
{
	//메세지
	return _T("");
}
const MCHAR * syExport::OtherMessage2() 
{
	return _T("");
}
unsigned int syExport::Version() 
{
	//버전
	return 100;	//1.0 1.1은 101
}
void syExport::ShowAbout(HWND hWnd) 
{

}
int	syExport::DoExport(const MCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options )
{
	//핵심함수
	m_syWriter.Set(name, i);
	m_syWriter.Export();
	return true;
}
