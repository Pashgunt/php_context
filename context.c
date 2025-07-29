#include "php.h"
#include "php_context.h"

typedef struct _context_t {
    zend_bool cancelled;
    time_t deadline;
} context_t;

context_t* context_create(time_t timeout) {
    context_t *ctx = emalloc(sizeof(context_t));
    ctx->cancelled = 0;

    if (timeout > 0) {
        ctx->deadline = time(NULL) + timeout;
    } else {
        ctx->deadline = 0;
    }

    return ctx;
}

void context_cancel(context_t *ctx) {
    ctx->cancelled = 1;
}

zend_bool context_is_cancelled(context_t *ctx) {
    if (ctx->cancelled) {
        return 1;
    }
    if (ctx->deadline > 0 && time(NULL) > ctx->deadline) {
        ctx->cancelled = 1;
        return 1;
    }
    return 0;
}

void context_free(context_t *ctx) {
    efree(ctx);
}

time_t context_get_deadline(context_t *ctx) {
    return ctx->deadline;
}
