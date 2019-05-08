
// RealSenseOSCDlg.h : ヘッダー ファイル
//

#pragma once



// for save/load config file
#include <fstream>
#include <string>
#define PATH_CONFIG_FILE	"config.txt"

// for osc
#include <osc/OscOutboundPacketStream.h>
#include <ip/UdpSocket.h>
#pragma comment(lib, "winmm.lib")
#define OUTPUT_BUFFER_SIZE 1024

// for output (stringstream)
#include <sstream>

// for realsense
#include <librealsense2/rs.hpp>


// CRealSenseOSCDlg ダイアログ
class CRealSenseOSCDlg : public CDialogEx
{
// コンストラクション
public:
	CRealSenseOSCDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALSENSEOSC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedCheckTop();
	afx_msg LRESULT OnUpdateOutput(WPARAM wParam, LPARAM lParam);

public:
	BOOL PreTranslateMessage(MSG* pMsg);

	static UINT ThreadParent(LPVOID pParam);
	void Thread();
	CWinThread* m_pThread = NULL;
	BOOL m_loopEnable = false;

	void LockInterface();
	void UnlockInterface();

	int LoadConfigFile();
	int SaveConfigFile();

	CButton m_button_start;

	CEdit m_cedit_ip;
	CEdit m_cedit_port;

	CString m_edit_ip;
	int m_edit_port;

	std::string m_ip;
	int m_port;
	CString m_edit_output;
	BOOL m_check_on_top = false;
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

std::string CString2string(const CString& src);