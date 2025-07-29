#ifndef PHP_CONTEXT_ARGINFO_H
#define PHP_CONTEXT_ARGINFO_H

ZEND_BEGIN_ARG_INFO(arginfo_context___construct, 0)
    ZEND_ARG_TYPE_INFO(0, timeout, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_context_cancel, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_context_isCancelled, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_context_run, 0)
    ZEND_ARG_CALLABLE_INFO(0, callback, 0)
ZEND_END_ARG_INFO()

#endif
