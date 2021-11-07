主要是看看Handler这个函数,简单来说功能就是设置HttpResponse报文而已，逻辑非常清晰

```c++
if (file.Exist())
    {
        if (file.IsFile())
        {
            response->SetStatusCode(HttpResponse::OK);
            response->SetStatusMessage("OK");
            // 设置内容类型及长度
            response->SetContentType(file.GetMimeType());
            response->AddHeader("Content-Length", std::to_string(file.GetSize()));
        
            // 将response头部部分先添加进缓冲区
            response->AppendHeaderToBuffer();
            // 将文件内容作为剩余的body部分添加进缓冲区
            file.ReadToBuffer(response->GetBuffer());
        }
        // 列出目录下的内容
        else
        {
            response->SetStatusCode(HttpResponse::OK);
            response->SetStatusMessage("OK");
            response->SetContentType("text/html");

            std::string body = "<pre>\n";
            std::string slash = "/";
            auto dir = file.ListDir();
            for (auto& i : dir)
            {
                slash = i.IsDir() ? "/" : "";
                body += "<a href=\"" + i.GetName() + slash + "\">" + i.GetName() + slash + "</a>\n";
            }
            body += "</pre>";
            // 设置body长度
            response->AddHeader("Content-Length", std::to_string(body.size()));

            response->AppendHeaderToBuffer();
            response->AppendBodyToBuffer(body);
        }
    }
```