
class CTimeEscape
{
public:
	CTimeEscape()
	{
		CString strEscape;
		strEscape.Format(_T("{{{\n 计时开始 "));
		OutputDebugString(strEscape);
		m_start = ::GetTickCount(); //获取毫秒级数目
	}

	~CTimeEscape()
	{
		DWORD end = ::GetTickCount();
		CString strEscape;
		strEscape.Format(_T("   计时开始 \n}}} - 花费时间 %d"), end - m_start );
		OutputDebugString(strEscape);
	}
private:
	DWORD m_start;

};