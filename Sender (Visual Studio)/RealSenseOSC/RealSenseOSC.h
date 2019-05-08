
// RealSenseOSC.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CRealSenseOSCApp:
// このクラスの実装については、RealSenseOSC.cpp を参照してください
//

class CRealSenseOSCApp : public CWinApp
{
public:
	CRealSenseOSCApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CRealSenseOSCApp theApp;
