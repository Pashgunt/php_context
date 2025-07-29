#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_context.h"
#include "php_context_arginfo.h"
#include <pthread.h>
#include <unistd.h>

#include "Zend/zend_exceptions.h"     // для zend_throw_exception
#include "ext/standard/info.h"  // для php_info_print_table_start и др.

static zend_class_entry *context_ce;

typedef struct _php_context_object {
    context_t *ctx;
    zend_object std;
} php_context_object;

static inline php_context_object* php_context_fetch(zend_object *obj) {
    return (php_context_object *)((char *)(obj) - XtOffsetOf(php_context_object, std));
}

static zend_object* context_create_object(zend_class_entry *ce) {
    php_context_object *intern = ecalloc(1, sizeof(php_context_object) + zend_object_properties_size(ce));
    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);
    intern->std.handlers = &std_object_handlers;
    return &intern->std;
}

PHP_METHOD(Context, run) {
    zval *callback;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callback)
    ZEND_PARSE_PARAMETERS_END();

    if (!zend_is_callable(callback, 0, NULL)) {
        zend_throw_exception(NULL, "Argument must be callable", 0);
        RETURN_FALSE;
    }

    php_context_object *obj = php_context_fetch(Z_OBJ_P(ZEND_THIS));

    time_t deadline = context_get_deadline(obj->ctx);
    time_t start = time(NULL);

    zval retval;
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    if (zend_fcall_info_init(callback, 0, &fci, &fcc, NULL, NULL) != SUCCESS) {
        zend_throw_exception(NULL, "Cannot initialize callback", 0);
        RETURN_FALSE;
    }

    fci.retval = &retval;

    if (zend_call_function(&fci, &fcc) != SUCCESS) {
        zend_throw_exception(NULL, "Error calling callback", 0);
        RETURN_FALSE;
    }

    time_t now = time(NULL);
    if (deadline > 0 && now - start >= deadline) {
        context_cancel(obj->ctx);
        zval_ptr_dtor(&retval);
        RETURN_NULL();
    }

    RETURN_COPY_VALUE(&retval);
    zval_ptr_dtor(&retval);
}


PHP_METHOD(Context, __construct) {
    zend_long timeout = 0;
    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(timeout)
    ZEND_PARSE_PARAMETERS_END();

    php_context_object *obj = php_context_fetch(Z_OBJ_P(ZEND_THIS));
    obj->ctx = context_create(timeout);
}

PHP_METHOD(Context, cancel) {
    php_context_object *obj = php_context_fetch(Z_OBJ_P(ZEND_THIS));
    context_cancel(obj->ctx);
}

PHP_METHOD(Context, isCancelled) {
    php_context_object *obj = php_context_fetch(Z_OBJ_P(ZEND_THIS));
    RETURN_BOOL(context_is_cancelled(obj->ctx));
}

static zend_function_entry context_methods[] = {
    PHP_ME(Context, __construct, arginfo_context___construct, ZEND_ACC_PUBLIC)
    PHP_ME(Context, cancel, arginfo_context_cancel, ZEND_ACC_PUBLIC)
    PHP_ME(Context, isCancelled, arginfo_context_isCancelled, ZEND_ACC_PUBLIC)
    PHP_ME(Context, run, arginfo_context_run, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

PHP_MINIT_FUNCTION(context) {
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Context", context_methods);
    context_ce = zend_register_internal_class(&ce);
    context_ce->create_object = context_create_object;
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(context) {
    return SUCCESS;
}

PHP_MINFO_FUNCTION(context) {
    php_info_print_table_start();
    php_info_print_table_row(2, "context support", "enabled");
    php_info_print_table_row(2, "Version", PHP_CONTEXT_VERSION);
    php_info_print_table_end();
}

zend_module_entry context_module_entry = {
    STANDARD_MODULE_HEADER,
    "context",
    NULL,
    PHP_MINIT(context),
    PHP_MSHUTDOWN(context),
    NULL,
    NULL,
    PHP_MINFO(context),
    PHP_CONTEXT_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CONTEXT
ZEND_GET_MODULE(context)
#endif
