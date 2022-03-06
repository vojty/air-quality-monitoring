#include "ESPAsyncWebServer.h"
#include "utils.h"

class FileResponse : public AsyncFileResponse {
  using File = fs::File;
  using FS = fs::FS;

  // https://stackoverflow.com/questions/54066315/how-to-inherit-constructor-from-a-non-direct-base-class
 public:
  FileResponse(FS& fs, const String& path, const String& contentType = String(), bool download = false,
               AwsTemplateProcessor callback = nullptr)
      : AsyncFileResponse(fs, path, contentType, download, callback) {
    // remove extra Content-Disposition header which breaks serving files
    this->_headers.free();

    // Fix for content-type since according to AsyncFileResponse everthing with .gz extension is application/x-gzip
    if (path.endsWith(".gz")) {
      String realpath = trim_gz(path);
      this->addHeader("Content-Encoding", "gzip");
      setContentType(this->getContentType(realpath));
    } else {
      setContentType(this->getContentType(path));
    }
  };

  String getContentType(const String& path) {
    if (path.endsWith(".html"))
      return "text/html";
    else if (path.endsWith(".htm"))
      return "text/html";
    else if (path.endsWith(".css"))
      return "text/css";
    else if (path.endsWith(".json"))
      return "application/json";
    else if (path.endsWith(".js"))
      return "application/javascript";
    else if (path.endsWith(".png"))
      return "image/png";
    else if (path.endsWith(".gif"))
      return "image/gif";
    else if (path.endsWith(".jpg"))
      return "image/jpeg";
    else if (path.endsWith(".ico"))
      return "image/x-icon";
    else if (path.endsWith(".svg"))
      return "image/svg+xml";
    else if (path.endsWith(".eot"))
      return "font/eot";
    else if (path.endsWith(".woff"))
      return "font/woff";
    else if (path.endsWith(".woff2"))
      return "font/woff2";
    else if (path.endsWith(".ttf"))
      return "font/ttf";
    else if (path.endsWith(".xml"))
      return "text/xml";
    else if (path.endsWith(".pdf"))
      return "application/pdf";
    else if (path.endsWith(".zip"))
      return "application/zip";
    else if (path.endsWith(".gz"))
      return "application/x-gzip";
    else
      return "text/plain";
  }
};
