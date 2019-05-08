
// RealSenseOSCDlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "RealSenseOSC.h"
#include "RealSenseOSCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRealSenseOSCDlg ダイアログ

CRealSenseOSCDlg::CRealSenseOSCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REALSENSEOSC_DIALOG, pParent)
	, m_edit_ip(_T("127.0.0.1"))
	, m_edit_port(12000)
	, m_edit_output(_T(""))
	, m_check_on_top(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRealSenseOSCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_START, m_button_start);
	DDX_Control(pDX, IDC_EDIT_IP, m_cedit_ip);
	DDX_Control(pDX, IDC_EDIT_PORT, m_cedit_port);
	DDX_Text(pDX, IDC_EDIT_IP, m_edit_ip);
	DDX_Text(pDX, IDC_EDIT_PORT, m_edit_port);
	DDX_Text(pDX, IDC_EDIT_OUTPUT, m_edit_output);
	DDX_Check(pDX, IDC_CHECK_TOP, m_check_on_top);
}


#define WM_USER_UPDATE_OUTPUT	(WM_USER + 1)

BEGIN_MESSAGE_MAP(CRealSenseOSCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_START, &CRealSenseOSCDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CRealSenseOSCDlg::OnBnClickedButtonStop)
	ON_MESSAGE(WM_USER_UPDATE_OUTPUT, CRealSenseOSCDlg::OnUpdateOutput)
	ON_BN_CLICKED(IDC_CHECK_TOP, &CRealSenseOSCDlg::OnBnClickedCheckTop)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CRealSenseOSCDlg メッセージ ハンドラー

BOOL CRealSenseOSCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	LoadConfigFile();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CRealSenseOSCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CRealSenseOSCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRealSenseOSCDlg::OnClose()
{
	m_loopEnable = false;
	SaveConfigFile();
	CDialogEx::OnClose();
}

BOOL CRealSenseOSCDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		case VK_RETURN:
		case VK_ESCAPE:
			return TRUE;
		default:
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CRealSenseOSCDlg::LockInterface()
{
	m_button_start.EnableWindow(false);
	m_cedit_ip.EnableWindow(false);
	m_cedit_port.EnableWindow(false);
}

void CRealSenseOSCDlg::UnlockInterface()
{
	m_button_start.EnableWindow(true);
	m_cedit_ip.EnableWindow(true);
	m_cedit_port.EnableWindow(true);
}

void CRealSenseOSCDlg::OnBnClickedButtonStart()
{
	UpdateData(true);	// get ui value
	m_ip = CString2string(m_edit_ip);
	m_port = m_edit_port;

	LockInterface();
	m_pThread = AfxBeginThread(ThreadParent, this);
}

void CRealSenseOSCDlg::OnBnClickedButtonStop()
{
	m_loopEnable = false;
}

// Thread (Parent)
UINT CRealSenseOSCDlg::ThreadParent(LPVOID pParam)
{
	((CRealSenseOSCDlg*)pParam)->Thread();
	return 0;
}

// Thread (Main)
void CRealSenseOSCDlg::Thread()
{
	TRACE("Thread : start\n");

	// osc setup
	UdpTransmitSocket transmitSocket(IpEndpointName(m_ip.c_str(), m_port));
	char buffer[OUTPUT_BUFFER_SIZE];
	osc::OutboundPacketStream p(buffer, OUTPUT_BUFFER_SIZE);
	std::stringstream ss;
	int count = 0;


	// realsense setup
	rs2::pipeline pipe;	// Declare RealSense pipeline, encapsulating the actual device and sensors
	rs2::config cfg;	// Create a configuration for configuring the pipeline with a non default profile
	pipe.start(cfg);	// Start pipeline with chosen configuration

	m_loopEnable = true;
	while (m_loopEnable) {

		// Wait for the next set of frames from the camera
        auto frames = pipe.wait_for_frames();

        // Get a frame from the pose stream
        auto f = frames.first_or_default(RS2_STREAM_POSE);

		// Cast the frame to pose_frame and get its data
        auto pose_data = f.as<rs2::pose_frame>().get_pose_data();

		p.Clear();
		p << osc::BeginBundleImmediate
			<< osc::BeginMessage("/realsense")
			<< pose_data.translation.x
			<< pose_data.translation.y
			<< pose_data.translation.z
			<< pose_data.rotation.w
			<< pose_data.rotation.x
			<< pose_data.rotation.y
			<< pose_data.rotation.z
			<< osc::EndMessage
			<< osc::EndBundle;
		transmitSocket.Send(p.Data(), p.Size());

		// buffer clear
		ss.str("");
		ss.clear(std::stringstream::goodbit);

		// test data
		ss << "Address Pattern: /realsense" << "\r\n"
			<< "[0] tx :" << pose_data.translation.x << "\r\n"
			<< "[1] ty :" << pose_data.translation.y << "\r\n"
			<< "[2] tz :" << pose_data.translation.z << "\r\n"
			<< "[3] qw :" << pose_data.rotation.w << "\r\n"
			<< "[4] qx :" << pose_data.rotation.w << "\r\n"
			<< "[5] qy :" << pose_data.rotation.y << "\r\n"
			<< "[6] qz :" << pose_data.rotation.z << "\r\n";

		m_edit_output = ss.str().c_str();
		AfxGetMainWnd()->SendMessage(WM_USER_UPDATE_OUTPUT);
	}
	m_pThread = NULL;

	TRACE("Thread : end\n");
	UnlockInterface();
}




int CRealSenseOSCDlg::LoadConfigFile()
{
	std::ifstream fin(PATH_CONFIG_FILE);
	if (!fin) {
		return -1; // can not open
	}
	std::string word;
	while (fin >> word) {
		if (word == "[Port]") {
			fin >> m_edit_port;
		}
		else if (word == "[TargetIPAddress]") {
			std::string ip;
			fin >> ip;
			m_edit_ip = ip.c_str();
		}
		else if (word == "[AlwaysOnTop]") {
			int flag;
			fin >> flag;
			if (flag == 1) {
				m_check_on_top = true;
			}
			else {
				m_check_on_top = false;
			}
		}
	}
	fin.close();
	UpdateData(false);	// apply ui value
	return 0;
}

int CRealSenseOSCDlg::SaveConfigFile()
{
	TRACE("in SaveConfigFile\n");
	UpdateData(true);	// get ui value

	std::ofstream fout(PATH_CONFIG_FILE);
	if ( !fout ) {
		return -1;	// can not open
	}

	fout << "[TargetIPAddress]  " << CString2string(m_edit_ip) << "\n";
	fout << "[Port]             " << m_edit_port << "\n";
	fout << "[AlwaysOnTop]      " << m_check_on_top << "\n";
	fout.close();

	return 0;
}


afx_msg LRESULT CRealSenseOSCDlg::OnUpdateOutput(WPARAM wParam, LPARAM lParam)
{
	UpdateData(false);
	return 0;
}

std::string CString2string(const CString& src)
{
	char* cbuf = new char[src.GetLength() + 1];
	int k;
	for (k = 0; k < src.GetLength(); k++) {
		cbuf[k] = (char)(src[k]);
	}
	cbuf[k] = '\0';
	std::string dst = cbuf;
	delete[] cbuf;
	return dst;
}

void CRealSenseOSCDlg::OnBnClickedCheckTop()
{
	UpdateData();

	if (m_check_on_top) {
		// 最前面表示の設定
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	else {
		// 最前面表示の解除
		SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}


// 起動直後に「Always on top」を反映させるために必要
void CRealSenseOSCDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ここにメッセージ ハンドラー コードを追加します。
	UpdateData();
	if (m_check_on_top) {
		// 最前面表示の設定
		SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
}
