/**
 * Copyright (c) 2016 DeepCortex GmbH <legal@eventql.io>
 * Authors:
 *   - Paul Asmuth <paul@eventql.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "eventql/util/http/HTTPFileDownload.h"

namespace http {

HTTPFileDownload::HTTPFileDownload(
    const HTTPRequest& http_req,
    const String& output_file) :
    http_req_(http_req),
    output_file_(output_file),
    file_(File::openFile(output_file, File::O_CREATE | File::O_WRITE)) {}

Future<HTTPResponse> HTTPFileDownload::download(HTTPConnectionPool* http) {
  return http->executeRequest(
      http_req_,
      [this] (const Promise<HTTPResponse> promise) -> HTTPResponseFuture* {
        return new HTTPFileDownload::ResponseFuture(promise, std::move(file_));
      });
}

HTTPResponse HTTPFileDownload::download(HTTPClient* http) {
  return http->executeRequest(
      http_req_,
      [this] (const Promise<HTTPResponse> promise) -> HTTPResponseFuture* {
        return new HTTPFileDownload::ResponseFuture(promise, std::move(file_));
      });
}

HTTPFileDownload::ResponseFuture::ResponseFuture(
    Promise<HTTPResponse> promise,
    File&& file) :
    HTTPResponseFuture(promise),
    file_(std::move(file)) {}

void HTTPFileDownload::ResponseFuture::onBodyChunk(
    const char* data,
    size_t size) {
  file_.write(data, size);
}

}
