/* Copyright (c) 2007 John Wilmes */
/* Use apxs2 utility to build */
/* Apache Core */
#include <httpd.h>
#include <http_protocol.h>
#include <http_config.h>
#include <http_log.h>

/* System */
#include <sys/stat.h>
#include <unistd.h>

/* Math */
#include <math.h>

/* Strings */
#include <string.h>
#include <apr_strings.h>

/* File I/O */
#include <apr_file_info.h>
#include <apr_file_io.h>

/* ApReq Library */
/*      in include/apreq2/ */
#include <apreq.h>
#include <apreq_module.h>
/*      in include/apache2/apreq2/ */
#include <apreq_module_apache2.h>

const char * wxIPC_DEFAULT_DIRECTORY = "/var/wx/ipc/";
const char * wxIPC_REQUEST_SUBDIR = "request/";
const char * wxIPC_RESPONSE_SUBDIR = "response/";
const unsigned int wxSID_LENGTH = 64;
/* keep low - frequent collisions are a security risk! */
const unsigned int wxSID_COLLISIONS_MAX = 2;
const unsigned int wxFIFO_BUFFER_SIZE = 1024 * 1024;
const int wxFIFO_WRITE_TIMEOUT = 100;
const int wxFIFO_READ_TIMEOUT = -1;
/* return values */
enum {
    wxSUCCESS,
    wxAPACHE_ERROR,
    wxOS_ERROR,
    wxSID_VALID,
    wxSID_INVALID,
    wxSID_EVIL,
    wxSID_COLLISIONS_ERROR,
    wxFIFO_READ_ERROR,
    wxIS_PIPE,
    wxIS_NOT_PIPE
};

static void * wx_create_dir_cfg(apr_pool_t* pool, char * dir);
static void * wx_merge_dir_cfg(apr_pool_t* pool, void* _base, void* _add);
static int wx_sid_check(apr_pool_t * p, const char * id, const char* fifodir);
static int wx_fifo_create(request_rec * r, char** sid, const char * fifodir);
static int wx_fifo_remove(const char * requestPath, const char * responsePath);
static int wx_set_headers(request_rec * r);
static int wx_fifo_print(request_rec * r, char * fifopath);
static int wx_handler(request_rec *r);
static void wx_hooks(apr_pool_t *pool);

typedef struct {
    const char * app;
    const char * ipcDir;
} wx_cfg;

static const command_rec wx_cmds[] = {
    AP_INIT_TAKE1("WxApplicationPath",
                  ap_set_file_slot,
                  (void*)APR_OFFSETOF(wx_cfg, app),
                  ACCESS_CONF,
                  "Path to wxWeb application to be executed"),
    AP_INIT_TAKE1("WxIpcDirectory",
                  ap_set_file_slot,
                  (void*)APR_OFFSETOF(wx_cfg, ipcDir),
                  ACCESS_CONF,
                  "Path to directory for storing IPC nodes"),
    { NULL }
};

module AP_MODULE_DECLARE_DATA wx_module = {
    STANDARD20_MODULE_STUFF,
    wx_create_dir_cfg,
    wx_merge_dir_cfg,
    NULL,
    NULL,
    wx_cmds,
    wx_hooks
};

static void * wx_create_dir_cfg(apr_pool_t* pool, char * dir) {
    wx_cfg* cfg = apr_palloc(pool, sizeof(wx_cfg));

    if (0 == system(NULL)) {
        /*TODO: this is probably not exactly the right place to check this */
        ap_log_perror(APLOG_MARK, APLOG_ERR, errno, pool, "system() unavailable!");
    }

    cfg->app = NULL;
    cfg->ipcDir = NULL;
    return cfg;
}

static void * wx_merge_dir_cfg(apr_pool_t* pool, void* _base, void* _add) {
    wx_cfg* base = _base;
    wx_cfg* add = _add;
    wx_cfg* cfg = apr_palloc(pool, sizeof(wx_cfg));

    cfg->app = apr_pstrdup(pool, (add->app == NULL) ? base->app : add->app);
    cfg->ipcDir = apr_pstrdup(pool, (add->ipcDir == NULL) ? base->ipcDir : add->ipcDir);
    return cfg;
}

static int wx_sid_check(apr_pool_t * p, const char * id, const char* fifodir) {
    /* checking for evil chars is cheaper than enforcing legal chars
     * and is just as safe */

    const char * EVIL_CHARS = "/\\:;'\"()+?*<>|\n";
    if (strlen(id) != wxSID_LENGTH) {
        return wxSID_INVALID;
    }
    if (strpbrk(id, EVIL_CHARS) != NULL) {
        return wxSID_EVIL;
    }
    return wxSID_VALID;
}

static int wx_fifo_create(request_rec * r, char** sid, const char * requestDir, const char * responseDir) {
    int tries = 0,
        rlen = floor(wxSID_LENGTH / 4.0 * 3.0),
        dlen = strlen(requestDir);
    char * c = NULL,
         * fifopath = apr_palloc(r->pool, dlen + wxSID_LENGTH + 1),
         * fifoname = &(fifopath[dlen]);
    unsigned char * rbuf = apr_palloc(r->pool, rlen);
    apr_cpystrn(fifopath, requestDir, dlen);
    fifoname[wxSID_LENGTH] = '\0';
    while (tries < wxSID_COLLISIONS_MAX) {
        ++tries;
        apr_generate_random_bytes(rbuf, rlen);
        apr_base64_encode_binary(*sid, rbuf, rlen);
        while ((c = strchr(*sid, '/')) != NULL) {
            *c = '-';
        }
        while ((c = strchr(*sid, '+')) != NULL) {
            *c = '_';
        }
        apr_cpystrn(fifoname, *sid, wxSID_LENGTH);

        /* don't use APR functions for fifo - we want indefinite lifetime */
        if (0 == mkfifo(fifopath, S_IRWXU)) {
            fifopath = apr_pstrcat(r->pool, responseDir, *sid, NULL);
            if (0 != mkfifo(fifopath, S_IRWXU)) {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, errno, r, "Unable to create response FIFO at '%s'", fifopath);
                ap
                return wxOS_ERROR;
            }
            return wxSUCCESS;
        } else if (errno != EEXIST) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, errno, r, "Unable to create request FIFO at '%s'", fifopath);
            return wxOS_ERROR;
        }
    }
    ap_log_rerror(APLOG_MARK, APLOG_ERR, errno, r, "Too many Session ID collisions!");
    return wxSID_COLLISIONS_ERROR;
}

static int wx_fifo_remove(const char * requestPath, const char * responsePath) {
    /* Always remove response FIFO first! */
    /* TODO: error checking */
    apr_file_remove(responsePath, r->pool);
    apr_file_remove(requestPath, r->pool);
    return wxSUCCESS;
}

static int wx_set_headers(request_rec * r) {
    ap_set_content_type(r, "text/html;charset=ascii");
    return wxSUCCESS;
}

static int wx_fifo_print(request_rec * r, char * fifopath) {
    int result = 0;
    apr_file_t * fifo;
    char * buf = apr_palloc(r->pool, wxFIFO_BUFFER_SIZE + 1);
    result = apr_file_open(&fifo, fifopath, APR_READ, APR_OS_DEFAULT, r->pool);
    if (APR_SUCCESS != result) {
        ap_log_rerror(APLOG_MARK, APLOG_NOTICE, result, r, "Unable to open FIFO, aborting response.");
        return wxFIFO_READ_ERROR;
    }
    apr_file_pipe_timeout_set(fifo, wxFIFO_READ_TIMEOUT);
    buf[wxFIFO_BUFFER_SIZE] = '\0';
    apr_size_t nread = wxFIFO_BUFFER_SIZE;
    while (nread == wxFIFO_BUFFER_SIZE) {
        result = apr_file_read_full(fifo, buf, wxFIFO_BUFFER_SIZE, &nread);
        if (APR_SUCCESS != result && APR_EOF != result) {
            ap_log_rerror(APLOG_MARK, APLOG_NOTICE, result, r, "Unable to read FIFO, aborting response.");
            return wxFIFO_READ_ERROR;
        }
        if (nread != ap_rputs((char*) buf, r)) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "Apache did not print full response!");
            return wxAPACHE_ERROR;
        }
    } 
    apr_file_close(fifo);
    return wxSUCCESS;
}

static int wx_handler(request_rec *r) {
    int e = 0;
    char * sid = NULL,
         * req = NULL,
         * requestDir = NULL,
         * responseDir = NULL,
         * requestPath = NULL,
         * responsePath = NULL,
         * cmd = NULL;
    apreq_param_t * wxsid = NULL,
                  * wxreq = NULL;
    apr_file_t * fifo = NULL;

    if (!r->handler || (strcmp(r->handler, "wx") != 0)) {
        return DECLINED;
    }
    if (r->method_number != M_GET && r->method_number != M_POST) {
        return HTTP_METHOD_NOT_ALLOWED;
    }

    apreq_handle_t * apreqh = apreq_handle_apache2(r);
    wx_cfg* cfg = ap_get_module_config(r->per_dir_config, &wx_module);
    if (cfg->app == NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "wxApplicationPath is not set");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (cfg->ipcDir == NULL) {
        cfg->ipcDir = wxDEFAULT_IPC_DIRECTORY;
    }
    requestDir = apr_pstrcat(r->pool, cfg->ipcDir, wxIPC_REQUEST_SUBDIR, NULL);
    responseDir = apr_pstrcat(r->pool, cfg->ipcDir, wxIPC_RESPONSE_SUBDIR, NULL);

    wxsid = apreq_param(apreqh, "wx_sid");
    sid = (wxsid == NULL) ? "" : apr_pstrndup(r->pool, wxsid->v.data, wxSID_LENGTH);
    switch (wx_sid_check(r->pool, sid, requestDir)) {

    case wxSID_EVIL:
        /*TODO: consider taking evasive action*/
        ap_log_rerror(APLOG_MARK, APLOG_ERR, wxSID_EVIL, r, "Possible malicious request received - aborting.");
        return HTTP_BAD_REQUEST;
        break;

    case wxSID_VALID:
        wxreq = apreq_param(apreqh, "wx_request");
        req = (wxreq == NULL) ? "" : apr_pstrdup(r->pool, wxreq->v.data);
        requestPath = apr_pstrcat(r->pool, requestDir, sid, NULL);
        if (APR_SUCCESS == apr_file_open(&fifo, requestPath, APR_WRITE, APR_OS_DEFAULT, r->pool)) {
            apr_file_pipe_timeout_set(fifo, wxFIFO_WRITE_TIMEOUT);
            if (APR_SUCCESS == apr_file_lock(fifo, APR_FLOCK_EXCLUSIVE) && APR_SUCCESS == apr_file_puts(req, fifo)) {
                apr_file_close(fifo);
                wx_set_headers(r);
                responsePath = apr_pstrcat(r->pool, responseDir, sid, NULL);
                switch(wx_fifo_print(r, responsePath)) {
                case wxSUCCESS:
                    return OK;
                    break;
                case wxAPACHE_ERROR:
                    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "Apache did not print full response!");
                    return HTTP_INTERNAL_SERVER_ERROR;
                    break;
                default: /* wxFIFO_READ_ERROR */
                    /* do nothing */
                    break;
                }
            } else {
                apr_file_close(fifo);
            }
        }
        /* FALL THROUGH */

    case wxSID_INVALID:
        sid = apr_palloc(r->pool, wxSID_LENGTH + 1);
        e = wx_fifo_create(r, &sid, requestDir, responseDir);
        if (e != wxSUCCESS) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, e, r, "Unable to create FIFO!");
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        responsePath = apr_pstrcat(r->pool, responseDir, sid, NULL);
        cmd = apr_pstrcat(r->pool, cfg->app, " \"",
                                sid, "\" \"",
                                r->connection->remote_ip, "\" \"",
                                requestPath, "\" \"",
                                responsePath, "\" &", NULL);
        if (0 != system(cmd)) { /*TODO: should we fork and exec instead? */
            ap_log_rerror(APLOG_MARK, APLOG_ERR, errno, r, "Error executing command: '%s'", cmd);
            requestPath = apr_pstrcat(r->pool, requestDir, sid, NULL);
            wx_fifo_remove(requestPath, responsePath);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        wx_set_headers(r);
        if (wxSUCCESS != wx_fifo_print(r, fifopath)) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, e, r, "Unable to print response from FIFO!");
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        return OK;
        break;

    default:
        ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "Unknwon error!");
        return HTTP_INTERNAL_SERVER_ERROR;
        break;
    }
    /* never reached */
    ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r, "Unknwon error!");
    return HTTP_INTERNAL_SERVER_ERROR;
}

static void wx_hooks(apr_pool_t *pool) {
    ap_hook_handler(wx_handler, NULL, NULL, APR_HOOK_MIDDLE);
}
