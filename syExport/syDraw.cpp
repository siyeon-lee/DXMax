#include "stdafx.h"
#include "syDraw.h"
#include<iostream>
#include<fstream>
#include<string>

syDraw::syDraw()
{
}


syDraw::~syDraw()
{
}
//
//bool syDraw::Import(TSTR filename)
//{
//	string in_line;
//	ifstream in(filename);
//	string classname = 0;
//	string objname = 0;
//	int iNumObj = 0;
//	int iNumTriangle = 0;
//	in >> classname >> iNumObj;
//	in >> objname >> iNumTriangle;
//	m_PNCTList.resize(iNumObj*iNumTriangle);
//	for (int ipnct = 0; ipnct < iNumObj*iNumTriangle; ipnct++)
//	{
//		in >> m_PNCTList[ipnct].p.x >> m_PNCTList[ipnct].p.y >> m_PNCTList[ipnct].p.z;
//		in >> m_PNCTList[ipnct].n.x >> m_PNCTList[ipnct].n.y >> m_PNCTList[ipnct].n.z;
//		in >> m_PNCTList[ipnct].c.x >> m_PNCTList[ipnct].c.y >> m_PNCTList[ipnct].c.z >> m_PNCTList[ipnct].c.w;
//		in >> m_PNCTList[ipnct].t.x >> m_PNCTList[ipnct].t.y;
//	}
//	in.close();
//
//	return true;
//}