#include "http_mimes.h"

char const *const mime_types[] = {
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
    "multipart"     /* MIME_MULTIPART_FORM_DATA */
};

char const *const mime_subtypes[] = {
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
    "form-data"           /* MIME_MULTIPART_FORM_DATA */
};
