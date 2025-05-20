#include "MyTreeWalker.h"
#include "Logger.h"
#include "Errors.h"

MyTreeWalker::MyTreeWalker(IUIAutomation* pUIAutomation)
	: pAutomation(pUIAutomation)
{
	if (pUIAutomation == nullptr)
	{
		Log(L"Failed to create TreeWalker. pUIAutomation was NULL", WARNING);
		throw std::invalid_argument("pUIAutomation cannot be null");
	}

	HRESULT hr = pUIAutomation->get_RawViewWalker(&pWalker);
	if (FAILED(hr))
	{
		Log(L"Failed to create TreeWalker.", WARNING);
		PrintErrorFromHRESULT(hr);
		throw std::runtime_error("Failed to get RawViewWalker");
	}
}

HRESULT MyTreeWalker::GetFirstAscendingWindowName(IUIAutomationElement* pAutomationElementChild, BSTR* bWindowName)
{
	if (bWindowName == nullptr || pAutomationElementChild == nullptr)
	{
		return E_POINTER;
	}
	
	CComPtr<IUIAutomationElement> pAutomationElementParent;
	HRESULT hr = pAutomationElementChild->get_CurrentName(bWindowName);
	if (SUCCEEDED(hr) && SysStringLen(*bWindowName) == 0)
	{
		CComPtr<IUIAutomationElement> pCurrentElement = pAutomationElementChild;
		
		while (true) {
			pAutomationElementParent = GetParent(pCurrentElement);
			if (!pAutomationElementParent)
			{
				Log(L"Can't find parent element", DBG);
				return E_APPLICATION_VIEW_EXITING;
			}

			hr = pAutomationElementParent->get_CurrentName(bWindowName);
			if (FAILED(hr))
			{
				Log(L"Failed to get parent name", DBG);
				return E_APPLICATION_VIEW_EXITING;
			}

			if (SysStringLen(*bWindowName) != 0)
			{
				break;
			}

			pCurrentElement = pAutomationElementParent;
		}
	}
	return S_OK;
}

MyTreeWalker::~MyTreeWalker()
{
	// CComPtr will automatically release pWalker
	// pAutomation is non-owning, so we don't release it
}

IUIAutomationElement* MyTreeWalker::GetParent(IUIAutomationElement* pChild)
{
	if (pChild == nullptr)
	{
		Log(L"GetParent: pChild was null", WARNING);
		return nullptr;
	}

	if (pWalker == nullptr)
	{
		Log(L"GetParent: pWalker was null", WARNING);
		return nullptr;
	}

	IUIAutomationElement* pParent = nullptr;
	HRESULT hr = pWalker->GetParentElement(pChild, &pParent);
	if (FAILED(hr))
	{
		Log(L"Failed to get parent.", WARNING);
		PrintErrorFromHRESULT(hr);
		return nullptr;
	}
	return pParent;
}

IUIAutomationElement* MyTreeWalker::FindFirstAscending(IUIAutomationElement* pStartElement, IUIAutomationCondition* pAutomationCondition)
{
	if (pStartElement == nullptr || pAutomationCondition == nullptr)
	{
		Log(L"FindFirstAscending: Invalid parameters", WARNING);
		return nullptr;
	}

	CComPtr<IUIAutomationElement> pCurrentElement = GetParent(pStartElement);
	if (!pCurrentElement)
	{
		return nullptr;
	}

	IUIAutomationElement* pFoundElement = nullptr;

	while (true) {
		HRESULT hr = pCurrentElement->FindFirst(TreeScope_Subtree, pAutomationCondition, &pFoundElement);
		if (SUCCEEDED(hr) && pFoundElement != nullptr) {
			return pFoundElement; 
		}

		CComPtr<IUIAutomationElement> pParent = GetParent(pCurrentElement);
		if (!pParent) {
			break;
		}
		
		pCurrentElement = pParent;
	}

	return nullptr;
}

IUIAutomation* MyTreeWalker::GetPAutomation()
{
	return pAutomation;
}

IUIAutomationTreeWalker* MyTreeWalker::GetPTreeWalker()
{
	return pWalker;
}