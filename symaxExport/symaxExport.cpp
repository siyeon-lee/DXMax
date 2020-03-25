// syExport.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "resource.h"
#include "syWriter.h"
#include "sySkinExp.h"
#include "syMatrixExp.h"
//#include "syDraw.h"
#define symaxExport_CLASS_ID Class_ID(0x5b927c16, 0x64d35c2f);		//맥스에서는 이 인덱스로 구별한다
extern HINSTANCE hInstance;

INT_PTR CALLBACK DlgProc(HWND hWnd,
	UINT msg, WPARAM wParam, LPARAM lParam);

class symaxExport : public UtilityObj
{
public:
	HWND		m_hPanel;
	Interface*	m_ip;
public:
	symaxExport() {
		m_hPanel = NULL;
	};
	~symaxExport() {};
	static symaxExport* GetInstance()
	{
		static symaxExport theExp;
		return &theExp;
	}
public:
	virtual void BeginEditParams(
		Interface *ip,
		IUtil *iu)
	{
		m_ip = ip;
		m_hPanel = ip->AddRollupPage(hInstance,
			MAKEINTRESOURCE(IDD_DIALOG1),
			DlgProc,
			_T("syMaxExp"), 0);		//이게뭐야??
	}
	virtual void EndEditParams(
		Interface *ip,
		IUtil *iu)
	{
		ip->DeleteRollupPage(m_hPanel);
	}
	virtual void DeleteThis()
	{
	}
	//virtual void SetStartupParam(MSTR param) {}
	virtual void SelectionSetChanged(Interface *ip, IUtil *iu)
	{
		syMatrixExp::Get().Set(NULL, ip);

		sySkinExp::Get().m_ObjectList.clear();
		sySkinExp::Get().m_MaterialList.clear();
		sySkinExp::Get().m_MtrlList.clear();
		//선택된 노드의 개수
		int iNumSelect = ip->GetSelNodeCount();
		if (iNumSelect <= 0)
		{
			return;
		}
		for (int iSelObj = 0; iSelObj < iNumSelect;
			iSelObj++)
		{
			INode* pNode = ip->GetSelNode(iSelObj);
			sySkinExp::Get().m_ObjectList.push_back(pNode);
			sySkinExp::Get().AddMaterial(pNode);
		}
	}


};

class symaxExportClassDesc : public ClassDesc2
{
	//max sdk가 돌아가는 기본 파이프라인 
public:
	virtual int IsPublic()
	{
		return TRUE;
	};
	virtual void* Create(BOOL /*loading = FALSE*/) 
	{ 
		//제일중요 
		return symaxExport::GetInstance();
	}
	virtual const TCHAR *	ClassName()
	{ 
		return _T("symaxExport100"); 
	}
	virtual SClass_ID SuperClassID() 
	{
		//중요
		return UTILITY_CLASS_ID; 
	}
	virtual Class_ID ClassID() 
	{ 
		//멕스 플러그인의 고유한 아이디를 만들어 냄. 유틸리티로 발급받아야 함
		return symaxExport_CLASS_ID;
	}
	virtual const TCHAR* Category()
	{ 
		return _T("symaxExporter");
	}

	virtual const TCHAR* InternalName()
	{
		return _T("symaxExportClassDesc");
	}	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 
	{ 
		return hInstance;		//HINSTANCE hInstance; dllmain에서 만들어
	}					// returns owning module handle

};

ClassDesc2* GetExportDesc()
{
	static symaxExportClassDesc symaxExportDesc;
	return &symaxExportDesc;
}


INT_PTR CALLBACK DlgProc(HWND hWnd,
	UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_SKINEXP:
		{
			TSTR szExpFile = sySkinExp::Get().SaveFileDlg(L"skm", L"SkinWriter");
			if (szExpFile != NULL)
			{
				sySkinExp::Get().Set(szExpFile,
					symaxExport::GetInstance()->m_ip);
				sySkinExp::Get().Export();
			}


		}break;
		case IDC_MATRIXEXP:
		{
			TSTR szExpFile = syMatrixExp::Get().SaveFileDlg(L"mat", L"MatrixWriter");
			if (szExpFile != NULL)
			{
				syMatrixExp::Get().Set(szExpFile,
					symaxExport::GetInstance()->m_ip);
				syMatrixExp::Get().Export();
			}

		}break;
		case IDC_SYEXP:
		{
			MessageBox(GetActiveWindow(), _T("Exp!"),
				_T("IDC_SYEXP!"), MB_OK);
			TSTR szExpFile = syMaxWriter::Get().SaveFileDlg(L"symax", L"symaxWriter");
			if (szExpFile != NULL)
			{
				syMaxWriter::Get().Set(szExpFile,
					symaxExport::GetInstance()->m_ip);
				syMaxWriter::Get().Export();
			}

		}break;
		}
	}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		GetCOREInterface()->RollupMouseMessage(
			hWnd, msg, wParam, lParam);
		break;
	default:
		return 0;
	}
	return 1;
}