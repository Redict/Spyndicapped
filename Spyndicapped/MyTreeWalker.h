#pragma once
#include <Windows.h>
#include <UIAutomationClient.h>
#include <atlbase.h>
#include <memory>

class MyTreeWalker;
extern std::unique_ptr<MyTreeWalker> g_pMyTreeWalker;

class MyTreeWalker
{
private:
	IUIAutomation* pAutomation = nullptr;
	CComPtr<IUIAutomationTreeWalker> pWalker;

public:
	MyTreeWalker(IUIAutomation* pUIAutomation);
	~MyTreeWalker();
	HRESULT GetFirstAscendingWindowName(IUIAutomationElement* pAutomationElement, BSTR* bWindowName);
	IUIAutomationElement* GetParent(IUIAutomationElement* child);
	IUIAutomationElement* FindFirstAscending(IUIAutomationElement* pStartElement, IUIAutomationCondition* pAutomationCondition);
	IUIAutomation* GetPAutomation();
	IUIAutomationTreeWalker* GetPTreeWalker();
};
