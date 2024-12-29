#ifndef HTTP_MIMES_H_
#define HTTP_MIMES_H_

#include "imlib/imstdinc.h"

enum MimeType {
  MIME_TEXT_PLAIN,
  MIME_TEXT_HTML,
  MIME_TEXT_CSS,
  MIME_TEXT_JAVASCRIPT,
  MIME_APPLICATION_JSON,
  MIME_APPLICATION_XML,
  MIME_APPLICATION_OCTET_STREAM,
  MIME_IMAGE_PNG,
  MIME_IMAGE_JPEG,
  MIME_IMAGE_GIF,
  MIME_IMAGE_XICON,
  MIME_VIDEO_MP4,
  MIME_VIDEO_WEBM,
  MIME_AUDIO_MP3,
  MIME_AUDIO_OGG,
  MIME_MULTIPART_FORM_DATA,
  MIME_UNKNOWN
};

PUBLIC char const *GetContentTypeStr(enum MimeType mime);
PUBLIC char const *GetMimeTypeStr(enum MimeType mime);
PUBLIC char const *GetMimeSubtypeStr(enum MimeType mime);
PUBLIC enum MimeType GetMimeTyeFromPath(char const *path);

#endif /* !HTTP_MIMES_H_ */

