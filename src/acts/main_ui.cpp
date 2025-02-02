#include <includes.hpp>
#include <CommCtrl.h>
#include <core/config.hpp>
#include <hook/error.hpp>
#include "tools/tools_ui.hpp"

namespace tool::ui {
	LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	ActsWindow actsWindow{};

	void ActsWindow::UpdateWindowName() {
		TCITEMW tci{};
		WCHAR achTemp[256];
		tci.mask = TCIF_TEXT;
		tci.pszText = achTemp;
		tci.cchTextMax = ARRAYSIZE(achTemp);
		int page = TabCtrl_GetCurSel(hwndTab);
		if (!TabCtrl_GetItem(hwndTab, page, &tci)) {
			tci.pszText = nullptr; // wtf?
		}

		const wchar_t* id = tci.pszText && *tci.pszText ? tci.pszText : nullptr;
		std::wstring winName{ std::format(L"Atian tools {} UI{}{}", core::actsinfo::VERSIONW, (id ? L" - " : L""), (id ? id : L"")) };
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)winName.c_str());
	}

	void ActsWindow::RelocateDisplay(int x, int y) {
		width = x;
		height = y;
		if (actsWindow.hwndTab) {
			RECT rec{};
			SetWindowPos(hwndTab, NULL, 0, 0, x, y, SWP_SHOWWINDOW);
			GetClientRect(hwndTab, &rec);
			TabCtrl_AdjustRect(hwndTab, FALSE, &rec);
			SetWindowPos(hwndDisplay, NULL, rec.left, rec.top, rec.right - rec.left, rec.bottom - rec.top, SWP_SHOWWINDOW);
			auto& page = pages[TabCtrl_GetCurSel(hwndTab)];
			if (page->m_resize) {
				page->m_resize(x, y);
			}
		}
		else if (actsWindow.hwndLogTab) {
			SetWindowPos(hwndLogTab, NULL, 0, 0, x, y, SWP_SHOWWINDOW);
		}
	}
	void ActsWindow::SetLogMessage(const std::wstring& str) {
		if (actsWindow.hwndLogTab) {
			static std::string logs{};
			logs = std::format("{}\n{}", logs, utils::WStrToStr(str));
			Static_SetText(hwndLogTab, logs.c_str());
		}
	}

	static HFONT titlefont = []() -> HFONT {
		return CreateFontA(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
	}();

	static HFONT deffont = []() -> HFONT {
		return CreateFontA(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
	}();
	
	bool CALLBACK SetFont(HWND child, LPARAM font) {
		SendMessage(child, WM_SETFONT, font, FALSE);
		return true;
	}

	void ActsWindow::SetTitleFont(HWND elem) {
		SendMessage(elem, WM_SETFONT, (LPARAM)titlefont, FALSE);
	}

	void ActsWindow::LoadPage() {
		if (hwndDisplay) {
			// clear previous display
			DestroyWindow(hwndDisplay);
			hwndDisplay = NULL;
		}

		if (pages.empty()) {
			LOG_WARNING("No acts-ui page");
			return;
		}

		auto* item = pages[TabCtrl_GetCurSel(hwndTab)];
		hwndDisplay = CreateWindowExW(
			0,
			PAGE_CLASS_NAME,
			L"",
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			0, 0, 0, 0,
			hwndTab, NULL, hinst, NULL
		);
		UpdateWindowName();
		// render page
		item->m_func(hwndDisplay, hinst);
		core::config::SetString("ui.last", item->m_id);
		core::config::SaveConfig(); // save last page
		EnumChildWindows(hwnd, (WNDENUMPROC)SetFont, (LPARAM)deffont);
		RelocateDisplay(width, height);
	}

	ActsWindow& window() {
		return actsWindow;
	}

	std::wstring GetWindowTextVal(HWND hwnd) {
		static thread_local WCHAR tmp[0x512];

		if (SUCCEEDED(GetWindowTextW(hwnd, &tmp[0], ARRAYSIZE(tmp)))) {
			tmp[ARRAYSIZE(tmp) - 1] = 0;
			return tmp;
		}
		return L"";
	}

	int MainActsUI(HINSTANCE hInstance, int nShowCmd) {
		// build UI
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		if (FAILED(hr)) {
			MessageBoxW(NULL, L"Can't init con.", L"ACTS", MB_OK | MB_ICONERROR);
			return -1;
		}

		hook::error::InstallErrorUI(hInstance, nShowCmd);

		actsWindow.hinst = hInstance;
		WNDCLASSW wc{};
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		wc.hIcon = LoadIconW(hInstance, L"acts_logo");
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);

		RegisterClassW(&wc);

		WNDCLASSW wc2{};
		wc2.lpfnWndProc = DialogProc;
		wc2.hInstance = hInstance;
		wc2.lpszClassName = PAGE_CLASS_NAME;
		wc2.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc2.hCursor = LoadCursor(NULL, IDC_ARROW);

		RegisterClassW(&wc2);

		// Create the window.
		actsWindow.hwnd = CreateWindowExW(
			0,
			CLASS_NAME,
			L"Menu",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_MAXIMIZEBOX,

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, actsWindow.width, actsWindow.height,

			NULL,
			NULL,
			hInstance,
			NULL
		);

		if (actsWindow.hwnd == NULL) {
			return -1;
		}
		actsWindow.hwndLogTab = CreateWindowExW(
			0,
			L"STATIC",
			L"",
			SS_CENTER | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			actsWindow.hwnd,
			NULL,
			hInstance,
			NULL
		);

		if (actsWindow.hwndLogTab == NULL) {
			return -1;
		}

		EnumChildWindows(actsWindow.hwndLogTab, (WNDENUMPROC)SetFont, (LPARAM)deffont);

		ShowWindow(actsWindow.hwnd, nShowCmd);
		actsWindow.SetLogMessage(L"Loading...");

		actsWindow.SetLogMessage(L"Loading hashes...");
		hashutils::ReadDefaultFile();

		DestroyWindow(actsWindow.hwndLogTab);
		actsWindow.hwndLogTab = NULL;

		actsWindow.hwndTab = CreateWindowExW(
			0,
			WC_TABCONTROLW,
			L"",
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
			0, 0, actsWindow.width, actsWindow.height,
			actsWindow.hwnd, 
			NULL, hInstance, NULL
		);

		if (actsWindow.hwndTab == NULL) {
			DestroyWindow(actsWindow.hwnd);
			return -1;
		}
		
		std::string lastPage = core::config::GetString("ui.last");

		TCITEMW tie;
		tie.mask = TCIF_TEXT;
		tie.iImage = -1;
		WCHAR achTemp[256];
		tie.pszText = achTemp;
		int i{};
		int lastPageIdx{};
		for (auto& [id, tool] : tool::ui::tools()) {
			wcscpy_s(achTemp, tool->m_description);

			tool->tabId = i;
			if (TabCtrl_InsertItem(actsWindow.hwndTab, i++, &tie) == -1) {
				DestroyWindow(actsWindow.hwnd);
				return -1;
			}
			actsWindow.pages.emplace_back(tool);

			if (id == lastPage) {
				lastPageIdx = tool->tabId;
			}
		}

		if (lastPageIdx) {
			TabCtrl_SetCurSel(actsWindow.hwndTab, lastPageIdx);
		}

		actsWindow.LoadPage();

		MSG msg{};
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		DestroyWindow(actsWindow.hwnd);
		CoUninitialize();

		return 0;
	}
	LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		for (const auto& [id, tool] : tool::ui::tools()) {
			if (tool->m_msg && !tool->m_msg(hwnd, uMsg, wParam, lParam)) {
				return S_OK; // handled by this tool
			}
		}
		switch (uMsg) {
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			// All painting occurs here, between BeginPaint and EndPaint.
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			EndPaint(hwnd, &ps);
			return S_OK;
		}
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_SIZE: {
			actsWindow.RelocateDisplay(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return S_OK;
		}
		case WM_NOTIFY: {
			LPNMHDR l = (LPNMHDR)lParam;

			switch (l->code) {
			case TCN_SELCHANGING:
				return S_OK;
			case TCN_SELCHANGE: {
				actsWindow.LoadPage();
				break;
			}
			}
			break;
		}
		case WM_COMMAND:
			if (wParam == BN_CLICKED) {
				//auto it = actsWindow.buttonEvent.find((HWND)lParam);
				//if (it != actsWindow.buttonEvent.end()) {
				//	it->second();
				//	return 0;
				//}
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return S_OK;

		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			// All painting occurs here, between BeginPaint and EndPaint.
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			EndPaint(hwnd, &ps);
			return S_OK;
		}
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}