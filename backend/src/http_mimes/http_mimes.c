#include "http_mimes.h"

#include <string.h>

static char const *const mime_types[] = {
  "text",         /* MIME_TEXT_PLAIN */
  "text",         /* MIME_TEXT_HTML */
  "text",         /* MIME_TEXT_CSS */
  "text",         /* MIME_TEXT_JAVASCRIPT */
  "application",  /* MIME_APPLICATION_JSON */
  "application",  /* MIME_APPLICATION_XML */
  "application",  /* MIME_APPLICATION_OCTET_STREAM */
  "image",        /* MIME_IMAGE_PNG */
  "image",        /* MIME_IMAGE_JPEG */
  "image",        /* MIME_IMAGE_GIF */
  "video",        /* MIME_VIDEO_MP4 */
  "video",        /* MIME_VIDEO_WEBM */
  "audio",        /* MIME_AUDIO_MP3 */
  "audio",        /* MIME_AUDIO_OGG */
  "multipart",    /* MIME_MULTIPART_FORM_DATA */
  "application"   /* MIME_UNKNOWN */
};

static char const *const mime_subtypes[] = {
  "plain",              /* MIME_TEXT_PLAIN */
  "html",               /* MIME_TEXT_HTML */
  "css",                /* MIME_TEXT_CSS */
  "javascript",         /* MIME_TEXT_JAVASCRIPT */
  "json",               /* MIME_APPLICATION_JSON */
  "xml",                /* MIME_APPLICATION_XML */
  "octet-stream",       /* MIME_APPLICATION_OCTET_STREAM */
  "png",                /* MIME_IMAGE_PNG */
  "jpeg",               /* MIME_IMAGE_JPEG */
  "gif",                /* MIME_IMAGE_GIF */
  "mp4",                /* MIME_VIDEO_MP4 */
  "webm",               /* MIME_VIDEO_WEBM */
  "mp3",                /* MIME_AUDIO_MP3 */
  "ogg",                /* MIME_AUDIO_OGG */
  "form-data",          /* MIME_MULTIPART_FORM_DATA */
  "octet-stream",       /* MIME_UNKNOWN */
};

static char const *const content_type[] = {
  "text/plain",                     /* MIME_TEXT_PLAIN */
  "text/html",                      /* MIME_TEXT_HTML */
  "text/css",                       /* MIME_TEXT_CSS */
  "text/javascript",                /* MIME_TEXT_JAVASCRIPT */
  "application/json",               /* MIME_APPLICATION_JSON */
  "application/xml",                /* MIME_APPLICATION_XML */
  "application/octet-stream",       /* MIME_APPLICATION_OCTET_STREAM */
  "image/png",                      /* MIME_IMAGE_PNG */
  "image/jpeg",                     /* MIME_IMAGE_JPEG */
  "image/gif",                      /* MIME_IMAGE_GIF */
  "video/mp4",                      /* MIME_VIDEO_MP4 */
  "video/webm",                     /* MIME_VIDEO_WEBM */
  "audio/mp3",                      /* MIME_AUDIO_MP3 */
  "audio/ogg",                      /* MIME_AUDIO_OGG */
  "multipart/form-data",            /* MIME_MULTIPART_FORM_DATA */
  "application/octet-stream",       /* MIME_UNKNOWN */
};

PUBLIC char const *GetContentTypeStr(register enum MimeType const mime) {
  if (mime > MIME_UNKNOWN) {
    return content_type[MIME_UNKNOWN];
  }
  return content_type[mime];
}

PUBLIC char const *GetMimeTypeStr(register enum MimeType const mime) {
  if (mime > MIME_UNKNOWN) {
    return mime_types[MIME_UNKNOWN];
  }
  return mime_types[mime];
}

PUBLIC char const *GetMimeSubtypeStr(register enum MimeType const mime) {
  if (mime > MIME_UNKNOWN) {
    return mime_subtypes[MIME_UNKNOWN];
  }
  return mime_subtypes[mime];
}

PUBLIC enum MimeType GetMimeTyeFromPath(char const *path) {
  register const char *const ext = strrchr(path, '.');
  if (ext == NULL || ext == path) {
    return MIME_UNKNOWN;
  }

  if (strcmp(path, "/") == 0 || strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
    return MIME_TEXT_HTML;
  } else if (strcmp(ext, ".css") == 0) {
    return MIME_TEXT_CSS;
  } else if (strcmp(ext, ".js") == 0) {
    return MIME_TEXT_JAVASCRIPT;
  } else if (strcmp(ext, ".json") == 0) {
    return MIME_APPLICATION_JSON;
  } else if (strcmp(ext, ".xml") == 0) {
    return MIME_APPLICATION_XML;
  } else if (strcmp(ext, ".png") == 0) {
    return MIME_IMAGE_PNG;
  } else if (strcmp(ext, ".jpeg") == 0 || strcmp(ext, ".jpg") == 0) {
    return MIME_IMAGE_JPEG;
  } else if (strcmp(ext, ".gif") == 0) {
    return MIME_IMAGE_GIF;
  } else if (strcmp(ext, ".mp4") == 0) {
    return MIME_VIDEO_MP4;
  } else if (strcmp(ext, ".webm") == 0) {
    return MIME_VIDEO_WEBM;
  } else if (strcmp(ext, ".mp3") == 0) {
    return MIME_AUDIO_MP3;
  } else if (strcmp(ext, ".ogg") == 0) {
    return MIME_AUDIO_OGG;
  } else if (strcmp(ext, ".form") == 0) {
    return MIME_MULTIPART_FORM_DATA;
  }

  return MIME_UNKNOWN;
}

