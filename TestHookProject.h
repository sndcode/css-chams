#define _MAX_PATH   260

enum TAKSI_INDICATE_TYPE
{
	// Indicate my current state to the viewer
	// Color a square to indicate my mode.
	TAKSI_INDICATE_Idle = 0,	// No app ready and not looking.
	TAKSI_INDICATE_Hooking,		// CBT is hooked. (looking for new app to load)
	TAKSI_INDICATE_Ready,		// current app focus is ready to record?
	TAKSI_INDICATE_Recording,	// actively recording right now
	TAKSI_INDICATE_RecordPaused,	
	//TAKSI_INDICATE_Error,		// sit in error state til cleared ??
	TAKSI_INDICATE_QTY,
};

enum TAKSI_PROCSTAT_TYPE
{
#define ProcStatProp(a,b,c,d) TAKSI_PROCSTAT_##a,
#undef ProcStatProp
	TAKSI_PROCSTAT_QTY,
};

enum TAKSI_GAPI_TYPE
{
	// enumerate the available modes we support.
	TAKSI_GAPI_NONE = 0,
	TAKSI_GAPI_DESKTOP,	// HWND_DESKTOP
	TAKSI_GAPI_GDI,		// prefer all other modes over this.
	TAKSI_GAPI_OGL,		// a static linked dll, so can get a false positive.
#ifdef USE_DIRECTX8
	TAKSI_GAPI_DX8,
#endif
#ifdef USE_DIRECTX9
	TAKSI_GAPI_DX9,		// highest priority, always pick DX9 (over others) if it is supported.
#endif
	TAKSI_GAPI_QTY,
};

struct CTaksiProcStats
{
	// Record relevant stats on the current foreground app/process.
	// Display stats in the dialog in real time. SHARED

public:
	void InitProcStats();
	void ResetProcStats();
	void CopyProcStats( const CTaksiProcStats& stats );
	void UpdateProcStat( TAKSI_PROCSTAT_TYPE eProp )
	{
		m_dwPropChangedMask |= (1<<eProp);
	}
	void CopyProcStat( const CTaksiProcStats& stats, TAKSI_PROCSTAT_TYPE eProp )
	{
		// Move just a single property.
		const WORD wOffset = sm_Props[eProp][0];
		const WORD wSize = sm_Props[eProp][1];
		const WORD wEnd = wSize+wOffset;
		memcpy(((BYTE*)this) + wOffset, ((const BYTE*)&stats) + wOffset, wSize );
		UpdateProcStat(eProp);
	}

	double get_DataRecMeg() const
	{
		// How much data has been recorded?
		return ((double) m_nDataRecorded ) / (1024*1024);
	}

public:
	DWORD m_dwProcessId;				// What process is this?
	TCHAR m_szProcessPath[ _MAX_PATH ];	// What is the file path for the current process. (lower case)

	// dynamic info.
	TCHAR m_szLastError[ _MAX_PATH ];	// last error message (if action failed)

	HWND m_hWndCap;					// the window the graphics is in
	SIZE m_SizeWnd;					// the window/backbuffer size. (pixels)
	TAKSI_GAPI_TYPE m_eGAPI;		// What is the primary graphics mode i detect.

	TAKSI_INDICATE_TYPE m_eState;	// What are we doing with the process?
	float m_fFrameRate;				// measured frame rate. recording or not.
	UINT64 m_nDataRecorded;			// How much video data recorded in current stream (if any)

public:
	static const WORD sm_Props[ TAKSI_PROCSTAT_QTY ][2]; // offset,size
	DWORD m_dwPropChangedMask;		// TAKSI_PROCSTAT_TYPE mask
};
extern CTaksiProcStats sg_ProcStats;	// For display in the Taksi.exe app.

struct CTaksiProcess
{
	// Info about the Process this DLL is bound to.
	// ASSUME The current application is attached to a graphics API.
	// What am i doing to the process? anthing?
public:
	CTaksiProcess()
		: m_dwConfigChangeCount(0)
		, m_bIsProcessIgnored(false)
		, m_bIsProcessDesktop(false)
		, m_bStopGAPIs(false)
	{
		m_Stats.InitProcStats();
		m_Stats.m_dwProcessId = ::GetCurrentProcessId();
	}
	
	bool CheckProcessMaster() const;
	bool CheckProcessIgnored() const;
	void CheckProcessCustom();

	void StopGAPIs();
	void DetachGAPIs();
	HRESULT AttachGAPIs( HWND hWnd );
	bool StartGAPI( TAKSI_GAPI_TYPE eMode );

	bool OnDllProcessAttach();
	bool OnDllProcessDetach();

	int MakeFileName( TCHAR* pszFileName, const TCHAR* pszExt );
	void UpdateStat( TAKSI_PROCSTAT_TYPE eProp );

public:
	CTaksiProcStats m_Stats;
	TCHAR m_szProcessTitleNoExt[_MAX_PATH]; // use as prefix for captured files.
	HANDLE m_hHeap;					// the process heap to allocate on for me.

	DWORD m_dwConfigChangeCount;		// my last reconfig check id.

	HWND m_hWndHookTry;				// The last window we tried to hook 

	// if set to true, then CBT should not take any action at all.
	bool m_bIsProcessDesktop;
	bool m_bIsProcessIgnored;		// Is Master TAKSI.EXE or special app.
	bool m_bStopGAPIs;				// I'm not the main app anymore. unhook the graphics modes.
};
extern CTaksiProcess g_Proc;

extern HINSTANCE g_hInst;	// the DLL instance handle for the process.
