#pragma once
#define GET_MAINFRM	static_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd())
#define GET_TOOLVIEW static_cast<CToolView*>(GET_MAINFRM->GetActiveView())