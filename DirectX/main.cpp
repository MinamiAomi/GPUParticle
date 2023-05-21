#include "Window.h"
#include "DirectXDevice.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	Window window;
	window.Create();

	DirectXDevice directXDevice;
	directXDevice.Initalize(window.GetHWND(), window.GetClientWidth(), window.GetClientHeight());

	window.Show();

	// メインループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			directXDevice.StertScreenRendering();

			directXDevice.FinishScreenRendering();
		}
	}

	directXDevice.Finalize();
	window.Close();

	return 0;
}