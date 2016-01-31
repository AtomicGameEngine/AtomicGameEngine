
#include "include/cef_browser.h"
#include "include/cef_callback.h"
#include "include/cef_frame.h"
#include "include/cef_resource_handler.h"
#include "include/cef_response.h"
#include "include/cef_request.h"
#include "include/cef_scheme.h"
#include "include/wrapper/cef_helpers.h"

#include <Atomic/Math/MathDefs.h>
#include <Atomic/Resource/ResourceCache.h>
#include <Atomic/IO/FileSystem.h>

#include "WebString.h"
#include "WebBrowserHost.h"
#include "WebSchemeHandler.h"

namespace Atomic
{

static WeakPtr<WebBrowserHost> webBrowserHost_;

// Implementation of the schema handler for atomic://resources/ requests.
class ResourceSchemeHandler : public CefResourceHandler
{

public:

    ResourceSchemeHandler() : fileLength_(0), offset_(0)
    {

    }

    virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                                CefRefPtr<CefCallback> callback) OVERRIDE
    {
        CEF_REQUIRE_IO_THREAD();

        if (webBrowserHost_.Null())
            return false;

        ConvertCEFString(request->GetURL(), url_);
        // shave off atomic://resources/
        url_ = url_.SubstringUTF8(19);

        if (IsAbsolutePath(url_))
        {
            file_ = new File(webBrowserHost_->GetContext(), url_, FILE_READ);
        }
        else
        {
            ResourceCache* cache = webBrowserHost_->GetSubsystem<ResourceCache>();
            file_ = cache->GetFile(url_, false);
        }


        if (!file_->IsOpen())
            return false;

        fileLength_ = file_->GetSize();

        // Indicate the headers are available.
        callback->Continue();
        return true;
    }

    virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                    int64& response_length,
                                    CefString& redirectUrl) OVERRIDE
    {
        CEF_REQUIRE_IO_THREAD();

        String pathName, fileName, ext;
        SplitPath(url_, pathName, fileName, ext);

        if (ext == ".js")
            response->SetMimeType("text/javascript");
        else if (ext == ".ts")
            response->SetMimeType("text/typescript");


        response->SetStatus(200);

        // Set the resulting response length
        response_length = fileLength_;
    }

    virtual void Cancel() OVERRIDE
    {
        CEF_REQUIRE_IO_THREAD();
    }

    virtual bool ReadResponse(void* data_out,
                              int bytes_to_read,
                              int& bytes_read,
                              CefRefPtr<CefCallback> callback) OVERRIDE
    {
        bool has_data = false;
        bytes_read = 0;

        if (offset_ < fileLength_)
        {
            // Copy the next block of data into the buffer.
            int transfer_size = Min(bytes_to_read, static_cast<int>(fileLength_ - offset_));

            file_->Read(data_out, transfer_size);
            offset_ += transfer_size;

            bytes_read = transfer_size;
            has_data = true;

            if (offset_ >= fileLength_)
                file_->Close();
        }


        return has_data;
    }

private:

    SharedPtr<File> file_;
    unsigned fileLength_;
    String url_;
    unsigned offset_;

    IMPLEMENT_REFCOUNTING(ResourceSchemeHandler);
};

// Implementation of the factory for for creating schema handlers.
class ResourceSchemeHandlerFactory : public CefSchemeHandlerFactory
{

public:
    // Return a new scheme handler instance to handle the request.
    virtual CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                                 CefRefPtr<CefFrame> frame,
                                                 const CefString& scheme_name,
                                                 CefRefPtr<CefRequest> request) OVERRIDE
    {
        CEF_REQUIRE_IO_THREAD();
        return new ResourceSchemeHandler();
    }

    IMPLEMENT_REFCOUNTING(ResourceSchemeHandlerFactory);
};


void RegisterWebSchemeHandlers(WebBrowserHost* host)
{
    webBrowserHost_ = host;

    CefRegisterSchemeHandlerFactory("atomic", "resources",
                                    new ResourceSchemeHandlerFactory());
}


}
