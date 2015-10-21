
#pragma once

#include "../../NETHost.h"

#define UNICODE
#include "windows.h"
#include <stdio.h>
#include "prebuilt/mscoree.h"
#include "prebuilt/palclr.h"

namespace Atomic
{

// Dynamically expanding string buffer to hold TPA list
class TPAStringBuffer {
    static const int m_defaultSize = 4096;
    wchar_t* m_buffer;
    size_t m_capacity;
    size_t m_length;

    TPAStringBuffer(const TPAStringBuffer&);
    TPAStringBuffer& operator =(const TPAStringBuffer&);

public:
    TPAStringBuffer() : m_capacity(0), m_buffer(nullptr), m_length(0) {
    }

    ~TPAStringBuffer() {
        delete[] m_buffer;
    }

    const wchar_t* CStr() const {
        return m_buffer;
    }

    void Append(const wchar_t* str, size_t strLen) {
        if (!m_buffer) {
            m_buffer = new wchar_t[m_defaultSize];
            m_capacity = m_defaultSize;
        }
        if (m_length + strLen + 1 > m_capacity) {
            size_t newCapacity = m_capacity * 2;
            wchar_t* newBuffer = new wchar_t[newCapacity];
            wcsncpy_s(newBuffer, newCapacity, m_buffer, m_length);
            delete[] m_buffer;
            m_buffer = newBuffer;
            m_capacity = newCapacity;
        }
        wcsncpy_s(m_buffer + m_length, m_capacity - m_length, str, strLen);
        m_length += strLen;
    }
};

class ATOMIC_API NETHostWindows : public NETHost
{

    OBJECT(NETHostWindows);

public:
    /// Construct.
    NETHostWindows(Context* context);
    /// Destruct.
    virtual ~NETHostWindows();

    bool Initialize();
    bool CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut);
    void WaitForDebuggerConnect();

    int ExecAssembly(const String& assemblyName, const Vector<String>& args);
    void Shutdown();

private:

    bool LoadCLRDLL();
    bool InitCLRRuntimeHost();

    bool TPAListContainsFile(wchar_t* fileNameWithoutExtension, wchar_t** rgTPAExtensions, int countExtensions);
    void RemoveExtensionAndNi(wchar_t* fileName);
    void AddFilesFromDirectoryToTPAList(const wchar_t* targetPath, wchar_t** rgTPAExtensions, int countExtensions);
    bool GenerateTPAList();
    bool CreateAppDomain();

    ICLRRuntimeHost2* clrRuntimeHost_;
    HMODULE clrModule_;
    DWORD appDomainID_;

    TPAStringBuffer tpaList_;

};

}
