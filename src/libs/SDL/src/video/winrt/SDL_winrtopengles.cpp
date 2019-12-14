/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_WINRT && SDL_VIDEO_OPENGL_EGL

/* EGL implementation of SDL OpenGL support */

#include "SDL_winrtvideo_cpp.h"
extern "C" {
#include "SDL_winrtopengles.h"
#include "SDL_loadso.h"
}

/* Windows includes */
#include <wrl/client.h>
using namespace Windows::UI::Core;

/* ANGLE/WinRT constants */
static const int ANGLE_D3D_FEATURE_LEVEL_ANY = 0;


/*
 * SDL/EGL top-level implementation
 */

extern "C" int
WINRT_GLES_LoadLibrary(_THIS, const char *path)
{
    SDL_VideoData *video_data = (SDL_VideoData *)_this->driverdata;

    if (SDL_EGL_LoadLibrary(_this, path, EGL_DEFAULT_DISPLAY) != 0) {
        return -1;
    }

    /* Load ANGLE/WinRT-specific functions */
    CreateWinrtEglWindow_Function CreateWinrtEglWindow = (CreateWinrtEglWindow_Function) SDL_LoadFunction(_this->egl_data->egl_dll_handle, "CreateWinrtEglWindow");
    if (!CreateWinrtEglWindow) {
        return SDL_SetError("Could not retrieve ANGLE/WinRT function CreateWinrtEglWindow");
    }

    /* Create an ANGLE/WinRT EGL-window */
    /* TODO, WinRT: check for XAML usage before accessing the CoreWindow, as not doing so could lead to a crash */
    CoreWindow ^ native_win = CoreWindow::GetForCurrentThread();
    Microsoft::WRL::ComPtr<IUnknown> cpp_win = reinterpret_cast<IUnknown *>(native_win);
    HRESULT result = CreateWinrtEglWindow(cpp_win, ANGLE_D3D_FEATURE_LEVEL_ANY, &(video_data->winrtEglWindow));
    if (FAILED(result)) {
        return -1;
    }

    /* Call eglGetDisplay and eglInitialize as appropriate.  On other
     * platforms, this would probably get done by SDL_EGL_LoadLibrary,
     * however ANGLE/WinRT's current implementation (as of Mar 22, 2014) of
     * eglGetDisplay requires that a C++ object be passed into it, so the
     * call will be made in this file, a C++ file, instead.
     */
    Microsoft::WRL::ComPtr<IUnknown> cpp_display = video_data->winrtEglWindow;
    _this->egl_data->egl_display = ((eglGetDisplay_Function)_this->egl_data->eglGetDisplay)(cpp_display);
    if (!_this->egl_data->egl_display) {
        return SDL_SetError("Could not get EGL display");
    }

    if (_this->egl_data->eglInitialize(_this->egl_data->egl_display, NULL, NULL) != EGL_TRUE) {
        return SDL_SetError("Could not initialize EGL");
    }

    return 0;
}

extern "C" void
WINRT_GLES_UnloadLibrary(_THIS)
{
    SDL_VideoData *video_data = (SDL_VideoData *)_this->driverdata;

    /* Release SDL's own COM reference to the ANGLE/WinRT IWinrtEglWindow */
    if (video_data->winrtEglWindow) {
        video_data->winrtEglWindow->Release();
        video_data->winrtEglWindow = nullptr;
    }

    /* Perform the bulk of the unloading */
    SDL_EGL_UnloadLibrary(_this);
}

extern "C" {
SDL_EGL_CreateContext_impl(WINRT)
SDL_EGL_SwapWindow_impl(WINRT)
SDL_EGL_MakeCurrent_impl(WINRT)
}

#endif /* SDL_VIDEO_DRIVER_WINRT && SDL_VIDEO_OPENGL_EGL */

/* vi: set ts=4 sw=4 expandtab: */

