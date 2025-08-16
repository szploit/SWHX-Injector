# SWHX Injector

Useful for injecting dlls using SetWindowHookEx

# How to use it?

Add a NextHook function to your dll like this 

```
extern "C" __declspec(dllexport)
LRESULT NextHook(int code, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
```


Drag the dll into the Injector's Directory
Make sure the dll's name matches the dll's name in the Injector, then open the injector and wait for it to inject.
